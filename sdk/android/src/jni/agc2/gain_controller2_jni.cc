#include <jni.h>

#include "modules/audio_processing/gain_controller2_c_api.h"

namespace {

Agc2Handle* HandleFromJlong(jlong handle) {
  return reinterpret_cast<Agc2Handle*>(handle);
}

}  // namespace

extern "C" JNIEXPORT jint JNICALL
Java_org_webrtc_agc2_GainController2_nativeFramesPer10ms(JNIEnv*,
                                                         jclass,
                                                         jint sample_rate_hz) {
  return agc2_frames_per_10ms(sample_rate_hz);
}

extern "C" JNIEXPORT jlong JNICALL
Java_org_webrtc_agc2_GainController2_nativeCreate(JNIEnv*,
                                                  jclass,
                                                  jint sample_rate_hz,
                                                  jint num_channels,
                                                  jboolean use_internal_vad,
                                                  jboolean enable_adaptive_digital,
                                                  jfloat adaptive_headroom_db,
                                                  jfloat adaptive_max_gain_db,
                                                  jfloat adaptive_initial_gain_db,
                                                  jfloat adaptive_max_gain_change_db_per_second,
                                                  jfloat adaptive_max_output_noise_level_dbfs,
                                                  jfloat fixed_gain_db) {
  Agc2Config config;
  config.sample_rate_hz = sample_rate_hz;
  config.num_channels = num_channels;
  config.use_internal_vad = use_internal_vad ? 1 : 0;
  config.enable_adaptive_digital = enable_adaptive_digital ? 1 : 0;
  config.adaptive_headroom_db = adaptive_headroom_db;
  config.adaptive_max_gain_db = adaptive_max_gain_db;
  config.adaptive_initial_gain_db = adaptive_initial_gain_db;
  config.adaptive_max_gain_change_db_per_second =
      adaptive_max_gain_change_db_per_second;
  config.adaptive_max_output_noise_level_dbfs =
      adaptive_max_output_noise_level_dbfs;
  config.fixed_gain_db = fixed_gain_db;

  Agc2Handle* handle = agc2_create(&config);
  return reinterpret_cast<jlong>(handle);
}

extern "C" JNIEXPORT void JNICALL
Java_org_webrtc_agc2_GainController2_nativeDestroy(JNIEnv*, jclass, jlong handle) {
  agc2_destroy(HandleFromJlong(handle));
}

extern "C" JNIEXPORT void JNICALL
Java_org_webrtc_agc2_GainController2_nativeSetFixedGainDb(JNIEnv*,
                                                          jclass,
                                                          jlong handle,
                                                          jfloat gain_db) {
  agc2_set_fixed_gain_db(HandleFromJlong(handle), gain_db);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_webrtc_agc2_GainController2_nativeProcess(JNIEnv* env,
                                                   jclass,
                                                   jlong handle,
                                                   jshortArray input_interleaved,
                                                   jshortArray output_interleaved,
                                                   jint num_frames,
                                                   jfloat speech_probability) {
  if (input_interleaved == nullptr || output_interleaved == nullptr) {
    return -1;
  }

  jshort* input_ptr = env->GetShortArrayElements(input_interleaved, nullptr);
  jshort* output_ptr = env->GetShortArrayElements(output_interleaved, nullptr);
  if (input_ptr == nullptr || output_ptr == nullptr) {
    if (input_ptr != nullptr) {
      env->ReleaseShortArrayElements(input_interleaved, input_ptr, JNI_ABORT);
    }
    if (output_ptr != nullptr) {
      env->ReleaseShortArrayElements(output_interleaved, output_ptr, JNI_ABORT);
    }
    return -1;
  }

  int result = agc2_process_int16(
      HandleFromJlong(handle),
      reinterpret_cast<const int16_t*>(input_ptr),
      reinterpret_cast<int16_t*>(output_ptr),
      num_frames,
      speech_probability);

  env->ReleaseShortArrayElements(input_interleaved, input_ptr, JNI_ABORT);
  env->ReleaseShortArrayElements(output_interleaved, output_ptr, 0);
  return result;
}
