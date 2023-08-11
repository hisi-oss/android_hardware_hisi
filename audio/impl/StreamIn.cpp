/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "StreamIn.h"

namespace android::hardware::audio::implementation {

StreamIn::StreamIn(sp<V2_0::IStreamIn> streamIn) : mStreamIn(streamIn), mStream(Stream(streamIn)) {}

// Methods from ::android::hardware::audio::V4_0::IStream follow.
Return<uint64_t> StreamIn::getFrameSize() {
    return mStream.getFrameSize();
}

Return<uint64_t> StreamIn::getFrameCount() {
    return mStream.getFrameCount();
}

Return<uint64_t> StreamIn::getBufferSize() {
    return mStream.getBufferSize();
}

Return<uint32_t> StreamIn::getSampleRate() {
    return mStream.getSampleRate();
}

Return<void> StreamIn::getSupportedSampleRates(common::V4_0::AudioFormat format,
                                               getSupportedSampleRates_cb _hidl_cb) {
    return mStream.getSupportedSampleRates(format, _hidl_cb);
}

Return<V4_0::Result> StreamIn::setSampleRate(uint32_t sampleRateHz) {
    return mStream.setSampleRate(sampleRateHz);
}

Return<hidl_bitfield<common::V4_0::AudioChannelMask>> StreamIn::getChannelMask() {
    return mStream.getChannelMask();
}

Return<void> StreamIn::getSupportedChannelMasks(common::V4_0::AudioFormat format,
                                                getSupportedChannelMasks_cb _hidl_cb) {
    return mStream.getSupportedChannelMasks(format, _hidl_cb);
}

Return<V4_0::Result> StreamIn::setChannelMask(hidl_bitfield<common::V4_0::AudioChannelMask> mask) {
    return mStream.setChannelMask(mask);
}

Return<common::V4_0::AudioFormat> StreamIn::getFormat() {
    return mStream.getFormat();
}

Return<void> StreamIn::getSupportedFormats(getSupportedFormats_cb _hidl_cb) {
    return mStream.getSupportedFormats(_hidl_cb);
}

Return<V4_0::Result> StreamIn::setFormat(common::V4_0::AudioFormat format) {
    return mStream.setFormat(format);
}

Return<void> StreamIn::getAudioProperties(getAudioProperties_cb _hidl_cb) {
    return mStream.getAudioProperties(_hidl_cb);
}

Return<V4_0::Result> StreamIn::addEffect(uint64_t effectId) {
    return mStream.addEffect(effectId);
}

Return<V4_0::Result> StreamIn::removeEffect(uint64_t effectId) {
    return mStream.removeEffect(effectId);
}

Return<V4_0::Result> StreamIn::standby() {
    return mStream.standby();
}

Return<void> StreamIn::getDevices(getDevices_cb _hidl_cb) {
    return mStream.getDevices(_hidl_cb);
}

Return<V4_0::Result> StreamIn::setDevices(const hidl_vec<V4_0::DeviceAddress>& devices) {
    return mStream.setDevices(devices);
}

Return<V4_0::Result> StreamIn::setHwAvSync(uint32_t hwAvSync) {
    return mStream.setHwAvSync(hwAvSync);
}

Return<void> StreamIn::getParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                     const hidl_vec<hidl_string>& keys, getParameters_cb _hidl_cb) {
    return mStream.getParameters(context, keys, _hidl_cb);
}

Return<V4_0::Result> StreamIn::setParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                             const hidl_vec<V4_0::ParameterValue>& parameters) {
    return mStream.setParameters(context, parameters);
}

Return<V4_0::Result> StreamIn::start() {
    return mStream.start();
}

Return<V4_0::Result> StreamIn::stop() {
    return mStream.stop();
}

Return<void> StreamIn::createMmapBuffer(int32_t minSizeFrames, createMmapBuffer_cb _hidl_cb) {
    return mStream.createMmapBuffer(minSizeFrames, _hidl_cb);
}

Return<void> StreamIn::getMmapPosition(getMmapPosition_cb _hidl_cb) {
    return mStream.getMmapPosition(_hidl_cb);
}

Return<V4_0::Result> StreamIn::close() {
    return mStream.close();
}

// Methods from ::android::hardware::audio::V4_0::IStreamIn follow.
Return<void> StreamIn::getAudioSource(getAudioSource_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<V4_0::Result> StreamIn::setGain(float gain) {
    // TODO implement
    return V4_0::Result{};
}

Return<void> StreamIn::updateSinkMetadata(const V4_0::SinkMetadata& sinkMetadata) {
    // TODO implement
    return Void();
}

Return<void> StreamIn::prepareForReading(uint32_t frameSize, uint32_t framesCount,
                                         prepareForReading_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<uint32_t> StreamIn::getInputFramesLost() {
    // TODO implement
    return uint32_t{};
}

Return<void> StreamIn::getCapturePosition(getCapturePosition_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<void> StreamIn::getActiveMicrophones(getActiveMicrophones_cb _hidl_cb) {
    // TODO implement
    return Void();
}

}  // namespace android::hardware::audio::implementation
