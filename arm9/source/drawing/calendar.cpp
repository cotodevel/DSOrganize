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
#include <stdlib.h>
#include <string.h>
#include <libcommon.h>
#include <libdt.h>
#include "calendar.h"
#include "dayview.h"
#include "../fatwrapper.h"
#include "../globals.h"
#include "../colors.h"
#include "../font_arial_11.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../general.h"
#include "../keyboard.h"
#include "../language.h"
#include "../controls.h"
#include "fsfatlayerTGDSLegacy.h"

static char *reminder = NULL;
static char *dayView = NULL;

uint16 reminders[32];
uint16 dayViews[32];
uint16 curDay;
uint16 curMonth;
uint16 curYear;
uint16 curTime;
uint16 oldYear;
uint16 oldDay;
uint16 oldMonth;

static bool calLoaded = false;

extern uint16 dayViews[32];
extern bool reverseDate;

void destroyDay()
{
	if(dayView != NULL)
		trackFree(dayView);
	
	dayView = NULL;
}

void destroyReminder()
{
	if(reminder != NULL)
		trackFree(reminder);
	
	reminder = NULL;
}

void createReminder()
{
	destroyReminder();
	
	reminder = (char *)trackMalloc(REM_SIZE, "reminder");
	memset(reminder,0,REM_SIZE);
}

void resetCalLoaded()
{
	destroyReminder();
	destroyDay();
	calLoaded = false;
}

bool isReminderEnd(int c)
{
	if(reminder == NULL)
		return true;
	
	if(reminder[c] == 0)
		return true;
	
	return false;
}

void drawCalendar()
{	
	uint16 x = 0;
	uint16 y = 0;
	uint16 z = 0;
	
	setFont((uint16 **)font_arial_11);
	setColor(genericTextColor);
	
	{
		char str[128];
		sprintf(str, "%s - %d",l_months[curMonth-1],curYear);
		bg_dispString(23,20-2,str);
	}
	
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);
	
	for(x=0;x<7;x++)
	{		
		bg_dispString(23+(x*31), 20+20, l_days[x+7]);
		bg_drawRect(23+(x*31), 28+20, 20+(x*31)+27, 28+20, calendarDayUnderlineColor);
	}
	
	z = 1;	
	x = dayOfWeek(1,curMonth,curYear);
	y = 0;
	
	if(curDay > daysInMonth(curMonth-1,curYear))
		curDay = daysInMonth(curMonth-1,curYear);
	
	while(z <= daysInMonth(curMonth-1,curYear))
	{	
		if(dayOfWeek(z,curMonth,curYear) == 0 || dayOfWeek(z,curMonth,curYear) == 6)
	    	setColor(calendarWeekendColor);
		else
			setColor(calendarNormalColor);		
		
		if(z == curDay)
		{
			// we gotta draw a rect behind today
			bg_drawRect(23+(x*31), 57+(15*y), 44+(x*31), 70+(15*y), genericHighlightColor);			
		}
		
		if(dayViews[z] == 1)
			bg_drawRect(26+(x*31), 68+(15*y), 41+(x*31), 68+(15*y), calendarHasDayPlannerColor);			
		
		if(reminders[z] == 1)
			setColor(calencarHasReminderColor);		
		
		if(curMonth == getMonth() && curYear == getYear() && (z == getDay()))
			setColor(calendarCurrentDateColor);
		
		char str[512];	
		
		sprintf(str,"%02d", z);
		bg_dispString(28+(x*31), 40 + 20 + (15*y), str);		
		
		x++;
		if(x > 6)
		{
			x = 0;
			y++;
		}
		
		z++;
	}
	
	drawLR(l_prev, l_next);
	drawButtonTexts(l_editreminder, l_viewday);	
	drawHome();
}

