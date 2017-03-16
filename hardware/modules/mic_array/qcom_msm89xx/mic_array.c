/*************************************************************************
	> File Name: mic_array.c
	> Author: 
	> Mail: 
	> Created Time: Mon May  4 14:22:33 2015
 ************************************************************************/
#define LOG_TAG "mic_array"

#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#undef _GNU_SOURCE
#undef __USE_GNU
#include <sys/syscall.h>
#include <hardware/hardware.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <tinyalsa/asoundlib.h>
#include <cutils/properties.h>

#include "mic/mic_array.h"

#define MODULE_NAME "mic_array"
#define MODULE_AUTHOR "shichaoge@rokid.com"

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


#define MIC_SAMPLE_RATE  48000
#define MIC_CHANNEL 8 //AEC channel included
//32 bit valid bit equals to 4 byte
#define MIC_BYTE_PER_POINT 4//4       
//10ms equals 480(frame) * 8(channel) * 4(32bit)
#define FRAME_COUNT ((MIC_SAMPLE_RATE/100)*MIC_CHANNEL*MIC_BYTE_PER_POINT)

static struct pcm_config pcm_config_iis = {
    .channels = MIC_CHANNEL,
    .rate = MIC_SAMPLE_RATE,
    .period_size = 480,
    .period_count = 8,
    .format = PCM_FORMAT_S24_LE,
};

static struct pcm_config pcm_config_xmos = {
    .channels = MIC_CHANNEL,
    .rate = MIC_SAMPLE_RATE,
    .period_size = 8192,
    .period_count = 16,
    .format = PCM_FORMAT_S32_LE,
};

static struct pcm* pcm = NULL;
FILE *file = NULL;

static int mic_array_device_open (const struct hw_module_t *module, const
        char *name, struct hw_device_t **device);

static int mic_array_device_close (struct hw_device_t *device);

static int mic_array_device_start_stream (struct mic_array_device_t *dev);

static int mic_array_device_stop_stream (struct mic_array_device_t *dev);

static int mic_array_device_finish_stream (struct mic_array_device_t *dev);

static int mic_array_device_read_stream (struct mic_array_device_t *dev, char *buff, uint64_t *frame_cnt);

static int mic_array_device_config_stream (struct mic_array_device_t *dev, int cmd, char *cmd_buff);

static int mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev);

static int mic_array_device_resume_stream (struct mic_array_device_t *dev);

//****************************************************************************************************
static struct hw_module_methods_t mic_array_module_methods = {
    .open = mic_array_device_open,
};

static int debug_fd = 0;

static int mic_timeout_count = 0;

#ifdef LATENCY_DEBUG
static char *raw_buffer = 0;
#endif

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

static void setCurrentThreadAffinityMask(cpu_set_t mask)
{
    int err, syscallres;
    pid_t pid = gettid();
    syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);
    if (syscallres)
    {
        err = errno;
        ALOGI ("set  affinity failed");
    } else {
        ALOGI ("set affinity done");
    }
}

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

static void tinymix_set_value(struct mixer *mixer, const char *control,
                              char **values, unsigned int num_values)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    unsigned int i;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        ALOGE("Invalid mixer control\n");
        return;
    }

    type = mixer_ctl_get_type(ctl);
    num_ctl_values = mixer_ctl_get_num_values(ctl);

    if (isdigit(values[0][0])) {
        if (num_values == 1) {
            /* Set all values the same */
            int value = atoi(values[0]);

            for (i = 0; i < num_ctl_values; i++) {
                if (mixer_ctl_set_value(ctl, i, value)) {
                    ALOGE("Error: invalid value\n");
                    return;
                }
            }
        } else {
            /* Set multiple values */
            if (num_values > num_ctl_values) {
                ALOGE("Error: %d values given, but control only takes %d\n",
						num_values, num_ctl_values);
				return;
			}
			for (i = 0; i < num_values; i++) {
				if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
					ALOGE("Error: invalid value for index %d\n", i);
					return;
				}
			}
		}
	} else {
		if (type == MIXER_CTL_TYPE_ENUM) {
			if (num_values != 1) {
				ALOGE("Enclose strings in quotes and try again\n");
				return;
			}
			if (mixer_ctl_set_enum_by_string(ctl, values[0]))
				ALOGE( "Error: invalid enum value\n");
		} else {
			ALOGE("Error: only enum types can be set with strings\n");
		}
	}
}


