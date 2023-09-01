/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "audio.hisi_wrapper"

#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <time.h>
#include <cutils/list.h>
#include <cutils/properties.h>
#include <cutils/str_parms.h>
#include <cutils/atomic.h>
#include <unistd.h>
#include <log/log.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <system/audio.h>

#include "audio_hisi.h"



/* These are the supported use cases by the hardware.
 * Each usecase is mapped to a specific PCM device.
 * Refer to pcm_device_table[].
 */
enum {
    USECASE_INVALID = -1,
    /* Playback usecases */
    
    AUDIO_USECASE_MAX
};

typedef int audio_usecase_t;

struct audio_patch_record {
    struct listnode list;
    audio_patch_handle_t handle;	   // My handle of this patch
    audio_patch_handle_t handle_hisi;    // The handle return by original hi6250 audio lib
    audio_usecase_t usecase;		   // Not use for the moment
    struct audio_patch patch;		   // audio patch struct	
};

struct hisi_wrapper_audio_device {
    struct audio_hw_device device;
    struct hisi_audio_hw_device* hisi_device;   //
    pthread_mutex_t lock;
    audio_patch_handle_t next_patch_handle;     // Protected by this->lock
    struct listnode audio_patch_record_list;    // list of the patch
    int hw_module; // hisi hw_device handle
};

static int adev_open_output_stream(struct audio_hw_device* dev, audio_io_handle_t handle,
                                   audio_devices_t devices, audio_output_flags_t flags,
                                   struct audio_config* config,
                                   struct audio_stream_out** stream_out, const char* address) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);
    int status=0;

	
    ALOGI("%s: enter: sample_rate(%d) channel_mask(%#x) format(%#x) devices(%#x) flags(%#x), address(%s)",
              __func__, config->sample_rate, config->channel_mask, config->format, devices, flags, address);
			  	

    status = ctx->hisi_device->open_output_stream(ctx->hisi_device, handle, devices, flags, config,
                                                stream_out, address);

   	
    return status;
}

static void adev_close_output_stream(struct audio_hw_device* dev, struct audio_stream_out* stream) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->close_output_stream(ctx->hisi_device, stream);
}

static int adev_set_parameters(struct audio_hw_device* dev, const char* kvpairs) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s : kvpairs: %s",  __func__, kvpairs);

    return ctx->hisi_device->set_parameters(ctx->hisi_device, kvpairs);
}

static char* adev_get_parameters(const struct audio_hw_device* dev, const char* keys) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

	
    ALOGI("enter function : %s", __func__);
    char* status=ctx->hisi_device->get_parameters(ctx->hisi_device, keys);
    ALOGI("exit function : %s : kvpairs: %s",  __func__, keys);
	
    return status;	
}

static int adev_init_check(const struct audio_hw_device* dev) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);
    int status=0;
	
    ALOGI("enter function : %s", __func__);
    status=ctx->hisi_device->init_check(ctx->hisi_device);
    ALOGI("pass step1 function : %s", __func__);
	
	//ctx->hisi_init_check(ctx->hisi_device);
    ALOGI("exit function : %s", __func__);
	
    return status;	
}

static int adev_set_voice_volume(struct audio_hw_device* dev, float volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->set_voice_volume(ctx->hisi_device, volume);
}

static int adev_set_master_volume(struct audio_hw_device* dev, float volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->set_master_volume(ctx->hisi_device, volume);
}

static int adev_get_master_volume(struct audio_hw_device* dev, float* volume) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

	ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->get_master_volume(ctx->hisi_device, volume);
}

static int adev_set_master_mute(struct audio_hw_device* dev, bool muted) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

	ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->set_master_mute(ctx->hisi_device, muted);
}

static int adev_get_master_mute(struct audio_hw_device* dev, bool* muted) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);
	bool master_mute=false;
	int status=0;

	ALOGI("enter function : %s", __func__);

	pthread_mutex_lock(&ctx->lock);	
    status=ctx->hisi_device->get_master_mute(ctx->hisi_device, &master_mute);
	pthread_mutex_unlock(&ctx->lock);	
	
    ALOGD("%s: %i", __func__, master_mute);

	
	return status;
}

static int adev_set_mode(struct audio_hw_device* dev, audio_mode_t mode) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->set_mode(ctx->hisi_device, mode);
}

