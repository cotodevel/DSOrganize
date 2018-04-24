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
#include "browser.h"
#include "address.h"
#include "pictureviewer.h"
#include "texteditor.h"
#include "viewer.h"
#include "todo.h"
#include "soundplayer.h"
#include "database.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../settings.h"
#include "../shortcuts.h"
#include "../general.h"
#include "../filerout.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../font_arial_11.h"
#include "../graphics.h"
#include "../keyboard.h"
#include "../globals.h"
#include "../language.h"
#include "../controls.h"
#include "fsfatlayerTGDSLegacy.h"

static bool dir_create = false;
static bool copyFlag = false;
static bool cutFlag = false;
static bool showMenu = false;
static bool testMenu = false;
static int startPX = 0;
static int startPY = 0;
static int oldPX = 0;
static int oldPY = 0;
static int isPopulated = 0;
static u32 bLoc[32];
static u32 bPos = 0;
static uint16 browserEntries;

FILE_INFO curFile;
uint16 browserMode = 0;
char curDir[256];
char copyFrom[256];
char fileName[256];

extern bool wavLoaded;
extern bool showHidden;
extern uint16 curDay;
extern uint16 curMonth;
extern uint16 curYear;
extern bool copying;
extern u32 maxSize;
extern u32 curSize;
extern bool picLoaded;
extern bool soundLoaded;
extern uint16 curTime;
extern BROWSER_FILE *dirList;
extern char binName[256];

void browserSetUnPopulated()
{
	isPopulated = 0;
}

void resetCurDir()
{
	strcpy(curDir,copyFrom);
}

void browserPushPos(u32 pos)
{
	if(bPos == 32)
		return;
	
	bLoc[bPos] = pos;
	
	bPos++;
}

u32 browserPopPos()
{
	if(!bPos)
		return 0;
	
	bPos--;
	
	return bLoc[bPos];
}

void browserResetPos()
{
	for(int i=0;i<32;i++)
		bLoc[i] = 0;
	
	bPos = 0;
}

void browserChangeDir(char *dir)
{
	freeDirList();
	
	strcpy(curDir, dir);
	
	browserEntries = populateDirList(curDir);
	isPopulated = 1;
	
	if(getCursor() >= browserEntries)
		moveCursorAbsolute(browserEntries - 1);
}

bool setBrowserCursor(char *file)
{
	resetCursor();
	strlwr(file);
	
	if(browserEntries == 0)
		return false;
	
	while(getCursor() < browserEntries)
	{
		char strTmp[256];
		strcpy(strTmp, dirList[getCursor()].longName);
		strlwr(strTmp);
		
		if(strcmp(strTmp, file) == 0)
			return true;
		
		moveCursorRelative(CURSOR_FORWARD);
	}
	
	resetCursor();
	return false;
}

bool isViewable(char *fName)
{
	char str[256];
	char str2[256];
	
	strcpy(str, fName);
	separateExtension(str,str2);
	
	strlwr(str2);
	
	if(strcmp(str2,".txt") == 0 || strcmp(str2,".htm") == 0 || strcmp(str2,".html") == 0 || strcmp(str2,".dss") == 0 || strcmp(str2,".pls") == 0 || strcmp(str2,".m3u") == 0)
		return true;
	
	return false;
}

void upOneDir()
{
	if(strlen(curDir) != 0 && strcmp(curDir, "/") != 0)
	{
		char str[256];
		
		strcpy(str, curDir);
		if(str[strlen(str)-1] == '/')
			str[strlen(str)-1] = 0;
		
		uint16 found = 0;
		for(uint16 findSlash=0;findSlash<strlen(str);findSlash++)
		{
			if(str[findSlash] == '/')
				found = findSlash;
		}
		
		found++;
		str[found] = 0;
		
		strcpy(curDir,str);
		
		destroyRandomList();		
		isPopulated = 0;
		moveCursorAbsolute(browserPopPos());
	}
}

void loadFileWrapper(int which)
{
	if(dirList[which].fileType == SHORTCUTFILE)
	{
		SHORTCUT sc;
		
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		if(loadShortcut(str, &sc))
			launchShortcut(&sc);		
		return;
	}
	
	loadFile(which);
}

