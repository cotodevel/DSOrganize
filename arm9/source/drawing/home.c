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
#include "home.h"
#include "calendar.h"
#include "dayview.h"
#include "todo.h"
#include "../colors.h"
#include "../shortcuts.h"
#include "../settings.h"
#include "../general.h"
#include "../globals.h"
#include "../fatwrapper.h"
#include "../font_gautami_10.h"
#include "../font_arial_9.h"
#include "../font_arial_11.h"
#include "../graphics.h"
#include "../language.h"
#include "../keyboard.h"
#include "../controls.h"
#include "usrsettingsTGDS.h"
#include "fsfatlayerTGDSLegacy.h"

char *strings[MAX_MODES];
uint16 *spritesHOME[MAX_MODES];
bool homeLoaded = false;
char *curReminder = NULL;
char *curDayPlanner = NULL;
char *curTodo = NULL;
int scCount = 0;
bool shortcutLoaded = false;
SHORTCUT homeSC[6];
bool shortcutLaunched = false;
char naString[] = "N/A";

bool wasShortcutLaunched()
{
	return shortcutLaunched;
}

void resetShortcutLaunchedFlag()
{
	shortcutLaunched = false;
}

void setStrings()
{
	for(int x=0;x<MAX_MODES;x++)
	{	
		switch(locations[x])
		{
			case 0:		
				strings[x] = l_calendar;
				spritesHOME[x] = spr_Cal;
				break;
			case 1:
				strings[x] = l_dayview;
				spritesHOME[x] = spr_Day;
				break;
			case 2:
				strings[x] = l_addressbook;
				spritesHOME[x] = spr_Add;
				break;
			case 3:
				strings[x] = l_todo;
				spritesHOME[x] = spr_Todo;
				break;
			case 4:
				strings[x] = l_scribble;
				spritesHOME[x] = spr_Scribble;
				break;
			case 5:
				strings[x] = l_browser;
				spritesHOME[x] = spr_browser;
				break;
			case 6:
				strings[x] = l_calculator;
				spritesHOME[x] = spr_calc;
				break;
			case 7:
				strings[x] = l_irc;
				spritesHOME[x] = spr_irc;
				break;
			case 8:
				strings[x] = l_webbrowser;
				spritesHOME[x] = spr_webbrowser;
				break;
			case 9:
				strings[x] = l_database;
				spritesHOME[x] = spr_hbdb;
				break;
			case 10:
			case 11:
				strings[x] = naString;
				spritesHOME[x] = spr_unavailable;
				break;
		}
	}
}

void destroyShortcuts()
{
	if(!scCount)
		return;
	
	for(int x = 0;x < scCount;x++)
		freeShortcutIcon(&homeSC[x]);
	
	memset(&homeSC[0], 0, sizeof(SHORTCUT) * MAX_SHORTCUTS);
	scCount = 0;
	
	shortcutLoaded = false;
}

void invalidateHomeLoaded()
{
	homeLoaded = false;
	shortcutLoaded = false;
	
	destroyShortcuts();
	
	if(curReminder)
		trackFree(curReminder);
	curReminder = NULL;
	
	if(curDayPlanner)
		trackFree(curDayPlanner);
	curDayPlanner = NULL;
	
	if(curTodo)
		trackFree(curTodo);
	curTodo = NULL;
}

