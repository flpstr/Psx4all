/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#if !defined(__WIN32__)
#include <dirent.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#if defined(ARM_ARCH)
#include <fcntl.h>
#include <sys/ioctl.h>

#if !defined(GIZMONDO) && !defined(POCKETPC) && !defined(IPHONE)
  #include <sys/mman.h>
  #include <sys/soundcard.h>
  #include <linux/fb.h>
  #include <dlfcn.h>
#endif

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#endif
#include "common.h"
#include "gui.h"
#include "sio.h"
#include <SDL/SDL.h>
//#include <SDL/SDL_mixer.h> Not used...


extern void backg ();
int psx4all_emulating=0;
s8 iphonefile[256];
s8 svsfilename[260];
s8* packfile = NULL;
s8 gamepath[256]={ '.', '/', 0, 0 };
s32 LoadCdBios;
PsxConfig Config;
FILE* emuLog;
s32 Log=0;
u32 gpuDisp;
s32 cdOpenCase = 0;
s32 ShowPic=0;
s32 StatesC = 0;
void gp2x_sound_frame(void *blah, void *buff, int samples) {}


#define PSX4ALL_MENU_START_POS		50
#define PSX4ALL_MENU_DEFAULT_STATE	0
#define PSX4ALL_MENU_OPTIONS_STATE	1
#define PSX4ALL_MENU_GPU_STATE		2	
#define PSX4ALL_MENU_SPU_STATE		3
#define PSX4ALL_MENU_GAMESTATE_STATE	4
#define PSX4ALL_MENU_ABOUT_STATE        5


//Menu defines:
#define PSX4ALL_VERSION "1"
#define PSX4ALL_BUILD "1"
#define PSX4ALL_BY "Flpstr"
#define PSX4ALL_NAME "Psx4all"
#define PSX4ALL_CREDITS "Credits: Unai - Zodttd - Hlide - Chui - Tinnus"
#define PSX4ALL_PORT "Dingoo/MIPS port by Ulrich Hecht"
#define PSX4ALL_PTI "Version  for:"
#define PSX4ALL_PLAT "GCW-Zero_OpenDingux"
#define PSX4ALL_PRM "Press B to return"
#define PSX4ALL_OR "Choose a GAME or PRESS L to exit"
#define PSX4ALL_ER "Error opening directory"  
#define PSX4ALL_SL "Are you sure you want to SELECT..."
#define PSX4ALL_YN "Press START for YES or SELECT for NO"
#define PSX4ALL_ROW  "======>"
#define PSX4ALL_ROW2 "====> "
#define PSX4ALL_OPT  "Options"
#define PSX4ALL_GS   "Graphics Options"
#define PSX4ALL_SS   "Sound Options"
#define PSX4ALL_FO   "File Options"
#define PSX4ALL_ABOUT "About"
#define PSX4ALL_QT   "Quit to Emulator"
#define PSX4ALL_BK   "<=  Back"
#define PSX4ALL_FPS  "Show FPS                 %s"     
#define PSX4ALL_GPU  "Show GPU Stats           %s"
#define PSX4ALL_MEM  "Display Video Memory     %s"
#define PSX4ALL_GPUS "Set NULL GPU             %s"
#define PSX4ALL_IC   "Interlace Count          %d"
#define PSX4ALL_FL   "Frame Limit              %s"
#define PSX4ALL_SKIP "Frame Skip               %d/%d"
#define PSX4ALL_HACK "Abe's Oddysee Fix        %s"
#define PSX4ALL_CM   "Cycle Multiplier         %d"
#define PSX4ALL_FRL  "Frameskip: Line          %s"
#define PSX4ALL_FPL  "Frameskip: Poly          %s"
#define PSX4ALL_FST  "Frameskip: Sprite        %s"
#define PSX4ALL_FIM  "Frameskip: Image         %s"
#define PSX4ALL_FBT  "Frameskip: Blit          %s"
#define PSX4ALL_WTM  "Wall Clock Timing        %s"
#define PSX4ALL_SND  "Sound is %s"
#define PSX4ALL_SST  "Save Game State"
#define PSX4ALL_LST  "Load Game State"
#define PSX4ALL_LG   "Load a Game"
#define PSX4ALL_LGW  "Load a Game Whithout Bios" 
#define PSX4ALL_SV   "Saving..."
#define PSX4ALL_SVD  "Saved!"
#define PSX4ALL_ESV  "Error Saving!"
#define PSX4ALL_STR  "%s"
#define PSX4ALL_SSR  "%s/%s"
#define PSX4ALL_LDG  "Loading Save State"
#define PSX4ALL_LDB  "Loading Bios"
#define PSX4ALL_LDD  "Load Failed!"
#define PSX4ALL_LSF  "Load Save Failed"
#define PSX4ALL_LDY  "Loaded!"
#define PSX4ALL_INT  "\n--- SysInit ---\n"
#define FORMAT_SV    ".svs"
#define PSX4ALL_RES  "Resume Emulation"


 //end