void loadFile(int which)
{	
	if(dirList[which].fileType == DIRECTORY)
	{
		if(strcmp(dirList[which].longName, ".") == 0)
			return;
			
		destroyRandomList();
		
		if(strcmp(dirList[which].longName, "..") == 0)
		{
			upOneDir();
			return;
		}
		
		int x = strlen(curDir)-1;
		if(curDir[x] == '/')
			curDir[x] = 0;
		
		strcat(curDir, "/");
		strcat(curDir, dirList[which].longName);
		
		isPopulated = 0;
		browserPushPos(getCursor());
		resetCursor();
		setMode(BROWSER);
		
		return;
	}
	
	if(dirList[which].fileType == NDSFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, dirList[which].longName);
		
		freeDirList();
		
		launchNDS(str);
		
		return;
	}
	
	if(dirList[which].fileType == TXTFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		strcpy(copyFrom,curDir);
		
		strcpy(curDir, str);
		strcpy(fileName, dirList[which].longName);
		
		if(isViewable(fileName)) // view it, since y = edit
		{
			setMode(VIEWER);
			pushCursor();
			initViewer();
			
			freeDirList();
		}
		else // edit it
		{
			setMode(TEXTEDITOR);
			pushCursor();
			initTextEditor();
			
			freeDirList();
		}
		
		return;
	}
	
	if(dirList[which].fileType == PICFILE || dirList[which].fileType == BINFILE)
	{
		if(dirList[which].fileType == BINFILE)
		{
			if((dirList[which].fileSize == 98304) || (dirList[which].fileSize == 65536) || (dirList[which].fileSize == 131072) || (dirList[which].fileSize == 49152))
			{
				if(browserMode == LOADBIN)
					return;
			}
			else
			{				
				if(browserMode != LOADBIN)
					return;
			}
		}
		
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		if(browserMode == 0)
		{
			if(dirList[which].fileSize == 65536)
			{
				setBinName(str, 256, 256);
				browserMode = LOADBIN;
				return;
			}
			if(dirList[which].fileSize == 49152)
			{
				setBinName(str, 256, 192);
				browserMode = LOADBIN;
				return;
			}
		}
		
		strcpy(fileName, str);
		
		setMode(PICTUREVIEWER);
		picLoaded = false;
		
		if(browserMode == LOADBIN || dirList[which].fileType != BINFILE)		
			browserMode = 0;
		
		return;
	}
	
	if(dirList[which].fileType == REMFILE)
	{
		setMode(CALENDAR);
		resetCursor();
		
		int x,y,z;
		
		sscanf(dirList[which].longName, "%02d%02d%04d.rem",&x,&y,&z);
		
		curMonth = x;
		curDay = y;
		curYear = z;
		
		freeDirList();
		
		return;
	}
	
	if(dirList[which].fileType == DVFILE)
	{
		setMode(DAYVIEW);
		curTime = getCurTime();
		resetCursor();
		
		int x,y,z;
		
		sscanf(dirList[which].longName, "%02d%02d%04d.dpl",&x,&y,&z);
		
		curMonth = x;
		curDay = y;
		curYear = z;
		
		freeDirList();
		
		return;
	}
	
	if(dirList[which].fileType == VCFILE)
	{
		strcpy(fileName, dirList[which].longName);
		
		setMode(ADDRESS);
		resetCursor();
		addressSetUnPopulated();		
		freeDirList();
		
		return;
	}
	
	if(dirList[which].fileType == TODOFILE)
	{
		strcpy(fileName, dirList[which].longName);
		
		setMode(TODOLIST);
		resetCursor();
		todoSetUnPopulated();		
		freeDirList();
		
		return;
	}
	
	if(dirList[which].fileType == SOUNDFILE || dirList[which].fileType == PLAYLISTFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		strcpy(fileName, str);
		
		if(dirList[which].fileType == SOUNDFILE)
			addToRandomList(fileName, which);
		
		setFirstTime();
		
		soundLoaded = false;
		setMode(SOUNDPLAYER);
		
		return;
	}
	if(dirList[which].fileType == HBFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
		
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		char tmpStr[256];
		if(debug_FileExists((const char*)str,5) == FT_FILE){
			DRAGON_FILE *fp = DRAGON_fopen(str, "r");	//debug_FileExists index: 5
			memset(tmpStr, 0, 256);
			DRAGON_fgets(tmpStr, 255, fp);
			DRAGON_fclose(fp);
			
			setMode(HOMEBREWDATABASE);
			resetDBMode();
			resetCursor();
			
			customDB(tmpStr);
		}
		return;
	}	
	if(dirList[which].fileType == PKGFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
		
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		char *tmpStr = (char *)safeMalloc(MAX_CUSTOM_LIST);
		
		if(debug_FileExists((const char*)str,6) == FT_FILE){
			DRAGON_FILE *fp = DRAGON_fopen(str, "r");	//debug_FileExists index: 6
			DRAGON_fread(tmpStr, MAX_CUSTOM_LIST, 1, fp);
			DRAGON_fclose(fp);
			
			setMode(HOMEBREWDATABASE);
			resetDBMode();
			pushCursor();
			
			customPackage(tmpStr);
		}
		return;
	}
}

void alternateLoadFile(int which)
{
	if(dirList[which].fileType == TXTFILE || dirList[which].fileType == SHORTCUTFILE || dirList[which].fileType == PLAYLISTFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		strcpy(copyFrom,curDir);
		
		strcpy(curDir, str);
		strcpy(fileName, dirList[which].longName);
		
		if(isViewable(fileName)) // edit it
		{
			if(dirList[which].fileType == SHORTCUTFILE) // ensure icon is redrawn after edit
				strcpy(curFile.longName, "");
			
			setMode(TEXTEDITOR);
			pushCursor();
			initTextEditor();
			
			freeDirList();
		}
		else
			strcpy(curDir, copyFrom);
		
		return;
	}
	else if(dirList[which].fileType == NDSFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, dirList[which].longName);
		
		launchNDSAlt(str);
		
		return;
	}	
	else if(dirList[which].fileType == SOUNDFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		strcpy(fileName, str);
		
		char str2[256];
		
		separateExtension(str,str2);
		
		strlwr(str2);
		
		if(strcmp(str2,".wav") == 0)
		{
			wavLoaded = false;
			setMode(SOUNDRECORDER);
		}
		
		return;
	}
	else if(dirList[which].fileType == PKGFILE || dirList[which].fileType == HBFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		strcpy(copyFrom,curDir);
		
		strcpy(curDir, str);
		strcpy(fileName, dirList[which].longName);
		
		setMode(TEXTEDITOR);
		pushCursor();
		initTextEditor();
		
		freeDirList();
		return;
	}
}

