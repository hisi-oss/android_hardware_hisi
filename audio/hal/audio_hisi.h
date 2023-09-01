/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <hardware/audio.h>

/**
 * This is basically just a copy of the audio_hw_device_t struct from
 * hardware/libhardware/include/hardware/audio.h, but without
 * get_microphones to match the audio_hw_device_t struct in
 * our audio HALs
 */
struct hisi_audio_hw_device {
    /**
     * Common methods of the audio device.  This *must* be the first member of audio_hw_device
     * as users of this structure will cast a hw_device_t to audio_hw_device pointer in contexts
     * where it's known the hw_device_t references an audio_hw_device.
     */
    struct hw_device_t common;
    
    
      /***** HUAWEI PRA order
        *(code **)(__arg + 0x1e) = adev_get_supported_devices;
	*(code **)(__arg + 0x20) = adev_init_check;
	*(code **)(__arg + 0x22) = adev_set_voice_volume;
	*(code **)(__arg + 0x24) = adev_set_master_volume;
	*(code **)(__arg + 0x26) = adev_get_master_volume;  
	*(code **)(__arg + 0x28) = adev_set_mode;
	*(code **)(__arg + 0x2a) = adev_set_mic_mute;  
	*(code **)(__arg + 0x2c) = adev_get_mic_mute;  
	*(code **)(__arg + 0x2e) = adev_set_parameters;
	*(code **)(__arg + 0x30) = adev_get_parameters;
	*(code **)(__arg + 0x32) = adev_get_input_buffer_size;
	*(code **)(__arg + 0x34) = adev_open_output_stream;
	*(code **)(__arg + 0x36) = adev_close_output_stream;
	*(code **)(__arg + 0x38) = adev_open_input_stream;
	*(code **)(__arg + 0x3a) = adev_close_input_stream;
	*(code **)(__arg + 0x3c) = adev_dump;
	*(code **)(__arg + 0x3e) = adev_set_master_mute;
	*(code **)(__arg + 0x40) = adev_get_master_mute;
	*(code **)(__arg + 0x42) = adev_create_audio_patch;
	*(code **)(__arg + 0x44) = adev_release_audio_patch;
	*(code **)(__arg + 0x46) = adev_get_audio_port;
	*(code **)(__arg + 0x48) = adev_set_audio_port_config;
	
	*(code **)(__arg + 0xae) = dsp_modem_loop;  
	*(code **)(__arg + 0xb0) = dsp_slimbus_i2s_loop;
	*(code **)(__arg + 0xb2) = dsp_set_device;
	*(code **)(__arg + 0xb4) = dsp_set_parameters;
	*(code **)(__arg + 0xb6) = dsp_get_parameters;
	*(code **)(__arg + 0xb8) = spk_dev_get_parameters;
	*(code **)(__arg + 0xba) = dsp_set_mmi_mode;
	*(code **)(__arg + 0xbc) = adev_set_tty_mode;

	*(code **)(__arg + 0xca) = dsp_set_ext_headset_hifi_bypass;
	*(code **)(__arg + 0xcc) = dsp_algo_bypass;
	*(code **)(__arg + 0xce) = dsp_smartPA_control;

	*(code **)(__arg + 0xd0) = spk_get_R0;
	*(code **)(__arg + 0xd2) = spk_set_calibration_value;
	*(code **)(__arg + 0xd4) = spk_get_F0;
	*(code **)(__arg + 0xd6) = spk_algo_bypass;
	*(code **)(__arg + 0xd8) = get_output_device;

	*(code **)(__arg + 200) = dsp_set_voicepp;

	*/

    uint32_t (*get_supported_devices)(const struct hisi_audio_hw_device *dev);
    int (*init_check)(const struct hisi_audio_hw_device *dev);
    int (*set_voice_volume)(struct hisi_audio_hw_device *dev, float volume);
    int (*set_master_volume)(struct hisi_audio_hw_device *dev, float volume);
    int (*get_master_volume)(struct hisi_audio_hw_device *dev, float *volume);
    int (*set_mode)(struct hisi_audio_hw_device *dev, audio_mode_t mode);
    int (*set_mic_mute)(struct hisi_audio_hw_device *dev, bool state);
    int (*get_mic_mute)(const struct hisi_audio_hw_device *dev, bool *state);
    int (*set_parameters)(struct hisi_audio_hw_device *dev, const char *kv_pairs);
    char * (*get_parameters)(const struct hisi_audio_hw_device *dev,
                             const char *keys);
    size_t (*get_input_buffer_size)(const struct hisi_audio_hw_device *dev,
                                    const struct audio_config *config);
    int (*open_output_stream)(struct hisi_audio_hw_device *dev,
                              audio_io_handle_t handle,
                              audio_devices_t devices,
                              audio_output_flags_t flags,
                              struct audio_config *config,
                              struct audio_stream_out **stream_out,
                              const char *address);
                              
                              
    void (*close_output_stream)(struct hisi_audio_hw_device *dev,
                                struct audio_stream_out* stream_out);
    int (*open_input_stream)(struct hisi_audio_hw_device *dev,
                             audio_io_handle_t handle,
                             audio_devices_t devices,
                             struct audio_config *config,
                             struct audio_stream_in **stream_in,
                             audio_input_flags_t flags,
                             const char *address,
                             audio_source_t source);

    void (*close_input_stream)(struct hisi_audio_hw_device *dev,
                               struct audio_stream_in *stream_in);
    int (*dump)(const struct hisi_audio_hw_device *dev, int fd);
    int (*set_master_mute)(struct hisi_audio_hw_device *dev, bool mute);
    int (*get_master_mute)(struct hisi_audio_hw_device *dev, bool *mute);
    int (*create_audio_patch)(struct hisi_audio_hw_device *dev,
                               unsigned int num_sources,
                               const struct audio_port_config *sources,
                               unsigned int num_sinks,
                               const struct audio_port_config *sinks,
                               audio_patch_handle_t *handle);
    int (*release_audio_patch)(struct hisi_audio_hw_device *dev,
                               audio_patch_handle_t handle);
    int (*get_audio_port)(struct hisi_audio_hw_device *dev,
                          struct audio_port *port);
    int (*set_audio_port_config)(struct hisi_audio_hw_device *dev,
                         const struct audio_port_config *config);
};

typedef struct hisi_audio_hw_device hisi_audio_hw_device_t;

