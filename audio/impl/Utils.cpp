/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Utils.h"

#define CHECK_SIZE(oldType, newType) \
    static_assert(sizeof(oldType) == sizeof(newType), "Size of types must be equal");

#define CREATE_COPY_FN(oldType, newType)          \
    CHECK_SIZE(oldType, newType)                  \
    newType convert(oldType oldVar) {             \
        newType newVar;                           \
        memcpy(&newVar, &oldVar, sizeof(oldVar)); \
        return newVar;                            \
    }

#define CREATE_ENUM_CAST_FN(oldType, newType) \
    CHECK_SIZE(oldType, newType)              \
    newType convert(oldType oldVar) { return static_cast<newType>(oldVar); }

#define CREATE_VEC_CONVERT_FN(oldType, newType)           \
    hidl_vec<newType> convert(hidl_vec<oldType> oldVar) { \
        hidl_vec<newType> newVar;                         \
        newVar.resize(oldVar.size());                     \
        for (size_t i = 0; i < oldVar.size(); i++) {      \
            newVar[i] = convert(oldVar[i]);               \
        }                                                 \
        return newVar;                                    \
    }

#define CREATE_COPY_FNS(oldType, newType) \
    CREATE_COPY_FN(oldType, newType)      \
    CREATE_COPY_FN(newType, oldType)

#define CREATE_ENUM_CAST_FNS(oldType, newType) \
    CREATE_ENUM_CAST_FN(oldType, newType)      \
    CREATE_ENUM_CAST_FN(newType, oldType)

#define CREATE_VEC_CONVERT_FNS(oldType, newType) \
    CREATE_VEC_CONVERT_FN(oldType, newType)      \
    CREATE_VEC_CONVERT_FN(newType, oldType)

#define CREATE_ALL_COPY_FNS(oldType, newType) \
    CREATE_COPY_FNS(oldType, newType)         \
    CREATE_VEC_CONVERT_FNS(oldType, newType)

#define CREATE_ALL_ENUM_CAST_FNS(oldType, newType) \
    CREATE_ENUM_CAST_FNS(oldType, newType)         \
    CREATE_VEC_CONVERT_FNS(oldType, newType)

