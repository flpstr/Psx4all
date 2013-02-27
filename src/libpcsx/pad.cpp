#include "common.h"

/* DEBUG */
#ifdef WITH_REG_STATS
extern u32 reg_count[32]; 
extern u32 reg_mapped_count[32];
#endif

s32  PadFlags = 0;

/* eventually these externs will no longer be needed when I decide on final implementation */
extern u32 firstTime;
extern u32 laceCnt;
extern s32 frameRate;
extern s8 *packfile;
extern u32 displayNumber;
extern s32 drawToDrawingArea;
extern u32 mStopEmulation;
extern int iUseXA;
extern int iSoundMuted;
extern s32 DrawingCount[4];
extern int  skipCount;
extern int  skipRate;
extern long SkipReset;
extern bool displayFrameInfo;
//extern bool displayVideoMemory;
//extern bool primitiveDebugMode;
extern int psx4all_emulating;
/*
typedef struct {
	u32 Version;b
	u32 GP1;
	u32 Control[256];
	u8 FrameBuffer[0x100000];
} GPUFREEZE;

s32 GPU_freeze(u32 p1, GPUFREEZE * p2);
*/
s32 PAD1_init(s32 flags) {
	PadFlags |= flags;
	
	return 0;
}

s32 PAD2_init(s32 flags) {
	PadFlags |= flags;
	
	return 0;
}

s32 PAD1_shutdown(void) {
	return 0;
}


s32 PAD2_shutdown(void) {
	return 0;
}


s32 PAD1_open(void)
{
	return 0;
}


s32 PAD2_open(void)
{
	return 0;
}


s32 PAD1_close(void) {
	return 0;
}


s32 PAD2_close(void) {
	return 0;
}

extern int skipValue;
extern int skipCountTable[];
extern int skipRateTable[];

static int keystate = 0;

u16 ReadZodKeys()
{
	return keystate;
}

