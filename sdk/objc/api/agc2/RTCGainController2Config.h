/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import "RTCMacros.h"

NS_ASSUME_NONNULL_BEGIN

RTC_OBJC_EXPORT
@interface RTC_OBJC_TYPE(RTCGainController2Config) : NSObject

@property(nonatomic, assign) int sampleRateHz;
@property(nonatomic, assign) int numChannels;
@property(nonatomic, assign) BOOL useInternalVad;
@property(nonatomic, assign) BOOL enableAdaptiveDigital;
@property(nonatomic, assign) float adaptiveHeadroomDb;
@property(nonatomic, assign) float adaptiveMaxGainDb;
@property(nonatomic, assign) float adaptiveInitialGainDb;
@property(nonatomic, assign) float adaptiveMaxGainChangeDbPerSecond;
@property(nonatomic, assign) float adaptiveMaxOutputNoiseLevelDbfs;
@property(nonatomic, assign) float fixedGainDb;

- (instancetype)initWithSampleRateHz:(int)sampleRateHz numChannels:(int)numChannels;

@end

NS_ASSUME_NONNULL_END
