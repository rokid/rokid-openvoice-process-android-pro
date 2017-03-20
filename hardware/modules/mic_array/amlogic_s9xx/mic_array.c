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
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#undef _GNU_SOURCE
#undef __USE_GNU
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


#define MIC_ARRAY_PATH "./data/mic_recorder"
#define MIC_ARRAY_PATH_DEBUG "./data/mic_recorder_debug"
#define MIC_ARRAY_BIN "mic_recorder"
#define MIC_ARRAY_BIN_DEBUG "mic_recorder_debug"
//#define LATENCY_DEBUG 0

#define DEVICE_STAT_ERR     0
#define DEVICE_STAT_INIT    1
#define DEVICE_STAT_START   2
#define DEVICE_STAT_STOP    3
#define DEVICE_STAT_FINISH  4

//XMOS
//
//
static struct pcm_config pcm_config_in =  {
    .channels = 8,
    .rate = 48000,
    .period_size = 1024,
    .period_count = 4,
    .format = PCM_FORMAT_S32_LE,
};

static struct pcm* pcm = NULL;
FILE *file = NULL;


static int xmos_mic_array_device_open (const struct hw_module_t *module, const
        char *name, struct hw_device_t **device);

static int xmos_mic_array_device_close (struct hw_device_t *device);

static int xmos_mic_array_device_start_stream (struct mic_array_device_t *dev);

static int xmos_mic_array_device_stop_stream (struct mic_array_device_t *dev);

static int xmos_mic_array_device_finish_stream (struct mic_array_device_t *dev);

static int xmos_mic_array_device_read_stream (struct mic_array_device_t *dev, char *buff, uint64_t *frame_cnt);

static int xmos_mic_array_device_config_stream (struct mic_array_device_t *dev, int cmd, char *cmd_buff);

static int xmos_mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev);


static int xmos_mic_array_device_resume_stream (struct mic_array_device_t *dev);
//***************************************************
//Legacy CPLD mic array
//open and close device interface
//***************************************************

static int mic_array_device_open (const struct hw_module_t *module, const char *name, struct hw_device_t **device);

static int mic_array_device_close (struct hw_device_t *device);

static int mic_array_device_start_stream (struct mic_array_device_t *dev);

static int mic_array_device_stop_stream (struct mic_array_device_t *dev);

static int mic_array_device_finish_stream (struct mic_array_device_t *dev);

static int mic_array_device_resume_stream (struct mic_array_device_t *dev);

static int mic_array_device_read_stream (struct mic_array_device_t *dev, char *buff, uint64_t *frame_cnt);

static int mic_array_device_config_stream (struct mic_array_device_t *dev, int cmd, char *cmd_buff);

static int mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev);


//****************************************************************************************************

//ok return 0; else return -1;
static int mic_check_channel(const char *buff, int ch) {
    int i = 0;
    for (i = 0; i < 480; i++) {
        if (buff[3 * 10 * i + ch] != 0) {
            return 0;
        }
        if (buff[3 * 10 * i + ch + 1] != 0) {
            return 0;
        }
        if (buff[3 * 10 * i + ch + 2] != 0) {
            return 0;
        }
    }
    return -1;
}

static int mic_check(const char * buff) {
    //mic format is 480 * 10 * 3
    int i = 0;
    for (i = 2; i < 10; i++) {
        //check ch2
        if (-1 == mic_check_channel (buff, i)) {
            ALOGE ("!!!!!!!!!!!!!mic channel %d is down!!!!!!!!!!!!!!!", i);
            return i;
        }
    }
    return -1;
}


static struct hw_module_methods_t mic_array_module_methods = {
    //.open = mic_array_device_open,
    .open = xmos_mic_array_device_open,
};

static int debug_fd = 0;

static int mic_timeout_count = 0;

#ifdef LATENCY_DEBUG
static char *raw_buffer = 0;
#endif

static int mics_mapping[] = {2, 4, 6, 1 ,3 ,5 ,7, 0};
static pthread_t log_th;
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


static void *log_thread (void *arg)
{
    char buff[4096];
    struct mic_array_device_t *dev = (struct mic_array_device_t *)arg;
    if (dev == NULL) {
        ALOGE ("ERROR log thread read null");
        return NULL;
    }
    int read_size = 0;

    while (1) {
        memset (buff, 0, sizeof(buff));
        read_size = read (dev->fd[1], buff, 4096);
        if (read_size < 0) {
            ALOGE ("ERROR read -1, try again later?");
            close (dev->fd[1]);
            return NULL;
        } else if (read_size == 0) {
            ALOGE ("ERROR read eof");
            close (dev->fd[1]);
            return NULL;
        }

        ALOGE ("LOG: %s", buff);
    }

    return NULL;
}

