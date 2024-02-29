#
# Copyright (C) 2024 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

ifneq ($(BOARD_USES_LIBBT_WRAPPER),)
include $(CLEAR_VARS)

BDROID_DIR := $(TOP_DIR)packages/modules/Bluetooth/system

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include \
    $(BDROID_DIR)/hci/include \
    $(BDROID_DIR)/include \
    $(BDROID_DIR)/device/include \
    $(BDROID_DIR)

LOCAL_SRC_FILES := \
    libbt-vendor.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog

LOCAL_MODULE := libbt-vendor
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)
endif # BOARD_USES_LIBBT_WRAPPER