void drawSplash()
{
	fb_setDefaultClipping();
	drawTimeDate();
	
	if(!homeLoaded)
	{
		if(curReminder){
			trackFree(curReminder);
		}
		curReminder = NULL;
		
		if(curDayPlanner){
			trackFree(curDayPlanner);
		}
		curDayPlanner = NULL;
		
		if(curTodo){
			trackFree(curTodo);
		}
		curTodo = NULL;
		
		// load reminder
		loadCurrentReminder(&curReminder);
		abbreviateString(curReminder, 240, (uint16 **)font_gautami_10);
		
		char *tmpStr = strchr(curReminder, RET);
		if(tmpStr)
		{
			tmpStr[0] = '.';
			tmpStr[1] = '.';
			tmpStr[2] = '.';
			tmpStr[3] = 0;
		}
		
		// load day planner
		loadCurrentDayPlanner(&curDayPlanner);
		
		// load todos
		loadCurrentTodos(&curTodo);
		homeLoaded = true;
	}
	
	// display reminder
	setColor(genericTextColor);
	setFont((uint16 **)font_gautami_10);
	fb_dispString(3, 41, l_reminder);	
	fb_drawFilledRect(3, 41 + 8, 252, 41 + 23, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 41 + 8, 252, 41 + 26);
	setColor(textEntryTextColor);
	fb_dispString(3, 5, curReminder);
	
	// display day planner
	fb_setDefaultClipping();
	setColor(genericTextColor);
	fb_dispString(3, 68, l_dayview);	
	fb_drawFilledRect(3, 68 + 8, 252, 68 + 48, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 68 + 8, 252, 68 + 51);
	setColor(textEntryTextColor);
	fb_dispString(3, 4, curDayPlanner);
	
	// display todo	
	fb_setDefaultClipping();
	setColor(genericTextColor);
	fb_dispString(3, 120, l_todo);	
	fb_drawFilledRect(3, 120 + 8, 252, 120 + 47, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 120 + 8, 252, 120 + 50);
	setColor(textEntryTextColor);
	fb_dispString(3, 5, curTodo);
	
	// draw bottom welcome
	fb_setDefaultClipping();
	setFont((uint16 **)font_arial_11);
	setColor(homeDateTextColor);
	
	fb_setClipping(0, 0, 255, 191);
	fb_drawRect(0,191-16, 255, 191, homeDateFillColor);
	
	char str[256] = {0};
	char name[20] = {0};
	
	int Size = (sizeof(getsIPCSharedTGDS()->nickname_schar8) > sizeof(name)) ? sizeof(name): sizeof(getsIPCSharedTGDS()->nickname_schar8);
	Size = (strlen((char*)&getsIPCSharedTGDS()->nickname_schar8[0]) > Size) ? Size: strlen((char*)&getsIPCSharedTGDS()->nickname_schar8[0]);
	snprintf (name, Size, "%s", &getsIPCSharedTGDS()->nickname_schar8[0]);
	
	sprintf(str, "%s, %s!", l_welcome, name);
	fb_dispString(3,191-17,str);			
	
	fb_drawRect(0,191-17,255,191-17,homeDateBorderColor);
	fb_drawRect(0,191-19,255,191-18,homeDateBorderFillColor);
	fb_drawRect(0,191-20,255,191-20,homeDateBorderColor);
	
}

void drawHomeScreen()
{
	char str[256];
	int adjust = 0;
	
	setStrings();
	strcpy(str, lLanguage);
	strlwr(str);
	
	setColor(genericTextColor);
	setFont((uint16 **)font_arial_9);
	
	if(getCursor() > 6)
		moveCursorAbsolute(6);
	
	adjust = 10;
	
	if(strcmp(str, "english") != 0)
	{	
		char modes[MAX_MODES-1][30];
		
		strcpy(modes[0], strings[0]);
		strcpy(modes[1], strings[1]);
		strcpy(modes[2], strings[2]);
		strcpy(modes[3], strings[3]);
		strcpy(modes[4], strings[4]);
		strcpy(modes[5], strings[5]);
		
		if(getCursor() <= 5)
			bg_dispString((255 - getStringWidth(modes[getCursor()],(uint16 **)font_arial_9))/2, 5, modes[getCursor()]);
	}

	for(int x=0;x<6;x++)
	{
		if(getCursor() == x)
			bg_drawBox(25 + ((x % 3) * (50 + 25)), 15 + adjust + ((x / 3) * (50 + 25)), 25 + ((x % 3) * (50 + 25)) + 53, 15 + adjust + ((x / 3) * (50 + 25)) + 53, homeHighlightColor);		
	}
	if(getCursor() == 6)
		bg_drawBox(55, 165, 198, 185, homeHighlightColor);
		
	bg_dispSprite(27, 27 - 10 + adjust, spritesHOME[0], 31775);
	bg_dispSprite(102, 27 - 10 + adjust, spritesHOME[1], 31775);
	bg_dispSprite(177, 27 - 10 + adjust, spritesHOME[2], 31775);
	bg_dispSprite(27, 102 - 10 + adjust, spritesHOME[3], 31775);
	bg_dispSprite(102, 102 - 10 + adjust, spritesHOME[4], 31775);
	bg_dispSprite(177, 102 - 10 + adjust, spritesHOME[5], 31775);
	
	bg_dispString((255 - getStringWidth(l_configuration,(uint16 **)font_arial_9))/2, 168, l_configuration);
	
	// next arrow
	bg_dispSprite(211, 165, spr_next, 31775);
}