/* EXTERNS FOR MENU SYSTEM	*/
/* NEEDS A BETTER PLACE		*/
extern int iSoundMuted;
extern int linesInterlace_user;
extern bool enableFrameLimit;
extern bool enableAbbeyHack;
extern bool displayFrameInfo;
extern bool displayGpuStats;
extern bool displayVideoMemory;
extern bool activeNullGPU;
extern int  skipCount;
extern int  skipRate;
int  skipValue = 0;
#ifdef IPHONE
static int  skipCountTablePhone[4] 	= { 0,2,3,4 };
static int  skipRateTablePhone[4] 	= { 1,3,4,5 };
#endif
int  skipCountTable[9] =  { 0,1,3,2,4,7,10,15,17 };
int  skipRateTable[9] 	= { 1,2,5,3,5,8,11,16,18 };

extern bool hardframeskip_line;
extern bool hardframeskip_poly;
extern bool hardframeskip_sprite;
extern bool hardframeskip_image;
extern bool hardframeskip_blit;
extern bool use_wall_clock_time;
static void ChangeWorkingDirectory(char *exe)
{
#ifndef __WIN32__
	s8 *s = strrchr(exe, '/');
	if (s != NULL) {
		*s = '\0';
		chdir(exe);
		*s = '/';
	}
#endif
}

struct dir_item {
	s8	*name;
	s32 type; 
};

void sort_dir(struct dir_item *list, int num_items, int sepdir) {
	s32 i;
	struct dir_item temp;

	for(i=0; i<(num_items-1); i++) {
		if(strcmp(list[i].name, list[i+1].name)>0) {
			temp=list[i];
			list[i]=list[i+1];
			list[i+1]=temp;
			i=0;
		}
	}
	if(sepdir) {
		for(i=0; i<(num_items-1); i++) {
			if((list[i].type!=0)&&(list[i+1].type==0)) {
				temp=list[i];
				list[i]=list[i+1];
				list[i+1]=temp;
				i=0;
			}
		}
	}
}

static s8 filereq_fullgamepath[257];
static struct dir_item filereq_dir_items[1024] = {{0,0},};

char *FileReq(char *dir, const char *ext)
{
	static s8 *cwd=NULL;
	static s32 cursor_pos=1;
	static s32 first_visible;
	static s32 num_items=0;
	DIR *dirstream;
	struct dirent *direntry;
	s8 *path;
	struct stat item;
	static s32 row;
	s32 pathlength;
	s8 tmp_string[32];
	s8 *selected;
	u32 keys;
#define MENU_Y 90
#define MENU_LS MENU_Y+10
#define MENU_HEIGHT 12

	if(dir!=NULL) cwd=dir;
	if(cwd==NULL)
	{
		sprintf(filereq_fullgamepath, "%s/", gamepath);
		cwd=filereq_fullgamepath;
	}


	for(;;)
	{
		keys = gp2x_joystick_read();

		gp2x_video_RGB_clearscreen16();
		backg();
		//exems();
		//gp2x_timer_delay(500000);
		//gp2x_printf(NULL, 0, 10,	PSX4ALL_NAME " V " PSX4ALL_VERSION "." PSX4ALL_BUILD " By " PSX4ALL_BY);
		//gp2x_printf(NULL, 0, 20,	PSX4ALL_CREDITS);
		//gp2x_printf(NULL, 0, 30,        PSX4ALL_PORT);
		//gp2x_printf(NULL, 0, 40,	PSX4ALL_OR);

		if( keys & GP2X_L )
		{
			for(int i=0; i<num_items; i++) if (filereq_dir_items[i].name)  { free(filereq_dir_items[i].name); filereq_dir_items[i].name = NULL; }
			num_items=0;
			gp2x_timer_delay(90);
			return NULL;
		}

		if(num_items==0) {
				s32 i2;
				dirstream=opendir(cwd);
				if(dirstream==NULL) {
					gp2x_printf(NULL, 0, 20, PSX4ALL_ER);
					return NULL;
				}
				while((direntry=readdir(dirstream))) {
					if( (ext == NULL &&
						 ((NULL == strstr(direntry->d_name, ".")) ||
						 (strlen(direntry->d_name) > 1 && 0 == strnicmp(direntry->d_name, "..", 2))								  ||
						 (strlen(direntry->d_name) > 2 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-2), ".z", 2))  ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".iso", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".bin", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".img", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".znx", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".cbn", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".pbp", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".cue", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".psx", 4)) ||
						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".zip", 4)) ||
 						 (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-4), ".mdf", 4))) ) ||						