void toggleHidden(int which)
{
	if(!showHidden)
		return;
	
	char str[256];
	
	strcpy(str, curDir);
	
	int x = strlen(str)-1;
	
	if(str[x] == '/')
		str[x] = 0;
	
	strcat(str, "/");
	strcat(str, dirList[which].longName);
	
	u8 attr;
	
	if(dirList[which].hidden)
		attr = 0;
	else
		attr = ATTRIB_HID;
	
	DRAGON_SetFileAttributes (str, attr, ATTRIB_HID);
	
	isPopulated = 0;	
}

void startPullUp()
{
	if(!testMenu)
		return;
	
	testMenu = false;
	
	if(oldPY >= 162) // && oldPY <= 182)
	{
		if(oldPX >= 23 && oldPX <= 126)
			browserForward();
		if(oldPX >= 130 && oldPX <= 233)
			browserBack();
		
		return;
	}
	
	if(abs(oldPX - startPX) < 20 && (startPY - oldPY) > 10 && (startPY - oldPY) < 100)
	{
		showMenu = true;
		browserMode = 0;
	}
}

void trackPullUp(int px, int py)
{
	if(!testMenu)
		return;
	if(!px || !py)
		return;
	
	oldPX = px;
	oldPY = py;
}

bool testPullUp(int px, int py)
{
	if(getMode() == ADDRESS)
		return false;
	if(!showMenu)
	{
		if(py >= 162) // && py <= 182)
		{
			if(px >= 23 && px <= 126)
			{
				if(browserMode < 4)
					testMenu = true;
				else
					browserForward();
			}
			if(px >= 130 && px <= 233)
			{
				if(browserMode < 4)
					testMenu = true;
				else
					browserBack();
			}
			
			startPX = px;
			startPY = py;
			
			oldPX = 0;
			oldPY = 0;
			
			return true;
		}
		
		return false;
	}
	
	// test for different menu clicks here
	
	if(py < 102 || py > 182)
	{
		showMenu = false;
		return true;
	}
	else
	{
		if(px < 10 || px > 244)
		{
			showMenu = false;
			return true;
		}
		if(px > 126 && px < 130)
		{
			showMenu = false;
			return true;
		}
	}
	
	if(px >= 23 && px <= 126)
	{
		if(py < 122)
			browserMode = 3;
		else if(py < 142)
			browserMode = 2;
		else if(py < 162)
			browserMode = 1;
		else if(py < 182)
			browserMode = 0;		
		
		browserForward();
	}
	
	if(px >= 130 && px <= 233)
	{
		if(py < 122)
			browserMode = 3;
		else if(py < 142)
			browserMode = 2;
		else if(py < 162)
			browserMode = 1;
		else if(py < 182)
			browserMode = 0;		
		
		browserBack();
	}
	
	return true;
}

void destroyPullUp()
{
	showMenu = false;
	testMenu = false;
}

bool isPullUp()
{
	return showMenu;
}

void resetCopyFlags()
{
	dir_create = false;
	copyFlag = false;
	cutFlag = false;
	showMenu = false;
	testMenu = false;
}

void drawMenu()
{
	if(!showMenu)
		return;
	
	bg_drawRect(10, 102, 126, 162, widgetFillColor);	
	bg_drawRect(130, 102, 244, 162, widgetFillColor);
	
	bg_drawBox(10, 102, 126, 122, widgetBorderColor);	
	bg_drawBox(130, 102, 244, 122, widgetBorderColor);
	bg_drawBox(10, 122, 126, 142, widgetBorderColor);	
	bg_drawBox(130, 122, 244, 142, widgetBorderColor);
	bg_drawBox(10, 142, 126, 162, widgetBorderColor);	
	bg_drawBox(130, 142, 244, 162, widgetBorderColor);
	
	setFont((uint16 **)font_arial_9);
	setColor(widgetTextColor);
	
	bg_dispString(centerOnPt(getLLocation(), l_new, (uint16 **)font_arial_9),145,l_new);	
	bg_dispString(centerOnPt(getRLocation(), l_delete, (uint16 **)font_arial_9),145,l_delete);
	
	bg_dispString(centerOnPt(getLLocation(), l_rename, (uint16 **)font_arial_9),125,l_rename);	
	if(!copyFlag)
		bg_dispString(centerOnPt(getRLocation(), l_copy, (uint16 **)font_arial_9),125,l_copy);
	else
		bg_dispString(centerOnPt(getRLocation(), l_paste, (uint16 **)font_arial_9),125,l_paste);
	
	bg_dispString(centerOnPt(getLLocation(), l_mkdir, (uint16 **)font_arial_9),105,l_mkdir);	
	
	if(!cutFlag)
		bg_dispString(centerOnPt(getRLocation(), l_cut, (uint16 **)font_arial_9),105,l_cut);
	else
		bg_dispString(centerOnPt(getRLocation(), l_paste, (uint16 **)font_arial_9),105,l_paste);
}