int cursorHomeScreen(int z, int y)
{
	int adjust = 10;
	
	if(z >= 55 && z <= 199 && y >= 165 && y <= 185)
		return 6;
	
	for(int x=0;x<6;x++)
	{
		if(z >= (25 + ((x % 3) * (50 + 25))) && y >= (15 + adjust + ((x / 3) * (50 + 25))) && z <= (25 + ((x % 3) * (50 + 25)) + 54) && y <= (15 + adjust + ((x / 3) * (50 + 25)) + 54))
			return x;
	}
	
	if(y >= 165)
	{
		if(z <= 50)
		{
			// back
			
			switch(getMode())
			{
				case HOMEMORE:
					setMode(HOME);
					break;
				case HOMESHORTCUTS:
					setMode(HOMEMORE);
					break;
			}
		}
		
		if(z >= 163)
		{
			// forward
			
			switch(getMode())
			{
				case HOME:
					setMode(HOMEMORE);
					break;
				case HOMEMORE:
					setMode(HOMESHORTCUTS);
					break;
			}
		}
	}
	
	return -1;
}

void drawHomeScreenMore()
{
	char str[256];
	int adjust = 0;
	
	setStrings();
	strcpy(str, lLanguage);
	strlwr(str);
	
	setColor(genericTextColor);
	setFont((uint16 **)font_arial_9);
		
	if(getCursor() > 6)
		moveCursorAbsolute(6);
	
	adjust = 10;
	
	if(strcmp(str, "english") != 0)
	{
		char modes[MAX_MODES-1][30];	
		
		strcpy(modes[0], strings[6]);
		strcpy(modes[1], strings[7]);
		strcpy(modes[2], strings[8]);
		strcpy(modes[3], strings[9]);
		strcpy(modes[4], strings[10]);
		strcpy(modes[5], strings[11]);
		
		if(getCursor() <= 5)
			bg_dispString((255 - getStringWidth(modes[getCursor()],(uint16 **)font_arial_9))/2, 5, modes[getCursor()]);
	}
	
	for(int x=0;x<6;x++)
	{
		if(getCursor() == x)
			bg_drawBox(25 + ((x % 3) * (50 + 25)), 15 + adjust + ((x / 3) * (50 + 25)), 25 + ((x % 3) * (50 + 25)) + 53, 15 + adjust + ((x / 3) * (50 + 25)) + 53, homeHighlightColor);		
	}
	
	if(getCursor() == 6)
		bg_drawBox(55, 165, 198, 185, homeHighlightColor);
	
	bg_dispSprite(27, 27 - 10 + adjust, spritesHOME[6], 31775);
	bg_dispSprite(102, 27 - 10 + adjust, spritesHOME[7], 31775);
	bg_dispSprite(177, 27 - 10 + adjust, spritesHOME[8], 31775);
	bg_dispSprite(27, 102 - 10 + adjust, spritesHOME[9], 31775);
	bg_dispSprite(102, 102 - 10 + adjust, spritesHOME[10], 31775);
	bg_dispSprite(177, 102 - 10 + adjust, spritesHOME[11], 31775);
	
	bg_dispString((255 - getStringWidth(l_configuration,(uint16 **)font_arial_9))/2, 168, l_configuration);
	
	// next/prev arrow
	bg_dispSprite(211, 165, spr_next, 31775);
	bg_dispSprite(5, 165, spr_back, 31775);
}