void UpdateZodKeys()
{
	u16 pad_status = 0xffff;
	u32 keys = gp2x_joystick_read();

/* DEBUG */
#ifdef WITH_REG_STATS
	if( keys & GP2X_PUSH )
	{
		int regcounter=0;
		for(regcounter = 0; regcounter < 32; regcounter++)
		{
			reg_count[regcounter] = 0; 
			reg_mapped_count[regcounter] = 0;
		}
	}
#endif

	//Read key state
#ifndef IPHONE	
	if (
		keys & GP2X_L && keys & GP2X_R &&
		keys & GP2X_A && keys & GP2X_B &&
		keys & GP2X_X && keys & GP2X_Y )
	{
		psx4all_prof_show();
		psxShutdown();
		CloseComponents();
		
		exit(0);
	}
#endif
	if( (keys & GP2X_SELECT && keys & GP2X_Y) || (keys & GP2X_VOL_DOWN) )
	{
#ifdef GP2X_SDL
		// Menu system
		u32 w = sdlscreen->w;
		u32 h = sdlscreen->h;

		gp2x_change_res(320, 240);
#endif

		if( 0 != SelectGame() )
		{
			psx4all_emulating=1;
			psx4all_prof_start(PROFILER_TOTAL);
			psxCpu->Execute();
			psx4all_prof_end(PROFILER_TOTAL);
			psx4all_emulating=0;

			psx4all_prof_show();
		}
#ifdef GP2X_SDL
		gp2x_change_res(w, h);	
#endif
	}
	else if (keys & GP2X_Y)
	{
		pad_status &= ~(1<<15); /* square */
	}

/*
	if( keys & GP2X_PUSH && displayVideoMemory)
    primitiveDebugMode = true;
  else
    primitiveDebugMode = false;
*/
	if(	keys & GP2X_L && keys & GP2X_START &&
		keys & GP2X_DOWN )
	{
		// Underclock
    u32 newClock = (PsxClockMult>2) ? (PsxClockMult-1) : PsxClockMult;
    UpdateClock (newClock);
    gp2x_printf(NULL, 0, 8, "PSX CLOCK %d %%            ", newClock);
		gp2x_video_flip();
		gp2x_timer_delay(250);
	}


	if( 	keys & GP2X_SELECT &&
		keys & GP2X_UP )
	{
		// Overclock
    u32 newClock = (PsxClockMult<200) ? (PsxClockMult+1) : PsxClockMult;
    UpdateClock (newClock);
    gp2x_printf(NULL, 0, 8, "PSX CLOCK %d %%            ", newClock);
		gp2x_video_flip();
		gp2x_timer_delay(250);
	}

	if(	keys & GP2X_SELECT &&
		keys & GP2X_DOWN )
	{
		// Underclock
		PsxCycleMult = (PsxCycleMult>1) ? (PsxCycleMult-1) : PsxCycleMult;
		gp2x_printf(NULL, 0, 8, "PSX CYCLE %d               ", PsxCycleMult);
#ifdef DYNAREC
		//psxCpu->Reset();
#endif
		gp2x_video_flip();
		gp2x_timer_delay(250);
	}


	if( 	keys & GP2X_R && keys & GP2X_START &&
		keys & GP2X_UP )
	{
		// Overclock
    		PsxCycleMult = (PsxCycleMult<10) ? (PsxCycleMult+1) : PsxCycleMult;
    		gp2x_printf(NULL, 0, 8, "PSX CYCLE %d               ", PsxCycleMult);
#ifdef DYNAREC
		//psxCpu->Reset();
#endif
		gp2x_video_flip();
		gp2x_timer_delay(250);
	}

	if( keys & GP2X_SELECT && keys & GP2X_B )
	{
		if( skipValue > 0 )
		{
			skipValue--;
			skipCount = skipCountTable[skipValue];
			skipRate = skipRateTable[skipValue];
			gp2x_printf(NULL, 0, 8, "Frameskip %d/%d           ", skipCount, skipRate);
			gp2x_video_flip();
			gp2x_timer_delay(250);
		}
	}
	else if (keys & GP2X_B)
	{
		pad_status &= ~(1<<14); /* cross */
	}

	if( keys & GP2X_SELECT && keys & GP2X_A )
	{
		if( skipValue < 8 )
		{
			skipValue++;
			skipCount = skipCountTable[skipValue];
			skipRate = skipRateTable[skipValue];
			gp2x_printf(NULL, 0, 8, "Frameskip %d/%d           ", skipCount, skipRate);
			gp2x_video_flip();
			gp2x_timer_delay(250);
		}
	}
	else if (keys & GP2X_A)
	{
		pad_status &= ~(1<<13); /* circle */
	}

	if ( keys & GP2X_SELECT && keys & GP2X_LEFT ) {
		displayFrameInfo = false;
	}
	else if (keys & GP2X_LEFT)
	{
		pad_status &= ~(1<<7);
	}

	if ( keys & GP2X_SELECT && keys & GP2X_RIGHT ) {
		displayFrameInfo = true;
	}
	else if (keys & GP2X_RIGHT)
	{
		pad_status &= ~(1<<5);
	}

	if((keys & GP2X_SELECT) && (keys & GP2X_L)) // L2
	{
		pad_status &= ~(1<<8);
	}
	else if (keys & GP2X_L)
	{
		pad_status &= ~(1<<10); // L
	}
	
	if((keys & GP2X_SELECT) && (keys & GP2X_R)) // R2
	{
		pad_status &= ~(1<<9);
	}
	else if (keys & GP2X_R)
	{
		pad_status &= ~(1<<11); // R
	}

	if (keys & GP2X_UP)
	{
		pad_status &= ~(1<<4); 
	}
	if (keys & GP2X_DOWN)
	{
		pad_status &= ~(1<<6);
	}
	if ((keys & GP2X_SELECT) && (keys & GP2X_START))
	{			
		pad_status &= ~(1);
	}
	else if (keys & GP2X_START)
	{
		pad_status &= ~(1<<3);
	}
		
	if (keys & GP2X_X)
	{
		pad_status &= ~(1<<12); /* triangle */
	}

	keystate = pad_status;
}

s32 PAD1_readPort1(PadDataS* pad) {
	u16 pad_status = 0xffff;
	//SysPrintf("start PAD1_readPort()\r\n");
	pad_status = ReadZodKeys();
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard	
  	return 0;
}

s32 PAD2_readPort1(PadDataS* pad) {
	u16 pad_status = 0xffff;
	//SysPrintf("start PAD1_readPort()\r\n");
	pad_status = 0xffff;
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard
 	return 0;
}

s32 PAD1_readPort2(PadDataS* a) {
	return -1;
}

s32 PAD2_readPort2(PadDataS* a) {
	return -1;
}
