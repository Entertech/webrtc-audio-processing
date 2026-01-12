/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_C_API_H_
#define MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_C_API_H_

#include <stdint.h>

#include "rtc_base/system/rtc_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Agc2Handle Agc2Handle;

typedef struct Agc2Config {
  int sample_rate_hz;
  int num_channels;
  int use_internal_vad;
  int enable_adaptive_digital;
  float adaptive_headroom_db;
  float adaptive_max_gain_db;
  float adaptive_initial_gain_db;
  float adaptive_max_gain_change_db_per_second;
  float adaptive_max_output_noise_level_dbfs;
  float fixed_gain_db;
} Agc2Config;

RTC_EXPORT int agc2_frames_per_10ms(int sample_rate_hz);

RTC_EXPORT Agc2Handle* agc2_create(const Agc2Config* config);
RTC_EXPORT void agc2_destroy(Agc2Handle* handle);
RTC_EXPORT void agc2_set_fixed_gain_db(Agc2Handle* handle, float gain_db);

// Processes interleaved 16-bit PCM. `num_frames` must be a multiple of
// `agc2_frames_per_10ms(sample_rate_hz)`. If `use_internal_vad` is false and
// adaptive digital is enabled, `speech_probability` must be in [0, 1]. Pass
// -1.0f to indicate "unknown" when internal VAD is enabled.
RTC_EXPORT int agc2_process_int16(Agc2Handle* handle,
                                 const int16_t* input_interleaved,
                                 int16_t* output_interleaved,
                                 int num_frames,
                                 float speech_probability);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_C_API_H_