(ext != NULL && (strlen(direntry->d_name) > 4 && 0 == strnicmp(direntry->d_name+(strlen(direntry->d_name)-strlen(ext)), ext, strlen(ext)))) )
					{
						 filereq_dir_items[num_items].name=(s8 *)malloc(strlen(direntry->d_name)+1);
						strcpy(filereq_dir_items[num_items].name, direntry->d_name);
						num_items++;
						if(num_items>1024) break;
					}
				}
				closedir(dirstream);
				for(i2=0; i2<num_items; i2++) {
					path=(s8 *)malloc(strlen(cwd)+strlen(filereq_dir_items[i2].name)+2);
					sprintf(path, PSX4ALL_SSR, cwd, filereq_dir_items[i2].name);
					if(!stat(path, &item)) {
						if(S_ISDIR(item.st_mode)) {
							filereq_dir_items[i2].type=0;
						}
						else
						{
							s32 len = strlen(filereq_dir_items[i2].name);

							filereq_dir_items[i2].type = 2;
							if (len >= 4)
							{
								if (!strnicmp(filereq_dir_items[i2].name+(len-2), ".Z", 2))
								{
									filereq_dir_items[i2].type = 1;
								}
								if (!strnicmp(filereq_dir_items[i2].name+(len-4), ".bin", 4))
								{
									filereq_dir_items[i2].type = 1;
								}
								if (!strnicmp(filereq_dir_items[i2].name+(len-4), ".ZNX", 4))
								{
									filereq_dir_items[i2].type = 1;
								}
							}
						}
					} else {
						filereq_dir_items[i2].type=0;
					}
					free(path);
				}
			sort_dir(filereq_dir_items, num_items, 1);
			cursor_pos=0;
			first_visible=0;
		}

		gp2x_printf(NULL, 90, MENU_Y, cwd);

		if(keys & GP2X_DOWN) { 
				if(cursor_pos<(num_items-1)) cursor_pos++;
				if((cursor_pos-first_visible)>=MENU_HEIGHT) first_visible++;
		}
		else if(keys & GP2X_UP) { 
				if(cursor_pos>0) cursor_pos--;
				if(cursor_pos<first_visible) first_visible--;
		}
		else if(keys & GP2X_LEFT) { 
				if(cursor_pos>=10) cursor_pos-=10;
				else cursor_pos=0;
				if(cursor_pos<first_visible) first_visible=cursor_pos;
		}
		else if(keys & GP2X_RIGHT) {
				if(cursor_pos<(num_items-11)) cursor_pos+=10;
				else cursor_pos=num_items-1;
				if((cursor_pos-first_visible)>=MENU_HEIGHT)
					first_visible=cursor_pos-(MENU_HEIGHT-1);
		}
		else if(keys & GP2X_B) 
		{
		
				path=(s8 *)malloc(strlen(cwd)
				+strlen(filereq_dir_items[cursor_pos].name)
				+2);
			sprintf(path, PSX4ALL_SSR, cwd, filereq_dir_items[cursor_pos].name);
			for(int i=0; i<num_items; i++) if (filereq_dir_items[i].name)  { free(filereq_dir_items[i].name); filereq_dir_items[i].name = NULL; }
			num_items=0;
			if(filereq_dir_items[cursor_pos].type==0) {
				pathlength=strlen(path);
				if(	path[pathlength-1]=='.' &&
					path[pathlength-2]=='/') 
				{
					path[pathlength-2]='\0';
					cwd=path;
				}
				else if(path[pathlength-1]=='.'
						&& path[pathlength-2]=='.'
						&& path[pathlength-3]=='/' ) 
				{
					if( pathlength > 4 )
					{
						s8* p = strrchr(path, '/');	
						p[0] = '\0';
						p = strrchr(path, '/');			
						p[0] = '\0';
						p = strrchr(path, '/');			
						p[1] = '\0';					
						cwd=path;
					}
				}
				else
				{
					if( path[0] == '/' &&
						path[1] == '/' )
					{
						cwd = path + 1; 
					}
					else
					{
						cwd = path;
					}
				}
			}
			else
			{
				//gp2x_video_RGB_clearscreen16();
				gp2x_printf(NULL, 10, 120, PSX4ALL_SL);
				gp2x_printf(NULL, 10, 130, PSX4ALL_STR, path);
				gp2x_printf(NULL, 10, 140, PSX4ALL_YN);
				gp2x_video_flip();
				for(;;)
				{
					u32 keys = gp2x_joystick_read();
					if( keys & GP2X_SELECT )
					{
						return NULL;
					}
					if( keys & GP2X_START )
					{
						char* p = strrchr(path, '/');
						if( p != NULL )
						{
							sprintf(CdromLabel, "%10.10s", p + 1);
						}
						return path;
					}

					gp2x_timer_delay(90);
				}
			}
		}

		row=0;
		while(row<num_items && row<MENU_HEIGHT) {
			if(row==(cursor_pos-first_visible)) {
				gp2x_printf(NULL, 90, MENU_LS+(10*row), PSX4ALL_ROW);

				selected=filereq_dir_items[row+first_visible].name;
			}

			if(filereq_dir_items[row+first_visible].type==0)
			{
        		gp2x_printf(NULL, 90, MENU_LS+(10*row), "DIR ");
			}
			snprintf(tmp_string, 30, PSX4ALL_STR, filereq_dir_items[row+first_visible].name);
			gp2x_printf(NULL, 90+(10*6), MENU_LS+(10*row), tmp_string);
			row++;
		}
		while(row<MENU_HEIGHT) {
			row++;
		}

		gp2x_video_flip();
		gp2x_timer_delay(75);

		if(keys & (GP2X_A|GP2X_B|GP2X_X|GP2X_Y|GP2X_L|GP2X_R|GP2X_PUSH|
			GP2X_LEFT|GP2X_RIGHT|GP2X_UP|GP2X_DOWN) )
		{
			gp2x_timer_delay(50);
		}
	}

	return NULL;
}