static void setCurrentThreadAffinityMask(cpu_set_t mask)
{
    int err, syscallres;
    pid_t pid = gettid();
    syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);
    if (syscallres)
    {
        err = errno;
        ALOGI ("set affinity failed");
    } else {
        ALOGI ("set affinity done");
    }
}


//10 ms frame size
//maybe no use in this hardware
static void preprocess_frames (char *frames, int frameSizeInByte, int channels, float *regroup_frames)
{
    char k0, k1, k2, k3;
    int  i, j = 0, k = 0, p = 0;
    for (i = 0; i < frameSizeInByte * channels; i++) {
        j = mics_mapping[i % channels];
        k = i / channels;

        k0 = frames[0 + 3 * i];
        k1 = frames[1 + 3 * i];
        k2 = frames[2 + 3 * i];
        k3 = 0;

        if ((k2 & 0x80) != 0)
            k3 = -1;

        p = (int)((k0 & 0xff)|(k1 & 0xff) << 8|(k2 & 0xff) << 16|(k3 & 0xff) << 24);
        regroup_frames[k + j * frameSizeInByte] = (float)p;
    }
}


static int mic_array_device_open (const struct hw_module_t *module, const char *name, struct hw_device_t **device)
{
    int i = 0;
    struct mic_array_device_t *dev = NULL;
    dev = (struct mic_array_device_t *)malloc(sizeof (struct mic_array_device_t));

    if (!dev) {
        ALOGE ("MIC_ARRAY: FAILED TO ALLOC SPACE");
        return -1;
    }

    memset (dev, 0, sizeof (struct mic_array_device_t));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t *)module;
    dev->common.close = mic_array_device_close;
    dev->start_stream = mic_array_device_start_stream;
    dev->stop_stream = mic_array_device_stop_stream;
    dev->finish_stream = mic_array_device_finish_stream;
    dev->resume_stream = mic_array_device_resume_stream;
    dev->read_stream = mic_array_device_read_stream;
    dev->config_stream = mic_array_device_config_stream;
    dev->get_stream_buff_size = mic_array_device_get_stream_buff_size;
    dev->record_pid = 0;
    memset ((char *)dev->fd, 0, sizeof (int) * 2);

    dev->channels = 10;
    //10ms
    dev->sample_rate = 48000;
    dev->bit = 24;
    dev->frame_cnt = 0;
#ifdef LATENCY_DEBUG
    raw_buffer = malloc (dev->sample_rate/100 * dev->channels * dev->bit / 8 + sizeof (struct timespec));
#endif
    ALOGI ("alloc tmp_frames with size %d", dev->sample_rate/100 * dev->channels * dev->bit / 8);

    for (i = 0; i < 8; i++) {
        dev->mics_mapping[i] = mics_mapping[i];
    }

    dev->timeout.tv_sec = 1;
    //500ms
    dev->timeout.tv_usec = 0;
    FD_ZERO (&dev->fdr);

    //init ok
    dev->ready = DEVICE_STAT_INIT;
    *device = &(dev->common);
    /*
    debug_fd = open("/data/mics.bin", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (debug_fd < 0) {
        ALOGE("open error\n");
        return -1;
    }
    */

    return 0;
}

static int mos_mic_array_device_open (const struct hw_module_t *module, const char *name, struct hw_device_t **device)
{
    int i = 0;
    struct mic_array_device_t *dev = NULL;
    dev = (struct mic_array_device_t *)malloc(sizeof (struct mic_array_device_t));

    if (!dev) {
        ALOGE ("MIC_ARRAY: FAILED TO ALLOC SPACE");
        return -1;
    }

    memset (dev, 0, sizeof (struct mic_array_device_t));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t *)module;
    dev->common.close = mic_array_device_close;
    dev->start_stream = mic_array_device_start_stream;
    dev->stop_stream = mic_array_device_stop_stream;
    dev->finish_stream = mic_array_device_finish_stream;
    dev->resume_stream = mic_array_device_resume_stream;
    dev->read_stream = mic_array_device_read_stream;
    dev->config_stream = mic_array_device_config_stream;
    dev->get_stream_buff_size = mic_array_device_get_stream_buff_size;
    dev->record_pid = 0;
    memset ((char *)dev->fd, 0, sizeof (int) * 2);

    dev->channels = 10;
    //10ms
    dev->sample_rate = 48000;
    dev->bit = 24;
    dev->frame_cnt = 0;
