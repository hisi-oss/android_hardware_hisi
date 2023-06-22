//
// Copyright (C) 2023 The LineageOS Project
//
// SPDX-License-Identifier: Apache-2.0
//

#define LOG_TAG "hisi_init"
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

#include <unistd.h>
#include <string>

constexpr const char* kChiptypePath = "/proc/connectivity/chiptype";
constexpr const char* kDeviceTreePath = "/proc/device-tree";
constexpr const char* kPropSubChipType = "ro.connectivity.sub_chiptype";
constexpr const char* kPropChipType = "ro.connectivity.chiptype";

void set_property(const std::string& prop, const std::string& value) {
    LOG(INFO) << "Setting property: " << prop << " to " << value;

    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Unable to set: " << prop << " to " << value;
    }
}

int main() {
    std::string chip_type;
    std::string subchip_path;

    // This is the main chip type, and it can be used to determine the hardware
    // revision. In our case, we can have either hisi or bcm.
    if (!android::base::ReadFileToString(kChiptypePath, &chip_type)) {
        LOG(ERROR) << "Unable to read: " << kChiptypePath;
        return 1;
    }

    // Set the property, so that the init scripts can be included conditionally.
    set_property(kPropChipType, chip_type);

    // This is the subchip type, and it may be different depending on the hardware
    // revision. In our case, we can have either hi11xx or bcm43xx.
    if (chip_type.find("hisi") == 0) {
        subchip_path = std::string(kDeviceTreePath) + "/hi110x/hi110x,subchip_type";
        if (access(subchip_path.c_str(), F_OK) != 0) {
            subchip_path = std::string(kDeviceTreePath) + "/hi1102/name";
        }
    } else {
        subchip_path = std::string(kDeviceTreePath) + "/bcm_wifi/ic_type";
    }

    if (!android::base::ReadFileToString(subchip_path, &chip_type)) {
        LOG(ERROR) << "Unable to determine a valid subchip type";
        return 1;
    }

    // Set the property, so that the init scripts can be included conditionally.
    set_property(kPropSubChipType, chip_type);

    return 0;
}
