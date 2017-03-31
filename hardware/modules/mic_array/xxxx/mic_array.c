/*************************************************************************
        > File Name: mic_array.c
        > Author:
        > Mail:
        > Created Time: Mon May  4 14:22:33 2015
 ************************************************************************/
#define LOG_TAG "mic_array_amlogic"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <hardware/hardware.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <pthread.h>

#include <time.h>
#include <tinyalsa/asoundlib.h>
#include <system/audio.h>
#include <hardware/audio.h>

#include "mic/mic_array.h"

#define MODULE_NAME "mic_array"
#define MODULE_AUTHOR "jiaqi@rokid.com"

static struct pcm_config pcm_config_in = {
    .channels = 8,
    .rate = 48000,
    .period_size = 1024,
    .period_count = 4,
    .format = PCM_FORMAT_S32_LE,
};

static struct amlogic_mic_array_device {
    struct mic_array_device_t mic_array;

    int pts;
    char* buffer;
};

static int mic_array_device_open(const struct hw_module_t* module,
    const char* name, struct hw_device_t** device);

static int mic_array_device_close(struct hw_device_t* device);

static int mic_array_device_start_stream(struct mic_array_device_t* dev);

static int mic_array_device_stop_stream(struct mic_array_device_t* dev);

static int mic_array_device_finish_stream(struct mic_array_device_t* dev);

static int mic_array_device_read_stream(
    struct mic_array_device_t* dev, char* buff, unsigned int frame_cnt);

static int mic_array_device_config_stream(
    struct mic_array_device_t* dev, int cmd, char* cmd_buff);

static int mic_array_device_get_stream_buff_size(
    struct mic_array_device_t* dev);

static int mic_array_device_resume_stream(struct mic_array_device_t* dev);

static struct hw_module_methods_t mic_array_module_methods = {
    .open = mic_array_device_open,
};

struct mic_array_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = MIC_ARRAY_HARDWARE_MODULE_ID,
        .name = MODULE_NAME,
        .author = MODULE_AUTHOR,
        .methods = &mic_array_module_methods,
    },
};

static int mic_array_device_open(const struct hw_module_t* module,
    const char* name, struct hw_device_t** device)
{
    int i = 0;
    struct amlogic_mic_array_device* am_dev = NULL;
    struct mic_array_device_t* dev = NULL;
    am_dev = (struct amlogic_mic_array_device*)malloc(
        sizeof(struct amlogic_mic_array_device));
    dev = (struct mic_array_device_t*)am_dev;

    if (!am_dev) {
        ALOGE("MIC_ARRAY: FAILED TO ALLOC SPACE");
        return -1;
    }

    memset(am_dev, 0, sizeof(struct amlogic_mic_array_device));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t*)module;
    dev->common.close = mic_array_device_close;
    dev->start_stream = mic_array_device_start_stream;
    dev->stop_stream = mic_array_device_stop_stream;
    dev->finish_stream = mic_array_device_finish_stream;
    dev->resume_stream = mic_array_device_resume_stream;
    dev->read_stream = mic_array_device_read_stream;
    dev->config_stream = mic_array_device_config_stream;
    dev->get_stream_buff_size = mic_array_device_get_stream_buff_size;

    // use pcm_config_in instead
    dev->channels = pcm_config_in.channels;
    dev->sample_rate = pcm_config_in.rate;
    dev->bit = pcm_format_to_bits(pcm_config_in.format);
    dev->frame_cnt = pcm_config_in.period_size * pcm_config_in.period_count
        * pcm_config_in.channels
        * (pcm_format_to_bits(pcm_config_in.format) >> 3);
    ALOGI("alloc frame buffer size %d", dev->frame_cnt);
    dev->pcm = NULL;
    am_dev->pts = 0;
    am_dev->buffer = (char*)malloc(dev->frame_cnt);

    *device = &(dev->common);
    return 0;
}

static int read_frame(struct mic_array_device_t* dev, char* buffer)
{
    return pcm_read(dev->pcm, buffer, dev->frame_cnt);
}

static void resetBuffer(struct amlogic_mic_array_device* dev) { dev->pts = 0; }

