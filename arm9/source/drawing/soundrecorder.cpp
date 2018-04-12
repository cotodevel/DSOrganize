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
#include "specific_shared.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libfb\libcommon.h>
#include "soundrecorder.h"
#include "browser.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../general.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../globals.h"
#include "../settings.h"
#include "../graphics.h"
#include "../sound.h"
#include "../filerout.h"
#include "../language.h"
#include "../controls.h"

static int recordMode = SR_RECORDING;
bool wavLoaded = false;
bool isRecording = false;
u32 ticCount = 0;
static WAV_INFO curWav;
static DRAGON_FILE *recFile = NULL;

extern char fileName[256];

void copyChunk()
{
	if(!isRecording)
		return;
	
	u8 *micData = NULL;
	
	switch(soundIPC->channels)
	{
		case 0:
			micData = (u8 *)soundIPC->arm9L;
			break;
		case 1:
			micData = (u8 *)soundIPC->arm9R;
			break;
		case 2:
			micData = (u8 *)soundIPC->interlaced;
			break;
	}
	
	DRAGON_fwrite(micData, 1, REC_BLOCK_SIZE, recFile);
	checkJustKeys();
}

void startRecording()
{
	if(isRecording)
		return;
	
	if(DRAGON_FileExists(fileName) != FE_NONE)
		DRAGON_remove(fileName);
	
	recFile = DRAGON_fopen(fileName, "w");
	
	// write header	
	char header[12];
	strncpy(header, "RIFF    WAVE", 12);
	DRAGON_fwrite(header, 1, 12, recFile);
	
	// write fmt tag
	wavFormatChunk headerChunk;
	
	strncpy(headerChunk.chunkID, "fmt ", 4);
	headerChunk.chunkSize = 16; // unknown now
	headerChunk.wFormatTag = 1;
	headerChunk.wChannels = 1;
	headerChunk.wBitsPerSample = 8;
	headerChunk.dwSamplesPerSec = REC_FREQ;
	headerChunk.wBlockAlign = 1;
	headerChunk.dwAvgBytesPerSec = REC_FREQ;
	
	DRAGON_fwrite(&headerChunk, sizeof(wavFormatChunk), 1, recFile);
	
	char data[8];
	strncpy(data, "data    ", 8);
	DRAGON_fwrite(data, 1, 8, recFile);
	
	soundIPC->arm9L = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");
	soundIPC->arm9R = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");
	soundIPC->interlaced = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");

	memset(soundIPC->arm9L, 0, REC_BLOCK_SIZE);
	memset(soundIPC->arm9R, 0, REC_BLOCK_SIZE);
	memset(soundIPC->interlaced, 0, REC_BLOCK_SIZE);
	
	setSoundFrequency(REC_FREQ);	
	setSoundLength(REC_BLOCK_SIZE);
	SendArm7Command(ARM7COMMAND_START_RECORDING, 0);
	
	ticCount = 0;
	isRecording = true;
}

void endRecording()
{
	if(!isRecording)
		return;
	
	SendArm7Command(ARM7COMMAND_STOP_RECORDING, 0);
	//copyChunk();
	isRecording = false;
	
	trackFree(soundIPC->arm9L);
	trackFree(soundIPC->arm9R);
	trackFree(soundIPC->interlaced);
	
	u32 length = DRAGON_flength(recFile) - 8;
	DRAGON_fseek(recFile, 4, 0);
	DRAGON_fwrite(&length, 4, 1, recFile);
	
	length = DRAGON_flength(recFile) - 44;
	DRAGON_fseek(recFile, 40, 0);
	DRAGON_fwrite(&length, 4, 1, recFile);
	
	DRAGON_fclose(recFile);
	
	recFile = NULL;
}

void getWavInformation(char *fName, WAV_INFO *wavInfo)
{
	wavFormatChunk headerChunk;
	char header[13];
	
	DRAGON_FILE *fp = DRAGON_fopen(fName, "r");	
	DRAGON_fread(header, 1, 12, fp);
	
	header[12] = 0;
	header[4] = ' ';
	header[5] = ' ';
	header[6] = ' ';
	header[7] = ' ';
	
	if(strcmp(header, "RIFF    WAVE") != 0)
	{
		// wrong header
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;
	}
	
	DRAGON_fread(&headerChunk, sizeof(wavFormatChunk), 1, fp);
	
	if(strncmp(headerChunk.chunkID, "fmt ", 4) != 0)
	{
		// wrong chunk at beginning
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;
	}
	
	if(headerChunk.wFormatTag != 1)
	{
		// compression used, hell no to loading this
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;
	}
	
	if(headerChunk.wChannels > 2)
	{
		// more than 2 channels.... uh no!
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;
	}
	else
	{
		wavInfo->channels = headerChunk.wChannels;
	}
	
	if(headerChunk.wBitsPerSample <= 8)
		wavInfo->significantBits = 8;
	else if(headerChunk.wBitsPerSample <= 16)
		wavInfo->significantBits = 16;
	else
	{
		// more than 16bit sound, not supported
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;	
	}
	
	DRAGON_fseek(fp, (headerChunk.chunkSize + 8) + 12, 0); // seek to next chunk
	
	char fmtHeader[5];
	
	DRAGON_fread(fmtHeader, 1, 4, fp);
	fmtHeader[4] = 0;
	
	if(strcmp(fmtHeader, "data") != 0)
	{
		// wrong chunk next, sorry, doing strict only
		
		DRAGON_fclose(fp);
		wavInfo->validWav = false;
		return;
	}
	
	uint len = 0;
	DRAGON_fread(&len, 4, 1, fp);
	
	wavInfo->length = len;
	wavInfo->frequency = headerChunk.dwSamplesPerSec;
	wavInfo->seconds = ((wavInfo->length / wavInfo->channels) / (wavInfo->significantBits >> 3)) / wavInfo->frequency;
	wavInfo->validWav = true;
	
	DRAGON_fclose(fp);
}