int getDayFromTouch(int tx, int ty)
{
	uint16 z = 1;	
	uint16 x = dayOfWeek(1,curMonth,curYear);
	uint16 y = 0;
	
	while(z <= daysInMonth(curMonth-1,curYear))
	{	
		if(tx > 23+(x*31) && tx < 44+(x*31))
		{
			if(ty > 57+(15*y) && ty < 70+(15*y))
				return z;
		}		
		
		x++;
		if(x > 6)
		{
			x = 0;
			y++;
		}
		
		z++;
	}
	
	return -1;
}

void drawCurDay()
{
	fb_setClipping(3, 152, 252, 192);
	
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);
	
	fb_dispString(0,5,dayView);
	
	fb_drawRect(0,191-39,255,191-39,homeDateBorderColor);
	fb_drawRect(0,191-41,255,191-40,homeDateBorderFillColor);
	fb_drawRect(0,191-42,255,191-42,homeDateBorderColor);
}

void drawReminders()
{
	
	//we have to load new reminders first...
	std::string PathFix;	//char str[MAX_TGDSFILENAME_LENGTH+1] = {0};
	
	if(!(curYear == oldYear && curMonth == oldMonth && curDay == oldDay) || !calLoaded)
	{
		if(curMonth != oldMonth || !calLoaded)
		{
			// we gotta update the reminders thing
			for(uint16 x=1;x<=31;x++)
			{	
				PathFix = string(getDefaultDSOrganizeReminderPath(std::to_string(curMonth) + std::to_string(x) + std::to_string(curYear) + string(".REM")));
				if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
					reminders[x] = 1;
				else
					reminders[x] = 0;
					
				PathFix = getDefaultDSOrganizeDayPath(std::to_string(curMonth) + std::to_string(x) + std::to_string(curYear) + (".DPL"));
				if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
				{
					dayViews[x] = 1;
				}
				else
				{
					PathFix = getDefaultDSOrganizeDayPath(std::to_string(dayOfWeek(x, curMonth, curYear)) + (".DPL"));
					if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
					{
						dayViews[x] = 1;
					}
					else
					{
						PathFix = getDefaultDSOrganizeDayPath(std::to_string(x));
						if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
						{
							dayViews[x] = 1;
						}
						else
						{
							PathFix = getDefaultDSOrganizeDayPath(std::to_string(curDay) + std::to_string(curMonth));
							if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
							{
								dayViews[x] = 1;
							}
							else
							{
								dayViews[x] = 0;
							}
						}
					}
				}
			}
		}
		
		calLoaded = true;
		

		oldYear = curYear;
		oldMonth = curMonth;
		oldDay = curDay;	
		
		std::string PathFix2 = (getDefaultDSOrganizeReminderPath(std::to_string(curMonth) + std::to_string(curDay) + std::to_string(curYear) + string(".REM")));
		createReminder();
		
		//ori
		/*
		if(DRAGON_FileExists(str) == FT_FILE)
		{		
			//removedforDebug:DRAGON_FILE *fFile = DRAGON_fopen(str, "r");
			uint16 len = DRAGON_fread(reminder, 1, REM_SIZE - 1, fFile);
			DRAGON_fclose(fFile);
			
			reminder[len] = 0;
		}
		*/
		
		//new
		if(debug_FileExists((const char*)PathFix2.c_str(),10) == FT_FILE){
			DRAGON_FILE *fFile = DRAGON_fopen(PathFix2.c_str(), "r");	//debug_FileExists index: 10
			uint16 len = DRAGON_fread(reminder, 1, REM_SIZE - 1, fFile);
			DRAGON_fclose(fFile);
			reminder[len] = 0;
		}
		
		destroyDay();
		loadDayPlanner(&dayView, curDay, curMonth, curYear);
	}
	
	fb_setClipping(3,41,252,150);
	
	setFont((uint16 **)font_arial_9);
	setColor(genericTextColor);
	
	fb_dispString(0,-3,reminder);
	
	drawCurDay();
}

