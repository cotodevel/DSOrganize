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

/***************************************************************************
 *                                                                         *
 *  This file is part of DSOrganize.                                       *
 *                                                                         *
 *  DSOrganize is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  DSOrganize is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with DSOrganize.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/
 

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
 
#include <string.h>
#include "main.h"
#include "InterruptsARMCores_h.h"
#include "interrupts.h"

#include "specific_shared.h"
#include "wifi_arm7.h"
#include "usrsettingsTGDS.h"
#include "timerTGDS.h"
#include "CPUARMTGDS.h"

#include <dswifi7.h>
#include "wifi_arm7.h"

#include "resetmem.h"
#include "wifi_shared.h"
#include "microphone7.h"

#include "click_raw.h"

#include "spiTGDS.h"
#include "spifwTGDS.h"

bool touchDown = false;
bool touchDelay = false;

s16 *strpcmL0 = NULL;
s16 *strpcmL1 = NULL;
s16 *strpcmR0 = NULL;
s16 *strpcmR1 = NULL;

int lastL = 0;
int lastR = 0;

int multRate = 1;
int pollCount = 100; //start with a read

u32 sndCursor = 0;
u32 micBufLoc = 0;
u32 sampleLen = 0;
int sndRate = 0;




// identical to the mic controls but i'm lazy, and arm7 has no need to be memory efficient

void pmSetReg(int reg, int control)
{
	SPIWAITCNT();
	REG_SPI_CR = BIT_SPICNT_ENABLE | BIT_SPICNT_POWERCNT | BIT_SPICLK_1MHZ | BIT_SPICNT_CSHOLDENABLE;
	REG_SPI_DATA = reg;
	SPIWAITCNT();
	REG_SPI_CR = BIT_SPICNT_ENABLE | BIT_SPICNT_POWERCNT | BIT_SPICLK_1MHZ;
	REG_SPI_DATA = control; 
}

u8 pmGetReg(int reg)
{
	SPIWAITCNT();
	REG_SPI_CR = BIT_SPICNT_ENABLE | BIT_SPICNT_POWERCNT | BIT_SPICLK_1MHZ | BIT_SPICNT_CSHOLDENABLE;
	REG_SPI_DATA = reg | 0x80;
	SPIWAITCNT();
	REG_SPI_CR = BIT_SPICNT_ENABLE | BIT_SPICNT_POWERCNT | BIT_SPICLK_1MHZ ;
	REG_SPI_DATA = 0;
	SPIWAITCNT();
	return REG_SPI_DATA & 0xFF;
}

void read_nvram(u32 src, void *dst, u32 size) {
	((call3)0x2437)(src,dst,size);
}

u8 writeread(u8 data) {
	while (REG_SPI_CR & BIT_SPICNT_BUSY);
	REG_SPI_DATA = data;
	while (REG_SPI_CR & BIT_SPICNT_BUSY);
	return REG_SPI_DATA;
}

int flash_verify(u8 *src,u32 dst)
{
	int i;
	u8 tmp[256];
	static int result=2;
	
	read_nvram(dst,tmp,256);
	for(i=0;i<256;i++) {
		if(tmp[i]!=src[i])
			break;
	}
	if(i==256)
		return result;
	
	result = 1;
	//write enable
	REG_SPI_CR = BIT_SPICNT_ENABLE|BIT_SPICNT_CSHOLDENABLE|BIT_SPICNT_NVRAMCNT;
	writeread(6);
	REG_SPI_CR = 0;
	
	//Wait for Write Enable Latch to be set
	REG_SPI_CR = BIT_SPICNT_ENABLE|BIT_SPICNT_CSHOLDENABLE|BIT_SPICNT_NVRAMCNT;
	writeread(5);
	while((writeread(0)&0x02)==0); //Write Enable Latch
	REG_SPI_CR = 0;
		
		

	 
	//page write
	REG_SPI_CR = BIT_SPICNT_ENABLE|BIT_SPICNT_CSHOLDENABLE|BIT_SPICNT_NVRAMCNT;
	writeread(0x0A);
	writeread((dst&0xff0000)>>16);
	writeread((dst&0xff00)>>8);
	writeread(0);
	for (i=0; i<256; i++) {
		writeread(src[i]);
	}

	REG_SPI_CR = 0;
	// wait programming to finish
	REG_SPI_CR = BIT_SPICNT_ENABLE|BIT_SPICNT_CSHOLDENABLE|BIT_SPICNT_NVRAMCNT;
	writeread(0x05);
	while(writeread(0)&0x01);	//Write In Progress
	REG_SPI_CR = 0;

	//read back & compare
	read_nvram(dst,tmp,256);
	for(i=0;i<256;i++) {
		if(tmp[i]!=src[i])
			return 0;
	}
	return 1;
}

void writeWifiSettings()
{
	// figure out where settings are
	u32 wfcBase = ReadFlashBytes(0x20, 2) * 8 - 0x400;
	
	// loop through wifi settings and reconstruct data
	int i;
	for(i=0;i<3;i++)
	{
		u8 wifiBlock[256];
		memset(wifiBlock, 0, 256);
		
		// read in the current data (if we don't do this, a new friendcode will have to be generated after saving)		
		readFirmwareSPI(wfcBase + (i<<8), (char *)wifiBlock, 256);	//Read_Flash(wfcBase + (i<<8), (char *)wifiBlock, 256);
		
		if(WifiData->wfc_enable[i] > 0)
		{
			// valid profile
			wifiBlock[0xE7] = 0x00;
			
			// modified from the wifi_arm7 read fuction
			wifiBlock[0xE6] = (WifiData->wfc_enable[i] & 0x0F);
			
			int n;
			for(n=0;n<16;n++) wifiBlock[0x80+n] = WifiData->wfc_wepkey[i][n];
			for(n=0;n<32;n++) wifiBlock[0x40+n] = WifiData->wfc_ap[i].ssid[n];
			
			memcpy(&wifiBlock[0xC0], (const void *)&WifiData->wfc_config[i][0], 4);
			memcpy(&wifiBlock[0xC4], (const void *)&WifiData->wfc_config[i][1], 4);
			memcpy(&wifiBlock[0xC8], (const void *)&WifiData->wfc_config[i][3], 4);
			memcpy(&wifiBlock[0xCC], (const void *)&WifiData->wfc_config[i][4], 4);
			
			// very simple subnet function
			// assumes they typed it in correctly
			char subnet = 0;
			for(n=0;n<32;n++)
			{
				if((WifiData->wfc_config[i][2] >> n) & 1)
				{
					subnet++;
				}
			}
			
			wifiBlock[0xD0] = subnet;
		}
		else
		{
			// invalid profile
			wifiBlock[0xE7] = 0xFF;
		}
		
		// calculate the crc
		uint16 crcBlock = crc16_slow(wifiBlock, 254);		
		memcpy(&wifiBlock[0xFE], &crcBlock, 2);
		
		// flash back to the firmware
		flash_verify(wifiBlock, wfcBase + (i<<8));
	}
}

s16 checkClipping(int data)
{
	if(data > 32767)
		return 32767;
	if(data < -32768)
		return -32768;
	
	return data;
}

void mallocData(int size)
{
    // this no longer uses malloc due to using vram bank d.
  
	strpcmL0 = VRAM_D;
	strpcmL1 = strpcmL0 + (size >> 1);
	strpcmR0 = strpcmL1 + (size >> 1);
	strpcmR1 = strpcmR0 + (size >> 1);
	
	// clear vram d bank to not have sound leftover
	
	int i;
	
	for(i=0;i<(size);++i)
	{
		strpcmL0[i] = 0;
	}
	
	for(i=0;i<(size);++i)
	{
		strpcmR0[i] = 0;
	}
}

void freeData()
{	
	//free(strpcmR0);
}

void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) 
{
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}

s32 getFreeSoundChannel() // modified to only look on channels 4 and up
{
	int i;
	for (i=4;i<16;++i) 
		if (!(SCHANNEL_CR(i) & SCHANNEL_ENABLE)) return i;
	
	return -1;
}


//---------------------------------------------------------------------------------
int writePowerManagement(int reg, int command) {
//---------------------------------------------------------------------------------
  // Write the register / access mode (bit 7 sets access mode)
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  REG_SPI_CR = BIT_SPICNT_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | BIT_SPICNT_CSHOLDENABLE | BIT_SPICNT_POWERCNT;
  REG_SPI_DATA = reg;

  // Write the command / start a read
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  REG_SPI_CR = BIT_SPICNT_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | BIT_SPICNT_POWERCNT;
  REG_SPI_DATA = command;

  // Read the result
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  return REG_SPI_DATA & 0xFF;
}

int readPowerManagement(int reg)
{
	return writePowerManagement((reg)|PM_READ_REGISTER, 0);
}


void setSwapChannel()
{
	s16 *buf;
  
	if(!sndCursor)
		buf = strpcmL0;
	else
		buf = strpcmL1;
    
	// Left channel
	SCHANNEL_SOURCE((sndCursor << 1)) = (uint32)buf;
	SCHANNEL_CR((sndCursor << 1)) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0) | SOUND_16BIT;
    	
	if(!sndCursor)
		buf = strpcmR0;
	else
		buf = strpcmR1;
	
	// Right channel
	SCHANNEL_SOURCE((sndCursor << 1) + 1) = (uint32)buf;
	SCHANNEL_CR((sndCursor << 1) + 1) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0x3FF) | SOUND_16BIT;
  
	sndCursor = 1 - sndCursor;
}

void SetupSound()
{
    sndCursor = 0;
	
	if(multRate != 1 && multRate != 2 && multRate != 4)
		multRate = 1;
	
	mallocData(sampleLen * 2 * multRate);
    
	TIMERXDATA(0) = SOUND_FREQ((sndRate * multRate));
	TIMERXCNT(0) = TIMER_DIV_1 | TIMER_ENABLE;
  
	TIMERXDATA(1) = 0x10000 - (sampleLen * 2 * multRate);
	TIMERXCNT(1) = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
	
	//irqSet(IRQ_TIMER1, TIMER1Handler);
	
	int ch;
	
	for(ch=0;ch<4;++ch)
	{
		SCHANNEL_CR(ch) = 0;
		SCHANNEL_TIMER(ch) = SOUND_FREQ((sndRate * multRate));
		SCHANNEL_LENGTH(ch) = (sampleLen * multRate) >> 1;
		SCHANNEL_REPEAT_POINT(ch) = 0;
	}
	
	//irqSet(IRQ_VBLANK, 0);
	//irqDisable(IRQ_VBLANK);
	DisableIrq(IRQ_VBLANK);
	
	lastL = 0;
	lastR = 0;
}

void StopSound()
{
	//irqSet(IRQ_TIMER1, 0);
	TIMERXCNT(0) = 0;
	TIMERXCNT(1) = 0;
	
	SCHANNEL_CR(0) = 0;
	SCHANNEL_CR(1) = 0;
	SCHANNEL_CR(2) = 0;
	SCHANNEL_CR(3) = 0;
	
	freeData();
	//irqSet(IRQ_VBLANK, VblankHandler);
	//irqEnable(IRQ_VBLANK);	
	EnableIrq(IRQ_VBLANK);
}

void TIMER1Handler()
{	
	setSwapChannel();
	SendMultipleWordACK(ARM9COMMAND_UPDATE_BUFFER, 0, 0, NULL);
}

void micStartRecording()
{
	micBufLoc = 0;
	sndCursor = 0;
	
	TurnOnMicrophone();
	PM_SetGain(GAIN_160);
	
	TIMERXDATA(2) = TIMER_FREQ(sndRate);
	TIMERXCNT(2) = TIMER_ENABLE | TIMER_DIV_1 | TIMER_IRQ_REQ;	
	//irqSet(IRQ_TIMER2, micInterrupt); // microphone stuff
}

void micStopRecording()
{
	//irqSet(IRQ_TIMER2, 0); // microphone stuff	
	TIMERXCNT(2) = 0;
	
	TurnOffMicrophone();
}

void micInterrupt()
{
#ifdef MIC_8
	u8 *micData = NULL;
	
	switch(sndCursor)
	{
		case 0:
			micData = (u8 *)getsIPCSharedTGDSSpecific()->sndregioninst.arm9L;
			break;
		case 1:
			micData = (u8 *)getsIPCSharedTGDSSpecific()->sndregioninst.arm9R;
			break;
		case 2:
			micData = (u8 *)getsIPCSharedTGDSSpecific()->sndregioninst.interlaced;
			break;
	}
	
	int tempX = (MIC_GetData8() - 128) * 4;
	
	if(tempX > 127)
		tempX = 127;
	if(tempX < -128)
		tempX = -128;
	
	tempX += 128;
	
	micData[micBufLoc] = (u8)tempX;
	
	++micBufLoc;
	if(micBufLoc == sampleLen)
	{
		micBufLoc = 0;
		getsIPCSharedTGDSSpecific()->sndregioninst.channels = sndCursor;
		++sndCursor;
		if(sndCursor > 2)
			sndCursor = 0;
		
		SendMultipleWordACK(ARM9COMMAND_SAVE_DATA, 0, 0, NULL); // send command to save the buffer
	}
#endif
#ifdef MIC_16
	s16 *micData = NULL;
	
	switch(sndCursor)
	{
		case 0:
			micData = (s16 *)getsIPCSharedTGDSSpecific()->sndregioninst.arm9L;
			break;
		case 1:
			micData = (s16 *)getsIPCSharedTGDSSpecific()->sndregioninst.arm9R;
			break;
		case 2:
			micData = (s16 *)getsIPCSharedTGDSSpecific()->sndregioninst.interlaced;
			break;
	}
	
	int tempX = (MIC_ReadData12() - 2048) << 6;

	if(tempX > 32767)
		tempX = 32767;
	if(tempX < -32768)
		tempX = -32768;
	
	micData[micBufLoc++] = tempX;
	
	if(micBufLoc == sampleLen)
	{
		micBufLoc = 0;
		getsIPCSharedTGDSSpecific()->sndregioninst.channels = sndCursor;
		++sndCursor;
		if(sndCursor > 2)
			sndCursor = 0;
		
		SendMultipleWordACK(ARM9COMMAND_SAVE_DATA, 0, 0, NULL); // send command to save the buffer
	}
#endif
}

/*
void SendMultipleWordACK(u32 command) 
{	
	while((REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL))
		IRQVBlankWait();
	
    REG_IPC_FIFO_TX = command;
}
*/

