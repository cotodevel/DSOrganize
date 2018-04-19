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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libcommon.h>
#include <libdt.h>
#include "dayview.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../font_arial_9.h"
#include "../font_arial_11.h"
#include "../font_gautami_10.h"
#include "../general.h"
#include "../graphics.h"
#include "../globals.h"
#include "../keyboard.h"
#include "../language.h"
#include "../controls.h"
#include "calendar.h"
#include "settings.h"

DV_DYNAMIC *dvStruct = NULL;
uint16 colors[12] = { 0x221F, 0x0200, 0x7C1F, 0x001F, 0x7C00, 0x4210, 0x0318, 0x0360, 0x4010, 0x2D7F, 0x4000, 0x6318 };
bool dvLoaded = false;
int whatType = TYPE_DAILY;

void resetDayView()
{
	dvLoaded = false;
	
	if(dvStruct)
		free(dvStruct);
	
	dvStruct = NULL;
}

void loadRepeatMode()
{
	whatType = TYPE_DAILY;
	
	if(strlen(dvStruct->dayView[curTime]) > 0)
	{
		whatType = dvStruct->dayType[curTime];
	}
}

bool dayViewCursorMove(int curT, int curC)
{
	return dvStruct->dayView[curT][curC] != 0;
}

void loadFileStream(char *tFile, DV_DYNAMIC *dvFile, int wType)
{
	if(DRAGON_FileExists(tFile) == FT_FILE)
	{
		// we can load, it exists		
		DRAGON_FILE *fFile = DRAGON_fopen(tFile, "r");
		
		int timeSlot = 0;
		uint16 textSize = 0;
		
		while(!DRAGON_feof(fFile))
		{
			// get the time it should appear
			timeSlot = DRAGON_fgetc(fFile);
			
			// get the size of the file
			textSize = DRAGON_fgetc(fFile);
			textSize = textSize | DRAGON_fgetc(fFile) << 8;
			
			if(strlen(dvFile->dayPath[timeSlot]) == 0)
			{
				// get the entry itself
				DRAGON_fread(dvFile->dayView[timeSlot], 1, textSize, fFile);
				dvFile->dayType[timeSlot] = wType;
				
				// pop the current filename into the path
				strncpy(dvFile->dayPath[timeSlot], tFile, 255);
			}
			else
			{
				// already have an event there
				DRAGON_fseek(fFile, textSize, SEEK_CUR);
			}
		}	
		
		DRAGON_fclose(fFile);
	}
}

void loadDay(int whichDay, int whichMonth, int whichYear)
{
	// clear out a prior run
	resetDayView();
	
	dvStruct = (DV_DYNAMIC *)trackMalloc(sizeof(DV_DYNAMIC), "tmp dayview");
	memset(dvStruct, 0, sizeof(DV_DYNAMIC));
	
	char tmpFile[256];
	
	// grab the latest day
	sprintf(tmpFile,"%s%02d%02d%04d.DPL", d_day, whichMonth, whichDay, whichYear);
	loadFileStream(tmpFile, dvStruct, TYPE_DAILY);
	
	// grab anything that is weekly	
	sprintf(tmpFile,"%s%02d.DPL", d_day, dayOfWeek(whichDay, whichMonth, whichYear));
	loadFileStream(tmpFile, dvStruct, TYPE_WEEKLY);
	
	// grab anything that is monthly
	sprintf(tmpFile,"%s--%02d----.DPL", d_day, whichDay);
	loadFileStream(tmpFile, dvStruct, TYPE_MONTHLY);
	
	// grab anything that is annual
	sprintf(tmpFile,"%s%02d%02d----.DPL", d_day, whichDay, whichMonth);
	loadFileStream(tmpFile, dvStruct, TYPE_ANNUALLY);
}

