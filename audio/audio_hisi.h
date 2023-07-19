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

    /**
     * used by audio flinger to enumerate what devices are supported by
     * each audio_hw_device implementation.
     *
     * Return value is a bitmask of 1 or more values of audio_devices_t
     *
     * NOTE: audio HAL implementations starting with
     * AUDIO_DEVICE_API_VERSION_2_0 do not implement this function.
     * All supported devices should be listed in audio_policy.conf
     * file and the audio policy manager must choose the appropriate
     * audio module based on information in this file.
     */
    uint32_t (*get_supported_devices)(const struct hisi_audio_hw_device* dev);

    /**
     * check to see if the audio hardware interface has been initialized.
     * returns 0 on success, -ENODEV on failure.
     */
    int (*init_check)(const struct hisi_audio_hw_device* dev);

    /** set the audio volume of a voice call. Range is between 0.0 and 1.0 */
    int (*set_voice_volume)(struct hisi_audio_hw_device* dev, float volume);

    /**
     * set the audio volume for all audio activities other than voice call.
     * Range between 0.0 and 1.0. If any value other than 0 is returned,
     * the software mixer will emulate this capability.
     */
    int (*set_master_volume)(struct hisi_audio_hw_device* dev, float volume);

    /**
     * Get the current master volume value for the HAL, if the HAL supports
     * master volume control.  AudioFlinger will query this value from the
     * primary audio HAL when the service starts and use the value for setting
     * the initial master volume across all HALs.  HALs which do not support
     * this method may leave it set to NULL.
     */
    int (*get_master_volume)(struct hisi_audio_hw_device* dev, float* volume);

    /**
     * set_mode is called when the audio mode changes. AUDIO_MODE_NORMAL mode
     * is for standard audio playback, AUDIO_MODE_RINGTONE when a ringtone is
     * playing, and AUDIO_MODE_IN_CALL when a call is in progress.
     */
    int (*set_mode)(struct hisi_audio_hw_device* dev, audio_mode_t mode);

    /* mic mute */
    int (*set_mic_mute)(struct hisi_audio_hw_device* dev, bool state);
    int (*get_mic_mute)(const struct hisi_audio_hw_device* dev, bool* state);

    /* set/get global audio parameters */
    int (*set_parameters)(struct hisi_audio_hw_device* dev, const char* kv_pairs);

    /*
     * Returns a pointer to a heap allocated string. The caller is responsible
     * for freeing the memory for it using free().
     */
    char* (*get_parameters)(const struct hisi_audio_hw_device* dev, const char* keys);

    /* Returns audio input buffer size according to parameters passed or
     * 0 if one of the parameters is not supported.
     * See also get_buffer_size which is for a particular stream.
     */
    size_t (*get_input_buffer_size)(const struct hisi_audio_hw_device* dev,
                                    const struct audio_config* config);

    /** This method creates and opens the audio hardware output stream.
     * The "address" parameter qualifies the "devices" audio device type if needed.
     * The format format depends on the device type:
     * - Bluetooth devices use the MAC address of the device in the form "00:11:22:AA:BB:CC"
     * - USB devices use the ALSA card and device numbers in the form  "card=X;device=Y"
     * - Other devices may use a number or any other string.
     */

    int (*open_output_stream)(struct hisi_audio_hw_device* dev, audio_io_handle_t handle,
                              audio_devices_t devices, audio_output_flags_t flags,
                              struct audio_config* config, struct audio_stream_out** stream_out,
                              const char* address);

    void (*close_output_stream)(struct hisi_audio_hw_device* dev,
                                struct audio_stream_out* stream_out);

    /** This method creates and opens the audio hardware input stream */
    int (*open_input_stream)(struct hisi_audio_hw_device* dev, audio_io_handle_t handle,
                             audio_devices_t devices, struct audio_config* config,
                             struct audio_stream_in** stream_in, audio_input_flags_t flags,
                             const char* address, audio_source_t source);

    void (*close_input_stream)(struct hisi_audio_hw_device* dev, struct audio_stream_in* stream_in);

    /** This method dumps the state of the audio hardware */
    int (*dump)(const struct hisi_audio_hw_device* dev, int fd);

    /**
     * set the audio mute status for all audio activities.  If any value other
     * than 0 is returned, the software mixer will emulate this capability.
     */
    int (*set_master_mute)(struct hisi_audio_hw_device* dev, bool mute);

    /**
     * Get the current master mute status for the HAL, if the HAL supports
     * master mute control.  AudioFlinger will query this value from the primary
     * audio HAL when the service starts and use the value for setting the
     * initial master mute across all HALs.  HALs which do not support this
     * method may leave it set to NULL.
     */
    int (*get_master_mute)(struct hisi_audio_hw_device* dev, bool* mute);

    /**
     * Routing control
     */

    /* Creates an audio patch between several source and sink ports.
     * The handle is allocated by the HAL and should be unique for this
     * audio HAL module. */
    int (*create_audio_patch)(struct hisi_audio_hw_device* dev, unsigned int num_sources,
                              const struct audio_port_config* sources, unsigned int num_sinks,
                              const struct audio_port_config* sinks, audio_patch_handle_t* handle);

    /* Release an audio patch */
    int (*release_audio_patch)(struct hisi_audio_hw_device* dev, audio_patch_handle_t handle);

    /* Fills the list of supported attributes for a given audio port.
     * As input, "port" contains the information (type, role, address etc...)
     * needed by the HAL to identify the port.
     * As output, "port" contains possible attributes (sampling rates, formats,
     * channel masks, gain controllers...) for this port.
     */
    int (*get_audio_port)(struct hisi_audio_hw_device* dev, struct audio_port* port);

    /* Set audio port configuration */
    int (*set_audio_port_config)(struct hisi_audio_hw_device* dev,
                                 const struct audio_port_config* config);

    /**
     * Applies an audio effect to an audio device.
     *
     * @param dev the audio HAL device context.
     * @param device identifies the sink or source device the effect must be applied to.
     *               "device" is the audio_port_handle_t indicated for the device when
     *               the audio patch connecting that device was created.
     * @param effect effect interface handle corresponding to the effect being added.
     * @return retval operation completion status.
     */
    int (*add_device_effect)(struct hisi_audio_hw_device* dev, audio_port_handle_t device,
                             effect_handle_t effect);

    /**
     * Stops applying an audio effect to an audio device.
     *
     * @param dev the audio HAL device context.
     * @param device identifies the sink or source device this effect was applied to.
     *               "device" is the audio_port_handle_t indicated for the device when
     *               the audio patch is created.
     * @param effect effect interface handle corresponding to the effect being removed.
     * @return retval operation completion status.
     */
    int (*remove_device_effect)(struct hisi_audio_hw_device* dev, audio_port_handle_t device,
                                effect_handle_t effect);

    /**
     * Fills the list of supported attributes for a given audio port.
     * As input, "port" contains the information (type, role, address etc...)
     * needed by the HAL to identify the port.
     * As output, "port" contains possible attributes (sampling rates, formats,
     * channel masks, gain controllers...) for this port. The possible attributes
     * are saved as audio profiles, which contains audio format and the supported
     * sampling rates and channel masks.
     */
    int (*get_audio_port_v7)(struct hisi_audio_hw_device* dev, struct audio_port_v7* port);

    /**
     * Called when the state of the connection of an external device has been changed.
     * The "port" parameter is only used as input and besides identifying the device
     * port, also may contain additional information such as extra audio descriptors.
     *
     * HAL version 3.2 and higher only. If the HAL does not implement this method,
     * it must leave the function entry as null, or return -ENOSYS. In this case
     * the framework will use 'set_parameters', which can only pass the device address.
     *
     * @param dev the audio HAL device context.
     * @param port device port identification and extra information.
     * @param connected whether the external device is connected.
     * @return retval operation completion status.
     */
    int (*set_device_connected_state_v7)(struct hisi_audio_hw_device* dev,
                                         struct audio_port_v7* port, bool connected);
};
typedef struct hisi_audio_hw_device hisi_audio_hw_device_t;