#ifdef LATENCY_DEBUG
    raw_buffer = malloc (dev->sample_rate/100 * dev->channels * dev->bit / 8 + sizeof (struct timespec));
#endif
    ALOGI ("alloc tmp_frames with size %d", dev->sample_rate/100 * dev->channels * dev->bit / 8);

    for (i = 0; i < 8; i++) {
        dev->mics_mapping[i] = mics_mapping[i];
    }

    dev->timeout.tv_sec = 1;
    //500ms
    dev->timeout.tv_usec = 0;
    FD_ZERO (&dev->fdr);

    //init ok
    dev->ready = DEVICE_STAT_INIT;
    *device = &(dev->common);
    /*
    debug_fd = open("/data/mics.bin", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (debug_fd < 0) {
        ALOGE("open error\n");
        return -1;
    }
    */

    return 0;
}






static int mic_array_device_close (struct hw_device_t *device)
{
    struct mic_array_device_t *mic_array_device = (struct mic_array_device_t *)device;

    if (mic_array_device) {
        //close here
        switch (mic_array_device->ready) {
        //when occur err directly exit
        case DEVICE_STAT_ERR:
            ALOGI ("close with stat err");
            break;
        case DEVICE_STAT_INIT:
            ALOGI ("close with stat init");
            break;
        case DEVICE_STAT_START:
            ALOGI ("close with stat start");
            mic_array_device->stop_stream (mic_array_device);
        //go through
        case DEVICE_STAT_STOP:
            ALOGI ("close with stat stop");
            mic_array_device->finish_stream (mic_array_device);
            break;
        case DEVICE_STAT_FINISH:
            ALOGI ("close with stat finish");
            break;
        default:
            ALOGI ("unknown stat %d", mic_array_device->ready);
        }
#ifdef LATENCY_DEBUG
        if (raw_buffer != NULL)
            free (raw_buffer);
#endif

        mic_timeout_count = 0;

        //free device
        if (mic_array_device != NULL)
            free (mic_array_device);
    }

    return 0;
}

static int mic_array_device_start_stream (struct mic_array_device_t *dev)
{
    ALOGI ("MIC_ARRAY: start stream in debug");
    if (dev->ready != DEVICE_STAT_INIT) {
        ALOGE ("stat err need DEVICE_STAT_INIT");
        return -1;
    }

    if (socketpair (AF_UNIX, SOCK_STREAM, 0, dev->fd) < 0) {
        ALOGE ("open socket failed");
        return -1;
    }
    //64 frame buff
#define DEFAULT_BUFF_SIZE 32 * 1024 * 1024
//#define DEFAULT_BUFF_SIZE 64 * 1024

    int buff_size = DEFAULT_BUFF_SIZE;
    setsockopt (dev->fd[0], SOL_SOCKET, SO_SNDBUF, &buff_size, sizeof (buff_size));
    setsockopt (dev->fd[0], SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof (buff_size));
    setsockopt (dev->fd[1], SOL_SOCKET, SO_SNDBUF, &buff_size, sizeof (buff_size));
    setsockopt (dev->fd[1], SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof (buff_size));

    buff_size = 0;
    socklen_t len=sizeof(int);
    getsockopt (dev->fd[0], SOL_SOCKET, SO_SNDBUF, &buff_size, &len);
    ALOGI ("dev->fd[0] SO_SNDBUF set to %d", buff_size);
    getsockopt (dev->fd[0], SOL_SOCKET, SO_RCVBUF, &buff_size, &len);
    ALOGI ("dev->fd[0] SO_RCVBUF set to %d", buff_size);
    getsockopt (dev->fd[1], SOL_SOCKET, SO_SNDBUF, &buff_size, &len);
    ALOGI ("dev->fd[1] SO_SNDBUF set to %d", buff_size);
    getsockopt (dev->fd[1], SOL_SOCKET, SO_RCVBUF, &buff_size, &len);
    ALOGI ("dev->fd[1] SO_RCVBUF set to %d", buff_size);

    //pthread_create (&log_th, NULL, log_thread, (void *)dev);
    dev->record_pid = fork();

    if (dev->record_pid == 0) {
        //set to core 1
        cpu_set_t mask;
        CPU_ZERO (&mask);
        //the same core
        CPU_SET (3, &mask);
        setCurrentThreadAffinityMask (mask);

        dup2 (dev->fd[0], STDOUT_FILENO);
        close (dev->fd[0]);
        dup2 (dev->fd[1], STDIN_FILENO);
        close (dev->fd[1]);

        //with -20 nice
        if (0 != setpriority (PRIO_PROCESS, getpid(), -20))
            ALOGE ("setpriority failed");

        //maybe need extra arguments in the future
        //never return
#ifdef LATENCY_DEBUG
        if (0 != execl (MIC_ARRAY_PATH_DEBUG, MIC_ARRAY_BIN_DEBUG, NULL)) {
            ALOGE ("execl recorder subroutines failed");
            return -1;
        }
#else
        if (0 != execl (MIC_ARRAY_PATH, MIC_ARRAY_BIN, NULL)) {
            ALOGE ("execl recorder subroutines failed");
            return -1;
        }
#endif

    } else if (dev->record_pid > 0) {
        //close (dev->fd[1]);
        //listen to fd0
        FD_ZERO (&dev->fdr);
        FD_SET (dev->fd[0], &dev->fdr);
        //setup new thread to capture log
        ALOGI ("start log_thread");
        pthread_create (&log_th, NULL, log_thread, (void *)dev);

        //set to core 2
        cpu_set_t mask;
        CPU_ZERO (&mask);
        CPU_SET (2, &mask);
        setCurrentThreadAffinityMask (mask);
    } else {
        ALOGE ("fork failed");
        return -1;
    }

    dev->ready = DEVICE_STAT_START;

    return 0;
}


