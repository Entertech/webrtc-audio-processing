package org.webrtc.agc2;

public final class GainController2 implements AutoCloseable {
  public static final class Config {
    public int sampleRateHz;
    public int numChannels;
    public boolean useInternalVad = true;
    public boolean enableAdaptiveDigital = true;
    public float adaptiveHeadroomDb = 5.0f;
    public float adaptiveMaxGainDb = 50.0f;
    public float adaptiveInitialGainDb = 15.0f;
    public float adaptiveMaxGainChangeDbPerSecond = 6.0f;
    public float adaptiveMaxOutputNoiseLevelDbfs = -50.0f;
    public float fixedGainDb;

    public Config(int sampleRateHz, int numChannels) {
      this.sampleRateHz = sampleRateHz;
      this.numChannels = numChannels;
    }
  }

  private long nativeHandle;
  private final int numChannels;

  public static int framesPer10ms(int sampleRateHz) {
    return nativeFramesPer10ms(sampleRateHz);
  }

  public static GainController2 create(Config config) {
    long handle = nativeCreate(
        config.sampleRateHz,
        config.numChannels,
        config.useInternalVad,
        config.enableAdaptiveDigital,
        config.adaptiveHeadroomDb,
        config.adaptiveMaxGainDb,
        config.adaptiveInitialGainDb,
        config.adaptiveMaxGainChangeDbPerSecond,
        config.adaptiveMaxOutputNoiseLevelDbfs,
        config.fixedGainDb);
    if (handle == 0) {
      throw new IllegalStateException("Failed to create AGC2");
    }
    return new GainController2(handle, config.numChannels);
  }

  private GainController2(long nativeHandle, int numChannels) {
    this.nativeHandle = nativeHandle;
    this.numChannels = numChannels;
  }

  public void setFixedGainDb(float gainDb) {
    checkHandle();
    nativeSetFixedGainDb(nativeHandle, gainDb);
  }

  public int process(short[] inputInterleaved, short[] outputInterleaved, float speechProbability) {
    checkHandle();
    if (inputInterleaved == null || outputInterleaved == null) {
      throw new IllegalArgumentException("input/output must be non-null");
    }
    if (inputInterleaved.length != outputInterleaved.length) {
      throw new IllegalArgumentException("input/output length mismatch");
    }
    int frames = inputInterleaved.length / numChannels;
    if (frames * numChannels != inputInterleaved.length) {
      throw new IllegalArgumentException("input length not divisible by channels");
    }
    return nativeProcess(nativeHandle, inputInterleaved, outputInterleaved, frames, speechProbability);
  }

  @Override
  public void close() {
    if (nativeHandle != 0) {
      nativeDestroy(nativeHandle);
      nativeHandle = 0;
    }
  }

  private void checkHandle() {
    if (nativeHandle == 0) {
      throw new IllegalStateException("AGC2 instance is closed");
    }
  }

  private static native int nativeFramesPer10ms(int sampleRateHz);
  private static native long nativeCreate(
      int sampleRateHz,
      int numChannels,
      boolean useInternalVad,
      boolean enableAdaptiveDigital,
      float adaptiveHeadroomDb,
      float adaptiveMaxGainDb,
      float adaptiveInitialGainDb,
      float adaptiveMaxGainChangeDbPerSecond,
      float adaptiveMaxOutputNoiseLevelDbfs,
      float fixedGainDb);
  private static native void nativeDestroy(long handle);
  private static native void nativeSetFixedGainDb(long handle, float gainDb);
  private static native int nativeProcess(
      long handle,
      short[] inputInterleaved,
      short[] outputInterleaved,
      int numFrames,
      float speechProbability);
}
