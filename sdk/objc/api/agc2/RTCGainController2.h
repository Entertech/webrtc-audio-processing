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
#import "RTCGainController2Config.h"

NS_ASSUME_NONNULL_BEGIN

RTC_OBJC_EXPORT
@interface RTC_OBJC_TYPE(RTCGainController2) : NSObject

/**
 * Returns the number of frames per 10ms for the given sample rate.
 */
+ (int)framesPer10ms:(int)sampleRateHz;

/**
 * Creates a new GainController2 instance with the given configuration.
 * Returns nil if creation fails.
 */
+ (nullable instancetype)createWithConfig:(RTC_OBJC_TYPE(RTCGainController2Config) *)config;

/**
 * Sets the fixed gain in dB.
 */
- (void)setFixedGainDb:(float)gainDb;

/**
 * Processes interleaved 16-bit PCM audio.
 *
 * @param input Input audio data (interleaved int16 samples).
 * @param output Output buffer for processed audio (must be same size as input).
 * @param speechProbability Speech probability in [0, 1], or -1.0 to use internal VAD.
 * @return 0 on success, negative on error.
 */
- (int)processInput:(NSData *)input
             output:(NSMutableData *)output
  speechProbability:(float)speechProbability;

/**
 * Closes the instance and releases native resources.
 * After calling this, the instance should not be used.
 */
- (void)close;

@end

NS_ASSUME_NONNULL_END
