/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#ifndef __main7_h__
#define __main7_h__

#include "typedefsTGDS.h"
#include "dsregs.h"

#define VRAM_D		((s16*)0x06000000)
#define SIWRAM		((s16*)0x037F8000)

#define MIC_8
#define SPI_BAUD_1MHZ    2
#define SPI_BYTE_MODE   (0<<10)
#define PM_BATTERY_REG    1
#define PM_READ_REGISTER (1<<7)

typedef void(*call3)(u32,void*,u32);

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern int main(int _argc, sint8 **_argv);

extern bool touchDown;
extern bool touchDelay;

extern s16 *strpcmL0;
extern s16 *strpcmL1;
extern s16 *strpcmR0;
extern s16 *strpcmR1;
extern int lastL;
extern int lastR;
extern int multRate;
extern int pollCount; //start with a read
extern u32 sndCursor;
extern u32 micBufLoc;
extern u32 sampleLen;
extern int sndRate;

extern void pmSetReg(int reg, int control);
extern u8 pmGetReg(int reg);
extern void read_nvram(u32 src, void *dst, u32 size);
extern u8 writeread(u8 data);
extern int flash_verify(u8 *src,u32 dst);
extern void writeWifiSettings();
extern s16 checkClipping(int data);

extern void mallocData(int size);
extern void freeData();
extern void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format);
extern s32 getFreeSoundChannel();
extern int writePowerManagement(int reg, int command);
extern int readPowerManagement(int reg);
extern void setSwapChannel();
extern void SetupSound();
extern void StopSound();
extern void TIMER1Handler();
extern void micStartRecording();
extern void micStopRecording();
extern void micInterrupt();

#ifdef __cplusplus
}
#endif

