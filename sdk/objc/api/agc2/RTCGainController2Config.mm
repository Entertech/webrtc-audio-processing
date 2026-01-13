/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCGainController2Config.h"

@implementation RTC_OBJC_TYPE(RTCGainController2Config)

- (instancetype)initWithSampleRateHz:(int)sampleRateHz numChannels:(int)numChannels {
  if (self = [super init]) {
    _sampleRateHz = sampleRateHz;
    _numChannels = numChannels;
    _useInternalVad = YES;
    _enableAdaptiveDigital = YES;
    _adaptiveHeadroomDb = 5.0f;
    _adaptiveMaxGainDb = 50.0f;
    _adaptiveInitialGainDb = 15.0f;
    _adaptiveMaxGainChangeDbPerSecond = 6.0f;
    _adaptiveMaxOutputNoiseLevelDbfs = -50.0f;
    _fixedGainDb = 0.0f;
  }
  return self;
}

@end