static int adev_set_mic_mute(struct audio_hw_device* dev, bool state) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->set_mic_mute(ctx->hisi_device, state);
}

static int adev_get_mic_mute(const struct audio_hw_device* dev, bool* state) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->get_mic_mute(ctx->hisi_device, state);
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device* dev,
                                         const struct audio_config* config) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

	ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->get_input_buffer_size(ctx->hisi_device, config);
}

static int adev_open_input_stream(struct audio_hw_device* dev, audio_io_handle_t handle,
                                  audio_devices_t devices, struct audio_config* config,
                                  struct audio_stream_in** stream_in, audio_input_flags_t flags,
                                  const char* address, audio_source_t source) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

	ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->open_input_stream(ctx->hisi_device, handle, devices, config, stream_in,
                                               flags, address, source);
}

static void adev_close_input_stream(struct audio_hw_device* dev, struct audio_stream_in* in) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

	ALOGI("enter function : %s", __func__);
		
    return ctx->hisi_device->close_input_stream(ctx->hisi_device, in);
}

static int adev_dump(const struct audio_hw_device* dev, int fd) {
    const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    return ctx->hisi_device->dump(ctx->hisi_device, fd);
}

static int adev_close(hw_device_t* device) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(device);

	ALOGI("enter function : %s", __func__);
		
    if (!ctx) {
        return 0;
    }

    // Close the HiSilicon audio device
    ctx->hisi_device->common.close((struct hw_device_t*)ctx->hisi_device);
	

    // TODO clear list 
    free(ctx);

    return 0;
}
                     
                                
static int adev_get_microphones(const struct audio_hw_device *dev,
                           struct audio_microphone_characteristic_t *mic_array,
                           size_t *mic_count)
{
   //const hisi_wrapper_audio_device* ctx = reinterpret_cast<const hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);
    
    if (mic_count == NULL) {
        return -ENOSYS;
    }
    if (*mic_count == 0) {
        *mic_count = 1;
        return 0;
    }
    if (mic_array == NULL) {
        return -ENOSYS;
    }

    return -ENOSYS;
}

/**
* Routing control - API 3.0 and supp
*/

const char *strAudioPatchRole[] = {"AUDIO_PORT_ROLE_NONE", "AUDIO_PORT_ROLE_SOURCE", "AUDIO_PORT_ROLE_SINK"};
const char *strAudioPatchType[] = {"AUDIO_PORT_TYPE_NONE", "AUDIO_PORT_TYPE_DEVICE", "AUDIO_PORT_TYPE_MIX", "AUDIO_PORT_TYPE_SESSION"};

static struct audio_patch_record *get_patch_from_list(struct audio_device *dev,
                                                    audio_patch_handle_t patch_id)
{
    struct audio_patch_record *patch;
    struct listnode *node;
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);
    
    
    list_for_each(node, &ctx->audio_patch_record_list) {
        patch = node_to_item(node, struct audio_patch_record, list);
        if (patch->handle == patch_id)
            return patch;
    }
    return NULL;
}


