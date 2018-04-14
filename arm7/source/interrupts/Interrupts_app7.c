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

#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "InterruptsARMCores_h.h"
#include "interrupts.h"
#include "wifi_arm7.h"
#include "main.h"

//User Handler Definitions
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer0handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer1handlerUser(){
	TIMER1Handler();	//DSO T1 handler
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer2handlerUser(){
	micInterrupt();		//DSO T2 handler
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer3handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HblankUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
	if(getsIPCSharedTGDSSpecific()->frameCounter7 < 60){
		getsIPCSharedTGDSSpecific()->frameCounter7++;
	}
	else{
		getsIPCSharedTGDSSpecific()->frameCounter7 = 0;
	}
	
	TransferSound *snd = &getsIPCSharedTGDSSpecific()->sndregioninst.transfersoundInst;
	//IPC->soundData = 0;	//pTransferSound soundData (pointer) that was deprecated long ago
	
	if (snd) {
		int i = 0;
		for (i=0; i<snd->count; ++i) {
			s32 chan = getFreeSoundChannel();
			
			if (chan >= 0) {
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VcounterUser(){
}