s32 SelectGame()
{
	u32 keys;
	s32 loadst = 0;

	s32 menu_state = 0;
	s32 menu_pos = 0;
	s8* newpackfile = NULL;

	gp2x_video_RGB_clearscreen16();
	gp2x_video_flip();

#if 1
	for(;;)
	{
		gp2x_video_RGB_clearscreen16();
		backg();
		//exems();
		//gp2x_timer_delay(500000);
		keys = gp2x_joystick_read();
		gp2x_printf(NULL, 10, (menu_pos * 10) + PSX4ALL_MENU_START_POS, PSX4ALL_ROW2);
		
		if( keys & GP2X_UP )
		{
			if( menu_pos > 0 ) menu_pos--;
		}

		switch(menu_state)
		{
		case PSX4ALL_MENU_DEFAULT_STATE:
			if( keys & GP2X_DOWN )
			{
				if (psx4all_emulating) {
				if( menu_pos < 4 ) menu_pos++;
				}else{

				if( menu_pos < 3 ) menu_pos++;
				}
			}
			break;
		case PSX4ALL_MENU_OPTIONS_STATE:
			if( keys & GP2X_DOWN )
			{
				if( menu_pos < 2 ) menu_pos++;
			}
			break;
		case PSX4ALL_MENU_GPU_STATE:
			if( keys & GP2X_DOWN )
			{
				if( menu_pos < 15 ) menu_pos++;
			}
			break;
		case PSX4ALL_MENU_SPU_STATE:
			if( keys & GP2X_DOWN )
			{
				if( menu_pos < 1 ) menu_pos++;
			}
			break;
		/*case PSX4ALL_MENU_ABOUT_STATE:
			if( keys & GP2X_B )
			{
			  	if( menu_pos < 0 ) menu_pos++;
			}
			break;*/
		case PSX4ALL_MENU_GAMESTATE_STATE:
			if( keys & GP2X_DOWN )
			{
				if( menu_pos < 4 ) menu_pos++;
			}
			break;
		
		}
		switch(menu_state)
		{
		case PSX4ALL_MENU_DEFAULT_STATE:
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 0,	PSX4ALL_OPT);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 10,	PSX4ALL_FO);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 20,	PSX4ALL_ABOUT);
			if (psx4all_emulating) {
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 30, PSX4ALL_RES);
			  if(psx4all_emulating){
			  gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 40, PSX4ALL_QT);
			  break;
			  }
			}else {
			  gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 30, PSX4ALL_QT);
			 
			}
			
			break;
			
		case PSX4ALL_MENU_OPTIONS_STATE:
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 0,	PSX4ALL_GS);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 10,	PSX4ALL_SS);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 20,     PSX4ALL_BK);
			break;
			
		case PSX4ALL_MENU_GPU_STATE:
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS +  0,
				PSX4ALL_FPS,
				(displayFrameInfo == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 10,
				PSX4ALL_GPU,
				(displayGpuStats == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 20,
				PSX4ALL_MEM,
				(displayVideoMemory == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 30,
				PSX4ALL_GPUS,
				(activeNullGPU == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 40,
				PSX4ALL_IC,
				linesInterlace_user );
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 50,
				PSX4ALL_FL,
				(enableFrameLimit == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 60,
				PSX4ALL_SKIP,
				skipCount, skipRate);
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 70,
				PSX4ALL_HACK,
				(enableAbbeyHack == false ? "OFF" : "ON"));
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 80,
				PSX4ALL_CM,
				PsxCycleMult);
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 90,
				PSX4ALL_FRL,
				hardframeskip_line ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 100,
				PSX4ALL_FPL,
				hardframeskip_poly ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 110,
				PSX4ALL_FST,
				hardframeskip_sprite ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 120,
				PSX4ALL_FIM,
				hardframeskip_image ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 130,
				PSX4ALL_FBT,
				hardframeskip_blit ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 140,
				PSX4ALL_WTM,
				use_wall_clock_time ? "ON" : "OFF");
			gp2x_printf(NULL, 80, PSX4ALL_MENU_START_POS + 150, PSX4ALL_BK);
			break;
		case PSX4ALL_MENU_SPU_STATE:
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 0,	PSX4ALL_SND, (iSoundMuted == 0 ? "ON" : "OFF"));
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 10,	PSX4ALL_BK);
			break;
		case PSX4ALL_MENU_GAMESTATE_STATE:
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 0,	PSX4ALL_SST);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 10,	PSX4ALL_LST);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 20,	PSX4ALL_LG);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 30,	PSX4ALL_LGW);
			gp2x_printf(NULL, 100, PSX4ALL_MENU_START_POS + 40,	PSX4ALL_BK);
			break;
		case PSX4ALL_MENU_ABOUT_STATE:
		        gp2x_printf(NULL, 0, 50, PSX4ALL_NAME " V " PSX4ALL_VERSION "." PSX4ALL_BUILD " By " PSX4ALL_BY);
			gp2x_printf(NULL, 0, 60, PSX4ALL_CREDITS);
			gp2x_printf(NULL, 0, 70, PSX4ALL_PORT);
			gp2x_printf(NULL, 0, 80, PSX4ALL_PTI " " PSX4ALL_PLAT);
			gp2x_printf(NULL, 0, 100, PSX4ALL_PRM);
			break;
		}
	
		switch(menu_state)
		{
		case PSX4ALL_MENU_DEFAULT_STATE:
		  if (psx4all_emulating) {
			if( keys & GP2X_B )
			{
				switch(menu_pos)
				{
				case 0:
					menu_state = PSX4ALL_MENU_OPTIONS_STATE;
					menu_pos = 0;
					break;
				case 1:
					menu_state = PSX4ALL_MENU_GAMESTATE_STATE;
					menu_pos = 0;
					break;
				case 2:
					menu_state = PSX4ALL_MENU_ABOUT_STATE;
					menu_pos = 0;
					break;
				case 3:
					gp2x_video_RGB_clearscreen16();
					return 0;
				case 4:
					SDL_Quit();
				default:
					break;
				}
			}
		  }else{
		       if( keys & GP2X_B )
			{
				switch(menu_pos)
				{
				case 0:
					menu_state = PSX4ALL_MENU_OPTIONS_STATE;
					menu_pos = 0;
					break;
				case 1:
					menu_state = PSX4ALL_MENU_GAMESTATE_STATE;
					menu_pos = 0;
					break;
				case 2:
					menu_state = PSX4ALL_MENU_ABOUT_STATE;
					menu_pos = 0;
					break;
				case 3:
					gp2x_video_RGB_clearscreen16();
					return 0;
				
				default:
					break;
				}
			}
		  }
			if (keys & GP2X_L && psx4all_emulating) {
				gp2x_video_RGB_clearscreen16();
				return 0;
			}
			break;
		case PSX4ALL_MENU_OPTIONS_STATE:
			
				switch(menu_pos)
				{
				  case 0:
					if (keys & GP2X_B){
					menu_state = PSX4ALL_MENU_GPU_STATE;
					menu_pos = 0;
					
					}break;
				  case 1:
					if( keys & GP2X_B){
					menu_state = PSX4ALL_MENU_SPU_STATE;
					menu_pos = 0;
					}
					break;
				  case 2:
					if( keys & GP2X_B )
					{
						menu_state = PSX4ALL_MENU_DEFAULT_STATE;
						menu_pos = 0;
					}
					break;
				default:
					break;  
				}
			if (keys & GP2X_L) {
					menu_state = PSX4ALL_MENU_DEFAULT_STATE;
					menu_pos = 0;
				}
				break;
				
				
		case PSX4ALL_MENU_GPU_STATE:
				switch(menu_pos)
				{
				case 0:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						displayFrameInfo = !displayFrameInfo;
					}
					break;
				case 1:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						displayGpuStats = !displayGpuStats;
					}
					break;
				case 2:
					if( keys & GP2X_B )
					{
						displayVideoMemory = !displayVideoMemory;
					}
					break;
				case 3:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						activeNullGPU = !activeNullGPU;
					}
					break;
				case 4:
					if ( keys & GP2X_LEFT && linesInterlace_user > 0) linesInterlace_user--;
					if ( keys & GP2X_RIGHT && linesInterlace_user < 7) linesInterlace_user++;
					break;
				case 5:
					if( keys & GP2X_B )
					{
						enableFrameLimit = !enableFrameLimit;
					}
					break;
				case 6:
					if( keys & GP2X_LEFT )
					{
						if( skipValue > 0 )
						{
							skipValue--;
							skipCount = skipCountTable[skipValue];
							skipRate = skipRateTable[skipValue];
						}
					}
					if( keys & GP2X_RIGHT )
					{
						if( skipValue < 8 )
						{
							skipValue++;
							skipCount = skipCountTable[skipValue];
							skipRate = skipRateTable[skipValue];
						}
					}
					break;
				case 7:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						enableAbbeyHack = !enableAbbeyHack;
					}
					break;
				case 8:
					if ( keys & GP2X_LEFT && PsxCycleMult > 1) PsxCycleMult--;
					if ( keys & GP2X_RIGHT && PsxCycleMult < 10) PsxCycleMult++;
					break;
				case 9:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						hardframeskip_line = !hardframeskip_line;
					}
					break;
				case 10:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						hardframeskip_poly = !hardframeskip_poly;
					}
					break;
				case 11:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						hardframeskip_sprite = !hardframeskip_sprite;
					}
					break;
				case 12:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						hardframeskip_image = !hardframeskip_image;
					}
					break;
				case 13:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						hardframeskip_blit = !hardframeskip_blit;
					}
					break;
				case 14:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
						use_wall_clock_time = !use_wall_clock_time;
					}
					break;
				case 15:
					if( keys & GP2X_B )
					{
						menu_state = PSX4ALL_MENU_OPTIONS_STATE;
						menu_pos = 0;
					}
					break;
				default:
					break;
				
					}
				if (keys & GP2X_L) {
					menu_state = PSX4ALL_MENU_OPTIONS_STATE;
					menu_pos = 0;
				}
				break;
		case PSX4ALL_MENU_SPU_STATE:
			switch(menu_pos)
			{
				case 0:
					if( keys & GP2X_B || keys & GP2X_LEFT || keys & GP2X_RIGHT )
					{
#ifndef NOSOUND
						iSoundMuted = !iSoundMuted;
#endif
					}
					break;
				case 1:
					if( keys & GP2X_B )
					{
						menu_state = PSX4ALL_MENU_OPTIONS_STATE;
						menu_pos = 0;
					}
					break;
				default:
					break;
			} 
			if (keys & GP2X_L) {
			      menu_state = PSX4ALL_MENU_OPTIONS_STATE;
			      menu_pos = 0;
			}
			break;
			
		case PSX4ALL_MENU_GAMESTATE_STATE:
			switch(menu_pos)
			{
				case 0:
					if( keys & GP2X_B )
					{
#ifndef IPHONE
						if( 1 == psx4all_emulating )
						{
							s32 ret;
							char buffer[360];
							char filename[260];

							struct stat s;
							for(int count = 1;; count++) {
								sprintf(filename, "%s-%04d.svs", packfile, count);
								if (stat(filename, &s)) break;
							}
							
							gp2x_printf(NULL, 100, 130, PSX4ALL_SV);
							gp2x_video_flip();
							GPU_freeze(2, NULL);
							ret = SaveState(filename);
							if (ret == 0)
								 sprintf(buffer, PSX4ALL_SVD);
							else sprintf(buffer, PSX4ALL_ESV);

							gp2x_printf(NULL, 100, 140, PSX4ALL_STR, buffer);
							gp2x_video_flip();
							gp2x_timer_delay(900);
						}
#endif
					}
					break;
				case 1:
					if( keys & GP2X_B )
					{
#ifndef IPHONE
						gp2x_timer_delay(500);
						newpackfile = FileReq(NULL, FORMAT_SV);
#endif
					}
					break;
				case 2:
					if( keys & GP2X_B )
					{
#ifndef IPHONE
						Config.HLE = 0;
						gp2x_timer_delay(500);
						newpackfile = FileReq(NULL, NULL);
#endif
					}
					break;
				case 3:
					if( keys & GP2X_B )
					{
#ifndef IPHONE
						Config.HLE = 1;
						gp2x_timer_delay(500);
						newpackfile = FileReq(NULL, NULL);
#endif
					}
					break;
				case 4:
					if( keys & GP2X_B )
					{
						menu_state = PSX4ALL_MENU_DEFAULT_STATE;
						menu_pos = 0;
					}
					break;
				default:
					break;
			}
				if (keys & GP2X_L) {
				menu_state = PSX4ALL_MENU_DEFAULT_STATE;
				menu_pos = 0;
				} break;
			
		case PSX4ALL_MENU_ABOUT_STATE:
			switch(menu_pos)
			{
				case 0:
					if( keys & GP2X_B )
					{
					  menu_state = PSX4ALL_MENU_DEFAULT_STATE;
					  menu_pos = 0;
					}
					break;
				default:
				  break;
			}
			
			if (keys & GP2X_L) {
				menu_state = PSX4ALL_MENU_DEFAULT_STATE;
				menu_pos = 0;
			} break;
			
			
		}
			
		if( newpackfile != NULL )
		{
			break;
		}
		gp2x_video_flip();
		gp2x_timer_delay(90);

		if(keys & (GP2X_A|GP2X_B|GP2X_X|GP2X_Y|GP2X_L|GP2X_R|GP2X_PUSH|
			GP2X_LEFT|GP2X_RIGHT|GP2X_UP|GP2X_DOWN) )
		{
			gp2x_timer_delay(50);
		}
	}
