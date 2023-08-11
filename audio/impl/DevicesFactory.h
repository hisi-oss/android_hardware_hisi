/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android/hardware/audio/2.0/IDevicesFactory.h>
#include <android/hardware/audio/4.0/IDevicesFactory.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "core/default/DevicesFactory.h"

namespace android::hardware::audio::implementation {

using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

using AOSPDevicesFactory = ::android::hardware::audio::V4_0::implementation::DevicesFactory;

struct DevicesFactory : public V4_0::IDevicesFactory {
  public:
    DevicesFactory();

    // Methods from ::android::hardware::audio::V4_0::IDevicesFactory follow.
    Return<void> openDevice(const hidl_string& device, openDevice_cb _hidl_cb) override;
    Return<void> openPrimaryDevice(openPrimaryDevice_cb _hidl_cb) override;

  private:
    sp<V2_0::IDevice> openDevice(const std::string& name);

    sp<V2_0::IDevicesFactory> mV2_0DevicesFactory;
    AOSPDevicesFactory mAOSPDevicesFactory;
};

extern "C" V4_0::IDevicesFactory* HIDL_FETCH_IDevicesFactory(const char* name);

}  // namespace android::hardware::audio::implementation