int compareShortcut(const void * a, const void * b)
{
	SHORTCUT *v1 = (SHORTCUT*)a;
	SHORTCUT *v2 = (SHORTCUT*)b;
	
	char str1[256];
	char str2[256];
	
	strcpy(str1, v1->location);
	strcpy(str2, v2->location);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

void drawHomeScreenShortcuts()
{
	int adjust = 0;
	
	setColor(0);
	setFont((uint16 **)font_arial_9);
	
	if(!shortcutLoaded)
	{
		// load shortcuts
		
		char tmpFile[MAX_TGDSFILENAME_LENGTH+1] = {0};
		char tStr[6][MAX_TGDSFILENAME_LENGTH+1] = {0};
		int fType;
		
		DRAGON_chdir(d_home);
		fType = DRAGON_FindFirstFile(tmpFile);
		
		while(fType != FT_NONE && scCount < MAX_SHORTCUTS)
		{
			if(getFileType(tmpFile) == SHORTCUTFILE)
			{
				sprintf(tStr[scCount], "%s%s", d_home, tmpFile);
				scCount++;
			}
			
			fType = DRAGON_FindNextFile(tmpFile);		
		}
		
		DRAGON_closeFind();		
		DRAGON_chdir("/");
		
		if(scCount > 0)
		{
			for(int x=0;x<scCount;x++)
			{				
				loadShortcut(tStr[x], &homeSC[x]);
				loadShortcutIcon(&homeSC[x]);
			}
		}
		
		if(scCount > 1) // lets sort this fucker
			qsort(homeSC, scCount, sizeof(SHORTCUT), compareShortcut);
		
		shortcutLoaded = true;
	}
	
	if(getCursor() >= scCount)
		moveCursorAbsolute(scCount-1);
	
	adjust = 10;	
	
	bg_dispString((255 - getStringWidth(homeSC[getCursor()].name,(uint16 **)font_arial_9))/2, 5, homeSC[getCursor()].name);
	
	for(int x=0;x<scCount;x++)
	{
		if(homeSC[x].loadedIcon)
		{
			int tX = 0;
			
			if(homeSC[x].loadedIcon[1] == 31)
				tX = 10;
				
			bg_dispSprite(27 + (75 * (x % 3)) + tX, 27 - 10 + adjust + (75 * (x / 3)) + tX, homeSC[x].loadedIcon, 31775);
		}
		
		if(getCursor() == x)
			bg_drawBox(25 + ((x % 3) * (50 + 25)), 15 + adjust + ((x / 3) * (50 + 25)), 25 + ((x % 3) * (50 + 25)) + 53, 15 + adjust + ((x / 3) * (50 + 25)) + 53, homeHighlightColor);		
	}
	
	// prev arrow
	bg_dispSprite(5, 165, spr_back, 31775);
}

void runShortcut(int cu)
{
	SHORTCUT tS;
	
	memcpy(&tS, &homeSC[cu], sizeof(SHORTCUT));	
	invalidateHomeLoaded();	
	
	shortcutLaunched = true;
	int oldCursor = getCursor();
	
	launchShortcut(&tS);
	
	switch(getFileType(tS.path))
	{
		case SOUNDFILE:
			fb_setBGColor(genericFillColor);
			break;
		default:
			if((getFileType(tS.path) & 0xFF00) == 0xFF00)
				moveCursorAbsolute(oldCursor);
			break;
	}
}

void runHomeAction()
{
	switch(getMode())
	{
		case HOME:
			if(getCursor() >= 0 && getCursor() < MAX_ITEMS)
				gotoApplication(getCursor(),true);
			else if(getCursor() == MAX_ITEMS)
				gotoApplication(GO_CONFIG,true);
			break;
		case HOMEMORE:			
			if(getCursor() >= 0 && getCursor() < MAX_ITEMS)
				gotoApplication(getCursor() + MAX_ITEMS,true);
			else if(getCursor() == MAX_ITEMS)
				gotoApplication(GO_CONFIG,true);
			break;
		case HOMESHORTCUTS:
			if(getCursor() >= 0 && getCursor() < MAX_ITEMS)
			{
				runShortcut(getCursor());
			}
			break;
	}
}