void drawFileInfoScreen()
{
	setColor(genericTextColor);
	setFont((uint16 **)font_arial_9);	
	
	if(browserMode == LOADBIN)
	{
		fb_setClipping(0,0,255,191);
		fb_dispString(5, 175, l_choosepal);
	}
	
	if(browserEntries == 0)
		return;
	
	char str[256];
	
	if(strcmp(curFile.longName, dirList[getCursor()].longName) != 0)
	{
		getInfo(&dirList[getCursor()], curDir, &curFile);
	}	

	fb_drawFilledRect(5, 5 + 7, 5 + 32 + 1, 5 + 32 + 1 + 7, widgetBorderColor, 0xFFFF); //RGB15(25,25,25)
	fb_dispSprite(6, 6 + 7, curFile.iconData, curFile.transColor);
	if(dirList[getCursor()].fileType == NDSFILE)
	{
		// careful now, display nds data!
		
		strcpy(str, curFile.formattedName[0]);
		abbreviateString(str, 205, (uint16 **)font_arial_9);
		fb_dispString(5 + 32 + 5 + 2, 5, str);
		
		strcpy(str, curFile.formattedName[1]);
		abbreviateString(str, 205, (uint16 **)font_arial_9);		
		fb_dispString(5 + 32 + 5 + 2, 5 + 15, str);
		
		strcpy(str, curFile.formattedName[2]);
		abbreviateString(str, 205, (uint16 **)font_arial_9);
		fb_dispString(5 + 32 + 5 + 2, 5 + 30, str);
	}
	else
	{
		int *pts = NULL;
		int numPts = getWrapPoints(5 + 32 + 5 + 2, 5 + 15, curFile.internalName, 0, 0, 250, 191, &pts, (uint16 **)font_arial_9);
		free(pts);
		
		if(numPts< 3)
		{
			fb_setClipping(0,0,250,191);
			fb_dispString(5 + 32 + 5 + 2, 5 + 15, curFile.internalName);
		}
		else
		{
			fb_setClipping(0,0,250,5 + 45);
			fb_dispString(5 + 32 + 5 + 2, 5, curFile.internalName);
			fb_setClipping(0,0,250,191);
		}
	}
	
	if(dirList[getCursor()].fileType == DIRECTORY)
		sprintf(str, "%s: N/A", l_filesize);
	else
	{
		uint16 curExt = 0;    
		float fst = (float)dirList[getCursor()].fileSize;
		  
		while(fst >= 1024)
		{
			fst = fst / 1024;
			curExt++;
		}
		  
		switch(curExt)
		{
		    case 0:
                sprintf(str, "%s: %.1f b", l_filesize, fst);
				break;
		    case 1:
                sprintf(str, "%s: %.1f kb", l_filesize, fst);
				break;
		    case 2:
                sprintf(str, "%s: %.1f mb", l_filesize, fst);
				break;
		    case 3:
                sprintf(str, "%s: %.1f gb", l_filesize, fst);
				break;
		    case 4:
                sprintf(str, "%s: %.1f tb", l_filesize, fst);
				break;
		}
	}
	
	fb_dispString(5, 55, str);
	
	switch(dirList[getCursor()].fileType)
	{
		case DIRECTORY:
			sprintf(str, "%s: %s", l_filetype, l_directory);
			break;				
		case NDSFILE:
			sprintf(str, "%s: %s", l_filetype, l_ndsfile);			
			break;			
		case TXTFILE:
			sprintf(str, "%s: %s", l_filetype, l_txtfile);			
			break;			
		case VCFILE:
			sprintf(str, "%s: %s", l_filetype, l_vcffile);			
			break;			
		case DVFILE:
			sprintf(str, "%s: %s", l_filetype, l_dvpfile);			
			break;			
		case REMFILE:
			sprintf(str, "%s: %s", l_filetype, l_remfile);			
			break;
		case PICFILE:
			sprintf(str, "%s: %s", l_filetype, l_picfile);			
			break;
		case BINFILE:
			sprintf(str, "%s: %s", l_filetype, l_binfile);			
			break;
		case TODOFILE:
			sprintf(str, "%s: %s", l_filetype, l_todofile);			
			break;
		case SOUNDFILE:
			sprintf(str, "%s: %s", l_filetype, l_soundfile);			
			break;
		case PLAYLISTFILE:
			sprintf(str, "%s: %s", l_filetype, l_playlistfile);			
			break;
		case HBFILE:
			sprintf(str, "%s: %s", l_filetype, l_hbfile);			
			break;
		case PKGFILE:
			sprintf(str, "%s: %s", l_filetype, l_pkgfile);			
			break;
		case SHORTCUTFILE:
			sprintf(str, "%s: %s", l_filetype, l_shortcutfile);			
			break;
		case UNKNOWN:
			sprintf(str, "%s: %s", l_filetype, l_unknownfile);			
			break;
		default:
			sprintf(str, "%s: %s", l_filetype, l_unknownfile);
	}
	
	fb_dispString(5, 70, str);
	
	if(getMode() == BROWSER)
	{
		if(dirList[getCursor()].fileType == TXTFILE || dirList[getCursor()].fileType == SHORTCUTFILE || dirList[getCursor()].fileType == PLAYLISTFILE)
		{
			if(isViewable(dirList[getCursor()].longName))
			{
				if(showHidden)
					sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_edit);
				else
					sprintf(str, "%c %s", BUTTON_Y, l_edit);
				
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
			else
			{
				if(showHidden)
				{
					sprintf(str, "%c %s", BUTTON_X, l_hide);
					fb_setDefaultClipping();
					fb_dispString(5, 175, str);
				}
			}
		}
		else if(dirList[getCursor()].fileType == SOUNDFILE)
		{
			char str2[256];
			
			strcpy(str, dirList[getCursor()].longName);
			separateExtension(str,str2);
			
			strlwr(str2);
			
			if(strcmp(str2,".wav") == 0)
			{
				if(showHidden)
					sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_record);
				else
					sprintf(str, "%c %s", BUTTON_Y, l_record);
				
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
			else
			{
				if(showHidden)
				{
					sprintf(str, "%c %s", BUTTON_X, l_hide);
					fb_setDefaultClipping();
					fb_dispString(5, 175, str);
				}
			}
		}
		else if(dirList[getCursor()].fileType == NDSFILE)
		{
			if(showHidden)
				sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_launchalt);
			else
				sprintf(str, "%c %s", BUTTON_Y, l_launchalt);
			
			fb_setDefaultClipping();
			fb_dispString(5, 175, str);
		}
		else if(dirList[getCursor()].fileType == PKGFILE || dirList[getCursor()].fileType == HBFILE)
		{
			if(showHidden)
				sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_edit);
			else
				sprintf(str, "%c %s", BUTTON_Y, l_edit);
			
			fb_setDefaultClipping();
			fb_dispString(5, 175, str);
		}
		else
		{
			if(showHidden)
			{
				sprintf(str, "%c %s", BUTTON_X, l_hide);
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
		}
	}
	else
	{
		fb_drawRect(5, 175, 216, 191, genericFillColor);
	}
}

