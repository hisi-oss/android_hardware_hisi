/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android/hardware/audio/2.0/IPrimaryDevice.h>
#include <android/hardware/audio/4.0/IPrimaryDevice.h>
#include <android/hardware/audio/4.0/types.h>
#include <android/hardware/audio/common/4.0/types.h>
#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <cmath>

namespace android {
namespace hardware {
namespace audio {
namespace implementation {

using ::android::sp;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct PrimaryDevice : public V4_0::IPrimaryDevice {
    explicit PrimaryDevice(sp<V2_0::IPrimaryDevice> device);

    // Methods from ::android::hardware::audio::V4_0::IDevice follow.
    Return<V4_0::Result> initCheck() override;
    Return<V4_0::Result> setMasterVolume(float volume) override;
    Return<void> getMasterVolume(getMasterVolume_cb _hidl_cb) override;
    Return<V4_0::Result> setMicMute(bool mute) override;
    Return<void> getMicMute(getMicMute_cb _hidl_cb) override;
    Return<V4_0::Result> setMasterMute(bool mute) override;
    Return<void> getMasterMute(getMasterMute_cb _hidl_cb) override;
    Return<void> getInputBufferSize(const common::V4_0::AudioConfig& config,
                                    getInputBufferSize_cb _hidl_cb) override;

    Return<void> openOutputStream(int32_t ioHandle, const V4_0::DeviceAddress& device,
                                  const common::V4_0::AudioConfig& config,
                                  hidl_bitfield<common::V4_0::AudioOutputFlag> flags,
                                  const V4_0::SourceMetadata& sourceMetadata,
                                  openOutputStream_cb _hidl_cb) override;
    Return<void> openInputStream(int32_t ioHandle, const V4_0::DeviceAddress& device,
                                 const common::V4_0::AudioConfig& config,
                                 hidl_bitfield<common::V4_0::AudioInputFlag> flags,
                                 const V4_0::SinkMetadata& sinkMetadata,
                                 openInputStream_cb _hidl_cb) override;

    Return<bool> supportsAudioPatches() override;
    Return<void> createAudioPatch(const hidl_vec<common::V4_0::AudioPortConfig>& sources,
                                  const hidl_vec<common::V4_0::AudioPortConfig>& sinks,
                                  createAudioPatch_cb _hidl_cb) override;
    Return<V4_0::Result> releaseAudioPatch(int32_t patch) override;
    Return<void> getAudioPort(const common::V4_0::AudioPort& port,
                              getAudioPort_cb _hidl_cb) override;
    Return<V4_0::Result> setAudioPortConfig(const common::V4_0::AudioPortConfig& config) override;

    Return<V4_0::Result> setScreenState(bool turnedOn) override;

    Return<void> getHwAvSync(getHwAvSync_cb _hidl_cb) override;
    Return<void> getParameters(const hidl_vec<V4_0::ParameterValue>& context,
                               const hidl_vec<hidl_string>& keys,
                               getParameters_cb _hidl_cb) override;
    Return<V4_0::Result> setParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                       const hidl_vec<V4_0::ParameterValue>& parameters) override;
    Return<void> getMicrophones(getMicrophones_cb _hidl_cb) override;
    Return<V4_0::Result> setConnectedState(const V4_0::DeviceAddress& address,
                                           bool connected) override;

    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& options) override;

    // Methods from ::android::hardware::audio::V4_0::IPrimaryDevice follow.
    Return<V4_0::Result> setVoiceVolume(float volume) override;
    Return<V4_0::Result> setMode(common::V4_0::AudioMode mode) override;
    Return<void> getBtScoNrecEnabled(getBtScoNrecEnabled_cb _hidl_cb) override;
    Return<V4_0::Result> setBtScoNrecEnabled(bool enabled) override;
    Return<void> getBtScoWidebandEnabled(getBtScoWidebandEnabled_cb _hidl_cb) override;
    Return<V4_0::Result> setBtScoWidebandEnabled(bool enabled) override;
    Return<void> getTtyMode(getTtyMode_cb _hidl_cb) override;
    Return<V4_0::Result> setTtyMode(V4_0::IPrimaryDevice::TtyMode mode) override;
    Return<void> getHacEnabled(getHacEnabled_cb _hidl_cb) override;
    Return<V4_0::Result> setHacEnabled(bool enabled) override;

    Return<V4_0::Result> setBtScoHeadsetDebugName(const hidl_string& name) override;
    Return<void> getBtHfpEnabled(getBtHfpEnabled_cb _hidl_cb) override;
    Return<V4_0::Result> setBtHfpEnabled(bool enabled) override;
    Return<V4_0::Result> setBtHfpSampleRate(uint32_t sampleRateHz) override;
    Return<V4_0::Result> setBtHfpVolume(float volume) override;
    Return<V4_0::Result> updateRotation(V4_0::IPrimaryDevice::Rotation rotation) override;

  private:
    sp<V2_0::IPrimaryDevice> mDevice;

    V4_0::Result setParameter(const char* name, const char* value);
    V4_0::Result setParameter(const char* name, bool value);
    V4_0::Result setParameter(const char* name, int value);
    V4_0::Result setParameter(const char* name, float value);
    V4_0::Result setParameter(const char* name, const V4_0::DeviceAddress& address);

    Return<void> getParameters(const hidl_vec<hidl_string>& keys, getParameters_cb _hidl_cb);
    Return<V4_0::Result> setParameters(const hidl_vec<V4_0::ParameterValue>& parameters);
};

}  // namespace implementation
}  // namespace audio
}  // namespace hardware
}  // namespace android
