/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "GloveModeService"

#include "GloveMode.h"

#include <fstream>

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace implementation {

static constexpr const char* kGloveModePath = "/sys/touchscreen/touch_glove";

Return<bool> GloveMode::isEnabled() {
    std::ifstream file(kGloveModePath);
    bool enabled;

    file >> enabled;

    return enabled;
}

Return<bool> GloveMode::setEnabled(bool enabled) {
    std::ofstream file(kGloveModePath);

    file << enabled << std::flush;

    return !file.fail();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