static int mic_array_device_stop_stream (struct mic_array_device_t *dev)
{
    ALOGI ("MIC_ARRAY: stop stream");
    return 0;
}

static int mic_array_device_finish_stream (struct mic_array_device_t *dev)
{
    ALOGI ("MIC_ARRAY: finish stream");
    return 0;
}

static int mic_array_device_resume_stream (struct mic_array_device_t *dev)
{
    ALOGI ("MIC_ARRAY: resume stream");
    return 0;
}

//always read samplerate / 100 * channels * bit / 8
static int mic_array_device_read_stream (struct mic_array_device_t *dev, char *buff, uint64_t *frame_cnt)
{
    if (dev->ready != DEVICE_STAT_START) {
        ALOGE ("device stat not start");
        return -1;
    }
    int ret = 0;
    dev->frame_cnt ++;
    //char *raw_buffer = malloc (dev->sample_rate / 100 * dev->channels * dev->bi
    //t / 8 + sizeof (struct timespec));
#ifdef LATENCY_DEBUG
    int read_size = dev->sample_rate / 100 * dev->channels * dev->bit /8 + sizeof (struct timespec);
#endif
    int read_10ms_size = dev->sample_rate / 100 * dev->channels * dev->bit / 8;// + sizeof (struct timespec);
    //int read_10ms_size = 64 * 1024;
    struct timespec ts1;
    struct timespec *t0 = NULL;
    long us = 0;
    FD_ZERO (&dev->fdr);
    FD_SET (dev->fd[0], &dev->fdr);
    dev->timeout.tv_sec = 1;
    dev->timeout.tv_usec = 0;
    switch (select(dev->fd[0]+1, &dev->fdr, NULL, NULL, &dev->timeout)) {
        case -1:
            ALOGE ("read from child error");
            ret = -1;
            break;
        case 0:
            ALOGE ("read from child timeout or end");
            ++mic_timeout_count;
            if (mic_timeout_count >= 5)
                ret = -1;
            break;
        default:
            if (FD_ISSET(dev->fd[0], &dev->fdr)) {
#ifdef LATENCY_DEBUG
                ret = read (dev->fd[0], (char *)raw_buffer, read_size);
                t0 = (struct timespec *)((char *)raw_buffer + dev->sample_rate / 100 * dev->channels * dev->bit / 8);
                clock_gettime (CLOCK_MONOTONIC, &ts1);
                us = (ts1.tv_sec * 1000000000 + ts1.tv_nsec - t0->tv_sec * 1000000000 - t0->tv_nsec)/1000;
                if (us >= 100000)
                    ALOGE ("delay ->>> %ld ms", us/1000);
                //        write (debug_fd, buff, read_10ms_size);
                memcpy (buff, raw_buffer, read_10ms_size);
                memset (raw_buffer, 0, read_size);
                if (ret <= 0) {
                    ret = -1;
                } else {
                    ret = (int)us/1000;
                }
#else
                ret = read (dev->fd[0], (char *)buff, read_10ms_size);
#endif
            }
        }

    *frame_cnt = dev->frame_cnt;
    return ret;
}

