/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "StreamOut.h"

namespace android::hardware::audio::implementation {

StreamOut::StreamOut(sp<V2_0::IStreamOut> streamOut)
    : mStreamOut(streamOut), mStream(Stream(streamOut)) {}

// Methods from ::android::hardware::audio::V4_0::IStream follow.
Return<uint64_t> StreamOut::getFrameSize() {
    return mStream.getFrameSize();
}

Return<uint64_t> StreamOut::getFrameCount() {
    return mStream.getFrameCount();
}

Return<uint64_t> StreamOut::getBufferSize() {
    return mStream.getBufferSize();
}

Return<uint32_t> StreamOut::getSampleRate() {
    return mStream.getSampleRate();
}

Return<void> StreamOut::getSupportedSampleRates(common::V4_0::AudioFormat format,
                                                getSupportedSampleRates_cb _hidl_cb) {
    return mStream.getSupportedSampleRates(format, _hidl_cb);
}

Return<V4_0::Result> StreamOut::setSampleRate(uint32_t sampleRateHz) {
    return mStream.setSampleRate(sampleRateHz);
}

Return<hidl_bitfield<common::V4_0::AudioChannelMask>> StreamOut::getChannelMask() {
    return mStream.getChannelMask();
}

Return<void> StreamOut::getSupportedChannelMasks(common::V4_0::AudioFormat format,
                                                 getSupportedChannelMasks_cb _hidl_cb) {
    return mStream.getSupportedChannelMasks(format, _hidl_cb);
}

Return<V4_0::Result> StreamOut::setChannelMask(hidl_bitfield<common::V4_0::AudioChannelMask> mask) {
    return mStream.setChannelMask(mask);
}

Return<common::V4_0::AudioFormat> StreamOut::getFormat() {
    return mStream.getFormat();
}

Return<void> StreamOut::getSupportedFormats(getSupportedFormats_cb _hidl_cb) {
    return mStream.getSupportedFormats(_hidl_cb);
}

Return<V4_0::Result> StreamOut::setFormat(common::V4_0::AudioFormat format) {
    return mStream.setFormat(format);
}

Return<void> StreamOut::getAudioProperties(getAudioProperties_cb _hidl_cb) {
    return mStream.getAudioProperties(_hidl_cb);
}

Return<V4_0::Result> StreamOut::addEffect(uint64_t effectId) {
    return mStream.addEffect(effectId);
}

Return<V4_0::Result> StreamOut::removeEffect(uint64_t effectId) {
    return mStream.removeEffect(effectId);
}

Return<V4_0::Result> StreamOut::standby() {
    return mStream.standby();
}

Return<void> StreamOut::getDevices(getDevices_cb _hidl_cb) {
    return mStream.getDevices(_hidl_cb);
}

Return<V4_0::Result> StreamOut::setDevices(const hidl_vec<V4_0::DeviceAddress>& devices) {
    return mStream.setDevices(devices);
}

Return<V4_0::Result> StreamOut::setHwAvSync(uint32_t hwAvSync) {
    return mStream.setHwAvSync(hwAvSync);
}

Return<void> StreamOut::getParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                      const hidl_vec<hidl_string>& keys,
                                      getParameters_cb _hidl_cb) {
    return mStream.getParameters(context, keys, _hidl_cb);
}

Return<V4_0::Result> StreamOut::setParameters(const hidl_vec<V4_0::ParameterValue>& context,
                                              const hidl_vec<V4_0::ParameterValue>& parameters) {
    return mStream.setParameters(context, parameters);
}

Return<V4_0::Result> StreamOut::start() {
    return mStream.start();
}

Return<V4_0::Result> StreamOut::stop() {
    return mStream.stop();
}

Return<void> StreamOut::createMmapBuffer(int32_t minSizeFrames, createMmapBuffer_cb _hidl_cb) {
    return mStream.createMmapBuffer(minSizeFrames, _hidl_cb);
}

Return<void> StreamOut::getMmapPosition(getMmapPosition_cb _hidl_cb) {
    return mStream.getMmapPosition(_hidl_cb);
}

Return<V4_0::Result> StreamOut::close() {
    return mStream.close();
}

// Methods from ::android::hardware::audio::V4_0::IStreamOut follow.
Return<uint32_t> StreamOut::getLatency() {
    // TODO implement
    return uint32_t{};
}

Return<V4_0::Result> StreamOut::setVolume(float left, float right) {
    // TODO implement
    return V4_0::Result{};
}

Return<void> StreamOut::updateSourceMetadata(const V4_0::SourceMetadata& sourceMetadata) {
    // TODO implement
    return Void();
}

Return<void> StreamOut::prepareForWriting(uint32_t frameSize, uint32_t framesCount,
                                          prepareForWriting_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<void> StreamOut::getRenderPosition(getRenderPosition_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<void> StreamOut::getNextWriteTimestamp(getNextWriteTimestamp_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<V4_0::Result> StreamOut::setCallback(const sp<V4_0::IStreamOutCallback>& callback) {
    // TODO implement
    return V4_0::Result{};
}

Return<V4_0::Result> StreamOut::clearCallback() {
    // TODO implement
    return V4_0::Result{};
}

Return<void> StreamOut::supportsPauseAndResume(supportsPauseAndResume_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<V4_0::Result> StreamOut::pause() {
    // TODO implement
    return V4_0::Result{};
}

Return<V4_0::Result> StreamOut::resume() {
    // TODO implement
    return V4_0::Result{};
}

Return<bool> StreamOut::supportsDrain() {
    // TODO implement
    return bool{};
}

Return<V4_0::Result> StreamOut::drain(V4_0::AudioDrain type) {
    // TODO implement
    return V4_0::Result{};
}

Return<V4_0::Result> StreamOut::flush() {
    // TODO implement
    return V4_0::Result{};
}

Return<void> StreamOut::getPresentationPosition(getPresentationPosition_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<V4_0::Result> StreamOut::selectPresentation(int32_t presentationId, int32_t programId) {
    // TODO implement
    return V4_0::Result{};
}

}  // namespace android::hardware::audio::implementation