static int adev_create_audio_patch(struct audio_hw_device *dev,
        unsigned int num_sources,
        const struct audio_port_config *sources,
        unsigned int num_sinks,
        const struct audio_port_config *sinks,
        audio_patch_handle_t *handle) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);
    int i=0;
    int status=0;
    struct audio_patch_record *patch_record = NULL;
    audio_patch_handle_t handle_wrapper  = AUDIO_PATCH_HANDLE_NONE;
    audio_patch_handle_t handle_hisi  = AUDIO_PATCH_HANDLE_NONE;
    
    ALOGI("enter function : %s - handle=%d - numsources=%d - numsink=%d", __func__, *handle, num_sources, num_sinks);

     if (!ctx) {
        return 0;
    }

    if (num_sources != 1 || num_sinks == 0 || num_sinks > AUDIO_PATCH_PORTS_MAX) {
        // Only accept mix->device and device->mix cases. In that case, the number of sources
        // must be 1. The number of sinks must be in the range of (0, AUDIO_PATCH_PORTS_MAX].
        return -EINVAL;
    }

    if (sources[0].type == AUDIO_PORT_TYPE_DEVICE) {
        // If source is a device, the number of sinks should be 1.
        if (num_sinks != 1 || sinks[0].type != AUDIO_PORT_TYPE_MIX) {
            return -EINVAL;
        }
    } else if (sources[0].type == AUDIO_PORT_TYPE_MIX) {
        // If source is a mix, all sinks should be device.
        for (unsigned int i = 0; i < num_sinks; i++) {
            if (sinks[i].type != AUDIO_PORT_TYPE_DEVICE) {
                ALOGE("%s() invalid sink type %#x for mix source", __func__, sinks[i].type);
                return -EINVAL;
            }
        }
    } else {
        // All other cases are invalid.
        return -EINVAL;
    }

    // Only for debug
    for (i = 0; i < num_sources ; i++)
    {
        ALOGD("== source [%d]/[%d] ==", i, num_sources);
        ALOGD("id 0x%x", sources[i].id);
        ALOGD("role 0x%x %s", sources[i].role, strAudioPatchRole[sources[i].role]);
        ALOGD("type 0x%x %s", sources[i].type, strAudioPatchType[sources[i].type]);
        ALOGD("config_mask 0x%x", sources[i].config_mask);
        ALOGD("sample_rate 0x%x", sources[i].sample_rate);
        ALOGD("channel_mask 0x%x", sources[i].channel_mask);
        ALOGD("gain.index 0x%x", sources[i].gain.index);
        ALOGD("gain.mode 0x%x", sources[i].gain.mode);
        ALOGD("gain.channel_mask 0x%x", sources[i].gain.channel_mask);
        ALOGD("gain.ramp_duration_ms 0x%x", sources[i].gain.ramp_duration_ms);
        if (sources[i].type == AUDIO_PORT_TYPE_DEVICE)
        {
            ALOGD("device.hw_module %x", sources[i].ext.device.hw_module);    // TODO Change the hw_module with original
            ALOGD("device.type %x", sources[i].ext.device.type);              // TODO Change the hw_module with original
            ALOGD("device.address %s", sources[i].ext.device.address);
        }
        else if (sources[i].type == AUDIO_PORT_TYPE_MIX)
        {
            ALOGD("mix.hw_module %x", sources[i].ext.mix.hw_module);          // TODO Change the hw_module with original
            ALOGD("mix.handle %x", sources[i].ext.mix.handle);                // TODO Change the hw_module with original
            ALOGD("mix.usecase.stream %x", sources[i].ext.mix.usecase.stream);
            ALOGD("mix.usecase.source %x", sources[i].ext.mix.usecase.source);
        }
        else if (sources[i].type == AUDIO_PORT_TYPE_SESSION)
        {

        }
    }

    for (i = 0; i < num_sinks ; i++)
    {
        ALOGD("== sinks [%d]/[%d] ==", i, num_sinks);
        ALOGD("id 0x%x", sinks[i].id);
        ALOGD("role 0x%x %s", sinks[i].role, strAudioPatchRole[sinks[i].role]);
        ALOGD("type 0x%x %s", sinks[i].type, strAudioPatchType[sinks[i].type]);
        ALOGD("config_mask 0x%x", sinks[i].config_mask);
        ALOGD("sample_rate 0x%x", sinks[i].sample_rate);
        ALOGD("channel_mask 0x%x", sinks[i].channel_mask);
        ALOGD("gain.index 0x%x", sinks[i].gain.index);
        ALOGD("gain.mode 0x%x", sinks[i].gain.mode);
        ALOGD("gain.channel_mask 0x%x", sinks[i].gain.channel_mask);
        ALOGD("gain.ramp_duration_ms 0x%x", sinks[i].gain.ramp_duration_ms);

        if (sinks[i].type == AUDIO_PORT_TYPE_DEVICE)
        {
            ALOGD("device.hw_module %x", sinks[i].ext.device.hw_module);
            ALOGD("device.type %x", sinks[i].ext.device.type);
            ALOGD("device.address %s", sinks[i].ext.device.address);
        }
        else if (sinks[i].type == AUDIO_PORT_TYPE_MIX)
        {
        
            //audio_module_handle_t srcModule = sinks[i].ext.mix.hw_module; 
        
            ALOGD("mix.hw_module %x", sinks[i].ext.mix.hw_module);         // TODO Change the hw_module with original
            ALOGD("mix.handle %x", sinks[i].ext.mix.handle);		      // TODO Change the handle with original
            ALOGD("mix.usecase.stream %x", sinks[i].ext.mix.usecase.stream);
            ALOGD("mix.usecase.source %x", sinks[i].ext.mix.usecase.source);
            
        }
        else if (sinks[i].type == AUDIO_PORT_TYPE_SESSION)
        {

        }
    }

    ALOGD("-%s num_sources [%d] , num_sinks [%d]", __FUNCTION__, num_sources, num_sinks);

    // Only handle patches for mix->devices and device->mix case.
    if (sources[0].type == AUDIO_PORT_TYPE_DEVICE) {
        ALOGI("%s() patch device", __func__);
    }


    if (*handle == AUDIO_PATCH_HANDLE_NONE) {
        ALOGI("%s() need generated patch handle", __func__);

		/* patch created success, add to patch record list */
		patch_record = (struct audio_patch_record *)calloc(1,sizeof(struct audio_patch_record));
			
		if (!patch_record) {
			ALOGE("%s fail to allocate patch_record", __func__);
			status = -ENOMEM;
			goto error;
		}


		pthread_mutex_lock(&ctx->lock);
		handle_wrapper = ++ctx->next_patch_handle;
		
		patch_record->handle = handle_wrapper;
		patch_record->handle_hisi =  AUDIO_PATCH_HANDLE_NONE;
		patch_record->patch.id = handle_wrapper;
		patch_record->usecase = USECASE_INVALID;
		
		// Store sources and sinks value
		patch_record->patch.num_sources = num_sources;
		patch_record->patch.num_sinks = num_sinks;
		for (int i = 0; i < num_sources; i++)
			patch_record->patch.sources[i] = sources[i];
		for (int i = 0; i < num_sinks; i++)
			patch_record->patch.sinks[i] = sinks[i];

		ALOGD("%s - call create_audio_patch", __FUNCTION__);
		
	  			
		// TODO change the for by correct value
		// If you have already get the handle, start with the new handle
		// Try only the 16 first values
		for (int hw_module = ctx->hw_module; hw_module < 0x10; hw_module++) {


			// replace hw_module by my current value
			for (int i = 0; i < num_sources; i++)
				patch_record->patch.sources[i].ext.mix.hw_module=hw_module;
			for (int i = 0; i < num_sinks; i++)
				patch_record->patch.sinks[i].ext.device.hw_module=hw_module;
						  	

			ALOGD("%s - try create_audio_patch sucess with %d handle", __FUNCTION__, hw_module);
			status=ctx->hisi_device->create_audio_patch(ctx->hisi_device, 
				patch_record->patch.num_sources, 
				patch_record->patch.sources, 
				patch_record->patch.num_sinks, 
				patch_record->patch.sinks, 
				&handle_hisi);

	
			if (status==0) {
				ALOGD("%s - call create_audio_patch sucess - new hw module id is %d", __FUNCTION__, hw_module);
				patch_record->handle_hisi = handle_hisi;
				ctx->hw_module = hw_module;
				break;
			}
			else {
				patch_record->handle_hisi = AUDIO_PATCH_HANDLE_NONE;
				ALOGD("%s - call create_audio_patch error %i", __FUNCTION__,status);
			}
		}

		ALOGD("%s - try to add new patch with handle 0x%x", __FUNCTION__, handle_wrapper);
		list_add_tail(&ctx->audio_patch_record_list, &patch_record->list);
		pthread_mutex_unlock(&ctx->lock);
		ALOGD("handlecheck %s sucess new *handle 0x%x", __FUNCTION__, handle_wrapper);
		
		// Now my patch handler
		*handle=handle_wrapper;
   
		status=0;
	}
	else {
		ALOGD("%s pass exiting *handle 0x%x", __FUNCTION__, (int)(*handle));
		status=0;
	}

    goto exit;