static int mic_array_device_config_stream (struct mic_array_device_t *dev, int cmd, char *cmd_buff)
{
    return 0;
}

//10ms
static int mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev)
{
    return dev->sample_rate / 100 * dev->channels * dev->bit / 8 ;
//    return 64 * 1024;
}

//XMOS implementation 
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

int find_snd(const char *snd)
{
	char *path="/proc/asound/cards";
	FILE *fs;
	char buf[4096];
	char *b, *e;
	int card = -1;
	int len;

	if (!(fs = fopen(path, "r"))) {
		perror("fopen");
		return errno;
	}

	len = fread(buf, 1, sizeof(buf)-1, fs);
	buf[len-1] = '\0';
	fclose(fs);


	b = buf;
	while (e = strchr(b, '\n')) {
		*e='\0';
		if (strstr(b, snd)) {
			card = atoi(b);
			break;
		}
		b = e+1;
	}
	
	ALOGI ("find -> %d", card);
	return card;
}


static int xmos_mic_array_device_open (const struct hw_module_t *module, const
        char *name, struct hw_device_t **device) {
    int i = 0;
    struct mic_array_device_t *dev = NULL;
    dev = (struct mic_array_device_t *)malloc(sizeof (struct mic_array_device_t));

    if (!dev) {
        ALOGE ("MIC_ARRAY: FAILED TO ALLOC SPACE");
        return -1;
    }
    //file = fopen ("/data/record8ch.pcm", "wb");

    memset (dev, 0, sizeof (struct mic_array_device_t));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t *)module;
    dev->common.close = xmos_mic_array_device_close;
    dev->start_stream = xmos_mic_array_device_start_stream;
    dev->stop_stream = xmos_mic_array_device_stop_stream;
    dev->finish_stream = xmos_mic_array_device_finish_stream;
    dev->resume_stream = xmos_mic_array_device_resume_stream;
    dev->read_stream = xmos_mic_array_device_read_stream;
    dev->config_stream = xmos_mic_array_device_config_stream;
    dev->get_stream_buff_size = xmos_mic_array_device_get_stream_buff_size;
    
    //xmos no use
    dev->record_pid = 0;
    memset ((char *)dev->fd, 0, sizeof (int) * 2);

    //use pcm_config_in instead
    dev->channels = 8;
    //10ms
    dev->sample_rate = 48000;
    dev->bit = 32;
	dev->frame_cnt = 1024 * 10;
#ifdef LATENCY_DEBUG
    raw_buffer = malloc (dev->sample_rate/100 * dev->channels * dev->bit / 8 + sizeof (struct timespec));
#endif
    ALOGI ("alloc tmp_frames with size %d", dev->sample_rate/100 * dev->channels * dev->bit / 8);

    for (i = 0; i < 8; i++) {
        dev->mics_mapping[i] = mics_mapping[i];
    }

    dev->timeout.tv_sec = 1;
    //500ms
    dev->timeout.tv_usec = 0;
    FD_ZERO (&dev->fdr);
#if 0
    cpu_set_t mask;
    CPU_ZERO (&mask);
    CPU_SET (2, &mask);
    setCurrentThreadAffinityMask (mask);
#endif
    pcm = NULL;

    //init ok
    dev->ready = DEVICE_STAT_INIT;
    *device = &(dev->common);
    return 0;
}


inline static void xmos_frames_format (char *in_buff, char *out_buff) {
    int i = 0;
    int j = 16;

    //can 0
    out_buff[0] = in_buff[5];
    out_buff[1] = in_buff[6];
    out_buff[2] = in_buff[7];
        //skip bit 24 - 32
    //can 1
    out_buff[3] = in_buff[13];
    out_buff[4] = in_buff[14];
    out_buff[5] = in_buff[15];
    //ch 0
    out_buff[6] = in_buff[1];
    out_buff[7] = in_buff[2];
    out_buff[8] = in_buff[3];
    //ch 1
    out_buff[9] = in_buff[9];
    out_buff[10] = in_buff[10];
    out_buff[11] = in_buff[11];
    //ch 2 - ch 7
    for (i = 12; i < 30;) {
        out_buff[i] = in_buff[j + 1];
        out_buff[i + 1] = in_buff[j + 2];
        out_buff[i + 2] = in_buff[j + 3];
        i+= 3;
        j+= 8;
    }
    //build 16[2 * 8] -> 10
}