/*
void FIFO_Receive() 
{
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
	{
		u32 command = REG_IPC_FIFO_RX;
		
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
				
				VblankHandler();
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
				
				VblankHandler();
				break;
			}
			case ARM7COMMAND_START_RECORDING:
				micStartRecording();
				break;				
			case ARM7COMMAND_STOP_RECORDING:
				micStopRecording();
				break;
			case 0xF0: // sgstair lib
				while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
					IRQVBlankWait();
				
				command = REG_IPC_FIFO_RX;
				
				Wifi_Init(command);	
				
				break;
			case 0xF1: // sgstair lib
				Wifi_Sync();
				break;
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
					startSound(11025, click_raw, click_raw_size, chan, 40, 63, 1);
				}
				break;
			}
		}
	}
}
*/
/*
void arm7_synctoarm9() 
{
	REG_IPC_FIFO_TX = 0x87654321;
}
*/


//---------------------------------------------------------------------------------
int main(int _argc, sint8 **_argv) {
//---------------------------------------------------------------------------------
	IRQInit();
	installWifiFIFO();		//use DSWIFI
	
	POWER_CR = POWER_SOUND;
	SOUND_CR = SOUND_ENABLE | 127;
	
	*(vu32 *)0x27FFFFC = 0; // arm7 loop address to default
 
	//irqInit();
	//irqSet(IRQ_VBLANK, VblankHandler);
	//irqEnable(IRQ_VBLANK);
	
	//irqSet(IRQ_TIMER1, 0);
	//irqEnable(IRQ_TIMER1);
	
	//irqSet(IRQ_WIFI, Wifi_Interrupt); // set up wifi interrupt
	//irqEnable(IRQ_WIFI);
	
	//irqSet(IRQ_TIMER2, 0); // microphone stuff
	//irqEnable(IRQ_TIMER2);
	
	//irqSet(IRQ_FIFO_NOT_EMPTY, FIFO_Receive);
	//irqEnable(IRQ_FIFO_NOT_EMPTY);
	//REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
	
	setSenderStatus(true);
    while (1) {
		IRQVBlankWait();
	}
   
	return 0;
}