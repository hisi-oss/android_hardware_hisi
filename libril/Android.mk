LOCAL_PATH := $(call my-dir)

ifneq ($(BOARD_USES_LIBRIL_WRAPPER),)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    hardware/ril/include

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libril-hisi

LOCAL_CFLAGS := -DLOG_TAG=\"libril-wrapper\"

LOCAL_SRC_FILES := \
    ril.cpp

LOCAL_MODULE := libril
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true
include $(BUILD_SHARED_LIBRARY)
endif