void loadCurrentReminder(char **rStr)
{
	std::string PathFix = (getDefaultDSOrganizeReminderPath(std::to_string(curMonth) + std::to_string(curDay) + std::to_string(curYear) + string(".REM")));
	//ori
	/*
	if(DRAGON_FileExists(str) == FT_FILE)
	{		
		//DRAGON_FILE *fFile = DRAGON_fopen(str, "r");
		u32 tLen = DRAGON_flength(fFile);
		
		*rStr = (char *)trackMalloc(tLen+5, "tmp reminder");
		memset(*rStr, 0, tLen+5);
		
		DRAGON_fread(*rStr, 1, tLen, fFile);
		DRAGON_fclose(fFile);
	}
	else
	{
		*rStr = (char *)trackMalloc(1, "tmp reminder");
		*rStr[0] = 0;
	}
	*/
	
	//new
	if(debug_FileExists((const char*)PathFix.c_str(),11) == FT_FILE){
		DRAGON_FILE *fFile = DRAGON_fopen(PathFix.c_str(), "r");	//debug_FileExists index: 11
		u32 tLen = DRAGON_flength(fFile);
		
		*rStr = (char *)trackMalloc(tLen+5, "tmp reminder");
		memset(*rStr, 0, tLen+5);
		
		DRAGON_fread(*rStr, 1, tLen, fFile);
		DRAGON_fclose(fFile);
	}
	else
	{
		*rStr = (char *)trackMalloc(1, "tmp reminder");
		*rStr[0] = 0;
	}
	
}

void drawReminder()
{	
	fb_setClipping(3,41,252, 150);
	
	fb_drawRect(0,37,255,148,textAreaFillColor);
	setFont((uint16 **)font_arial_9);
	setColor(textAreaTextColor);	
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
	
	fb_dispString(0,-3,reminder);
	
	setWrapNormal();
	hideCursor();
	clearHighlight();
	drawCurDay();
}

void drawEditReminder()
{	
	char str[128];

	drawKeyboard();
	drawButtonTexts(l_save, l_back);
	
	setFont((uint16 **)font_arial_11);
	
	if(reverseDate)
		sprintf(str,"%s, %d %s, %d", l_days[dayOfWeek(curDay, curMonth, curYear)], curDay, l_months[curMonth-1], curYear);
	else
		sprintf(str,"%s, %s %d, %d", l_days[dayOfWeek(curDay, curMonth, curYear)], l_months[curMonth-1], curDay, curYear);
	
	setColor(genericTextColor);
	bg_dispString(13,20-2,str);
}

void saveReminder()
{// here is where we write back the reminder if we've gone and edited it
	
	std::string PathFix = (getDefaultDSOrganizeReminderPath(std::to_string(curMonth) + std::to_string(curDay) + std::to_string(curYear) + string(".REM")));
	if(reminder == NULL || reminder[0] == 0)
	{
		if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
		{
			//delete file, nothing in it!
			DRAGON_remove(PathFix.c_str());			
		}
		
		reminders[curDay] = 0;
		return; // gtfo!
	}
	
	if(debug_FileExists((const char*)PathFix.c_str(),12) == FT_FILE){
		DRAGON_FILE *fFile = DRAGON_fopen(PathFix.c_str(), "w");	//debug_FileExists index: 12	
		DRAGON_fwrite (reminder, 1, strlen(reminder), fFile);
		DRAGON_fclose(fFile);
		reminders[curDay] = 1;
	}
}

void editReminderAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == CLEAR_KEY)
	{
		memset(reminder, 0, REM_SIZE);
		resetCursor();
		return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	char *tmpBuffer = (char *)trackMalloc(REM_SIZE,"reminder kbd");
	memcpy(tmpBuffer,reminder,REM_SIZE);
	int oldCursor = getCursor();
	
	genericAction(tmpBuffer, REM_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuffer, 3, 41, 252, 190, &pts, (uint16 **)font_arial_9);
	free(pts);
	
	if(numPts <= MAX_LINES_REMINDER)
		memcpy(reminder,tmpBuffer,REM_SIZE);
	else
		moveCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}
