/***************************************************************************
                            gpu.h  -  description
                             -------------------
    begin                : Sun Mar 08 2009
    copyright            : (C) 1999-2009 by Pete Bernert
    web                  : www.pbernert.com   
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************// 
// History of changes:
//
// 2009/03/08 - Pete  
// - generic cleanup for the Peops release
//
//*************************************************************************// 

#ifndef _GPU_PLUGIN_H
#define _GPU_PLUGIN_H

/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif 
 extern int use_fsaa;
#if !defined(_WINDOWS) && !defined(__NANOGL__)
#define glOrtho(x,y,z,xx,yy,zz) glOrthof(x,y,z,xx,yy,zz)
#endif

#define PRED(x)   ((x << 3) & 0xF8)
#define PBLUE(x)  ((x >> 2) & 0xF8)
#define PGREEN(x) ((x >> 7) & 0xF8)

#define RED(x) (x & 0xff)
#define BLUE(x) ((x>>16) & 0xff)
#define GREEN(x) ((x>>8) & 0xff)
#define COLOR(x) (x & 0xffffff)

#ifdef _WINDOWS
#include "Externals.h"
#include "plugin.h"
#include <gl/gl.h>
#else
//#ifndef MAEMO_CHANGES
//	#include "psxCommon.h"
//#else
//	#include "../psxCommon.h"
//#endif
#include "gpuExternals.h"
#ifdef __NANOGL__
#include <gl/gl.h>
#else
#ifdef SOFT_LINKAGE
#pragma softfp_linkage
#endif
#ifndef MAEMO_CHANGES
	#include </home/flpstr/Psx4all/src/gpu-gles/gles/GL/gl.h> // for opengl es types 
	//#include </home/flpstr/Psx4all/src/gpu-gles/gles/GL/egltypes.h>
#else
#include "gpuStdafx.h"
#endif
#ifdef SOFT_LINKAGE
#pragma no_softfp_linkage
#endif
#endif
#endif
/////////////////////////////////////////////////////////////////////////////

#define CALLBACK

#define bool unsigned short

typedef struct {
	u32 ulFreezeVersion;
	u32 ulStatus;
	u32 ulControl[256];
	u8 psxVRam[1024*1024*2];
} GPUFreeze_t;

#define GPUinit GPU_init
#define GPUshutdown GPU_shutdown
#define GPUopen GPU_open
#define GPUclose GPU_close
#define GPUreadData GPU_readData
#define GPUreadDataMem GPU_readDataMem
#define GPUreadStatus GPU_readStatus
#define GPUwriteData GPU_writeData
#define GPUwriteDataMem GPU_writeDataMem
#define GPUwriteStatus GPU_writeStatus
#define GPUdmaChain GPU_dmaChain
#define GPUupdateLace GPU_updateLace
#define GPUmakeSnapshot GPU_makeSnapshot
#define GPUfreeze GPU_freeze
#define GPUgetScreenPic GPU_getScreenPic
#define GPUshowScreenPic GPU_showScreenPic
#define GPUconfigure GPU_configure
#define GPUtest GPU_test
#define GPUabout GPU_about
#define GPUvSinc GPU_vSinc


long CALLBACK GPUinit();
long CALLBACK GPUshutdown();
long CALLBACK GPUopen(int hwndGPU);
long CALLBACK GPUclose();
unsigned long CALLBACK GPUreadData(void);
void CALLBACK GPUreadDataMem(unsigned long * pMem, int iSize);
unsigned long CALLBACK GPUreadStatus(void);
void CALLBACK GPUwriteData(unsigned long gdata);
void CALLBACK GPUwriteDataMem(unsigned long * pMem, int iSize);
void CALLBACK GPUwriteStatus(unsigned long gdata);
long CALLBACK GPUdmaChain(unsigned long * baseAddrL, unsigned long addr);
void CALLBACK GPUupdateLace(void);
void CALLBACK GPUmakeSnapshot(void);
long CALLBACK GPUfreeze(unsigned long ulGetFreezeData,GPUFreeze_t * pF);
long CALLBACK GPUgetScreenPic(u8 * pMem);
long CALLBACK GPUshowScreenPic(u8 * pMem);
//void CALLBACK GPUkeypressed(int keycode);
//void CALLBACK GPUdisplayText(s8 * pText);
//void CALLBACK GPUclearDynarec(void (CALLBACK *callback)(void));
long CALLBACK GPUconfigure(void);
long CALLBACK GPUtest(void);
void CALLBACK GPUabout(void);


void           DoSnapShot(void);
void		   GPUvSinc(void);
void           updateDisplay(void);
void           updateFrontDisplay(void);
void           SetAutoFrameCap(void);
void           SetAspectRatio(void);
void           CheckVRamRead(int x, int y, int dx, int dy, bool bFront);
void           CheckVRamReadEx(int x, int y, int dx, int dy);
void           SetFixes(void);

void PaintPicDot(u8 * p,u8 c);
//void DrawNumBorPic(u8 *pMem, int lSelectedSlot);
void ResizeWindow();

////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif 


#endif // _GPU_INTERNALS_H