#else
	Config.HLE = 1;
	//newpackfile = "Cotton Jap.cbn";
#endif
	DEBUGF("loaded "PSX4ALL_STR, newpackfile);
	packfile = newpackfile;

	gp2x_video_RGB_clearscreen16();

	keys = gp2x_joystick_read();
	backg();
	
	LoadCdBios = 0;

	if( (!strcasecmp(packfile + (strlen(packfile)-4), FORMAT_SV)) )
	{
		char *pos;
		loadst = 1;
		sprintf(svsfilename, PSX4ALL_STR, packfile);
		pos = strrchr(packfile, '-');
		if (pos) *pos = '\0';
	}
	else
	{
		loadst = 0;
	}

	if( loadst > 0 )
	{
		gp2x_printf(NULL, 120, 90, PSX4ALL_LDG);
	}
	else
	{
		gp2x_printf(NULL, 120, 90, PSX4ALL_LDB);

	}

	gp2x_video_flip();

	if( 1 == psx4all_emulating )
	{
		psxShutdown();
		CloseComponents();
	}

	if (SysInit() == -1)
	{
		gp2x_deinit();
		return 0;
	}

	if (InitComponents() == -1)
	{
		gp2x_deinit();
		return 0;
	}

	SysReset();
	CheckCdrom();

	if( Config.HLE )
	{
		LoadCdBios = 0;
	 	if( LoadCdrom() == -1 )
		{
			gp2x_printf(NULL, 120, 120, PSX4ALL_LDD);
			gp2x_video_flip();
			gp2x_timer_delay(2000);
			gp2x_video_RGB_clearscreen16();
			backg();
			return 0;
		}
	}
	else
	{
		gp2x_printf(NULL, 120, 120, PSX4ALL_LDY);
		gp2x_video_flip();
		gp2x_timer_delay(90);
		backg();
	}

	if (loadst) {
		if( LoadState(svsfilename) == -1 )
		{
			gp2x_printf(NULL, 120, 120, PSX4ALL_LSF);
			gp2x_video_flip();
			gp2x_timer_delay(2000);
			gp2x_video_RGB_clearscreen16();
			return 0;
		}
	}

	return 1;
}