error:

exit:
    ALOGV("%s: exit: handle 0x%x", __func__, *handle);
    return status;
}

static int adev_release_audio_patch(struct audio_hw_device *dev,
        audio_patch_handle_t handle) {
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);
    struct audio_device *adev = (struct audio_device *)dev;
    struct audio_patch_record *patch_record = NULL;
    int status = 0;
    
    ALOGI("enter function : %s", __func__);

    if (!ctx) {
         return 0;
    }

    if (handle == AUDIO_PATCH_HANDLE_NONE)
    {
        ALOGW("[%s] [%d]", __FUNCTION__, __LINE__);
        status = -EINVAL;
        return status;
    }
    
    /* get the patch record from handle */
    pthread_mutex_lock(&ctx->lock);
    patch_record = get_patch_from_list(adev, handle);
    pthread_mutex_unlock(&ctx->lock);    
    
    if(!patch_record) {
        ALOGE("%s: failed to find the patch record with handle (%d) in the list",
                __func__, handle);
        status = -EINVAL;
        return status;
    }
	else {
		// release hisi audio patch also
		status=ctx->hisi_device->release_audio_patch(ctx->hisi_device,  patch_record->handle_hisi);	
	}


    /* remove the patch record from list and free it */
    pthread_mutex_lock(&ctx->lock);
    list_remove(&patch_record->list);
    pthread_mutex_unlock(&ctx->lock);
    free(patch_record);


    ALOGV("%s: exit", __func__);
    return status;
}

