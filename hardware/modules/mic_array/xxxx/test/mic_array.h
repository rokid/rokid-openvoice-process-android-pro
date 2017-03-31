/*************************************************************************
	> File Name: mic_array.h
	> Author: 
	> Mail: 
	> Created Time: Mon May  4 14:19:38 2015
 ************************************************************************/

#ifndef ANDROID_MIC_ARRAY_H
#define ANDROID_MIC_ARRAY_H

#include <stdint.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

#define MIC_ARRAY_HARDWARE_MODULE_ID "mic_array"

struct mic_array_module_t {
    struct hw_module_t common;
};

struct mic_array_device_t {
    struct hw_device_t common;
    
    int fd[2];
    fd_set fdr;
    struct timeval timeout;
    int mics_mapping[8];
    int record_pid;
    int ready;
    int channels;
    int sample_rate;
    int bit;
    uint64_t frame_cnt;

    int (*get_stream_buff_size) (struct mic_array_device_t *dev);
    int (*start_stream) (struct mic_array_device_t *dev);
    int (*stop_stream) (struct mic_array_device_t *dev);
    int (*finish_stream) (struct mic_array_device_t * dev);
    int (*resume_stream) (struct mic_array_device_t *dev);
    int (*read_stream) (struct mic_array_device_t *dev, char *buff, uint64_t *frame_cnt);
    int (*config_stream) (struct mic_array_device_t *dev, int cmd, char *cmd_buff);
};
__END_DECLS

#endif
