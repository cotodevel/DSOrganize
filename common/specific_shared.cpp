
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

#include "ipcfifoTGDS.h"
#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"

#ifdef ARM7
#include <string.h>

#include "main.h"
#include "wifi_arm7.h"
#include "spifwTGDS.h"
#include "resetmem.h"
#include "biosTGDS.h"
#include "biosDSOrganize.h"

extern const char click_raw[];
extern const int click_raw_length;

#endif

#ifdef ARM9

#include <stdbool.h>
#include "main.h"
#include "wifi_arm9.h"

#endif

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific(){
	struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = (__attribute__((packed)) struct sIPCSharedTGDSSpecific*)(getUserIPCAddress());
	return sIPCSharedTGDSSpecificInst;
}

//inherits what is defined in: common_shared.c
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
	
	switch (cmd1) {
		//NDS7: uses NDS IPC FIFO as a layer from GBA IO @ ARM9
		#ifdef ARM7
		
		u32 command = cmd1;
		switch((command >> 24) & 0xFF) 
		{
			case ARM7COMMAND_START_SOUND:
				SetupSound();
				break;
			case ARM7COMMAND_STOP_SOUND:
				StopSound();
				break;
			case ARM7COMMAND_SOUND_SETRATE:			
				sndRate = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_SETLEN:		
				sampleLen = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_SETMULT:		
				multRate = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_COPY:
			{
				s16 *lbuf = NULL;
				s16 *rbuf = NULL;
				
				if(!sndCursor)
				{
					lbuf = strpcmL0;
					rbuf = strpcmR0;
				}
				else
				{
					lbuf = strpcmL1;
					rbuf = strpcmR1;
				}
				
				u32 i;
				int vMul = getsIPCSharedTGDSSpecific()->sndregioninst.volume;
				int lSample = 0;
				int rSample = 0;
				s16 *arm9LBuf = getsIPCSharedTGDSSpecific()->sndregioninst.arm9L;
				s16 *arm9RBuf = getsIPCSharedTGDSSpecific()->sndregioninst.arm9R;
				
				switch(multRate)
				{
					case 1:				
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);
						}
						
						break;
					case 2:	
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							*lbuf++ = checkClipping(midLSample);
							*rbuf++ = checkClipping(midRSample);
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);
							
							lastL = lSample;
							lastR = rSample;
						}
						
						break;
					case 4:
						// unrolling this one out completely because it's soo much slower
						
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							int firstLSample = (lastL + midLSample) >> 1;
							int firstRSample = (lastR + midRSample) >> 1;
							
							int secondLSample = (midLSample + lSample) >> 1;
							int secondRSample = (midRSample + rSample) >> 1;
							
							*lbuf++ = checkClipping(firstLSample);
							*rbuf++ = checkClipping(firstRSample);
							*lbuf++ = checkClipping(midLSample);
							*rbuf++ = checkClipping(midRSample);
							*lbuf++ = checkClipping(secondLSample);
							*rbuf++ = checkClipping(secondRSample);
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);							
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
				}
				
				VblankUser();
				break;
			}
			case ARM7COMMAND_SOUND_DEINTERLACE:
			{
				s16 *lbuf = NULL;
				s16 *rbuf = NULL;
				
				if(!sndCursor)
				{
					lbuf = strpcmL0;
					rbuf = strpcmR0;
				}
				else
				{
					lbuf = strpcmL1;
					rbuf = strpcmR1;
				}
				
				s16 *iSrc = getsIPCSharedTGDSSpecific()->sndregioninst.interlaced;
				u32 i = 0;
				int vMul = getsIPCSharedTGDSSpecific()->sndregioninst.volume;
				int lSample = 0;
				int rSample = 0;
				
				switch(multRate)
				{
					case 1:
						if(getsIPCSharedTGDSSpecific()->sndregioninst.channels == 2)
						{
							for(i=0;i<sampleLen;++i)
							{					
								lSample = *iSrc++;
								rSample = *iSrc++;
								
								*lbuf++ = checkClipping((lSample * vMul) >> 2);
								*rbuf++ = checkClipping((rSample * vMul) >> 2);
							}
						}
						else
						{
							for(i=0;i<sampleLen;++i)
							{					
								lSample = *iSrc++;
								
								lSample = checkClipping((lSample * vMul) >> 2);
								
								*lbuf++ = lSample;
								*rbuf++ = lSample;
							}
						}
						
						break;
					case 2:	
						for(i=0;i<sampleLen;++i)
						{					
							if(getsIPCSharedTGDSSpecific()->sndregioninst.channels == 2)
							{
								lSample = *iSrc++;
								rSample = *iSrc++;
							}
							else
							{
								lSample = *iSrc++;
								rSample = lSample;
							}
							
							lSample = ((lSample * vMul) >> 2);
							rSample = ((rSample * vMul) >> 2);
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							lbuf[(i << 1)] = checkClipping(midLSample);
							rbuf[(i << 1)] = checkClipping(midRSample);
							lbuf[(i << 1) + 1] = checkClipping(lSample);
							rbuf[(i << 1) + 1] = checkClipping(rSample);
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
					case 4:
						for(i=0;i<sampleLen;++i)
						{				
							if(getsIPCSharedTGDSSpecific()->sndregioninst.channels == 2)
							{
								lSample = *iSrc++;
								rSample = *iSrc++;
							}
							else
							{
								lSample = *iSrc++;
								rSample = lSample;
							}
							
							lSample = ((lSample * vMul) >> 2);
							rSample = ((rSample * vMul) >> 2);
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							int firstLSample = (lastL + midLSample) >> 1;
							int firstRSample = (lastR + midRSample) >> 1;
							
							int secondLSample = (midLSample + lSample) >> 1;
							int secondRSample = (midRSample + rSample) >> 1;
							
							lbuf[(i << 2)] = checkClipping(firstLSample);
							rbuf[(i << 2)] = checkClipping(firstRSample);
							lbuf[(i << 2) + 1] = checkClipping(midLSample);
							rbuf[(i << 2) + 1] = checkClipping(midRSample);
							lbuf[(i << 2) + 2] = checkClipping(secondLSample);
							rbuf[(i << 2) + 2] = checkClipping(secondRSample);
							lbuf[(i << 2) + 3] = checkClipping(lSample);
							rbuf[(i << 2) + 3] = checkClipping(rSample);							
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
				}
				
				VblankUser();
				break;
			}
			case ARM7COMMAND_START_RECORDING:
				micStartRecording();
				break;				
			case ARM7COMMAND_STOP_RECORDING:
				micStopRecording();
				break;
			/*
			case 0xF0: // sgstair lib
				while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
					IRQVBlankWait();
				
				command = REG_IPC_FIFO_RX;
				
				Wifi_Init(command);	
				
				break;
			case 0xF1: // sgstair lib
				Wifi_Sync();
				break;
			*/
			case ARM7COMMAND_BOOT_GBAMP:
				// clear ram that stops certain things from booting
				
				REG_IE = 0;
				REG_IME = 0;
				REG_IF = 0xFFFF;
				
				u32 y;
					
				for(y=0x04800000;y<0x04800FFF;y+=4)
					*((vu32*)y) = 0;
					
				for(y=0x04804000;y<0x04805FFF;y+=4)
					*((vu32*)y) = 0;
					
				for(y=0x037F8000;y<0x037FFFFF;y+=4)
					*((vu32*)y) = 0;
				
				bootGBAMP();
				
				*((vu32*)0x027FFE34) = (u32)0x08000000;	// Bootloader start address
				swiSoftReset();	// Jump to boot loader
				
				break;
			case ARM7COMMAND_BOOT_SUPERCARD:
				REG_IE = 0;
				REG_IME = 0;
				REG_IF = 0xFFFF;
				
				bootOther();
				break;
			case ARM7COMMAND_BOOT_MIGHTYMAX:
				while(1)
				{
					if ((*(vu32 *)0x27FFFFC)) 
					{
						REG_IE = 0 ;
						REG_IME = 0 ;
						REG_IF = 0xFFFF ;
						// now jump where the arm9 calls us too
						// exclusive ram should now be out of use, 
						// and is then ready to be overwritten with the new arm7 code
						
						typedef void (*eloop_type)(void);
						
						eloop_type eloop = *(eloop_type*)0x27FFFFC;
						
						*(vu32 *)0x27FFFFC = 0;
						*(vu32 *)0x27FFFF8 = 0;
						
						eloop();
					}
					
					IRQVBlankWait();
				}
				break;
			case ARM7COMMAND_BOOT_CHISHM:
				REG_IE = 0;
				REG_IME = 0;
				REG_IF = 0xFFFF;
				
				*((vu32*)0x027FFE34) = (u32)0x06000000;
				swiSoftReset();
				
				break;
			case ARM7COMMAND_PSG_COMMAND:
			{				
				SCHANNEL_CR(getsIPCSharedTGDSSpecific()->sndregioninst.psgChannel) = getsIPCSharedTGDSSpecific()->sndregioninst.cr;
                SCHANNEL_TIMER(getsIPCSharedTGDSSpecific()->sndregioninst.psgChannel) = getsIPCSharedTGDSSpecific()->sndregioninst.timer;
				break;
			}
			case ARM7COMMAND_SAVE_WIFI:
			{
				writeWifiSettings();
				getsIPCSharedTGDSSpecific()->sndregioninst.channels = 1;
				break;
			}
			case ARM7COMMAND_LOAD_WIFI:
			{
				GetWfcSettings();
				getsIPCSharedTGDSSpecific()->sndregioninst.channels = 1;
				break;
			}
			case ARM7COMMAND_PLAYCLICK:
			{
				s32 chan = getFreeSoundChannel();
				
				if (chan >= 0) 
				{
					startSound(11025, click_raw, click_raw_length, chan, 40, 63, 1);
				}
				break;
			}
		}
		
		#endif
		
		//NDS9: uses NDS IPC FIFO as a layer from GBA IO @ ARM7
		#ifdef ARM9
		
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
}

//project specific stuff