void saveFile(char *fName, DV_DYNAMIC *dvFile) 
{
	bool dataExists = false;
	
	// check to see if this file even has data
	for(int tTime = 0;tTime < DV_COUNT; tTime++)
	{
		if(dvStruct->dayView[tTime][0] != 0)
		{
			dataExists = true;
			break;
		}
	}
	
	if(!dataExists)
	{
		// delete the file because data is not present
		
		if(DRAGON_FileExists(fName) == FT_FILE)
		{
			//delete file, nothing in it!
			DRAGON_remove(fName);			
		}
		
		return;
	}
	
	DRAGON_FILE *fFile = DRAGON_fopen(fName, "w");
	
	for(int tTime = 0; tTime < DV_COUNT; tTime++)
	{
		int textSize = strlen(dvFile->dayView[tTime]);
		
		if(textSize > 0)
		{
			DRAGON_fputc((char)tTime, fFile);
			DRAGON_fputc((char)(textSize & 0xFF), fFile);
			DRAGON_fputc((char)((textSize >> 8) & 0xFF), fFile);			
			DRAGON_fwrite(dvFile->dayView[tTime], 1, textSize, fFile); // write the entry itself
		}	
	}
	
	DRAGON_fclose(fFile);
}

void saveData(char *fName)
{
	// create some space for the temporary file
	DV_DYNAMIC *dvSave = (DV_DYNAMIC *)trackMalloc(sizeof(DV_DYNAMIC), "tmp dayview");	
	memset(dvSave, 0, sizeof(DV_DYNAMIC));
	
	// grab the current contents of the file to be updated
	loadFileStream(fName, dvSave, -1);
	
	// loop through the current struct, updating as necessary	
	for(int tTime = 0;tTime < DV_COUNT; tTime++)
	{
		if(strcmp(dvStruct->dayPath[tTime], fName) == 0)
		{
			// an entry needs to be updated			
			memcpy(dvSave->dayView[tTime], dvStruct->dayView[tTime], DV_SIZE);
		}
	}
	
	saveFile(fName, dvSave);
	trackFree(dvSave);
}

void saveDayView()
{
	// set up filenames for new entries
	char tFile[256];
	
	switch(whatType)
	{
		case TYPE_DAILY:
			sprintf(tFile,"%s%02d%02d%04d.DPL", d_day, curMonth, curDay, curYear);
			break;
		case TYPE_WEEKLY:
			sprintf(tFile,"%s%02d.DPL", d_day, dayOfWeek(curDay, curMonth, curYear));
			break;
		case TYPE_MONTHLY:
			sprintf(tFile,"%s--%02d----.DPL", d_day, curDay);
			break;
		case TYPE_ANNUALLY:
			sprintf(tFile,"%s%02d%02d----.DPL", d_day, curDay, curMonth);
			break;
	}
	
	// check to see if the data changed repeating types (and existed before)
	if(dvStruct->dayPath[curTime][0] != 0 && whatType != dvStruct->dayType[curTime])
	{
		// it hopped types, so we must clear it out of the old data		
		char tData[DV_SIZE];
		
		memcpy(tData, dvStruct->dayView[curTime], DV_SIZE);
		memset(dvStruct->dayView[curTime], 0, DV_SIZE);
		
		// save the temporary erased data
		saveData(dvStruct->dayPath[curTime]);
		
		// restore data
		memcpy(dvStruct->dayView[curTime], tData, DV_SIZE);
	}
	
	// now it's safe to update
	dvStruct->dayType[curTime] = whatType;
	strncpy(dvStruct->dayPath[curTime], tFile, 255);
	saveData(dvStruct->dayPath[curTime]);
}

void loadDayPlanner(char **rStr, int tDay, int tMonth, int tYear)
{
	loadDay(tDay, tMonth, tYear);
	
	*rStr = (char *)trackMalloc(1024, "tmp dayview");
	memset(*rStr, 0, 1024);
	
	int a = getCurTime();
	int z = a + 2;
	
	while(z > DV_COUNT - 1)
	{
		a--;
		z--;
	}
	
	for(uint16 x=a;x<=z;x++)
	{
		char str[1024];		
		memset(str, 0, 1024);
		
		if(milTime)
		{
			if(x & 1 == 1)
				sprintf(str,"%02d:30\t%s\n", x >> 1, dvStruct->dayView[x]);
			else
				sprintf(str,"%02d:00\t%s\n", x >> 1, dvStruct->dayView[x]);	
		}
		else
		{
			if(x & 1 == 1)
				sprintf(str,"%d:30\t%s\n", format12(x >> 1), dvStruct->dayView[x]);
			else
				sprintf(str,"%d:00\t%s\n", format12(x >> 1), dvStruct->dayView[x]);
		}			
		
		abbreviateString(str, 240, (uint16 **)font_gautami_10);
		strcat(*rStr, str);
	}
	
	resetDayView();
}

