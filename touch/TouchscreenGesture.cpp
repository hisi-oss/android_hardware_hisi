/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "TouchscreenGestureService"

#include "TouchscreenGesture.h"
#include <fstream>
#include <type_traits>
#include <vector>

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace implementation {

const std::string kGesturePath = "/sys/touchscreen/easy_wakeup_gesture";

const std::vector<TouchscreenGesture::GestureInfo> TouchscreenGesture::kGestures = {
        {66, "Letter C", 0x080},
        {67, "Letter e", 0x100},
        {68, "Letter M", 0x200},
        {87, "Letter W", 0x400},
};

Return<void> TouchscreenGesture::getSupportedGestures(getSupportedGestures_cb resultCb) {
    std::vector<Gesture> gestures;

    for (int i = 0; i < kGestures.size(); i++) {
        gestures.push_back({i, kGestures[i].name, kGestures[i].keycode});
    }

    resultCb(gestures);

    return Void();
}

Return<bool> TouchscreenGesture::setGestureEnabled(const Gesture& gesture, bool enabled) {
    GestureInfo gestureInfo;
    uint16_t gestureMode;
    std::string line;
    std::stringstream ss;
    std::fstream file(kGesturePath);

    if (gesture.id >= kGestures.size()) {
        return false;
    }

    gestureInfo = kGestures[gesture.id];

    file >> line;
    ss << std::hex << line.substr(2);
    ss >> gestureMode;

    if (enabled) {
        gestureMode |= gestureInfo.mask;
    } else {
        gestureMode &= ~gestureInfo.mask;
    }

    file << gestureMode;

    return !file.fail();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