#if defined(IPHONE)
extern "C" int iphone_main(char* filename)
#else
int main(int argc, char *argv[])
#endif
{
#ifndef IPHONE
#if defined(ARM_ARCH)
	ChangeWorkingDirectory(argv[0]);
	getcwd(gamepath, 256);
#else
#if defined(__WIN32__)
	if(argc == 1)
		strncpy(gamepath,"E:\\ps1",256);
	else
		strncpy(gamepath,argv[1],256);
#else
	strncpy(gamepath,ROM_PREFIX,256);
#endif
#endif
#endif

#ifdef PSP
	sprintf(gamepath,"");
#endif

#ifdef IPHONE
	sprintf(gamepath,"");
#endif

	memset(&Config, 0, sizeof(PsxConfig));
	Config.PsxAuto = 1;
	Config.Cdda = 1;
	Config.Xa = 0;
#ifdef DYNAREC
	Config.Cpu = 0;
#else
	Config.Cpu = 1;
#endif

#ifdef WITH_HLE
	Config.HLE = 1;
#else
	Config.HLE = 0;
#endif

	Config.Mdec = 0;
	Config.PsxOut = 0;
	Config.PsxType = 0;
	Config.QKeys = 0;
	Config.RCntFix = 0;
	Config.Sio = 0;
	Config.SpuIrq = 1;
	Config.VSyncWA = 0;

#if defined(PSP) || defined(SDL) || defined(IPHONE)
    	sprintf(Config.BiosDir, "%s/Media/ROMs/PSX/", appworkdir);
	sprintf(Config.Bios, "scph1001.bin");
	sprintf(Config.Mcd1, "mcd001.mcr");
	sprintf(Config.Mcd2, "mcd002.mcr");
#else
	sprintf(Config.BiosDir, PSX4ALL_STR, gamepath);
	sprintf(Config.Bios, "/bios/scph1000.bin");
	sprintf(Config.Mcd1, "%s/memory_card/mcd001.mcr", gamepath);
	sprintf(Config.Mcd2, "%s/memory_card/mcd002.mcr", gamepath);
	
#endif
	gp2x_init(900, 16, 11025, 16, 1, 60, 1);
#ifndef GP2X
	gp2x_video_flip_single();
#endif

#ifdef IPHONE
	u32 loadsvs = 0;
	linesInterlace_user = preferences.interlace;
	skipCount = skipCountTablePhone[preferences.frameSkip];
	skipRate = skipRateTablePhone[preferences.frameSkip];   
	iSoundMuted = preferences.muted;
	Config.Cdda = preferences.muted;
	Config.Xa = preferences.muted;
#ifdef WITH_HLE
	Config.HLE = !preferences.bios;
#else
	Config.HLE = 0;
#endif
	if( (!strcasecmp(filename + (strlen(filename)-4), FORMAT_SV )) )
	{
		u32 pos;
		loadsvs = 1;
		sprintf(svsfilename, PSX4ALL_STR, filename);
		sprintf(iphonefile, PSX4ALL_STR, filename);
		pos = strlen(iphonefile)-18;
		iphonefile[pos] = '\0';
		packfile = iphonefile;
	}
	else
	{
		loadsvs = 0;
		sprintf(iphonefile, PSX4ALL_STR, filename);
		packfile = iphonefile;
	}

	gp2x_video_RGB_clearscreen16();
	LoadCdBios = 0;

	if (SysInit() == -1)
	{
		gp2x_deinit();
		return 0;
	}

	if (InitComponents() == -1)
	{
		gp2x_deinit();
		return 0;
	}

	SysReset();
	CheckCdrom();

	if( Config.HLE )
	{
		LoadCdBios = 0;
		if( LoadCdrom() == -1 )
		{
			gp2x_printf(NULL, 120, 120, PSX4ALL_LDD);
			gp2x_video_flip();
			gp2x_timer_delay(2000);
			gp2x_video_RGB_clearscreen16();
			return 0;
		}
	}
	
	if (loadsvs) {
		if( LoadState(svsfilename) == -1 )
		{
			gp2x_printf(NULL, 120, 120,  PSX4ALL_LSF);
			gp2x_video_flip();
			gp2x_timer_delay(2000);
			gp2x_video_RGB_clearscreen16();

			psxShutdown();
			CloseComponents();
			
			gp2x_deinit();
			pthread_exit(NULL);
		}
	}

	psx4all_emulating=1;
	psx4all_prof_start(PROFILER_TOTAL);
	psxCpu->Execute();
	psx4all_prof_end(PROFILER_TOTAL);
	psx4all_emulating=0;

	psx4all_prof_show();
#else
	if( 0 != SelectGame() )
	{
		psx4all_emulating=1;
		psx4all_prof_start(PROFILER_TOTAL);
		psxCpu->Execute();
		psx4all_prof_end(PROFILER_TOTAL);
		psx4all_emulating=0;

		psx4all_prof_show();
	}
#endif
	gp2x_deinit();

	return 0;
}