void loadCurrentDayPlanner(char **rStr)
{
	loadDayPlanner(rStr, getDay(), getMonth(), getYear());
}

void drawCurDVTime()
{
	char str[512];
	setFont((uint16 **)font_arial_11);
	setColor(homeDateTextColor);
	
	fb_setClipping(0, 0, 255, 191);
	fb_drawRect(0,191-16, 255, 191, homeDateFillColor);
	
	char date[20];
	if(reverseDate)
		sprintf(date, "%02d/%02d/%04d", curDay, curMonth, curYear);
	else
		sprintf(date, "%d/%02d/%04d", curMonth, curDay, curYear);
	
	if(milTime)
		sprintf(str,"%s, %d:%02d", date, curTime >> 1, 30 *(curTime & 1));
	else
		sprintf(str,"%s, %d:%02d %s", date, format12(curTime >> 1), 30 *(curTime & 1), l_ampm[curTime / 24]);
	
	fb_dispString(3,191-16,str);			
	
	fb_drawRect(0,191-17,255,191-17,homeDateBorderColor);
	fb_drawRect(0,191-19,255,191-18,homeDateBorderFillColor);
	fb_drawRect(0,191-20,255,191-20,homeDateBorderColor);
}

void drawCurrentEvent()
{
	if(!dvLoaded)
	{
		loadDay(curDay, curMonth, curYear);		
		dvLoaded = true;
	}
	
	setFont((uint16 **)font_arial_9);
	setColor(genericTextColor);
	fb_setClipping(3,41,250,190-20);
	fb_dispString(0,-3,dvStruct->dayView[curTime]);
	
	drawCurDVTime();	
}

void drawEditDayView()
{	
	setFont((uint16 **)font_arial_9);
	setColor(textAreaTextColor);
	fb_drawRect(0,37,255,191-18,textAreaFillColor);
	fb_setClipping(3,41,250,190-20);
	setWrapToBorder();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, 1, -1, 0);
		else
			setCursorProperties(cursorOverwriteColor, 1, -1, 0);
			
		showCursor();
		setCursorPos(getCursor());		
	}
	
	setFakeHighlight();
	
	fb_dispString(0,-3,dvStruct->dayView[curTime]);
	
	setWrapNormal();
	hideCursor();
	clearHighlight();
	
	drawCurDVTime();
}

void drawEditCurrentEvent()
{
	char str[512];

	drawKeyboard();
	drawButtonTexts(l_save, l_back);
	
	switch(whatType)
	{
		case TYPE_DAILY:
			drawLREX("N/A", false, l_weekly, true);
			break;
		case TYPE_WEEKLY:
			drawLREX(l_daily, true, l_monthly, true);
			break;
		case TYPE_MONTHLY:
			drawLREX(l_weekly, true, l_annually, true);
			break;
		case TYPE_ANNUALLY:
			drawLREX(l_monthly, true, "N/A", false);
			break;
	}
	
	setFont((uint16 **)font_arial_11);
	setColor(genericTextColor);
	
	char date[20];
	if(reverseDate)
		sprintf(date, "%02d/%02d/%04d", curDay, curMonth, curYear);
	else
		sprintf(date, "%d/%02d/%04d", curMonth, curDay, curYear);
	
	char *whatMode = NULL;
	
	switch(whatType)
	{
		case TYPE_DAILY:			
			whatMode = l_daily;
			break;
		case TYPE_WEEKLY:
			whatMode = l_weekly;
			break;
		case TYPE_MONTHLY:
			whatMode = l_monthly;
			break;
		case TYPE_ANNUALLY:
			whatMode = l_annually;
			break;
	}
	
	if(milTime)
	{
		if(curTime & 1 == 1)
			sprintf(str,"%s @ %02d:30 - %s", date, curTime >> 1, whatMode);
		else
			sprintf(str,"%s @ %02d:00 - %s", date, curTime >> 1, whatMode);	
	}
	else
	{
		if(curTime & 1 == 1)
			sprintf(str,"%s @ %d:30 - %s", date, format12(curTime >> 1), whatMode);
		else
			sprintf(str,"%s @ %d:00 - %s", date, format12(curTime >> 1), whatMode);
	}
	
	bg_dispString(13,20-2,str);
}