static int read_left_frame(
    struct amlogic_mic_array_device* dev, char* buff, int left)
{
    int ret = 0;
    if (dev->pts == 0) {
        if ((ret = read_frame(dev, dev->buffer)) != 0) {
            ALOGE("read frame return %d, pcm read error", ret);
            resetBuffer(dev);
            return ret;
        }

        memcpy(buff, dev->buffer, left);
        memcpy(dev->buffer, dev->buffer + left, dev->mic_array.frame_cnt - left);
        dev->pts = dev->mic_array.frame_cnt - left;
    } else {
        if (dev->pts >= left) {
            memcpy(buff, dev->buffer, left);
            dev->pts -= left;
            if (dev->pts != 0) {
                memcpy(dev->buffer, dev->buffer + left, dev->pts);
            }
        } else {
            memcpy(buff, dev->buffer, dev->pts);
            left -= dev->pts;
            if ((ret = read_frame(dev, dev->buffer)) != 0) {
                ALOGE("read frame return %d, pcm read error", ret);
                resetBuffer(dev);
                return ret;
            }
            memcpy(buff + dev->pts, dev->buffer, left);
            memcpy(dev->buffer, dev->buffer + left, dev->mic_array.frame_cnt - left);
            dev->pts = dev->mic_array.frame_cnt - left;
        }
    }
    return 0;
}

static int mic_array_device_close(struct hw_device_t* device)
{
    ALOGI("mic array close");
    struct mic_array_device_t* mic_array_device
        = (struct mic_array_device_t*)device;
    struct amlogic_mic_array_device* am_device
        = (struct amlogic_mic_array_device*)mic_array_device;

    // free device
    if (mic_array_device != NULL) {
        free(am_device->buffer);
        free(mic_array_device);

        mic_array_device = NULL;
    }

    return 0;
}

#define PCM_CARD 0
#define PCM_CARD_SPDIF 1
#define PCM_DEVICE 0

static int mic_array_device_start_stream(struct mic_array_device_t* dev)
{
    ALOGI("mic array start stream");
    struct mic_array_device_t* mic_array_device
        = (struct mic_array_device_t*)dev;
    struct pcm* pcm = NULL;

    pcm = pcm_open(PCM_CARD, PCM_DEVICE, PCM_IN, &pcm_config_in);
    if (pcm == NULL && !pcm_is_ready(pcm)) {
        ALOGE("pcm open failed");
        if (pcm != NULL) {
            pcm_close(pcm);
            pcm = NULL;
        }
        return -1;
    }

    mic_array_device->pcm = pcm;
    return 0;
}

static int mic_array_device_stop_stream(struct mic_array_device_t* dev)
{
    ALOGI("stop stream");
    struct pcm* pcm = dev->pcm;
    if (pcm != NULL) {
        pcm_close(pcm);
        pcm = NULL;
    }

    return 0;
}

static int mic_array_device_finish_stream(struct mic_array_device_t* dev)
{
    ALOGE("finish stream is no use");
    return -1;
}

static int mic_array_device_read_stream(
    struct mic_array_device_t* dev, char* buff, unsigned int frame_cnt)
{
    struct pcm* pcm = dev->pcm;
    struct amlogic_mic_array_device* am_dev
        = (struct amlogic_mic_array_device*)dev;
    char *target = NULL;

    int ret = 0;
    int left = 0;
    int size = dev->frame_cnt;
    if (size <= 0) {
        ALOGE("frame cnt lt 0");
        return -1;
    }

    if (buff == NULL) {
        ALOGE("null buffer");
        return -1;
    }

    if (frame_cnt >= size) {
        int cnt = frame_cnt / size;
        int i;
        left = frame_cnt % size;
        for (i = 0; i < cnt; i++) {
            if ((ret = read_frame(dev, buff + i * size)) != 0) {
                ALOGE("read frame return %d, pcm read error", ret);
                resetBuffer(am_dev);
                return ret;
            }
        }

        if (left != 0) {
            if ((ret = read_frame(dev, am_dev->buffer)) != 0) {
                ALOGE("read frame return %d, pcm read error", ret);
                resetBuffer(am_dev);
                return ret;
            }
        }

        target = buff + cnt * size;
    } else {
        target = buff;
        left = frame_cnt;
    }

    if ((ret = read_left_frame(am_dev, target, left)) != 0) {
        ALOGE("read frame return %d, pcm read error", ret);
        resetBuffer(am_dev);
        return ret;
    }

    return ret;
}

static int mic_array_device_config_stream(
    struct mic_array_device_t* dev, int cmd, char* cmd_buff)
{
    ALOGE("do not change the config of xmos in runtime");
    return -1;
}

static int mic_array_device_get_stream_buff_size(
    struct mic_array_device_t* dev)
{
    return dev->frame_cnt;
}

static int mic_array_device_resume_stream(struct mic_array_device_t* dev)
{
    ALOGI("not implmentation");
    return -1;
}