void browserListCallback(int pos, int x, int y)
{
	char str[256];
	char ext[256];
	
	strcpy(str, dirList[pos].longName);
	
	separateExtension(str, ext);
	
	if(list_right - (list_left + 19 + getStringWidth(ext, (uint16 **)font_arial_9)) <= 10)
	{
		strcpy(str, dirList[pos].longName);
		abbreviateString(str, list_right - (list_left + 19), (uint16 **)font_arial_9);	
	}
	else
	{
		abbreviateString(str, list_right - (list_left + 19 + getStringWidth(ext, (uint16 **)font_arial_9)), (uint16 **)font_arial_9);
		strcat(str, ext);
	}
	
	if(dirList[pos].hidden)
		setColor(browserHiddenColor);
	else
		setColor(listTextColor);
	
	bg_dispString(15, 0, str);
	
	switch(dirList[pos].fileType)
	{
		case DIRECTORY:
			bg_dispSprite(x + 2, y + 2, folder, 31775);
			break;				
		case NDSFILE:
			bg_dispSprite(x + 2, y + 2, ndsfile, 31775);
			break;			
		case TXTFILE:
			bg_dispSprite(x + 2, y + 2, txtfile, 31775);
			break;			
		case VCFILE:
			bg_dispSprite(x + 2, y + 2, vcfile, 31775);
			break;			
		case DVFILE:
			bg_dispSprite(x + 2, y + 2, dpfile, 31775);
			break;			
		case REMFILE:
			bg_dispSprite(x + 2, y + 2, reminderfile, 31775);
			break;
		case PICFILE:
		case BINFILE:
			bg_dispSprite(x + 2, y + 2, picfile, 31775);
			break;
		case TODOFILE:
			bg_dispSprite(x + 2, y + 2, todofile, 31775);
			break;		
		case PLAYLISTFILE:
			bg_dispSprite(x + 2, y + 2, playlistfile, 31775);
			break;		
		case SOUNDFILE:
			bg_dispSprite(x + 2, y + 2, soundfile, 31775);
			break;		
		case HBFILE:
		case PKGFILE:
			bg_dispSprite(x + 2, y + 2, hbfile, 31775);
			break;
		case SHORTCUTFILE:
			bg_dispSprite(x + 2, y + 2, shortcutfile, 31775);
			break;
		case UNKNOWN:
			bg_dispSprite(x + 2, y + 2, unknown, 31775);
			break;
		default:						
			break;		
	}
}

