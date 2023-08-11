/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Stream.h"

#include "Utils.h"

namespace android::hardware::audio::implementation {

Stream::Stream(sp<V2_0::IStream> stream) : mStream(stream) {}

Return<uint64_t> Stream::getFrameSize() {
    return mStream->getFrameSize();
}

Return<uint64_t> Stream::getFrameCount() {
    return mStream->getFrameCount();
}

Return<uint64_t> Stream::getBufferSize() {
    return mStream->getBufferSize();
}

Return<uint32_t> Stream::getSampleRate() {
    return mStream->getSampleRate();
}

Return<void> Stream::getSupportedSampleRates(common::V4_0::AudioFormat /*format*/,
                                             getSupportedSampleRates_cb _hidl_cb) {
    return mStream->getSupportedSampleRates(
            [&](const hidl_vec<uint32_t>& ratesHz) { _hidl_cb(V4_0::Result::OK, ratesHz); });
}

Return<V4_0::Result> Stream::setSampleRate(uint32_t sampleRateHz) {
    return convert(mStream->setSampleRate(sampleRateHz));
}

Return<hidl_bitfield<common::V4_0::AudioChannelMask>> Stream::getChannelMask() {
    return TO_HIDL_BITFIELD(common::V4_0::AudioChannelMask, convert(mStream->getChannelMask()));
}

Return<void> Stream::getSupportedChannelMasks(common::V4_0::AudioFormat /*format*/,
                                              getSupportedChannelMasks_cb _hidl_cb) {
    return mStream->getSupportedChannelMasks(
            [&](const hidl_vec<common::V2_0::AudioChannelMask>& masks) {
                _hidl_cb(V4_0::Result::OK, convert(masks));
            });
}

Return<V4_0::Result> Stream::setChannelMask(hidl_bitfield<common::V4_0::AudioChannelMask> mask) {
    return convert(mStream->setChannelMask(
            convert(DOWNCAST_HIDL_BITFIELD(common::V4_0::AudioChannelMask, mask))));
}

Return<common::V4_0::AudioFormat> Stream::getFormat() {
    return convert(mStream->getFormat());
}

Return<void> Stream::getSupportedFormats(getSupportedFormats_cb _hidl_cb) {
    return mStream->getSupportedFormats([&](const hidl_vec<common::V2_0::AudioFormat>& formats) {
        _hidl_cb(convert(formats));
    });
}

Return<V4_0::Result> Stream::setFormat(common::V4_0::AudioFormat format) {
    return convert(mStream->setFormat(convert(format)));
}

Return<void> Stream::getAudioProperties(getAudioProperties_cb _hidl_cb) {
    return mStream->getAudioProperties([&](uint32_t sampleRateHz,
                                           common::V2_0::AudioChannelMask mask,
                                           common::V2_0::AudioFormat format) {
        _hidl_cb(sampleRateHz, TO_HIDL_BITFIELD(common::V4_0::AudioChannelMask, convert(mask)),
                 convert(format));
    });
}

Return<V4_0::Result> Stream::addEffect(uint64_t effectId) {
    return convert(mStream->addEffect(effectId));
}

Return<V4_0::Result> Stream::removeEffect(uint64_t effectId) {
    return convert(mStream->removeEffect(effectId));
}

Return<V4_0::Result> Stream::standby() {
    return convert(mStream->standby());
}

Return<void> Stream::getDevices(getDevices_cb _hidl_cb) {
    V2_0::AudioDevice audioDevice = mStream->getDevice();

    V4_0::DeviceAddress deviceAddress{};
    deviceAddress.device = audioDevice;

    hidl_vec<V4_0::DeviceAddress> devices;
    devices.resize(1);
    devices[0] = deviceAddress;

    _hidl_cb(V4_0::Result::OK, devices);

    return Void();
}

Return<V4_0::Result> Stream::setDevices(const hidl_vec<V4_0::DeviceAddress>& devices) {
    if (devices.size() > 1) {
        return Result::NOT_SUPPORTED;
    }

    V4_0::DeviceAddress address{};
    if (devices.size() == 1) {
        address = devices[0];
    }

    return convert(mStream->setDevice(address));
}

Return<V4_0::Result> Stream::setHwAvSync(uint32_t hwAvSync) {
    return convert(mStream->setHwAvSync(hwAvSync));
}

Return<void> Stream::getParameters(const hidl_vec<V4_0::ParameterValue>& /*context*/,
                                   const hidl_vec<hidl_string>& keys, getParameters_cb _hidl_cb) {
    return mStream->getParameters(
            keys, [&](V2_0::Result result, const hidl_vec<V2_0::ParameterValue>& parameters) {
                _hidl_cb(convert(result), convert(parameters));
            });
}

Return<V4_0::Result> Stream::setParameters(const hidl_vec<V4_0::ParameterValue>& /*context*/,
                                           const hidl_vec<V4_0::ParameterValue>& parameters) {
    return convert(mStream->setParameters(convert(parameters)));
}

Return<V4_0::Result> Stream::start() {
    return convert(mStream->start());
}

Return<V4_0::Result> Stream::stop() {
    return convert(mStream->stop());
}

Return<void> Stream::createMmapBuffer(int32_t minSizeFrames, createMmapBuffer_cb _hidl_cb) {
    return mStream->createMmapBuffer(minSizeFrames, _hidl_cb);
}

Return<void> Stream::getMmapPosition(getMmapPosition_cb _hidl_cb) {
    return mStream->getMmapPosition([&](V2_0::Result result, V2_0::MmapPosition position) {
        _hidl_cb(convert(result), convert(position));
    });
}

Return<V4_0::Result> Stream::close() {
    return convert(mStream->close());
}

}  // namespace android::hardware::audio::implementation
