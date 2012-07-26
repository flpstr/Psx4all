/*
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>

#include "app_iPhone.h"
#include "JoyPad.h"

unsigned short *BaseAddress;

/* Globals (from obj land) */
extern unsigned long __isLoadState;
extern char *__fileName;
extern int   __mute;
extern float __audioVolume;
extern CoreSurfaceBufferRef __screenSurface;

extern void updateScreen(); 

/* Main Resources */
extern int iphone_main(char* fileName);
//extern void sound_callback(void *userdata, u8 *stream, int length);
extern int   __emulation_run;

char __fileNameTempSave[512];
byte IS_DEBUG;
byte IS_CHANGING_ORIENTATION;
struct app_Preferences preferences;
unsigned long cPad1;

AQCallbackStruct in;
int soundInit = 0;
struct timeval ptv;
int ismobile = 0;
char appworkdir[512];

FILE* fopen_home(char* filename, char* fileop)
{
	char* homeval;
	char  tempdir[1024];

	// Quick fix for fw 1.1.3 messed up HOME value
	FILE* fp;
	sprintf(tempdir, "/var/mobile/%s", filename);
	fp = fopen(tempdir, fileop);
	if(fp != NULL)
	{
		sprintf(appworkdir, "/var/mobile");
		ismobile = 1;
		return fp;
	}

	sprintf(tempdir, "/var/root/%s", filename);
	fp = fopen(tempdir, fileop);
	if(fp != NULL)
	{
		sprintf(appworkdir, "/var/root");
		return fp;
	}

	return NULL;
}

void app_getControllerState(unsigned long  *pad1) {
    *pad1 = cPad1;
}

static void AQBufferCallback(
    void *in,
    AudioQueueRef inQ,
    AudioQueueBufferRef outQB)
{
    AQCallbackStruct * inData;
    short *coreAudioBuffer;
    inData = (AQCallbackStruct *)in;
    coreAudioBuffer = (short*) outQB->mAudioData;

    if (inData->frameCount > 0) {
		AudioQueueSetParameter(inQ, kAudioQueueParam_Volume, __audioVolume);
		sound_callback(NULL, (byte*)coreAudioBuffer, inData->frameCount * 4);
        outQB->mAudioDataByteSize = 4*inData->frameCount;
        AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
    }
}

void app_MuteSound(void) {
    LOGDEBUG("app_MuteSound()");
    if( soundInit == 1 )
    {
	    app_CloseSound();
	}
}

void app_DemuteSound(void) {
    LOGDEBUG("app_DemuteSound()");
    if( soundInit == 0 )
    {
    	app_OpenSound(1, 44100);
	}
}


int app_OpenSound(int samples_per_sync, int sample_rate) {
    Float64 sampleRate = 44100.0;
    int i;

    LOGDEBUG("app_SoundOpen()");
    
    app_MuteSound();
    
    if(preferences.muted)
    {
    	return 0;
    }
#if 1
    soundInit = 1;

    in.mDataFormat.mSampleRate = sampleRate;
    in.mDataFormat.mFormatID = kAudioFormatLinearPCM;
    in.mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger
                                | kAudioFormatFlagIsPacked;
    in.mDataFormat.mBytesPerPacket = 4;
    in.mDataFormat.mFramesPerPacket = 1;
    in.mDataFormat.mBytesPerFrame = 4;
    in.mDataFormat.mChannelsPerFrame = 2;
    in.mDataFormat.mBitsPerChannel = 16;

    /* Pre-buffer before we turn on audio */
    UInt32 err;
    err = AudioQueueNewOutput(&in.mDataFormat,
                      AQBufferCallback,
                      &in,
                      NULL,
                      kCFRunLoopCommonModes,
                      0,
                      &in.queue);
   if (err) {
     LOGDEBUG("AudioQueueNewOutput err %d\n", err);
   }

   in.frameCount = (1024 * (16)) / 4;
   UInt32 bufferBytes = in.frameCount * in.mDataFormat.mBytesPerFrame;

   for (i=0; i<AUDIO_BUFFERS; i++) {
      err = AudioQueueAllocateBuffer(in.queue, bufferBytes, &in.mBuffers[i]);
      if (err) {
	LOGDEBUG("AudioQueueAllocateBuffer[%d] err %d\n",i, err);
      }
      /* "Prime" by calling the callback once per buffer */
      AQBufferCallback (&in, in.queue, in.mBuffers[i]);
   }

   LOGDEBUG("app_QueueSample.AudioQueueStart");
   err = AudioQueueStart(in.queue, NULL);
#endif
    return 0;
}

void app_CloseSound(void) {
    LOGDEBUG("app_CloseSound.AudioQueueDispose()");
    
    soundInit = 0;
#if 1
    AudioQueueDispose(in.queue, true);
#endif
}

void setDefaultPreferences() {
    preferences.frameSkip = 1;
    preferences.debug = 0;
    preferences.canDeleteROMs = 0;
    preferences.landscape = 1;
    preferences.muted = 1;
    preferences.scaled = 1;
    preferences.selectedSkin = 0;
    preferences.interlace = 0;
    preferences.bios = 1;
}

int app_LoadPreferences() {
    FILE *f;
    int r;

    setDefaultPreferences();

    /* Load Preferences */
    f = fopen_home("Library/Preferences/psx4all.v1", "rb");
    if (!f) 
        return -1;
    r = fread(&preferences, sizeof(preferences), 1, f);
    fclose(f);
    if (!r) 
        setDefaultPreferences();
#ifdef GUI_DEBUG
    IS_DEBUG = preferences.debug;
#else
    IS_DEBUG = 0;
#endif
    LOGDEBUG("app_LoadPreferences: Loading preferences");
    return (r) ? 0 : -1;
}

int app_SavePreferences() {
    FILE *f;
    int r;

    /* Load Preferences */
    f = fopen_home("Library/Preferences/psx4all.v1", "wb");
    if (!f) return -1;
    LOGDEBUG("Saving Preferences");
    r = fwrite(&preferences, sizeof(preferences), 1, f);
    fclose(f);
    return (r) ? 0 : -1;
}

#ifdef GUI_DEBUG
void LOGDEBUG(const char *text, ...)
{
  char debug_text[1024];
  va_list args;
  FILE *f;

  if (!IS_DEBUG) return;

  va_start (args, text);
  vsnprintf (debug_text, sizeof (debug_text), text, args);
  va_end (args);

  f = fopen("/tmp/psx4all.debug", "a");
  fprintf(f, "%s\n", debug_text);
  fclose(f);
}
#endif

void *app_Thread_Start(void *args) {
    iphone_main(__fileName);
}

void app_Halt(void) {
	app_MuteSound();
    __emulation_run = 0;
}

void app_Resume(void)
{
	sprintf(__fileName, "%s", __fileNameTempSave);
}

void app_SetSvsFile(char* filename)
{
	sprintf(__fileNameTempSave, "%s", filename);
}

void app_DeleteTempState(void)
{
	unlink(__fileNameTempSave);
}

int app_LoadROM(const char *fileName) {

    return 0;
}

unsigned long gp2x_joystick_read(void)
{
	return	cPad1 | Read_joypad();
}