void drawFileBrowserScreen()
{	
	// actual code here!
	
	if(isPopulated == 0)
	{
		browserEntries = populateDirList(curDir);
		isPopulated = 1;
		
		if(getCursor() >= browserEntries)
			moveCursorAbsolute(browserEntries - 1);
	}
	
	drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, getCursor(), browserEntries, l_noentries, browserListCallback);
	drawScrollBar(getCursor(), browserEntries, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT);
	
	char lText[128];
	char rText[128];
	
	switch(browserMode)
	{
		case 0:
			strcpy(rText, l_up);
			
			switch(dirList[getCursor()].fileType)
			{
				case DIRECTORY:
				case VCFILE:
				case DVFILE:
				case REMFILE:
				case PICFILE:
				case TODOFILE:
				case HBFILE:
				case PKGFILE:
					strcpy(lText, l_open);
					break;
				case SOUNDFILE:
				case PLAYLISTFILE:
					strcpy(lText, l_play);
					break;
				case BINFILE:					
					if((dirList[getCursor()].fileSize == 98304) || (dirList[getCursor()].fileSize == 65536) || (dirList[getCursor()].fileSize == 131072) || (dirList[getCursor()].fileSize == 49152))
						strcpy(lText, l_open);
					else
						strcpy(lText, "N/A");
					break;
				case TXTFILE:
					if(isViewable(dirList[getCursor()].longName))
						strcpy(lText, l_view);
					else
						strcpy(lText, l_edit);
					break;		
				case NDSFILE:
				case SHORTCUTFILE:
					strcpy(lText, l_launch);
					break;
				default:
					strcpy(lText, "N/A");
					break;
			}
			
			break;
		case 1:			
			strcpy(lText, l_new);
			strcpy(rText, l_delete);
			
			break;
		case 2:
			strcpy(lText, l_rename);
			
			if(!copyFlag)
				strcpy(rText, l_copy);
			else
				strcpy(rText, l_paste);
			
			break;
		case 3:
			strcpy(lText, l_mkdir);
			
			if(!cutFlag)
				strcpy(rText, l_cut);
			else
				strcpy(rText, l_paste);
			break;
		case LOADBIN:
			if(dirList[getCursor()].fileType == BINFILE)
			{
				if((dirList[getCursor()].fileSize == 98304) || (dirList[getCursor()].fileSize == 65536) || (dirList[getCursor()].fileSize == 131072) || (dirList[getCursor()].fileSize == 49152))
					strcpy(lText, "N/A");
				else
					strcpy(lText, l_open);
			}
			else
				strcpy(lText, "N/A");
			
			strcpy(rText, l_back);
			
			break;
		case CONFIRMDELETE:
			strcpy(lText, l_confirm);
			strcpy(rText, l_back);
			
			break;
	}
	
	drawHome();
	drawButtonTexts(lText, rText);
	drawLR(l_more, l_more);
	drawMenu();
}

void browserSafeFileName()
{
	safeFileName(fileName);
}

void drawEditFileName()
{	
	fb_setDefaultClipping();
	setFont((uint16 **)font_gautami_10);
	
	fb_drawFilledRect(3, 165 + 8 - 24, 252, 165 + 23, widgetBorderColor, textEntryFillColor);	
	fb_dispString(3, 165 - 26, l_filename);
	
	setColor(textEntryTextColor);
	fb_setClipping(3, 165 + 8 - 26, 250, 165 + 26);
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, -2, -3, -1);
		else
			setCursorProperties(cursorOverwriteColor, -2, -3, -1);
			
		showCursor();
		setCursorPos(getCursor());		
	}
	
	setFakeHighlight();
	
	fb_dispString(3, 5, fileName);
	
	clearHighlight();
	hideCursor();	
}

void drawEditFileControls()
{
	char str[256];

	drawKeyboard();
	drawButtonTexts(l_confirm, l_back);
	
	setFont((uint16 **)font_arial_11);
	
	if(strcmp(copyFrom,"") == 0)
	{
		// new file
		if(dir_create)
			sprintf(str,"%s", l_newdir);
		else		
			sprintf(str,"%s", l_new);
	}
	else
	{
		// current file name
		sprintf(str,"%s", copyFrom);
	}
	
	setColor(genericTextColor);
	abbreviateString(str, 230, (uint16 **)font_arial_11);
	bg_dispString(13,20-2,str);
}

int getMaxCursorBrowser()
{
	return strlen(fileName);
}

void editFileNameAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == RET)
	{
		goForward(); // simulate A button pressed
		return;		
	}
	
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	if(c == CLEAR_KEY)
	{
		memset(fileName, 0, FILENAME_SIZE);
		getCursor();
		return;
	}
	
	char tmpBuffer[256];
	memset(tmpBuffer, 0, FILENAME_SIZE);
	strcpy(tmpBuffer, fileName);
	int oldCursor = getCursor();
	
	genericAction(tmpBuffer, FILENAME_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(3, 5, tmpBuffer, 3, 165 + 8 - 25, 242, 165 + 16, &pts, (uint16 **)font_gautami_10);
	free(pts);
	
	if(numPts < 4)
		strcpy(fileName,tmpBuffer);
	else
		moveCursorAbsolute(oldCursor);
}

void renameEntry(char *oldFileName, char *newFileName)
{
	if(strlen(oldFileName) == 0)
		return;
	if(strlen(newFileName) == 0)
		return;
	
	if(strcmp(oldFileName,".") == 0)
		return;
	if(strcmp(newFileName,".") == 0)
		return;
	
	if(strcmp(oldFileName,"..") == 0)
		return;
	if(strcmp(newFileName,"..") == 0)
		return;
	
	char oldFile[256];
	char newFile[256];
	char oldF[256];
	char newF[256];
	char tmpExt[256];
	
	if(strlen(curDir) == 0) // it's root
	{
		strcpy(oldF, "/");
		strcpy(newF, "/");
	}
	else // curDir is a directory
	{
		strcpy(oldF, curDir);
		strcpy(newF, curDir);
		
		if(curDir[strlen(curDir)-1] != '/')
		{
			strcat(oldF, "/");
			strcat(newF, "/");
		}
	}
	
	// copy after directory
	strcat(oldF, oldFileName);
	strcat(newF, newFileName);
	
	// save a copy for testing same thing
	strcpy(oldFile, oldF);
	strcpy(newFile, newF);
	strlwr(oldFile);
	strlwr(newFile);

	if(strcmp(oldFile,newFile) == 0)
		return;
	
	// save a copy for bmf renaming
	strcpy(oldFile, oldF);
	strcpy(newFile, newF);
	separateExtension(oldFile, tmpExt);	
	
	// test to make sure they aren't the same thing!		
	if(strcmp(oldFileName,newFileName) == 0)
		return;
	
	DRAGON_rename(oldF, newF);
	
	strlwr(tmpExt);
	if(strcmp(tmpExt, ".bmf") != 0) // this isn't a bmf itself
	{
		separateExtension(newFile, tmpExt);
		
		strcat(oldFile, ".bmf");
		strcat(newFile, ".bmf");
		
		if(DRAGON_FileExists(oldFile) != FT_NONE)
			DRAGON_rename(oldFile, newFile);
	}
}

