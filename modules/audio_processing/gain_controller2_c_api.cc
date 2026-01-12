/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/gain_controller2_c_api.h"

#include <optional>

#include "api/audio/audio_processing.h"
#include "api/environment/environment_factory.h"
#include "modules/audio_processing/agc2/input_volume_controller.h"
#include "modules/audio_processing/audio_buffer.h"
#include "modules/audio_processing/gain_controller2.h"

namespace {

webrtc::AudioProcessing::Config::GainController2 BuildConfig(
    const Agc2Config& config) {
  webrtc::AudioProcessing::Config::GainController2 agc2_config;
  agc2_config.enabled = true;
  agc2_config.input_volume_controller.enabled = false;
  agc2_config.adaptive_digital.enabled = config.enable_adaptive_digital != 0;
  agc2_config.adaptive_digital.headroom_db = config.adaptive_headroom_db;
  agc2_config.adaptive_digital.max_gain_db = config.adaptive_max_gain_db;
  agc2_config.adaptive_digital.initial_gain_db = config.adaptive_initial_gain_db;
  agc2_config.adaptive_digital.max_gain_change_db_per_second =
      config.adaptive_max_gain_change_db_per_second;
  agc2_config.adaptive_digital.max_output_noise_level_dbfs =
      config.adaptive_max_output_noise_level_dbfs;
  agc2_config.fixed_digital.gain_db = config.fixed_gain_db;
  return agc2_config;
}

class Agc2HandleImpl {
 public:
  explicit Agc2HandleImpl(const Agc2Config& config)
      : env_(webrtc::CreateEnvironment()),
        config_(BuildConfig(config)),
        stream_config_(config.sample_rate_hz,
                       static_cast<size_t>(config.num_channels)),
        audio_buffer_(config.sample_rate_hz,
                      config.num_channels,
                      config.sample_rate_hz,
                      config.num_channels,
                      config.sample_rate_hz,
                      config.num_channels),
        agc2_(env_,
              config_,
              input_volume_controller_config_,
              config.sample_rate_hz,
              config.num_channels,
              config.use_internal_vad != 0),
        frames_per_chunk_(static_cast<int>(stream_config_.num_frames())),
        use_internal_vad_(config.use_internal_vad != 0),
        adaptive_enabled_(config.enable_adaptive_digital != 0),
        num_channels_(config.num_channels) {}

  int ProcessInt16(const int16_t* input_interleaved,
                   int16_t* output_interleaved,
                   int num_frames,
                   float speech_probability) {
    if (num_frames <= 0 || (num_frames % frames_per_chunk_) != 0) {
      return -2;
    }
    std::optional<float> speech_prob;
    if (!use_internal_vad_) {
      if (speech_probability > 1.0f) {
        return -3;
      }
      if (speech_probability < 0.0f) {
        if (adaptive_enabled_) {
          return -3;
        }
      } else {
        speech_prob = speech_probability;
      }
    }
    const int frames_per_call = frames_per_chunk_;
    for (int offset = 0; offset < num_frames; offset += frames_per_call) {
      const int16_t* in_ptr =
          input_interleaved + (offset * num_channels_);
      int16_t* out_ptr = output_interleaved + (offset * num_channels_);
      audio_buffer_.CopyFrom(in_ptr, stream_config_);
      agc2_.Analyze(/*applied_input_volume=*/0, audio_buffer_);
      agc2_.Process(speech_prob, /*input_volume_changed=*/false,
                    &audio_buffer_);
      audio_buffer_.CopyTo(stream_config_, out_ptr);
    }
    return 0;
  }

  void SetFixedGainDb(float gain_db) { agc2_.SetFixedGainDb(gain_db); }

  int frames_per_chunk() const { return frames_per_chunk_; }

 private:
  webrtc::Environment env_;
  webrtc::AudioProcessing::Config::GainController2 config_;
  webrtc::InputVolumeController::Config input_volume_controller_config_;
  webrtc::StreamConfig stream_config_;
  webrtc::AudioBuffer audio_buffer_;
  webrtc::GainController2 agc2_;
  const int frames_per_chunk_;
  const bool use_internal_vad_;
  const bool adaptive_enabled_;
  const int num_channels_;
};

}  // namespace

int agc2_frames_per_10ms(int sample_rate_hz) {
  return webrtc::AudioProcessing::GetFrameSize(sample_rate_hz);
}

Agc2Handle* agc2_create(const Agc2Config* config) {
  if (!config) {
    return nullptr;
  }
  if (config->sample_rate_hz <= 0 || config->num_channels <= 0) {
    return nullptr;
  }
  webrtc::AudioProcessing::Config::GainController2 agc2_config =
      BuildConfig(*config);
  if (!webrtc::GainController2::Validate(agc2_config)) {
    return nullptr;
  }
  return reinterpret_cast<Agc2Handle*>(new Agc2HandleImpl(*config));
}

void agc2_destroy(Agc2Handle* handle) {
  delete reinterpret_cast<Agc2HandleImpl*>(handle);
}

void agc2_set_fixed_gain_db(Agc2Handle* handle, float gain_db) {
  if (!handle) {
    return;
  }
  reinterpret_cast<Agc2HandleImpl*>(handle)->SetFixedGainDb(gain_db);
}

int agc2_process_int16(Agc2Handle* handle,
                       const int16_t* input_interleaved,
                       int16_t* output_interleaved,
                       int num_frames,
                       float speech_probability) {
  if (!handle || !input_interleaved || !output_interleaved) {
    return -1;
  }
  return reinterpret_cast<Agc2HandleImpl*>(handle)->ProcessInt16(
      input_interleaved, output_interleaved, num_frames, speech_probability);
}
