/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android/hardware/audio/2.0/IStream.h>
#include <android/hardware/audio/4.0/IStream.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android::hardware::audio::implementation {

using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct Stream : public V4_0::IStream {
  public:
    explicit Stream(sp<V2_0::IStream> stream);

    // Methods from ::android::hardware::audio::V4_0::IStream follow.
    Return<uint64_t> getFrameSize() override;
    Return<uint64_t> getFrameCount() override;
    Return<uint64_t> getBufferSize() override;
    Return<uint32_t> getSampleRate() override;
    Return<void> getSupportedSampleRates(common::V4_0::AudioFormat format,
                                         getSupportedSampleRates_cb _hidl_cb) override;
    Return<V4_0::Result> setSampleRate(uint32_t sampleRateHz) override;
    Return<hidl_bitfield<common::V4_0::AudioChannelMask>> getChannelMask() override;
    Return<void> getSupportedChannelMasks(common::V4_0::AudioFormat format,
                                          getSupportedChannelMasks_cb _hidl_cb) override;
    Return<V4_0::Result> setChannelMask(
            hidl_bitfield<common::V4_0::AudioChannelMask> mask) override;
    Return<common::V4_0::AudioFormat> getFormat() override;
    Return<void> getSupportedFormats(getSupportedFormats_cb _hidl_cb) override;
    Return<V4_0::Result> setFormat(common::V4_0::AudioFormat format) override;
    Return<void> getAudioProperties(getAudioProperties_cb _hidl_cb) override;
    Return<V4_0::Result> addEffect(uint64_t effectId) override;
    Return<V4_0::Result> removeEffect(uint64_t effectId) override;
    Return<V4_0::Result> standby() override;
    Return<void> getDevices(getDevices_cb _hidl_cb) override;
    Return<V4_0::Result> setDevices(const hidl_vec<V4_0::DeviceAddress>& devices) override;
    Return<V4_0::Result> setHwAvSync(uint32_t hwAvSync) override;
    Return<void> getParameters(const hidl_vec<V4_0::ParameterValue>& context,
                               const hidl_vec<hidl_string>& keys,
                               getParameters_cb _hidl_cb) override;
    Return<V4_0::Result> setParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                       const hidl_vec<V4_0::ParameterValue>& parameters) override;
    Return<V4_0::Result> start() override;
    Return<V4_0::Result> stop() override;
    Return<void> createMmapBuffer(int32_t minSizeFrames, createMmapBuffer_cb _hidl_cb) override;
    Return<void> getMmapPosition(getMmapPosition_cb _hidl_cb) override;
    Return<V4_0::Result> close() override;

  private:
    sp<V2_0::IStream> mStream;
};

}  // namespace android::hardware::audio::implementation
