/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "audio.hisi_wrapper"

#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>

#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <system/audio.h>

#include "audio_hisi.h"

struct hisi_wrapper_audio_device {
    struct audio_hw_device device;
    struct hisi_audio_hw_device* hisi_device;
};

static int adev_open_output_stream(struct audio_hw_device* dev, audio_io_handle_t handle,
                                   audio_devices_t devices, audio_output_flags_t flags,
                                   struct audio_config* config,
                                   struct audio_stream_out** stream_out, const char* address) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->open_output_stream(ctx->hisi_device, handle, devices, flags, config,
                                                stream_out, address);
}

static void adev_close_output_stream(struct audio_hw_device* dev, struct audio_stream_out* stream) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->close_output_stream(ctx->hisi_device, stream);
}

static int adev_set_parameters(struct audio_hw_device* dev, const char* kvpairs) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_parameters(ctx->hisi_device, kvpairs);
}

static char* adev_get_parameters(const struct audio_hw_device* dev, const char* keys) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->get_parameters(ctx->hisi_device, keys);
}

static int adev_init_check(const struct audio_hw_device* dev) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->init_check(ctx->hisi_device);
}

static int adev_set_voice_volume(struct audio_hw_device* dev, float volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_voice_volume(ctx->hisi_device, volume);
}

static int adev_set_master_volume(struct audio_hw_device* dev, float volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_master_volume(ctx->hisi_device, volume);
}

static int adev_get_master_volume(struct audio_hw_device* dev, float* volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->get_master_volume(ctx->hisi_device, volume);
}

static int adev_set_master_mute(struct audio_hw_device* dev, bool muted) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_master_mute(ctx->hisi_device, muted);
}

static int adev_get_master_mute(struct audio_hw_device* dev, bool* muted) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->get_master_mute(ctx->hisi_device, muted);
}

static int adev_set_mode(struct audio_hw_device* dev, audio_mode_t mode) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_mode(ctx->hisi_device, mode);
}

static int adev_set_mic_mute(struct audio_hw_device* dev, bool state) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->set_mic_mute(ctx->hisi_device, state);
}

static int adev_get_mic_mute(const struct audio_hw_device* dev, bool* state) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->get_mic_mute(ctx->hisi_device, state);
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device* dev,
                                         const struct audio_config* config) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->get_input_buffer_size(ctx->hisi_device, config);
}

static int adev_open_input_stream(struct audio_hw_device* dev, audio_io_handle_t handle,
                                  audio_devices_t devices, struct audio_config* config,
                                  struct audio_stream_in** stream_in, audio_input_flags_t flags,
                                  const char* address, audio_source_t source) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->open_input_stream(ctx->hisi_device, handle, devices, config, stream_in,
                                               flags, address, source);
}

static void adev_close_input_stream(struct audio_hw_device* dev, struct audio_stream_in* in) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->close_input_stream(ctx->hisi_device, in);
}

static int adev_dump(const struct audio_hw_device* dev, int fd) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    return ctx->hisi_device->dump(ctx->hisi_device, fd);
}

static int adev_close(hw_device_t* device) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(device);

    if (!ctx) {
        return 0;
    }

    // Close the HiSilicon audio device
    ctx->hisi_device->common.close((struct hw_device_t*)ctx->hisi_device);

    free(ctx);

    return 0;
}

static int adev_open(const hw_module_t* module, const char* name, hw_device_t** device) {
    int rc;
    struct hisi_wrapper_audio_device* adev;
    struct hw_module_t* hisi_module;

    ALOGI("adev_open: %s", name);

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0) {
        return -EINVAL;
    }

    adev = (struct hisi_wrapper_audio_device*)calloc(1, sizeof(struct hisi_wrapper_audio_device));
    if (!adev) {
        return -ENOMEM;
    }

    // Get the HiSilicon audio module
    rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "primary_hisi",
                                (const hw_module_t**)&hisi_module);
    if (rc < 0) {
        ALOGE("Could not get audio hw module");
        free(adev);
        return rc;
    }

    // Open it
    rc = hisi_module->methods->open(hisi_module, name, (struct hw_device_t**)&adev->hisi_device);
    if (rc < 0) {
        ALOGE("Could not open hisi audio hw device");
        free(adev);
        return rc;
    }

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->device.common.module = (struct hw_module_t*)module;
    adev->device.common.close = adev_close;

    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
    adev->device.get_master_volume = adev_get_master_volume;
    adev->device.set_master_mute = adev_set_master_mute;
    adev->device.get_master_mute = adev_get_master_mute;
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters;
    adev->device.get_parameters = adev_get_parameters;
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream;
    adev->device.close_output_stream = adev_close_output_stream;
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
        .open = adev_open,
};

// clang-format off
extern "C" struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "HiSilicon audio HAL wrapper",
        .author = "Sebastiano Barezzi, The LineageOS Project",
        .methods = &hal_module_methods,
    },
};
// clang-format on