void scrollCallBack()
{
	// draw scroll bar contents
	
	for(uint16 x=0;x<=DV_COUNT - 1;x++)
	{
		if(dvStruct->dayView[x][0] != 0)
		{
			double z = (double)100 * (double)x;
			z = z / (double)DV_COUNT - 1;
			bg_drawRect(default_scroll_left + 2, DEFAULT_SCROLL_TOP + SCROLL_HANDLE_HEIGHT + 10 + (int)z, default_scroll_left + DEFAULT_SCROLL_WIDTH - 2, DEFAULT_SCROLL_TOP + SCROLL_HANDLE_HEIGHT + 12 + (int)z, colors[x%12]);
		}
	}
}

void dayviewListCallback(int pos, int x, int y)
{
	char str[512];
	
	if(milTime)
	{
		if(pos & 1 == 1)
			sprintf(str,"%02d:30", (pos >> 1));
		else
			sprintf(str,"%02d:00", (pos >> 1));		
	}
	else
	{
		if(pos & 1 == 1)
			sprintf(str,"%d:30", format12(pos >> 1));
		else
			sprintf(str,"%d:00", format12(pos >> 1));
	}
		
	if(pos == getCurTime() && curMonth == getMonth() && curYear == getYear() && curDay == getDay())
		setColor(calendarCurrentDateColor);
	else
	{
		if(strlen(dvStruct->dayView[pos]) > 0)
			setColor(colors[pos % 12]);
		else
			setColor(listTextColor);			
	}
	
	bg_dispString(13, 0, str);
	
	if(strlen(dvStruct->dayView[pos]) > 0)
	{
		int tType = dvStruct->dayType[pos];
		
		uint16 *tSprite[] = { dv_daily, dv_weekly, dv_monthly, dv_annually };
		tSprite[tType][4] = listTextColor;
		tSprite[tType][5] = listFillColor;
		
		bg_dispSprite(x + 2, y + 2, tSprite[tType], 0xFFFF);
	}
	
	setColor(listTextColor);
	
	memset(str, 0, 512);
	
	for(uint16 bb=0;bb<strlen(dvStruct->dayView[pos]);bb++)
	{
		if(dvStruct->dayView[pos][bb] != 10 && dvStruct->dayView[pos][bb] != 13)
			str[bb] = dvStruct->dayView[pos][bb];			
		else
			str[bb] = ' ';
	}
	
	abbreviateString(str, list_right - (list_left + 46), (uint16 **)font_arial_9);
	bg_dispString(46, 0, str);
}

void increaseDuration()
{
	whatType++;
	
	if(whatType > TYPE_ANNUALLY)
		whatType = TYPE_ANNUALLY;
}

void decreaseDuration()
{
	whatType--;
	
	if(whatType < TYPE_DAILY)
		whatType = TYPE_DAILY;
}

void drawDayView()
{
	drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, curTime, DV_COUNT, NULL, dayviewListCallback);
	bg_drawBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, listBorderColor);
	drawScrollBarCustom(curTime, DV_COUNT, scrollCallBack, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT);
	
	drawButtonTexts(l_editselected, l_calendar);
	drawHome();
	drawLR(l_prevday, l_nextday);
}

void dvBookMarkUp()
{
	if(curTime == 0)
		return;
	
	for(int i=curTime-1;i>=0;--i)
	{
		if(dvStruct->dayView[i][0] != 0)
		{
			curTime = i;
			return;
		}
	}
}

void dvBookMarkDown()
{
	if(curTime == DV_COUNT - 1)
		return;
	
	for(int i=curTime+1;i<DV_COUNT;++i)
	{
		if(dvStruct->dayView[i][0] != 0)
		{
			curTime = i;
			return;
		}
	}
}

void editDayViewAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == CLEAR_KEY)
	{
		memset(dvStruct->dayView[curTime],0,DV_SIZE);
		resetCursor();
		return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
		
	char *tmpBuffer = (char *)trackMalloc(DV_SIZE,"dayviewer kbd");
	memcpy(tmpBuffer,dvStruct->dayView[curTime],DV_SIZE);
	int oldCursor = getCursor();
	
	genericAction(tmpBuffer, DV_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuffer, 3, 41, 250, 190-20, &pts, (uint16 **)font_arial_9);
	free(pts);
	
	if(numPts <= 8)
		memcpy(dvStruct->dayView[curTime],tmpBuffer,DV_SIZE);
	else
		moveCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}
