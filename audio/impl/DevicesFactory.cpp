/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DevicesFactory.h"

#include "PrimaryDevice.h"

namespace android::hardware::audio::implementation {

DevicesFactory::DevicesFactory()
    : mV2_0DevicesFactory(V2_0::IDevicesFactory::getService()),
      mAOSPDevicesFactory(AOSPDevicesFactory()) {}

Return<void> DevicesFactory::openDevice(const hidl_string& device, openDevice_cb _hidl_cb) {
    if (device == AUDIO_HARDWARE_MODULE_ID_PRIMARY) {
        sp<V2_0::IDevice> V2_0Device = openDevice(device);
        sp<V2_0::IPrimaryDevice> V2_0PrimaryDevice = V2_0::IPrimaryDevice::castFrom(V2_0Device);

        _hidl_cb(V2_0PrimaryDevice ? V4_0::Result::OK : V4_0::Result::INVALID_ARGUMENTS,
                 V2_0PrimaryDevice ? new PrimaryDevice(V2_0PrimaryDevice) : nullptr);

        return Void();
    }

    return mAOSPDevicesFactory.openDevice(device, _hidl_cb);
}

Return<void> DevicesFactory::openPrimaryDevice(openPrimaryDevice_cb _hidl_cb) {
    sp<V2_0::IDevice> V2_0Device = openDevice(AUDIO_HARDWARE_MODULE_ID_PRIMARY);
    sp<V2_0::IPrimaryDevice> V2_0PrimaryDevice = V2_0::IPrimaryDevice::castFrom(V2_0Device);

    _hidl_cb(V2_0PrimaryDevice ? V4_0::Result::OK : V4_0::Result::INVALID_ARGUMENTS,
             V2_0PrimaryDevice ? new PrimaryDevice(V2_0PrimaryDevice) : nullptr);

    return Void();
}

static V2_0::IDevicesFactory::Device hwModuleToV2_0Device(const std::string& name) {
    if (name == AUDIO_HARDWARE_MODULE_ID_PRIMARY) return V2_0::IDevicesFactory::Device::PRIMARY;
    if (name == AUDIO_HARDWARE_MODULE_ID_A2DP) return V2_0::IDevicesFactory::Device::A2DP;
    if (name == AUDIO_HARDWARE_MODULE_ID_USB) return V2_0::IDevicesFactory::Device::USB;
    if (name == AUDIO_HARDWARE_MODULE_ID_REMOTE_SUBMIX)
        return V2_0::IDevicesFactory::Device::R_SUBMIX;
    if (name == AUDIO_HARDWARE_MODULE_ID_STUB) return V2_0::IDevicesFactory::Device::STUB;

    return V2_0::IDevicesFactory::Device::PRIMARY;
}

sp<V2_0::IDevice> DevicesFactory::openDevice(const std::string& name) {
    V2_0::Result result;
    sp<V2_0::IDevice> device;

    V2_0::IDevicesFactory::Device deviceType = hwModuleToV2_0Device(name);

    if (mV2_0DevicesFactory) {
        auto ret = mV2_0DevicesFactory->openDevice(
                deviceType, [&result, &device](V2_0::Result ret, sp<V2_0::IDevice> dev) {
                    result = ret;
                    device = dev;
                });

        if (ret.isOk() && result == V2_0::Result::OK && device != nullptr) {
            return device;
        }
    }

    return nullptr;
}

V4_0::IDevicesFactory* HIDL_FETCH_IDevicesFactory(const char* /* name */) {
    return new DevicesFactory();
}

}  // namespace android::hardware::audio::implementation
