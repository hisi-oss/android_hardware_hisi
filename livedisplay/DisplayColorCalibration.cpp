/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "DisplayColorCalibration"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include <fstream>

#include "DisplayColorCalibration.h"

using android::base::ReadFileToString;
using android::base::Split;
using android::base::Trim;
using android::base::WriteStringToFile;

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace hisi {

static constexpr const char* kColorPath = "/sys/devices/virtual/graphics/fb0/lcd_color_temperature";

bool DisplayColorCalibration::isSupported() {
    std::fstream rgb(kColorPath, rgb.in | rgb.out);
    return rgb.good();
}

Return<int32_t> DisplayColorCalibration::getMaxValue() {
    return 32768;
}

Return<int32_t> DisplayColorCalibration::getMinValue() {
    return 1;
}

Return<void> DisplayColorCalibration::getCalibration(getCalibration_cb _hidl_cb) {
    std::string tmp;

    std::lock_guard<std::mutex> lock(mCachedColorsMutex);
    if (ReadFileToString(kColorPath, &tmp)) {
        std::vector<std::string> values = Split(Trim(tmp), ",");
        if (values.size() == 9) {
            mCachedColors[0] = std::stoi(values[0]);  // R
            mCachedColors[1] = std::stoi(values[4]);  // G
            mCachedColors[2] = std::stoi(values[8]);  // B
        }
    } else {
        LOG(ERROR) << "Failed to read color calibration file.";
    }

    _hidl_cb(mCachedColors);
    return Void();
}

Return<bool> DisplayColorCalibration::setCalibration(const hidl_vec<int32_t>& rgb) {
    std::string contents;

    std::lock_guard<std::mutex> lock(mCachedColorsMutex);
    for (size_t i = 0; i < rgb.size(); ++i) {
        contents += std::to_string(rgb[i]);

        if (i < rgb.size() - 1) {
            contents += ",0,0,0,";
        }
    }

    mCachedColors[0] = rgb[0];  // R
    mCachedColors[1] = rgb[1];  // G
    mCachedColors[2] = rgb[2];  // B

    return WriteStringToFile(Trim(contents), kColorPath, true);
}

}  // namespace hisi
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