static int adev_get_audio_port(struct audio_hw_device *dev, struct audio_port *port)
{
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    if (!ctx) {
        return 0;
    }

    if (port->type != AUDIO_PORT_TYPE_DEVICE) {
        return -EINVAL;
    }


    return ctx->hisi_device->get_audio_port(ctx->hisi_device,  port);
}

static int adev_set_audio_port_config(struct audio_hw_device *dev,
                         const struct audio_port_config *config)
{
    hisi_wrapper_audio_device* ctx = reinterpret_cast<hisi_wrapper_audio_device*>(dev);

    ALOGI("enter function : %s", __func__);

    if (!ctx) {
        return 0;
    }

    return ctx->hisi_device->set_audio_port_config(ctx->hisi_device,  config);
}



static int adev_open(const hw_module_t* module, const char* name, hw_device_t** device) {
    int rc;
    struct hisi_wrapper_audio_device* adev;
    struct hw_module_t* hisi_module;

    ALOGI("adev_open: %s", name);

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0) {
        return -EINVAL;
    }

    adev = (struct hisi_wrapper_audio_device*)calloc(1, sizeof *adev);
    if (!adev) {
        return -ENOMEM;
    }

    // Init
    pthread_mutex_init(&adev->lock, (const pthread_mutexattr_t *) NULL);
    list_init(&adev->audio_patch_record_list);
    adev->next_patch_handle = AUDIO_PATCH_HANDLE_NONE;
    adev->hw_module = 1;
	
	
    // Get the HiSilicon audio module
    rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "primary_hisi",
                                (const hw_module_t**)&hisi_module);
    if (rc < 0) {
        ALOGE("Could not get audio hw module");
        free(adev);
        return rc;
    }

    ALOGI("HISI Module : id=%s name=%s author=%s versionmajor=%04x versionminor=%04x",hisi_module->id,hisi_module->name,hisi_module->author,hisi_module->version_major,hisi_module->version_minor);

	
    // Open it
    rc = hisi_module->methods->open(hisi_module, name, (struct hw_device_t**)&adev->hisi_device);
    // 	rc = audio_hw_device_open(hisi_module, (struct hw_device_t**)&adev->hisi_device);
    if (rc < 0) {
        ALOGE("Could not open hisi audio hw device");
        free(adev);
        return rc;
    } 
	
    struct audio_hw_device *audiohw=(struct audio_hw_device *)adev->hisi_device;
    ALOGI("HISI Device API Version : %x - rc %d", audiohw->common.version, rc);

	
    /* default audio HAL major version */
    uint32_t maj_version = 3;
    uint32_t min_version = 0;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = HARDWARE_DEVICE_API_VERSION(maj_version, min_version);
    adev->device.common.module = (struct hw_module_t *)module;
    adev->device.common.close = adev_close;

    ALOGI("OSS Audio device API Version %04x ", adev->device.common.version);	

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
    adev->device.get_microphones = adev_get_microphones;                // don't exist in hi6250 EMUI8 PRA
    adev->device.create_audio_patch = adev_create_audio_patch;
    adev->device.release_audio_patch = adev_release_audio_patch;
    adev->device.get_audio_port = adev_get_audio_port;
    adev->device.set_audio_port_config = adev_set_audio_port_config;

    // 3.2 API
    //adev->hw_device.get_audio_port_v7 = adev_get_audio_port_v7;
    //adev->hw_device.add_device_effect
    //adev->hw_device.remove_device_effect
    //adev->hw_device.set_device_connected_state_v7
	
	
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
        .author = "Sebastiano Barezzi, Raphael Mounier, The LineageOS Project",
        .methods = &hal_module_methods,
    },
};
// clang-format on

