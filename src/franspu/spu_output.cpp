#include "spu.h"
#include <minimal.h>
#include <pthread.h>
#include <ao/ao.h>

#ifndef NOSOUND
static const int BITS_PER_SAMPLE = 16;
static const int CHANNELS = 2;

ao_device *ao_dev= NULL;

/* Sound Thread variables */
#define DEFAULT_SAMPLE_NUM_BUFF 16 								// Number of sound buffers
pthread_t gp2x_sound_thread=0;									// Thread for gp2x_sound_thread_play()
volatile int gp2x_sound_thread_exit=0;								// Flag to end gp2x_sound_thread_play() thread
volatile int gp2x_sound_buffer=0;								// Current sound buffer
short gp2x_sound_buffers[32768*DEFAULT_SAMPLE_NUM_BUFF];					// Sound buffers
volatile int sndlen=32768;									// Current sound buffer length
#endif

/* Sound Thread*/
static void *gp2x_sound_thread_play(unsigned sampleRate_, unsigned bufferSize_)
{
#ifndef NOSOUND

	ao_initialize();
		

}
						// Until the end of the sound thread
#endif

/* Mute Sound Thread*/
/*void gp2x_sound_thread_mute(void)
{
#ifndef NOSOUND

	memset(gp2x_sound_buffers,0,32768*DEFAULT_SAMPLE_NUM_BUFF*2);
	sndlen=32768;
#endif
}
*/
/* Start Sound Core */
void SetupSound(unsigned sampleRate_, unsigned bufferSize_)
{
#ifndef NOSOUND
 	int driver = ao_default_driver_id();
 	ao_sample_format format;
 	memset(&format, 0, sizeof(format));
 	format.bits = BITS_PER_SAMPLE;
 	format.channels = CHANNELS;
 	format.rate = sampleRate_;
 	format.byte_format = AO_FMT_NATIVE;

#endif
}

/* Stop Sound Core */
void RemoveSound(void)
{
#ifndef NOSOUND
 	ao_close(ao_dev);
	ao_shutdown();
#endif
}

/* Feed Sound Data */
void SoundFeedStreamData(char *buff, int len)
{
#ifndef NOSOUND
 	ao_play(ao_dev, buff, len);
									// Update the sound buffer length
	//gp2x_sound_buffer=nbuff;								// Update the current sound buffer
#endif
}
