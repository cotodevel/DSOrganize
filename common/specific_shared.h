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

//TGDS required version: IPC Version: 1.3

//inherits what is defined in: ipcfifoTGDS.h
#ifndef __specific_shared_h__
#define __specific_shared_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "ipcfifoTGDS.h"
#include "dswnifi.h"
#include "memoryHandleTGDS.h"
#include "memoryHandleTGDS.h"

#define ARM9COMMAND_SUCCESS (uint32)(0xFFFFFF01)
#define ARM9COMMAND_INIT (uint32)(0xFFFFFF02)
#define ARM9COMMAND_UPDATE_BUFFER (uint32)(0xFFFFFF03)
#define ARM9COMMAND_SAVE_DATA (uint32)(0xFFFFFF04)
#define ARM9COMMAND_TOUCHDOWN (uint32)(0xFFFFFF05)
#define ARM9COMMAND_TOUCHMOVE (uint32)(0xFFFFFF06)
#define ARM9COMMAND_TOUCHUP (uint32)(0xFFFFFF07)

#define ARM7STATE_IDLE (uint32)(0)
#define ARM7STATE_WAITING (uint32)(1)
#define ARM7STATE_WAITCOPY (uint32)(2)

#define ARM7COMMAND_START_SOUND (uint32)(0xC1)
#define ARM7COMMAND_STOP_SOUND (uint32)(0xC2)
#define ARM7COMMAND_SOUND_SETMULT (uint32)(0xC3)
#define ARM7COMMAND_SOUND_SETRATE (uint32)(0xC5)
#define ARM7COMMAND_SOUND_SETLEN (uint32)(0xC6)
#define ARM7COMMAND_SOUND_COPY (uint32)(0xC7)
#define ARM7COMMAND_SOUND_DEINTERLACE (uint32)(0xC8)
#define ARM7COMMAND_START_RECORDING (uint32)(0xC9)
#define ARM7COMMAND_STOP_RECORDING (uint32)(0xCA)
#define ARM7COMMAND_BOOT_GBAMP (uint32)(0xCB)
#define ARM7COMMAND_BOOT_SUPERCARD (uint32)(0xCC)
#define ARM7COMMAND_BOOT_MIGHTYMAX (uint32)(0xCD)
#define ARM7COMMAND_BOOT_CHISHM (uint32)(0xCE)
#define ARM7COMMAND_PSG_COMMAND (uint32)(0xD0)
#define ARM7COMMAND_SAVE_WIFI (uint32)(0xD1)
#define ARM7COMMAND_LOAD_WIFI (uint32)(0xD2)
#define ARM7COMMAND_PLAYCLICK (uint32)(0xE0)

#define BIT(n) (1 << (n))

typedef struct sTransferSoundData {
//---------------------------------------------------------------------------------
const void *data;
u32 len;
u32 rate;
u8 vol;
u8 pan;
u8 format;
u8 PADDING;
} TransferSoundData, * pTransferSoundData;


//---------------------------------------------------------------------------------
typedef struct sTransferSound {
//---------------------------------------------------------------------------------
TransferSoundData data[16];
u8 count;
u8 PADDING[3];
} TransferSound, * pTransferSound;

struct soundregion
{
	s16 *arm9L;
	s16 *arm9R;
	
	s16 *interlaced;
	int channels;
	u8 volume;
	
	u32 tX;
	u32 tY;
	
	int psgChannel;
	u32 cr;
	u32 timer;
	
	//snd
	TransferSound transfersoundInst;
	
	/*
	//bugfix
	int sendHeartbeat_arm9;
	int sendHeartbeat_arm7;
	int recvHeartbeat_arm9;
	int recvHeartbeat_arm7;	
	int vblankHeartbeat;
	int timerHeartbeat;
	int fullHeartbeat;
	
	int arbitraryCommand[17];*/
};


struct sIPCSharedTGDSSpecific {
	uint32 frameCounter7;	//VBLANK counter7
	uint32 frameCounter9;	//VBLANK counter9
	
	struct soundregion sndregioninst;
	bool canSend;
};

//types used by DSOrganize
typedef sint16 int16;

#endif

#ifdef __cplusplus
extern "C" {
#endif

//NOT weak symbols : the implementation of these is project-defined (here)
extern void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);
extern void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);

extern struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific();

extern bool getSenderStatus();
extern void setSenderStatus(bool status);

#ifdef __cplusplus
}
#endif