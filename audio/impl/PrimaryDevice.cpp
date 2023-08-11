/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PrimaryDevice.h"

#include <HidlUtils.h>
#include <hardware/audio.h>
#include "StreamIn.h"
#include "StreamOut.h"
#include "Utils.h"

namespace android::hardware::audio::implementation {

using ::android::hardware::audio::common::COMMON_TYPES_CPP_VERSION::implementation::HidlUtils;

PrimaryDevice::PrimaryDevice(sp<V2_0::IPrimaryDevice> device) : mDevice(device) {}

Return<V4_0::Result> PrimaryDevice::initCheck() {
    return convert(mDevice->initCheck());
}

Return<V4_0::Result> PrimaryDevice::setMasterVolume(float volume) {
    return convert(mDevice->setMasterVolume(volume));
}

Return<void> PrimaryDevice::getMasterVolume(getMasterVolume_cb _hidl_cb) {
    return mDevice->getMasterVolume(
            [&](V2_0::Result result, float volume) { _hidl_cb(convert(result), volume); });
}

Return<V4_0::Result> PrimaryDevice::setMicMute(bool mute) {
    return convert(mDevice->setMicMute(mute));
}

Return<void> PrimaryDevice::getMicMute(getMicMute_cb _hidl_cb) {
    return mDevice->getMicMute(
            [&](V2_0::Result result, bool mute) { _hidl_cb(convert(result), mute); });
}

Return<V4_0::Result> PrimaryDevice::setMasterMute(bool mute) {
    return convert(mDevice->setMasterMute(mute));
}

Return<void> PrimaryDevice::getMasterMute(getMasterMute_cb _hidl_cb) {
    return mDevice->getMasterMute(
            [&](V2_0::Result result, bool mute) { _hidl_cb(convert(result), mute); });
}

Return<void> PrimaryDevice::getInputBufferSize(const common::V4_0::AudioConfig& config,
                                               getInputBufferSize_cb _hidl_cb) {
    return mDevice->getInputBufferSize(convert(config),
                                       [&](V2_0::Result result, uint64_t bufferSize) {
                                           _hidl_cb(convert(result), bufferSize);
                                       });
}

Return<void> PrimaryDevice::openOutputStream(int32_t ioHandle, const V4_0::DeviceAddress& device,
                                             const common::V4_0::AudioConfig& config,
                                             hidl_bitfield<common::V4_0::AudioOutputFlag> flags,
                                             const V4_0::SourceMetadata& /*sourceMetadata*/,
                                             openOutputStream_cb _hidl_cb) {
    return mDevice->openOutputStream(
            ioHandle, convert(device), convert(config),
            convert(DOWNCAST_HIDL_BITFIELD(common::V4_0::AudioOutputFlag, flags)),
            [&](V2_0::Result result, const sp<V2_0::IStreamOut>& stream,
                const common::V2_0::AudioConfig& suggestedConfig) {
                _hidl_cb(convert(result), new StreamOut(stream), convert(suggestedConfig));
            });
}

Return<void> PrimaryDevice::openInputStream(int32_t ioHandle, const V4_0::DeviceAddress& device,
                                            const common::V4_0::AudioConfig& config,
                                            hidl_bitfield<common::V4_0::AudioInputFlag> flags,
                                            const V4_0::SinkMetadata& sinkMetadata,
                                            openInputStream_cb _hidl_cb) {
    common::V4_0::AudioSource source = common::V4_0::AudioSource::DEFAULT;
    if (sinkMetadata.tracks.size() > 0) {
        source = sinkMetadata.tracks[0].source;
    }

    return mDevice->openInputStream(
            ioHandle, convert(device), convert(config),
            convert(DOWNCAST_HIDL_BITFIELD(common::V4_0::AudioInputFlag, flags)), convert(source),
            [&](V2_0::Result result, const sp<V2_0::IStreamIn>& stream,
                const common::V2_0::AudioConfig& suggestedConfig) {
                _hidl_cb(convert(result), new StreamIn(stream), convert(suggestedConfig));
            });
}

Return<bool> PrimaryDevice::supportsAudioPatches() {
    return mDevice->supportsAudioPatches();
}

Return<void> PrimaryDevice::createAudioPatch(const hidl_vec<common::V4_0::AudioPortConfig>& sources,
                                             const hidl_vec<common::V4_0::AudioPortConfig>& sinks,
                                             createAudioPatch_cb _hidl_cb) {
    return mDevice->createAudioPatch(
            convert(sources), convert(sinks),
            [&](V2_0::Result result, int32_t patch) { _hidl_cb(convert(result), patch); });
}

Return<V4_0::Result> PrimaryDevice::releaseAudioPatch(int32_t patch) {
    return convert(mDevice->releaseAudioPatch(patch));
}

Return<void> PrimaryDevice::getAudioPort(const common::V4_0::AudioPort& port,
                                         getAudioPort_cb _hidl_cb) {
    return mDevice->getAudioPort(convert(port),
                                 [&](V2_0::Result result, const common::V2_0::AudioPort& port) {
                                     _hidl_cb(convert(result), convert(port));
                                 });
}

Return<V4_0::Result> PrimaryDevice::setAudioPortConfig(
        const common::V4_0::AudioPortConfig& config) {
    return convert(mDevice->setAudioPortConfig(convert(config)));
}

Return<V4_0::Result> PrimaryDevice::setScreenState(bool turnedOn) {
    return convert(mDevice->setScreenState(turnedOn));
}

Return<void> PrimaryDevice::getHwAvSync(getHwAvSync_cb _hidl_cb) {
    _hidl_cb(V4_0::Result::OK, mDevice->getHwAvSync());
    return Void();
}

Return<void> PrimaryDevice::getParameters(const hidl_vec<V4_0::ParameterValue>& /*context*/,
                                          const hidl_vec<hidl_string>& keys,
                                          getParameters_cb _hidl_cb) {
    return mDevice->getParameters(
            keys, [&](V2_0::Result result, const hidl_vec<V2_0::ParameterValue>& parameters) {
                _hidl_cb(convert(result), convert(parameters));
            });
}

Return<V4_0::Result> PrimaryDevice::setParameters(
        const hidl_vec<V4_0::ParameterValue>& /*context*/,
        const hidl_vec<V4_0::ParameterValue>& parameters) {
    return convert(mDevice->setParameters(convert(parameters)));
}

Return<void> PrimaryDevice::getMicrophones(getMicrophones_cb _hidl_cb) {
    // return mDevice->getMicrophones(_hidl_cb);
    V4_0::Result retval = V4_0::Result::NOT_SUPPORTED;
    hidl_vec<V4_0::MicrophoneInfo> microphones;

    _hidl_cb(retval, microphones);
    return Void();
}

Return<V4_0::Result> PrimaryDevice::setConnectedState(const V4_0::DeviceAddress& address,
                                                      bool connected) {
    return setParameter(
            connected ? AUDIO_PARAMETER_DEVICE_CONNECT : AUDIO_PARAMETER_DEVICE_DISCONNECT,
            address);
}

Return<void> PrimaryDevice::debug(const hidl_handle& fd, const hidl_vec<hidl_string>& options) {
    return mDevice->debug(fd, options);
}

Return<V4_0::Result> PrimaryDevice::setVoiceVolume(float volume) {
    return convert(mDevice->setVoiceVolume(volume));
}

Return<V4_0::Result> PrimaryDevice::setMode(common::V4_0::AudioMode mode) {
    return convert(mDevice->setMode(convert(mode)));
}

Return<void> PrimaryDevice::getBtScoNrecEnabled(getBtScoNrecEnabled_cb _hidl_cb) {
    return mDevice->getBtScoNrecEnabled(
            [&](V2_0::Result result, bool enabled) { _hidl_cb(convert(result), enabled); });
}

Return<V4_0::Result> PrimaryDevice::setBtScoNrecEnabled(bool enabled) {
    return convert(mDevice->setBtScoNrecEnabled(enabled));
}

Return<void> PrimaryDevice::getBtScoWidebandEnabled(getBtScoWidebandEnabled_cb _hidl_cb) {
    return mDevice->getBtScoWidebandEnabled(
            [&](V2_0::Result result, bool enabled) { _hidl_cb(convert(result), enabled); });
}

Return<V4_0::Result> PrimaryDevice::setBtScoWidebandEnabled(bool enabled) {
    return convert(mDevice->setBtScoWidebandEnabled(enabled));
}

Return<void> PrimaryDevice::getTtyMode(getTtyMode_cb _hidl_cb) {
    return mDevice->getTtyMode([&](V2_0::Result result, V2_0::IPrimaryDevice::TtyMode mode) {
        _hidl_cb(convert(result), convert(mode));
    });
}

Return<V4_0::Result> PrimaryDevice::setTtyMode(V4_0::IPrimaryDevice::TtyMode mode) {
    return convert(mDevice->setTtyMode(convert(mode)));
}

Return<void> PrimaryDevice::getHacEnabled(getHacEnabled_cb _hidl_cb) {
    return mDevice->getHacEnabled(
            [&](V2_0::Result result, bool enabled) { _hidl_cb(convert(result), enabled); });
}

Return<V4_0::Result> PrimaryDevice::setHacEnabled(bool enabled) {
    return convert(mDevice->setHacEnabled(enabled));
}

Return<V4_0::Result> PrimaryDevice::setBtScoHeadsetDebugName(const hidl_string& name) {
    return setParameter(AUDIO_PARAMETER_KEY_BT_SCO_HEADSET_NAME, name.c_str());
}

Return<void> PrimaryDevice::getBtHfpEnabled(getBtHfpEnabled_cb _hidl_cb) {
    // return mDevice->getBtHfpEnabled(_hidl_cb);
    //  TODO
    _hidl_cb(V4_0::Result::NOT_SUPPORTED, false);
    return Void();
}

Return<V4_0::Result> PrimaryDevice::setBtHfpEnabled(bool enabled) {
    return setParameter(AUDIO_PARAMETER_KEY_HFP_ENABLE, enabled);
}

Return<V4_0::Result> PrimaryDevice::setBtHfpSampleRate(uint32_t sampleRateHz) {
    return setParameter(AUDIO_PARAMETER_KEY_HFP_SET_SAMPLING_RATE, int(sampleRateHz));
}

Return<V4_0::Result> PrimaryDevice::setBtHfpVolume(float volume) {
    if (!isGainNormalized(volume)) {
        return V4_0::Result::INVALID_ARGUMENTS;
    }
    // Map the normalized volume onto the range of [0, 15]
    return setParameter(AUDIO_PARAMETER_KEY_HFP_VOLUME, static_cast<int>(std::round(volume * 15)));
}

Return<V4_0::Result> PrimaryDevice::updateRotation(V4_0::IPrimaryDevice::Rotation rotation) {
    // legacy API expects the rotation in degree
    return setParameter(AUDIO_PARAMETER_KEY_ROTATION, static_cast<int>(rotation) * 90);
}

// Utils

V4_0::Result PrimaryDevice::setParameter(const char* name, const char* value) {
    return setParameters({{hidl_string(name), hidl_string(value)}});
}

V4_0::Result PrimaryDevice::setParameter(const char* name, bool value) {
    return setParameters({{name, value ? AUDIO_PARAMETER_VALUE_ON : AUDIO_PARAMETER_VALUE_OFF}});
}

V4_0::Result PrimaryDevice::setParameter(const char* name, int value) {
    return setParameters({{name, std::to_string(value)}});
}

V4_0::Result PrimaryDevice::setParameter(const char* name, float value) {
    return setParameters({{name, std::to_string(value)}});
}

V4_0::Result PrimaryDevice::setParameter(const char* name, const V4_0::DeviceAddress& address) {
    audio_devices_t halDeviceType;
    char halDeviceAddress[AUDIO_DEVICE_MAX_ADDRESS_LEN];

    if (HidlUtils::deviceAddressToHalImpl(address, &halDeviceType, halDeviceAddress) != NO_ERROR) {
        return V4_0::Result::INVALID_ARGUMENTS;
    }

    // TODO: halDeviceAddress split and add
    return setParameters({{name, std::to_string(halDeviceType)}});
}

Return<void> PrimaryDevice::getParameters(const hidl_vec<hidl_string>& keys,
                                          getParameters_cb _hidl_cb) {
    return getParameters(hidl_vec<V4_0::ParameterValue>(), keys, _hidl_cb);
}

Return<V4_0::Result> PrimaryDevice::setParameters(
        const hidl_vec<V4_0::ParameterValue>& parameters) {
    return setParameters(hidl_vec<V4_0::ParameterValue>(), parameters);
}

}  // namespace android::hardware::audio::implementation