namespace android::hardware::audio::implementation {

CREATE_ALL_ENUM_CAST_FNS(V2_0::IPrimaryDevice::TtyMode, V4_0::IPrimaryDevice::TtyMode)

CREATE_ALL_ENUM_CAST_FNS(common::V2_0::AudioChannelMask, common::V4_0::AudioChannelMask)

CREATE_ALL_ENUM_CAST_FNS(common::V2_0::AudioFormat, common::V4_0::AudioFormat)

CREATE_ALL_ENUM_CAST_FNS(common::V2_0::AudioInputFlag, common::V4_0::AudioInputFlag)

CREATE_ALL_ENUM_CAST_FNS(common::V2_0::AudioOutputFlag, common::V4_0::AudioOutputFlag)

common::V2_0::AudioMode convert(common::V4_0::AudioMode mode) {
    switch (mode) {
        case common::V4_0::AudioMode::NORMAL:
            return common::V2_0::AudioMode::NORMAL;
        case common::V4_0::AudioMode::RINGTONE:
            return common::V2_0::AudioMode::RINGTONE;
        case common::V4_0::AudioMode::IN_CALL:
            return common::V2_0::AudioMode::IN_CALL;
        case common::V4_0::AudioMode::IN_COMMUNICATION:
            return common::V2_0::AudioMode::IN_COMMUNICATION;
        default:
            return common::V2_0::AudioMode::INVALID;
    }
}

common::V4_0::AudioMode convert(common::V2_0::AudioMode mode) {
    switch (mode) {
        case common::V2_0::AudioMode::INVALID:
            return common::V4_0::AudioMode(-2);
        case common::V2_0::AudioMode::CURRENT:
            return common::V4_0::AudioMode(-1);
        case common::V2_0::AudioMode::NORMAL:
            return common::V4_0::AudioMode::NORMAL;
        case common::V2_0::AudioMode::RINGTONE:
            return common::V4_0::AudioMode::RINGTONE;
        case common::V2_0::AudioMode::IN_CALL:
            return common::V4_0::AudioMode::IN_CALL;
        case common::V2_0::AudioMode::IN_COMMUNICATION:
            return common::V4_0::AudioMode::IN_COMMUNICATION;
        default:
            return common::V4_0::AudioMode(-2);
    }
}

CREATE_VEC_CONVERT_FNS(common::V2_0::AudioMode, common::V4_0::AudioMode)

common::V2_0::AudioSource convert(common::V4_0::AudioSource mode) {
    switch (mode) {
        case common::V4_0::AudioSource::DEFAULT:
            return common::V2_0::AudioSource::DEFAULT;
        case common::V4_0::AudioSource::MIC:
            return common::V2_0::AudioSource::MIC;
        case common::V4_0::AudioSource::VOICE_UPLINK:
            return common::V2_0::AudioSource::VOICE_UPLINK;
        case common::V4_0::AudioSource::VOICE_DOWNLINK:
            return common::V2_0::AudioSource::VOICE_DOWNLINK;
        case common::V4_0::AudioSource::VOICE_CALL:
            return common::V2_0::AudioSource::VOICE_CALL;
        case common::V4_0::AudioSource::CAMCORDER:
            return common::V2_0::AudioSource::CAMCORDER;
        case common::V4_0::AudioSource::VOICE_RECOGNITION:
            return common::V2_0::AudioSource::VOICE_RECOGNITION;
        case common::V4_0::AudioSource::VOICE_COMMUNICATION:
            return common::V2_0::AudioSource::VOICE_COMMUNICATION;
        case common::V4_0::AudioSource::REMOTE_SUBMIX:
            return common::V2_0::AudioSource::REMOTE_SUBMIX;
        case common::V4_0::AudioSource::UNPROCESSED:
            return common::V2_0::AudioSource::UNPROCESSED;
        case common::V4_0::AudioSource::FM_TUNER:
            return common::V2_0::AudioSource::FM_TUNER;
        default:
            return common::V2_0::AudioSource(-1);
    }
}

common::V4_0::AudioSource convert(common::V2_0::AudioSource mode) {
    switch (mode) {
        case common::V2_0::AudioSource::DEFAULT:
            return common::V4_0::AudioSource::DEFAULT;
        case common::V2_0::AudioSource::MIC:
            return common::V4_0::AudioSource::MIC;
        case common::V2_0::AudioSource::VOICE_UPLINK:
            return common::V4_0::AudioSource::VOICE_UPLINK;
        case common::V2_0::AudioSource::VOICE_DOWNLINK:
            return common::V4_0::AudioSource::VOICE_DOWNLINK;
        case common::V2_0::AudioSource::VOICE_CALL:
            return common::V4_0::AudioSource::VOICE_CALL;
        case common::V2_0::AudioSource::CAMCORDER:
            return common::V4_0::AudioSource::CAMCORDER;
        case common::V2_0::AudioSource::VOICE_RECOGNITION:
            return common::V4_0::AudioSource::VOICE_RECOGNITION;
        case common::V2_0::AudioSource::VOICE_COMMUNICATION:
            return common::V4_0::AudioSource::VOICE_COMMUNICATION;
        case common::V2_0::AudioSource::REMOTE_SUBMIX:
            return common::V4_0::AudioSource::REMOTE_SUBMIX;
        case common::V2_0::AudioSource::UNPROCESSED:
            return common::V4_0::AudioSource::UNPROCESSED;
        case common::V2_0::AudioSource::FM_TUNER:
            return common::V4_0::AudioSource::FM_TUNER;
        case common::V2_0::AudioSource::HOTWORD:
            return common::V4_0::AudioSource(1999);
        default:
            return common::V4_0::AudioSource(-1);
    }
}

CREATE_VEC_CONVERT_FNS(common::V2_0::AudioSource, common::V4_0::AudioSource)

CREATE_ALL_COPY_FNS(common::V2_0::AudioConfig, common::V4_0::AudioConfig)

CREATE_ALL_COPY_FNS(common::V2_0::AudioPort, common::V4_0::AudioPort)

CREATE_ALL_COPY_FNS(common::V2_0::AudioPortConfig, common::V4_0::AudioPortConfig)

CREATE_ALL_COPY_FNS(V2_0::DeviceAddress, V4_0::DeviceAddress)

CREATE_ALL_COPY_FNS(V2_0::MmapPosition, V4_0::MmapPosition)

CREATE_ALL_COPY_FNS(V2_0::ParameterValue, V4_0::ParameterValue)

CREATE_ALL_ENUM_CAST_FNS(V2_0::Result, V4_0::Result)

}  // namespace android::hardware::audio::implementation
