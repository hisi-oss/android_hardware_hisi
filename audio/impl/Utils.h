/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android/hardware/audio/2.0/IDevice.h>
#include <android/hardware/audio/2.0/IPrimaryDevice.h>
#include <android/hardware/audio/4.0/IDevice.h>
#include <android/hardware/audio/4.0/IPrimaryDevice.h>
#include <android/hardware/audio/4.0/types.h>
#include <android/hardware/audio/common/4.0/types.h>

#define CREATE_CONVERT_FN_DECL(oldType, newType) newType convert(oldType oldVar);

#define CREATE_VEC_CONVERT_FN_DECL(oldType, newType) \
    CREATE_CONVERT_FN_DECL(hidl_vec<oldType>, hidl_vec<newType>)

#define CREATE_ALL_CONVERT_FN_DECL(oldType, newType) \
    CREATE_CONVERT_FN_DECL(oldType, newType)         \
    CREATE_VEC_CONVERT_FN_DECL(oldType, newType)

#define CREATE_CONVERT_FNS_DECL(oldType, newType) \
    CREATE_CONVERT_FN_DECL(oldType, newType)      \
    CREATE_CONVERT_FN_DECL(newType, oldType)

#define CREATE_VEC_CONVERT_FNS_DECL(oldType, newType) \
    CREATE_VEC_CONVERT_FN_DECL(oldType, newType)      \
    CREATE_VEC_CONVERT_FN_DECL(newType, oldType)

#define CREATE_ALL_CONVERT_FNS_DECL(oldType, newType) \
    CREATE_ALL_CONVERT_FN_DECL(oldType, newType)      \
    CREATE_ALL_CONVERT_FN_DECL(newType, oldType)

/**
 * Create a HIDL bitfield definition of a given HIDL enum.
 * @param type The HIDL enum type.
 * @param val The value of the bitfield.
 */
#define TO_HIDL_BITFIELD(type, val) hidl_bitfield<type>(val)

/**
 * Downcast a HIDL bitfield to its original type.
 * @param type The original type of the bitfield.
 * @param val The bitfield value to downcast.
 */
#define DOWNCAST_HIDL_BITFIELD(type, val) static_cast<type>(val)

namespace android {
namespace hardware {
namespace audio {
namespace implementation {

using ::android::hardware::hidl_vec;

CREATE_ALL_CONVERT_FNS_DECL(V2_0::IPrimaryDevice::TtyMode, V4_0::IPrimaryDevice::TtyMode)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioChannelMask, common::V4_0::AudioChannelMask)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioFormat, common::V4_0::AudioFormat)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioInputFlag, common::V4_0::AudioInputFlag)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioOutputFlag, common::V4_0::AudioOutputFlag)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioMode, common::V4_0::AudioMode)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioSource, common::V4_0::AudioSource)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioConfig, common::V4_0::AudioConfig)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioPort, common::V4_0::AudioPort)

CREATE_ALL_CONVERT_FNS_DECL(common::V2_0::AudioPortConfig, common::V4_0::AudioPortConfig)

CREATE_ALL_CONVERT_FNS_DECL(V2_0::DeviceAddress, V4_0::DeviceAddress)

CREATE_ALL_CONVERT_FNS_DECL(V2_0::MmapPosition, V4_0::MmapPosition)

CREATE_ALL_CONVERT_FNS_DECL(V2_0::ParameterValue, V4_0::ParameterValue)

CREATE_ALL_CONVERT_FNS_DECL(V2_0::Result, V4_0::Result)

/** @return true if gain is between 0 and 1 included. */
constexpr bool isGainNormalized(float gain) {
    return gain >= 0.0 && gain <= 1.0;
}

}  // namespace implementation
}  // namespace audio
}  // namespace hardware
}  // namespace android