#define PCM_CARD 0
#define PCM_DEVICE 0 

static int mic_array_device_open (const struct hw_module_t *module, const
       									char *name, struct hw_device_t **device) {
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

    //use pcm_config_in instead
    dev->channels = MIC_CHANNEL;
    //10ms
    dev->sample_rate = MIC_SAMPLE_RATE;
    dev->bit = 32;
	dev->frame_cnt = FRAME_COUNT;
#ifdef LATENCY_DEBUG
    raw_buffer = malloc (dev->sample_rate/100 * dev->channels * dev->bit / 8 + sizeof (struct timespec));
#endif
    ALOGI ("alloc tmp_frames with size %d", dev->sample_rate/100 * dev->channels * dev->bit / 8);

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

static int mic_array_device_close (struct hw_device_t *device) {
    ALOGI ("pcm close");
    
    struct mic_array_device_t *mic_array_device = (struct mic_array_device_t *)device;
    //free device
    if (mic_array_device != NULL) {
        free (mic_array_device);
        mic_array_device = NULL;
    }
    
    pcm = NULL;  
    return 0;
}

static int mic_array_device_start_stream (struct mic_array_device_t *dev) 
{
	char value[PROPERTY_VALUE_MAX];
	struct mixer *mixer;
	char **values = "1";
	int card;  
    struct mic_array_device_t *mic_array_device = (struct mic_array_device_t *)dev;
#if 0
    cpu_set_t mask;
    CPU_ZERO (&mask);
    CPU_SET (2, &mask);
    setCurrentThreadAffinityMask (mask);
#endif

	//use qualcomm sound card as default
	property_get("ro.boardinfo.usbaudio", value, "1");

    //file = fopen ("/data/record8ch.pcm", "wb");
	
    if (strcmp(value, "0") == 0) {
		//iis direct link cpu
	    card = find_snd ("msm8974-taiko-m");
		if (card  < 0) {
		   ALOGE("Can't find qualcomm sound card");
	       card = PCM_CARD;
		}
		mixer = mixer_open(card);
		if (!mixer) {
			ALOGE("Failed to open mixer +++\n");
			return -1;
		}
		tinymix_set_value(mixer, "MultiMedia1 Mixer QUAT_MI2S_TX" , &values, 1);
		mixer_close(mixer);
		pcm = pcm_open (card, PCM_DEVICE, PCM_IN, &pcm_config_iis);
	 } else {
    	//xmos version
    	ALOGE("xmos card");
		card = find_snd ("xCORE");
		if (card  < 0) {
		   ALOGE("Can't find xmos sound card");
	       card = PCM_CARD;
		}
		pcm = pcm_open (card, PCM_DEVICE, PCM_IN, &pcm_config_xmos); 
    }
	 	  //  pcm = pcm_open (card, PCM_DEVICE, PCM_IN, &pcm_config_in); 
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

static int mic_array_device_stop_stream (struct mic_array_device_t *dev) {  
    if (pcm != NULL) {
        pcm_close (pcm);
        //dev->frame_cnt = 0;
        pcm = NULL;
    }
    
    return 0;
}

static int mic_array_device_finish_stream (struct mic_array_device_t *dev) {
    ALOGE ("finish stream is no use");
    return -1;
}

//Note, the read buffer size is deriverd form dev structer, frame_cnt is a parameter that denote the actual read size
static int mic_array_device_read_stream (struct mic_array_device_t *dev,
        char *buff, uint64_t *frame_cnt) {
    int c = 0;
    int ret = 0;
    int size = dev->frame_cnt;
	
    ret = pcm_read(pcm, buff, size);
    if (ret != 0) {
        *frame_cnt = 0;
        ALOGE ("pcm_read error: %s", strerror(errno));
		//pcm_close(pcm);
		//pcm = NULL;
	} else {
        *frame_cnt = size;
    }

    return ret;
}

static int mic_array_device_config_stream (struct mic_array_device_t *dev,
        int cmd, char *cmd_buff) {
    return -1;
}

static int mic_array_device_get_stream_buff_size (struct mic_array_device_t *dev) {
    int size = dev->frame_cnt; 
    ALOGI ("alloc %d bytes", size);
    
    return size;
}

static int mic_array_device_resume_stream (struct mic_array_device_t *dev) {
    ALOGI ("not implmentation");
    return -1;
}