void drawTopRecordScreen()
{
	if(!wavLoaded)
	{
		DRAGON_FILE *fp = DRAGON_fopen(fileName, "r");
		memset(&curWav, 0, sizeof(WAV_INFO));
		
		if(DRAGON_flength(fp) == 0)
		{
			// this file is new
			
			recordMode = SR_RECORDING;
			DRAGON_fclose(fp);
		}
		else
		{
			DRAGON_fclose(fp);
			getWavInformation(fileName, &curWav);
			
			if(!curWav.validWav)
				recordMode = SR_RECORDING;
			else
				recordMode = SR_PLAYBACK;
		}
		
		wavLoaded = true;
	}
	
	drawFileInfoScreen();
	
	char str[256];
	
	sprintf(str, "%c %s: ", BUTTON_X, l_recordingmode);
	
	switch(recordMode)
	{
		case SR_RECORDING:
			strcat(str, l_record);
			break;
		case SR_PLAYBACK:
			strcat(str, l_playback);
			break;
	}
	
	setColor(genericTextColor);
	fb_setDefaultClipping();
	fb_dispString(5, 175, str);
}

void drawPlayProgress(u32 curSize, u32 maxSize)
{
	bg_drawFilledRect(10, 20, 244, 40, widgetBorderColor, soundSeekFillColor);
	
	float percent = (float)(curSize) / (float)maxSize;
	int pixel = (int)(232 * percent);
	
	bg_drawRect(11, 21, 11 + pixel, 39, soundSeekForeColor);	
}

void drawBottomRecordScreen()
{	
	char str[20];
	
	if(recordMode == SR_RECORDING)
	{			
		if(isRecording)
			drawButtonTexts(l_stop, NULL);
		else
			drawButtonTexts(l_record, NULL);
		
		setColor(genericTextColor);
		sprintf(str, "%s: %d", l_length, ticCount / 60);
		bg_dispString(10, 20, str);	
	}
	else
	{
		if(getState() == STATE_STOPPED || getState() == STATE_UNLOADED)
		{
			drawPlayProgress(0, 1);
		}
		else
		{
			u32 curSize;
			u32 maxSize;
			getSoundLoc(&curSize, &maxSize);
			
			drawPlayProgress(curSize, maxSize);
		}
		
		if(getState() == STATE_PLAYING)
			drawButtonTexts(l_pause, l_stop);
		else
			drawButtonTexts(l_play, l_stop);
		
		setColor(genericTextColor);
		sprintf(str, "%s: %d", l_length, curWav.seconds);
		bg_dispString(10, 60, str);	
	}
	
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);
	sprintf(str,"%c\a\a%s", BUTTON_START, l_back);	
	bg_dispString(centerOnPt(128,str, (uint16 **)font_gautami_10), 3, str);
}

void recorderStartPressed() // save and bail to browser
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
	}
	
	if(recordMode == SR_RECORDING)
	{
		if(isRecording)
			endRecording();
	}
	
	setMode(BROWSER);
	browserSetUnPopulated();
	freeDirList();
}

void recorderForward() // a button pressed
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_PLAYING:
				pauseSound(true);
				break;
			case STATE_PAUSED:
				pauseSound(false);
				break;
			case STATE_STOPPED: // sound hasn't started yet
			case STATE_UNLOADED: // sound hasn't started yet
				loadSound(fileName);
				break;
		}
		
		return;
	}
	
	if(recordMode == SR_RECORDING)
	{
		if(isRecording)
			endRecording();
		else
			startRecording();
		
		return;
	}
}

void recorderBack() // b button pressed
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
		return;
	}
}

void seekRecordSound(int xPos)
{
	if(recordMode != SR_PLAYBACK)
		return;
	if(getState() == STATE_STOPPED || getState() == STATE_UNLOADED)
		return;
	
	if(xPos < 10 || xPos > 244)
		return;
	
	u32 curSize;
	u32 maxSize;
	getSoundLoc(&curSize, &maxSize);
	
	float percent = (float)(xPos - 10) / (float)(234);
	u32 newLoc = (int)(maxSize * percent);
	
	setSoundLoc(newLoc);
}

void switchRecordingMode()
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
		
		recordMode = SR_RECORDING;		
		return;
	}
	
	if(recordMode == SR_RECORDING)
	{	
		if(isRecording)
			endRecording();
		
		recordMode = SR_PLAYBACK;
		
		getWavInformation(fileName, &curWav);	
		return;
	}
}
