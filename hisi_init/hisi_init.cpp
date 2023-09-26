/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_init"

#include "include/hisi_connectivity.h"
#include "include/hisi_dalvik.h"
#include "include/hisi_nve.h"

#include <android-base/logging.h>

int main() {
    LOG(INFO) << "Running hisi_connectivity";
    load_hisi_connectivity();
    LOG(INFO) << "Running hisi_dalvik";
    load_hisi_dalvik();
    LOG(INFO) << "Running hisi_nve";
    load_hisi_nve();
}
