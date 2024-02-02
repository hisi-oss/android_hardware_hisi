LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    hardware/ril/include

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libril \
    libcom2ril

LOCAL_CFLAGS := -DLOG_TAG=\"libcommril\"

LOCAL_SRC_FILES := \
    commRil.cpp

LOCAL_MODULE := libcommril
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true
include $(BUILD_SHARED_LIBRARY)