void renameFile()
{	
	if(strlen(fileName) == 0)
		return;
	
	if(strcmp(copyFrom,"") == 0)
	{
		if(strcmp(fileName,".") == 0)
		return;
		
		if(strcmp(fileName,"..") == 0)
			return;
		
		// new file
		if(dir_create)
			DRAGON_mkdir(fileName);
		else{
			
			if(debug_FileExists((const char*)fileName,7) == FT_FILE){
				DRAGON_FILE * df = DRAGON_fopen(fileName, "w");	//debug_FileExists index: 7
				DRAGON_fclose(df);
			}
		}
	}
	else
	{	
		char oldFile[256];
		char newFile[256];
		
		strcpy(oldFile, copyFrom);
		strcpy(newFile, fileName);
		strlwr(oldFile);
		strlwr(newFile);

		if(strcmp(oldFile,newFile) == 0)
		{
			// Make sure we can change capitalization
			renameEntry(copyFrom, "_TMP_REN.DSO");
			renameEntry("_TMP_REN.DSO", fileName);
		}
		else
		{
			renameEntry(copyFrom, fileName);
		}
	}
}

void deleteFAT(char *entry, bool deleteFinal)
{
	if(DRAGON_FileExists(entry) == FT_NONE)
		return;
	
	if(DRAGON_FileExists(entry) == FT_FILE)
	{
		DRAGON_remove(entry);
		return;
	}
	
	// type is FT_DIR, make sure it's empty
	
	char toDelete[MAX_TGDSFILENAME_LENGTH+1] = {0};
	char done = false;
	int type;
	
	type = DRAGON_FindFirstFile(toDelete);
	
	while(!done)
	{
		if(type == FT_DIR)
		{
			if(strcmp(toDelete, ".") != 0 && strcmp(toDelete, "..") != 0)
			{
				char str[256];
				
				strcpy(str, entry);
				
				int x = strlen(str)-1;
				
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				strcat(str, toDelete);
				
				deleteFAT(str, true);
			}
		}
		else if(type == FT_FILE)
			DRAGON_remove(toDelete);
		else
			done = true;
		
		if(!done)
			type = DRAGON_FindNextFile(toDelete);
	}
	
	DRAGON_closeFind();
	
	if(deleteFinal)
	{
		DRAGON_remove(entry);
	}
}

void browserForward()
{
	showMenu = false;
	testMenu = false;
	
	switch(browserMode)
	{
		case 0:	 // open
			if(browserEntries == 0)
				return;
			loadFileWrapper(getCursor());
			break;
		case 1:	 // new file
			strcpy(copyFrom, "");
			strcpy(fileName, "");
			setLastMode(BROWSER);
			setMode(BROWSERRENAME);
			pushCursor();
			browserMode = 0;
			copyFlag = false;
			cutFlag = false;
			dir_create = false;
			break;
		case 2: // rename
			if(browserEntries == 0)
				return;
			
			if(strcmp(dirList[getCursor()].longName, ".") == 0)
				return;
			
			if(strcmp(dirList[getCursor()].longName, "..") == 0)
				return;
			
			strcpy(copyFrom, dirList[getCursor()].longName);
			strcpy(fileName, dirList[getCursor()].longName);
			
			setLastMode(BROWSER);
			setMode(BROWSERRENAME);
			pushCursor();
			browserMode = 0;
			copyFlag = false;
			cutFlag = false;
			dir_create = false;
			
			break;
		case 3: // mkdir
			strcpy(copyFrom, "");
			strcpy(fileName, "");
			setLastMode(BROWSER);
			setMode(BROWSERRENAME);
			pushCursor();
			browserMode = 0;
			copyFlag = false;
			dir_create = true;
			cutFlag = false;
			
			break;
		case LOADBIN: // load bin
			if(browserEntries == 0)
				return;
			if(dirList[getCursor()].fileType == BINFILE)
				loadFile(getCursor());				
			break;
		case CONFIRMDELETE: { // confirm delete
			char str[256];
			
			strcpy(str, curDir);
			
			int x = strlen(str)-1;
			
			if(str[x] == '/')
				str[x] = 0;
			
			strcat(str, "/");
			strcat(str, dirList[getCursor()].longName);
			
			deleteFAT(str, true);
			if(getCursor() > 0)
				moveCursorRelative(CURSOR_BACKWARD);
			
			browserMode = 0;
			isPopulated = 0;
		}
			break;			
	}
}

