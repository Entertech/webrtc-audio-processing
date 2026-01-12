/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCGainController2.h"

#include "modules/audio_processing/gain_controller2_c_api.h"

@implementation RTC_OBJC_TYPE(RTCGainController2) {
  Agc2Handle *_handle;
  int _numChannels;
}

+ (int)framesPer10ms:(int)sampleRateHz {
  return agc2_frames_per_10ms(sampleRateHz);
}

+ (nullable instancetype)createWithConfig:(RTC_OBJC_TYPE(RTCGainController2Config) *)config {
  Agc2Config nativeConfig;
  nativeConfig.sample_rate_hz = config.sampleRateHz;
  nativeConfig.num_channels = config.numChannels;
  nativeConfig.use_internal_vad = config.useInternalVad ? 1 : 0;
  nativeConfig.enable_adaptive_digital = config.enableAdaptiveDigital ? 1 : 0;
  nativeConfig.adaptive_headroom_db = config.adaptiveHeadroomDb;
  nativeConfig.adaptive_max_gain_db = config.adaptiveMaxGainDb;
  nativeConfig.adaptive_initial_gain_db = config.adaptiveInitialGainDb;
  nativeConfig.adaptive_max_gain_change_db_per_second = config.adaptiveMaxGainChangeDbPerSecond;
  nativeConfig.adaptive_max_output_noise_level_dbfs = config.adaptiveMaxOutputNoiseLevelDbfs;
  nativeConfig.fixed_gain_db = config.fixedGainDb;

  Agc2Handle *handle = agc2_create(&nativeConfig);
  if (handle == nullptr) {
    return nil;
  }

  RTC_OBJC_TYPE(RTCGainController2) *instance = [[RTC_OBJC_TYPE(RTCGainController2) alloc] init];
  instance->_handle = handle;
  instance->_numChannels = config.numChannels;
  return instance;
}

- (void)dealloc {
  [self close];
}

- (void)setFixedGainDb:(float)gainDb {
  if (_handle != nullptr) {
    agc2_set_fixed_gain_db(_handle, gainDb);
  }
}

- (int)processInput:(NSData *)input
             output:(NSMutableData *)output
  speechProbability:(float)speechProbability {
  if (_handle == nullptr) {
    return -1;
  }

  if (input == nil || output == nil) {
    return -1;
  }

  if (input.length != output.length) {
    return -1;
  }

  NSUInteger numSamples = input.length / sizeof(int16_t);
  int numFrames = (int)(numSamples / _numChannels);

  const int16_t *inputPtr = (const int16_t *)input.bytes;
  int16_t *outputPtr = (int16_t *)output.mutableBytes;

  return agc2_process_int16(_handle, inputPtr, outputPtr, numFrames, speechProbability);
}

- (void)close {
  if (_handle != nullptr) {
    agc2_destroy(_handle);
    _handle = nullptr;
  }
}

@end