int SysInit() {
#ifdef GTE_DUMP
	gteLog = fopen("gteLog.txt","wb");
	setvbuf(gteLog, NULL, _IONBF, 0);
#endif

#ifdef EMU_LOG
#ifndef LOG_STDOUT
	emuLog = fopen("emuLog.txt","wb");
#else
	emuLog = stdout;
#endif
	setvbuf(emuLog, NULL, _IONBF, 0);

	SysMessage(PSX4ALL_INT);
	fflush(emuLog);
#endif

	psxInit();

	LoadMcds(Config.Mcd1, Config.Mcd2);

	return 0;
}

void SysReset() {
	psxReset();
}

void SysClose() {
	psxShutdown();

	if (emuLog != NULL) fclose(emuLog);
}

static char __sysmsg[1024];

void SysPrintf(const char *fmt, ...) {
	va_list list;

	va_start(list, fmt);
	vsprintf(__sysmsg, fmt, list);
	va_end(list);

	__sysmsg[1023]=0;

	if (Config.PsxOut) printf (PSX4ALL_STR, __sysmsg);
#if defined(__WIN32__) && !defined(GIZMONDO)
  OutputDebugString(__sysmsg);
#endif

	gp2x_printf(NULL, 0, 90, __sysmsg);
	gp2x_video_flip_single();

#ifdef EMU_LOG
	fprintf(emuLog, PSX4ALL_STR, __sysmsg);
#endif
}

void SysMessage(const char* fmt, ...) {
#ifdef EMU_LOG

	va_list list;

	va_start(list, fmt);
	vsprintf(__sysmsg, fmt, list);
	va_end(list);

	__sysmsg[127]=0;

	if (Config.PsxOut) printf (PSX4ALL_STR, __sysmsg);
	fprintf(emuLog, "%s\r\n", __sysmsg);
	fflush(emuLog);
#endif
}

void SysRunGui(){ }