void browserBack()
{
	showMenu = false;
	testMenu = false;
	
	switch(browserMode)
	{
		case 0:	 // up
			upOneDir();
			break;
		case 1: // delete
			if(browserEntries == 0)
				return;
			
			if(strcmp(dirList[getCursor()].longName, ".") == 0)
				return;
			
			if(strcmp(dirList[getCursor()].longName, "..") == 0)
				return;
			
			browserMode = CONFIRMDELETE;
			break;
		case 2: // copy/paste
		{
			char str[256];
			
			if(copyFlag == false)
			{				
				if(browserEntries == 0)
					return;
				// check if its a dir, we dont support that
				if(dirList[getCursor()].fileType != DIRECTORY)
				{
					// ok we support, set the copyflag and the copy from
					strcpy(str, curDir);
					
					int x = strlen(str)-1;
					
					if(str[x] == '/')
						str[x] = 0;
						
					strcat(str, "/");
					strcat(str, dirList[getCursor()].longName);
					
					strcpy(copyFrom, str);
					browserMode = 0;
					
					copyFlag = true;
				}
			}
			else
			{
				// already copied, lets paste!
				
				char oldDir[256];
				char newDir[256];
				
				// ok first, we get the current directory				
				strcpy(str, curDir);
					
				int x = strlen(str)-1;
					
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				
				// change to current directory
				
				strcpy(newDir,str);
				
				// strip file out of copyFrom and place into str
				
				memset(str, 0, 256);
				
				x = 0;
				int y = 0;
				
				while(copyFrom[x] != 0)
				{
					if(copyFrom[x] == '/')
						y = x;
					
					x++;
				}
				
				
				memset(oldDir, 0, 256);
				strncpy(oldDir, copyFrom, y + 1);
				
				if(strcmp(oldDir,newDir) == 0)
				{
					// src and dest cant be the same!
					return;
				}
				
				// y holds the position of the last '/'
				
				x = y + 1;
				y = 0;
				while(copyFrom[x] != 0)
				{
					str[y] = copyFrom[x];
					x++;
					y++;
				}
				
				str[y] = 0;
				
				// str now holds the filename from copyfrom
				if(debug_FileExists((const char*)str,8) == FT_FILE){
					DRAGON_FILE *dest = DRAGON_fopen(str, "w");	//debug_FileExists index: 8
				
					if(debug_FileExists((const char*)copyFrom,9) == FT_FILE){
						DRAGON_FILE *src = DRAGON_fopen(copyFrom, "r");	//debug_FileExists index: 9
						
						uint8 *cpyBuffer = (uint8 *)safeMalloc(COPY_SIZE);
						uint16 cnt;
						
						maxSize = DRAGON_flength(src);
						curSize = 0;
						
						copying = true;
						
						while(!DRAGON_feof(src))
						{
							cnt = DRAGON_fread(cpyBuffer, 1, COPY_SIZE, src);
							DRAGON_fwrite(cpyBuffer, 1, cnt, dest);
							
							curSize += cnt;
						}
						
						copying = false;
						
						DRAGON_fclose(dest);
						DRAGON_fclose(src);
						
						copyFlag = false;
						isPopulated = 0;
						browserMode = 0;
						
						free(cpyBuffer);
					}
				}
			}
			break;	
		}
		case 3: // cut/paste
		{
			char str[256];
			
			if(cutFlag == false)
			{				
				if(browserEntries == 0)
					return;
				
				// check if its a dir, we dont support that
				if(dirList[getCursor()].fileType != DIRECTORY)
				{
					// ok we support, set the cutflag and the copy from
					strcpy(str, curDir);
					
					int x = strlen(str)-1;
					
					if(str[x] == '/')
						str[x] = 0;
						
					strcat(str, "/");
					strcat(str, dirList[getCursor()].longName);
					
					strcpy(copyFrom, str);
					browserMode = 0;
					
					cutFlag = true;
				}
			}
			else
			{
				// already cut, lets paste!
				
				char oldDir[256];
				char newDir[256];
				
				// ok first, we get the current directory				
				strcpy(str, curDir);
					
				int x = strlen(str)-1;
					
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				strcpy(newDir,str);								
				
				memset(str, 0, 256);
				
				x = 0;
				int y = 0;
				
				while(copyFrom[x] != 0)
				{
					if(copyFrom[x] == '/')
						y = x;
					
					x++;
				}
				
				memset(oldDir, 0, 256);
				strncpy(oldDir, copyFrom, y + 1);
				
				if(strcmp(oldDir,newDir) == 0)
				{
					// src and dest cant be the same!
					return;
				}
				
				// y holds the position of the last '/'
				
				x = y + 1;
				y = 0;
				while(copyFrom[x] != 0)
				{
					str[y] = copyFrom[x];
					x++;
					y++;
				}
				
				str[y] = 0;
				
				// str now has the filename
				strcat(newDir, str);
				strcpy(oldDir, copyFrom);
				
				DRAGON_rename(oldDir, newDir);
				
				cutFlag = false;
				isPopulated = 0;
				browserMode = 0;
			}
			break;	
		}
		case LOADBIN: // cancel load bin
			browserMode = 0;
			clearBinName();
			break;
		case CONFIRMDELETE: // cancel delete
			browserMode = 1;
			break;
	}
}

int getBrowserEntries()
{
	return browserEntries;
}

bool confirmDelete()
{
	return browserMode == CONFIRMDELETE;
}
