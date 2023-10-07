/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_dalvik"

#include "include/hisi_dalvik.h"
#include "include/hisi_utils.h"

#include <android-base/logging.h>

#include <sys/sysinfo.h>

#define HEAPSTARTSIZE_PROP "dalvik.vm.heapstartsize"
#define HEAPGROWTHLIMIT_PROP "dalvik.vm.heapgrowthlimit"
#define HEAPSIZE_PROP "dalvik.vm.heapsize"
#define HEAPMINFREE_PROP "dalvik.vm.heapminfree"
#define HEAPMAXFREE_PROP "dalvik.vm.heapmaxfree"
#define HEAPTARGETUTILIZATION_PROP "dalvik.vm.heaptargetutilization"

#define GB(b) (b * 1024ull * 1024 * 1024)

static const dalvik_heap_info_t dalvik_heap_info_6144 = {
        .heapstartsize = "16m",
        .heapgrowthlimit = "256m",
        .heapsize = "512m",
        .heapminfree = "8m",
        .heapmaxfree = "32m",
        .heaptargetutilization = "0.5",
};

static const dalvik_heap_info_t dalvik_heap_info_4096 = {
        .heapstartsize = "8m",
        .heapgrowthlimit = "256m",
        .heapsize = "512m",
        .heapminfree = "8m",
        .heapmaxfree = "16m",
        .heaptargetutilization = "0.6",
};

static const dalvik_heap_info_t dalvik_heap_info_2048 = {
        .heapstartsize = "8m",
        .heapgrowthlimit = "192m",
        .heapsize = "512m",
        .heapminfree = "512k",
        .heapmaxfree = "8m",
        .heaptargetutilization = "0.75",
};

void load_hisi_dalvik() {
    struct sysinfo sys;
    const dalvik_heap_info_t* dhi;

    sysinfo(&sys);

    if (sys.totalram > GB(5)) {
        LOG(INFO) << "Setting dalvik props for >6gb devices";
        dhi = &dalvik_heap_info_6144;
    } else if (sys.totalram > GB(3)) {
        LOG(INFO) << "Setting dalvik props for >4gb devices";
        dhi = &dalvik_heap_info_4096;
    } else {
        LOG(INFO) << "Setting dalvik props for <3gb devices";
        dhi = &dalvik_heap_info_2048;
    }

    set_property(HEAPSTARTSIZE_PROP, dhi->heapstartsize);
    set_property(HEAPGROWTHLIMIT_PROP, dhi->heapgrowthlimit);
    set_property(HEAPSIZE_PROP, dhi->heapsize);
    set_property(HEAPTARGETUTILIZATION_PROP, dhi->heaptargetutilization);
    set_property(HEAPMINFREE_PROP, dhi->heapminfree);
    set_property(HEAPMAXFREE_PROP, dhi->heapmaxfree);
}