static void xmos_total_format (char *in_buff, char *out_buff) {
    int i = 0;
    for (i = 0; i < 480; i++) {
        xmos_frames_format (&in_buff[4 * 16 * i], &out_buff[3 * 10 * i]); 
    }
}

static int xmos_mic_array_device_close (struct hw_device_t *device) {
    ALOGI ("xmos pcm close");
    
    struct mic_array_device_t *mic_array_device = (struct mic_array_device_t *)device;
    //free device
    if (mic_array_device != NULL) {
        free (mic_array_device);
        mic_array_device = NULL;
    }
    
    pcm = NULL;  
    return 0;
}

#define PCM_CARD 0
#define PCM_CARD_SPDIF 1

#define PCM_DEVICE 0 

static int xmos_mic_array_device_start_stream (struct mic_array_device_t *dev) {
    ALOGI ("xmos start stream");

    struct mic_array_device_t *mic_array_device = (struct mic_array_device_t *)dev;
#if 0
    cpu_set_t mask;
    CPU_ZERO (&mask);
    CPU_SET (2, &mask);
    setCurrentThreadAffinityMask (mask);
#endif

    //int card = find_snd ("AML-M8AUDIO");
    int card = 0;
    if (card  < 0) {
        ALOGE ("cannot ROKID sound card");
        card = PCM_CARD;
    } else {
        ALOGI ("find x20 sound device with card %d", card);
    }

    pcm = pcm_open (card, PCM_DEVICE, PCM_IN, &pcm_config_in); 
    if (pcm == NULL && !pcm_is_ready (pcm)) {
        ALOGE ("pcm open failed");
        if (pcm != NULL) { 
            pcm_close (pcm);
            pcm = NULL;
        }
        return -1;
    }

    return 0;
}

static int xmos_mic_array_device_stop_stream (struct mic_array_device_t *dev) {
    ALOGI ("stop stream");
    
    if (pcm != NULL) {
        pcm_close (pcm);
        //dev->frame_cnt = 0;
        pcm = NULL;
    }
    
    return 0;
}

static int xmos_mic_array_device_finish_stream (struct mic_array_device_t *dev) {
    ALOGE ("finish stream is no use");
    return -1;
}

static int xmos_mic_array_device_read_stream (struct mic_array_device_t *dev,
        char *buff, uint64_t *frame_cnt) {
    //char tmp_buff[480 * 16 * 4];
    int c = 0;
/*
    if (dev->frame_cnt == 0) {
        ALOGE ("size is 0");
        return -1;
    }
*/
	/*
    while (pcm == NULL && !pcm_is_ready(pcm)) {
        usleep (500000);
        int card = find_snd ("ROKID");
        if (card  < 0) {
            ALOGE ("cannot ROKID sound card");
            card = PCM_CARD;
        } else {
            ALOGI ("find x20 sound device with card %d", card);
        }

        //open
        pcm = pcm_open (card, PCM_DEVICE, PCM_IN, &pcm_config_in); 
    } 
	*/

    int ret = 0;
    int size = dev->frame_cnt;
    ret = pcm_read(pcm, buff, 1024 * 4);
    //ALOGE("===================lazymedea===============");
    if (ret != 0) {
        *frame_cnt = 0;
        ALOGE ("pcm_read error: %s", strerror(errno));
		ALOGE ("read snd cards %d", find_snd ("xCORE"));
		//pcm_close(pcm);
		//pcm = NULL;
	} else {
        *frame_cnt = size;
    }

    //now change frames back to legacy
    //
    //xmos_total_format (tmp_buff, buff); 

   /* 
    if ((c = mic_check(buff)) != -1) {
        ALOGE ("channel breaks");
        return -1;
    }
    */

    //fwrite (buff, 1, 480 * 10 * 3, file);

    return ret;
}

static int xmos_mic_array_device_config_stream (struct mic_array_device_t *dev,
        int cmd, char *cmd_buff) {
    ALOGE ("do not change the config of xmos in runtime");
    return -1;
}

static int xmos_mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev) {
    //int size = pcm_frames_to_bytes (pcm, pcm_get_buffer_size(pcm));
    //for legacy compatible
    int size = 1024 * 4; 
    ALOGI ("alloc %d bytes", size);
    
    //dev->frame_cnt = size;
    return size;
}


static int xmos_mic_array_device_resume_stream (struct mic_array_device_t *dev) {
    ALOGI ("not implmentation");
    return -1;
}
