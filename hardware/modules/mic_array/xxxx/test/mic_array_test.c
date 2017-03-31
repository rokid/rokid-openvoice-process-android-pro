
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#include <hardware/hardware.h>

#define MIC_ARRAY_HARDWARE_MODULE_ID "mic_array"

struct mic_array_module_t {
	struct hw_module_t common;
} *module;

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
	int (*read_stream) (struct mic_array_device_t *dev, char *buff, int *frame_cnt);
	int (*config_stream) (struct mic_array_device_t *dev, int cmd, char *cmd_buff);
} *mic_array_device;

static inline int mic_array_device_open (const hw_module_t *module, struct mic_array_device_t **device) {
	return module->methods->open (module, MIC_ARRAY_HARDWARE_MODULE_ID, (struct hw_device_t **) device);
}


void main()
{
	int ret;
	int mic_array_frame_size;
	int read_size = 65535;

	char *Buff;
	FILE *fp = NULL;
	int frame = 5535;

	if (hw_get_module (MIC_ARRAY_HARDWARE_MODULE_ID, (const struct hw_module_t **)&module) == 0) {
		//open mic array
		if (0 != mic_array_device_open(&module->common, &mic_array_device)) {
			return;
		}
	}

	fp = fopen("/data/debug.pcm", "wb");


	printf("File open\n");
	mic_array_frame_size = mic_array_device->get_stream_buff_size(mic_array_device);
	mic_array_device->start_stream(mic_array_device);
	printf("start stream\n");
	Buff = (char *)malloc(read_size);

	while(frame > 0){
		ret = mic_array_device->read_stream(mic_array_device, Buff, &read_size);
		if(ret != 0) {
			printf("read stream failed\n");
		}
		fwrite (Buff, sizeof(char), 480*6*3, fp);
		frame--;
	}
	fclose(fp);
	mic_array_device->stop_stream(mic_array_device);
}
