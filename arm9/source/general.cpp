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

#include "consoleTGDS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include <libpicture.h>
#include <stdarg.h>
#include "fatwrapper.h"
#include "lcdfunctions.h"
#include "colors.h"
#include "general.h"
#include "language.h"
#include "settings.h"
#include "globals.h"
#include "resources.h"
#include "keyboard.h"
#include "controls.h"
#include "graphics.h"
#include "boot.h"
#include "sound.h"
#include "specific_shared.h"
#include "nds_loader_arm9.h"
#include "supercard.h"
#include "autopatch.h"
#include "splash.h"
#include "font_arial_11.h"
#include "font_arial_9.h"
#include "font_gautami_10.h"
#include "drawing/browser.h"
#include "drawing/calendar.h"
#include "drawing/dayview.h"
#include "drawing/address.h"
#include "drawing/configuration.h"
#include "drawing/pictureviewer.h"
#include "drawing/todo.h"
#include "drawing/help.h"
#include "drawing/scribble.h"
#include "drawing/database.h"
#include "drawing/irc.h"
#include "drawing/home.h"
#include "drawing/webbrowser.h"
#include "drawing/calculator.h"

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "keypadTGDS.h"
#include "videoTGDS.h"
#include "InterruptsARMCores_h.h"
#include "biosDSOrganize.h"
#include "fsfatlayerTGDSLegacy.h"
#include "loader.h"

bool copying = false;
bool loading = false;
bool rendering = false;
u32 maxSize;
u32 curSize;
uint16 homePage = HOME;
uint16 homeMode = 0;
int vblCount = 0;
int locations[13];
bool allowSwapBuffer = true;

static bool foundCursor = false;
static int tCursor = 0;
static int cowAnim = 0;
static int cowCount = 0;
static int direction = 1;
static u32 webAniHeartBeat = 0;
static bool haltOnOutOfMem = true;
static int repCount = 0;
static u32 globalError = 0;

extern bool working;
extern uint16 curTime;
extern char fileName[256];
extern uint16 browserMode;
extern bool cLoadedPage1;
extern char curDir[256];
extern char *memory;
extern bool isRecording;
extern u32 ticCount;
extern u32 ircCount;
extern int waitCount;
extern bool cancelOnWait;
extern bool swapAB;
extern int normalBoot;
extern int altBoot;
extern bool disablePatching;

extern PICTURE_DATA curPicture;
extern bool isQueued();

#ifdef MALLOC_TRACK
	static MALLOC_LIST mallocList[100];
	static int mallocListPtr = 0;
#endif


int getRepCount()
{
	return repCount;
}

void setRepCount(int newVal)
{
	repCount = newVal;
}

void setGlobalError(u32 sE)
{
	globalError = sE;
}

void quickSwap(int *x, int *y)
{
	if (x != y) 
	{
		*x ^= *y;
		*y ^= *x;
		*x ^= *y;
	}
}

void findDataDirectory()
{
	//use the default 0:/<dir> (root)
	//file access / file or directory listing is relative to inmediate path passed as argument. So we remove the crazy ammount of DRAGON_chdir calls there is around DSOrganize code
}

void makeDirectories() // for creating the right paths to the data dir
{
	
}

u32 getAniHeartBeat()
{
	return webAniHeartBeat;
}

void vBlank() // in case i ever need it... oops apparently I did.
{
	webAniHeartBeat++;
	
	if(getMode() == INITFAT || getMode() == INITPLUGIN || getMode() == DISPLAYCOW)
	{
		cowAnim++;
		
		if(cowAnim >= 15)
		{
			cowAnim = 0;
			cowCount++;
			
			if(cowCount > 3)
				cowCount = 0;
			
			drawStartSplash();
			
			if(getMode() != DISPLAYCOW)
			{
				setFont((uint16 **)font_arial_11);
				setColor(0xFFFF);
				setBold(true);
				
				bg_setClipping(5,5,250,181);
				bg_dispString(0,0,l_fatinit);
				
				setBold(false);
				
				setFont((uint16 **)font_arial_9);
				
				if(getMode() == INITFAT)
				{
					if(getSenderStatus() == true){
						//ARM7 INIT OK.
						char tStr[64];
						sprintf(tStr, "[ARM7OK]Trying %s...", DRAGON_tryingInterface()); // \n\n%d, curSize);
						bg_dispString(0,25,tStr);
					}
					else{
						//ARM7 INIT WAIT
					}
				}
				else
				{
					char tStr[64];
					sprintf(tStr, "Initialized %s", DRAGON_tryingInterface()); // \n\n%d, curSize);
					
					bg_dispString(0,25,tStr);
					
					bg_dispString(0,40,"Initializing settings...");
					
					bg_drawRect(10, 65, 15, 70, 0xFFFF);
				}
				
				bg_swapBuffers();
			}
			
			uint16 *fourCows[] = { s_cow1, s_cow2, s_cow3, s_cow4 };
			fb_dispSprite(112, 154, fourCows[cowCount], RGB15(0,0,31));
			fb_swapBuffers();
		}
		
		return;
	}
	
	if(cancelOnWait)
		waitCount++;
	
	if(isRecording)
	{
		ticCount++;
		return;
	}
	
	incrimentBCount();
	
	rand(); // make sure we get real random stuff
	
	ircCount++;
	repCount++;
	
	if(loading || rendering)
	{
		if(loading)
		{
			setColor(genericTextColor);
			fb_dispString(5, 5, l_loadpic);
		}
		
		if(curSize == 0)
			direction = 1;
		if(curSize >= (u32)((getScreenWidth() - 24) >> 2))
			direction = -1;
		
		fb_drawFilledRect(5, getScreenHeight()-16, getScreenWidth()-6, getScreenHeight()-6, loadingBorderColor, loadingFillColor);
		fb_drawRect(6 + (curSize << 2), getScreenHeight()-15, 17 + (curSize << 2), getScreenHeight()-7, loadingBarColor);
		
		curSize += direction;
		
		fb_swapBuffers();	
		
		return;
	}
	
	if(copying)
	{
		// we gotta display the bar graph of how far we are
		if(repCount%10 == 0)
		{
			float percent = (float)(curSize) / (float)maxSize;
			int pixel = (int)((255 - 12) * percent);
			
			fb_drawFilledRect(5, 191-15, 255-5, 191-5, loadingBorderColor, loadingFillColor);
			fb_drawRect(6, 191-14, 6 + pixel, 191-6, loadingBarColor);
			
			fb_swapBuffers();
		}
		
		return;
	}
	
	if(working)
	{
		if(repCount%10 == 0)
		{
			drawAnswerScreen();
			fb_swapBuffers();
		}
		
		return;
	}
	
	if(curPicture.picType == PIC_ANI && (curPicture.aniDelays != NULL || curPicture.usingFile))
	{
		vblCount++;
		
		if(!curPicture.usingFile)
		{
			if(vblCount >= curPicture.aniDelays[curPicture.curAni])
			{
				vblCount = 0;
				curPicture.curAni++;
				if(curPicture.curAni > curPicture.aniCount - 1)
					curPicture.curAni = 0;
			}
		}
		else
		{
			if(vblCount >= curPicture.aniDelay)
			{
				vblCount = 0;
				curPicture.curAni = 1;
			}
		}
	}
}

void disableHalt()
{
	haltOnOutOfMem = false;
}

struct touchScr touchReadXYNew()
{
	struct touchScr touchPos;
	touchScrRead(&touchPos);
	
	switch(getOrientation())
	{
		case ORIENTATION_90:
			// reverse coordinates
			touchPos.touchYpx = touchPos.touchXpx;
			touchPos.touchXpx = 191 - touchPos.touchYpx;
			break;
		case ORIENTATION_270:
			// reverse coordinates
			touchPos.touchYpx = 255 - touchPos.touchXpx;
			touchPos.touchXpx = touchPos.touchYpx;
			break;
		default:
			touchPos.touchXpx = touchPos.touchXpx;
			touchPos.touchYpx = touchPos.touchYpx;
			break;
	}
	
	return touchPos;
}

void *safeMalloc(size_t size)
{
	void *tmp = malloc(size);
	
	if(!tmp)
	{
		if(!haltOnOutOfMem)
		{
			haltOnOutOfMem = true;
			return NULL;
		}
		
		char err_str[512];		
		sprintf(err_str, "%s\n\n%d [%X]", l_fatalerror, globalError, globalError);		
		printf("%s",err_str);
		while(1==1);
		
	}
	
	memset(tmp, 0, size);
	
	// make sure next time will halt
	haltOnOutOfMem = true;
	
	return tmp;
}

void safeFree(void *p)
{
	if(p)
	{
		free(p);
	}
}

bool isGBA(char *fName)
{
	char str[256];
	char ext[256];
	
	strcpy(str, fName);
	separateExtension(str, ext);
	separateExtension(str, ext);
	
	strlwr(ext);
	
	if(strcmp(ext, ".sc") == 0 || strcmp(ext, ".ds") == 0)
	{
		return true;
	}
	
	return false;
}

void launchNDSMethod0(char *file)
{	
	switch(DRAGON_DiscType())
	{
		case 0x4643504D: // gbamp
		{
			freeDirList();
			//vramSetBankD(VRAM_D_LCD);
			VRAMBLOCK_SETBANK_D(VRAM_D_LCDC_MODE);
	
			u32 fCluster;
			
			DRAGON_FILE *fFile = NULL;
			if(debug_FileExists((const char*)file,47) == FT_FILE){
				fFile = DRAGON_fopen(file, "r");	//debug_FileExists index: 47
			}
			
			fCluster = fFile->firstCluster;
			DRAGON_fclose(fFile);	
			DRAGON_FreeFiles();			
			
			REG_IME = IME_DISABLE;	// Disable interrupts
			WAIT_CR |= (0x8080);  // ARM7 has access to GBA cart
			
			for(u32 y=0x027FF800;y<0x02800000;y+=4)
				*((vu32*)y) = 0;
			
			*((vu32*)0x027FFFFC) = fCluster;  // Start cluster of NDS to load
			*((vu32*)0x027FFE04) = (u32)0xE59FF018;  // ldr pc, 0x027FFE24
			*((vu32*)0x027FFE24) = (u32)0x027FFE04;  // Set ARM9 Loop address
			SendArm7Command(ARM7COMMAND_BOOT_GBAMP, 0);
			swiSoftReset(); 
			
			while(1);
			
			break;
		}
		case 0x46434353: // supercard cf
		case 0x44534353: // supercard sd
		{		
			freeDirList();	
			//vramSetBankD(VRAM_D_LCD);
			VRAMBLOCK_SETBANK_D(VRAM_D_LCDC_MODE);
			
			REG_IME = IME_DISABLE;
			
			//boot_SCCF(file);	//not yet
			
			while(1);
			
			break;
		}
		default: // message to say not supported and to change
		{
			fb_setClipping(5,5,250,187);
			
			setColor(genericTextColor);			
			setFont((uint16 **)font_arial_11);
			fb_dispString(0,0, l_nodefaultboot);
			fb_swapBuffers();
			
			while(1);
			
			break;
		}
	}
}

void launchNDSMethod1(char *file)
{	
	//vramSetBankD(VRAM_D_LCD);
	VRAMBLOCK_SETBANK_D(VRAM_D_LCDC_MODE);
	REG_IE = 0;
	REG_IME = 0;
	REG_IF = 0xFFFF;	// Acknowledge interrupt
	
	bool doGBABoot = isGBA(file);
	
	char shortFile[MAX_TGDSFILENAME_LENGTH+1] = {0};
	char directory[MAX_TGDSFILENAME_LENGTH+1] = {0};
	
	strcpy(directory,file);
	
	char *whereSlash = directory;
	
	while(strchr(whereSlash, '/') != NULL)
		whereSlash = strchr(whereSlash, '/') + 1;
	
	whereSlash[0] = 0;	
	
	DRAGON_FILE *fp = NULL;
	if(debug_FileExists((const char*)file,48) == FT_FILE){
		fp = DRAGON_fopen(file, "r");	//debug_FileExists index: 48
		DRAGON_GetAlias(shortFile);
		DRAGON_fclose(fp);	
	}
	strcat(directory, shortFile);
	SendArm7Command(ARM7COMMAND_BOOT_MIGHTYMAX, 0);
	exec(directory, doGBABoot, false);
}

void launchNDSMethod2(char *file)
{	
	//vramSetBankD(VRAM_D_LCD);
	VRAMBLOCK_SETBANK_D(VRAM_D_LCDC_MODE);
	
	// get cluster
	
	u32 fCluster;
	
	DRAGON_FILE *fFile = NULL;
	if(debug_FileExists((const char*)file,49) == FT_FILE){
		fFile = DRAGON_fopen(file, "r");	//debug_FileExists index: 49
		fCluster = fFile->firstCluster;
		DRAGON_fclose(fFile);
	}
	
	DRAGON_chdir("/");
	// get loader
	std::string FileTest = string(getfatfsPath((char*)"DSOrganize/resources/load.bin"));
	DRAGON_FILE *stub = NULL;
	
	if(debug_FileExists((const char*)FileTest.c_str(),50) == FT_FILE){
		stub = DRAGON_fopen(FileTest.c_str(),"r");	//debug_FileExists index: 50
		u32 tLen = DRAGON_flength(stub);
		char *buffer = (char *)safeMalloc(tLen);
		DRAGON_fread(buffer,tLen,1,stub);
		DRAGON_fclose(stub);
		
		fb_setClipping(5,5,250,187);
		
		setColor(genericTextColor);			
		setFont((uint16 **)font_arial_11);
		fb_dispString(0,0, l_launchingchishm);
		fb_swapBuffers();
		
		REG_IE = 0;
		REG_IME = 0;
		REG_IF = 0xFFFF;	// Acknowledge interrupt
		
		//runNds(buffer, tLen, fCluster, false);
	}
	else{
		//file not exists
		printfDebugger("MissingFile:%s",FileTest.c_str());
	}
	while(1);
}


void launchNDS(char *file)
{
	bootndschishms(file,true);	//homebrew boot
	while(1==1){}	//boot or loop here forever
	
	
	if(!disablePatching)
		patchFile(file);
	
	switch(normalBoot)
	{
		case BOOT_INTERNAL:
			launchNDSMethod0(file);
			break;
		case BOOT_MIGHTYMAX:
			launchNDSMethod1(file);
			break;
		case BOOT_CHISHM:
			launchNDSMethod2(file);
			break;
	}
	
}

void launchNDSAlt(char *file)
{
	bootndschishms(file,1);	//homebrew boot
	while(1==1){}	//boot or loop here forever
	
	if(!disablePatching)
		patchFile(file);
	
	switch(altBoot)
	{
		case BOOT_INTERNAL:
			launchNDSMethod0(file);
			break;
		case BOOT_MIGHTYMAX:
			launchNDSMethod1(file);
			break;
		case BOOT_CHISHM:
			launchNDSMethod2(file);
			break;
	}
}

uint16 getCurTime()
{
	uint16 ct = getHour(true) * 2;
	
	if(getMinute() > 29)
		ct++;
	
	return ct;
}

int centerOnPt(int pt, char *str, uint16 **FP)
{
	return pt - (getStringWidth(str, FP)/2);
}

int format12(int x)
{
	if( x > 12 )
		x -= 12;
	
	if( x == 0 )
		x = 12;
		
	return x;
}

char properCaps(char c)
{
	if(c >= 'A' && c <= 'Z' && !isCaps())
		return c - ('A' - 'a');
	return c;
}

void abbreviateString(char *str, int maxLen, uint16 **FP)
{
	if(getStringWidth(str, FP) <= maxLen)
		return;
		
	strcat(str,"...");
	while(getStringWidth(str, FP) > maxLen)
	{
		str[strlen(str)-4] = '.';
		str[strlen(str)-1] = 0;
	}		
}

void separateExtension(char *str, char *ext)
{
	int x = 0;
	
	for(int y = strlen(str) - 1; y > 0; y--)
	{
		if(str[y] == '.')
		{
			// found last dot
			x = y;
			break;
		}
		if(str[y] == '/')
		{
			// found a slash before a dot, no ext
			ext[0] = 0;
			return;
		}
	}
	
	if(x > 0)
	{
		int y = 0;
		while(str[x] != 0)
		{
			ext[y] = str[x];
			str[x] = 0;
			x++;
			y++;
		}
		ext[y] = 0;
	}
	else
		ext[0] = 0;	
}

void enableVBlank()
{
    //irqSet(IRQ_VBLANK, vBlank);
	//irqEnable(IRQ_VBLANK);	
	REG_IE|=IRQ_VBLANK;
}

void disableVBlank()
{
	//irqSet(IRQ_VBLANK, 0);
	//irqDisable(IRQ_VBLANK);	
	REG_IE&=~IRQ_VBLANK;
}

bool checkHelp()
{
	#ifndef MALLOC_TRACK
	#ifndef SCREENSHOT_MODE
	
	#ifdef DUMP_ARM7
	if(keysPressed() & KEY_SELECT)
	{
		soundIPC->interlaced = (s16 *)trackMalloc(64*1024, "dump");
		SendArm7Command(ARM7COMMAND_DUMP, 0);
	}
	#else
	if(((keysPressed() & KEY_SELECT) || (keysPressed() & KEY_SELECT)) && getMode() != ABOUT && !isQueued())
	{
		drawHelpScreen();
		
		IRQVBlankWait();
		if(allowSwapBuffer)
			fb_swapBuffers();
		bg_swapBuffers();
		
		if(getMode() == SOUNDPLAYER)
			flagRefresh();
		
		return true;
	}
	#endif
	#endif
	#else
	if(keysPressed() & KEY_SELECT)
	{
		printMallocList();
	}
	#endif
	
	return false;
}

void debugPrint(char *str)
{
	// To ensure it comes out on any debugging screens i have up
	printf(str);
	
	fb_setDefaultClipping();
	fb_dispString(3,3,str);
	
	IRQVBlankWait();	
	fb_swapBuffers();
}

void debugPrintHalt(char *str)
{
	//irqSet(IRQ_VBLANK, 0);
	fb_swapBuffers();
	
	fb_setBGColor(0xFFFF);
	setColor(0);
	
	debugPrint(str);
	
	while(1)
	{
		IRQVBlankWait();
		
		if(keysPressed() & KEY_SELECT)
		{
			while(keysPressed() & KEY_SELECT){
			}
			
			enableVBlank();
			
			return;
		}
	}
}


void *trackMalloc(u32 length, char *desc)
{
	void *tmp = safeMalloc(length);
	
	#ifdef MALLOC_TRACK
	
	// add here	
	if(mallocListPtr < 100)
	{	
		mallocList[mallocListPtr].memLocation = (u32)tmp;
		mallocList[mallocListPtr].size = (u32)length;
		strcpy(mallocList[mallocListPtr].description, desc);
		
		mallocListPtr++;	
	}
	
	// return normal
	#endif
	
	return tmp;
}

void trackFree(void *tmp)
{
	#ifdef MALLOC_TRACK
	
	// subtract here	
	for(int i=0;i<mallocListPtr;i++)
	{
		if(mallocList[i].memLocation == (u32)tmp)
		{
			for(int j=i;j<mallocListPtr;j++)
			{
				mallocList[j].memLocation = mallocList[j+1].memLocation;
				mallocList[j].size = mallocList[j+1].size;
				strcpy(mallocList[j].description, mallocList[j+1].description);
			}
			
			mallocListPtr--;
			break;
		}
	}
	
	// do normal
	#endif
	
	free(tmp);
}

void *trackRealloc(void *tmp, u32 length)
{
	#ifdef MALLOC_TRACK
	u32 old = (u32)tmp;
	#endif
	
	void *newTmp = realloc(tmp,length);
	
	if(!newTmp)
	{
		debugPrintHalt(l_fatalerror);
		
		while(1) 
		{ 
			// empty, halting processor
		}
	}
	
	#ifdef MALLOC_TRACK
	for(int i=0;i<mallocListPtr;i++)
	{
		if(mallocList[i].memLocation == old)
		{
			if(newTmp)
			{
				mallocList[i].size = (u32)length;
				mallocList[i].memLocation = (u32)newTmp;
				break;
			}
			else
			{
				// handle realloc of null
				for(int j=i;j<mallocListPtr;j++)
				{
					mallocList[j].memLocation = mallocList[j+1].memLocation;
					mallocList[j].size = mallocList[j+1].size;
					strcpy(mallocList[j].description, mallocList[j+1].description);
				}
				
				mallocListPtr--;
				break;
			}
		}
	}
	#endif
	
	return newTmp;
}

#ifdef MALLOC_TRACK
void printMallocList()
{
	fb_swapBuffers();
	
	setFont((uint16 **)font_gautami_10);
	setColor(0);
	
	if(mallocListPtr > 0)
	{
		for(int i=0;i<mallocListPtr;i++)
		{
			fb_dispString((i%3) * 86,(i/3)*11, mallocList[i].description);
			
			//char str[30];
			//sprintf(str, "%d", mallocList[i].size);
			
			//fb_dispString((i%3) * 86,(i/3)*22+11, str);
		}
	}
	else
		fb_dispString(0,0,"Nothing in malloc list");
	
	IRQVBlankWait();
	fb_swapBuffers();
	
	while(keysPressed()){
	}
}
#endif

void defaultLocations()
{
	locations[0] = 0;
	locations[1] = 1;
	locations[2] = 2;
	locations[3] = 3;
	locations[4] = 4;
	locations[5] = 5;
	locations[6] = 6;
	locations[7] = 7;
	locations[8] = 8;
	locations[9] = 9;
	locations[10] = 10;
	locations[11] = 11;
	locations[12] = 12;
}

void gotoApplication(int app, bool transPos)
{
	int whichApp;

	if(transPos)
	{
		homeMode = getCursor();
		homePage = getMode();
		
		if(homePage != HOME && homePage != HOMEMORE)
		{
			if(app < 6)
				homePage = HOME;
			else
				homePage = HOMEMORE;
		}
		
		whichApp = locations[app]; // in the future allow for editable
	}
	else
	{
		int x;
		
		for(x=0;x<12;x++)
		{
			if(locations[x] == app)
			{
				if(x < 6)
				{
					homePage = HOME;
					homeMode = x;
				}
				else
				{
					homePage = HOMEMORE;
					homeMode = x - 6;
				}
				
				break;
			}
		}
		
		whichApp = app;
	}
	
	invalidateHomeLoaded();
	
	switch(whichApp)
	{
		case 0:			
			setMode(CALENDAR);
			resetCalLoaded();
			resetCursor();
			break;
		case 1:
			setMode(DAYVIEW);
			resetDayView();
			curTime = getCurTime();
			resetCursor();
			break;
		case 2:
			addressSetUnPopulated();
			strcpy(fileName,"");
			setMode(ADDRESS);
			resetCursor();
			break;
		case 3:
			todoSetUnPopulated();
			strcpy(fileName,"");
			setMode(TODOLIST);
			resetCursor();
			break;
		case 4:
			scribbleSetUnPopulated();
			strcpy(fileName,"");
			setMode(SCRIBBLEPAD);
			resetCursor();
			break;
		case 5:
			strcpy(curDir, "/");
			browserResetPos();
			browserSetUnPopulated();
			setMode(BROWSER);
			browserMode = 0;
			resetCursor();
			break;
		case 6:
		{
			//int oldMode = getMode();
			
			setMode(CALCULATOR);
			clearCalc();
			/*if(!clearCalc())
			{
				setMode(oldMode);
				break;
			}
				*/
			strcpy(memory,"0");
			break;
		}
		case 7:
			setMode(IRC);
			resetIRCMode();
			resetCursor();
			break;
		case 8:
			setMode(WEBBROWSER);
			initWebBrowser();
			resetCursor();
			break;
		case 9:
			setMode(HOMEBREWDATABASE);
			resetDBMode();
			resetCursor();
			break;
		case 10:
		case 11:
			break;
		case GO_CONFIG:
			setMode(CONFIGURATION);
			cLoadedPage1 = false;
			configurationResetCursor();			
			break;
	}
}

void returnHome()
{	
	if(getMode() == DAYVIEW)	
		resetDayView();
	else if(getMode() == CONFIGURATION)
	{
		clearHelpMemory();
		homeMode = 6;
	}
	else if(getMode() == BROWSER) // fix for plugins
	{
		resetCopyFlags();
		for(int x=0;x<12;x++)
		{
			if(locations[x] == 5) // browser
			{
				if(x < 6)
					homePage = HOME;
				else
					homePage = HOMEMORE;
				
				if(x < 6)
					homeMode = x;
				else
					homeMode = x - 6;
				
				break;
			}
		}
	}
	
	setMode(homePage);
	moveCursorAbsolute(homeMode);
	browserResetPos();
	
	invalidateHomeLoaded();
}

void fakeConfiguration()
{
	homeMode = 6;
}

// normal
bool isNumerical(char c)
{
	if(c < '0')
		return false;
	if(c > '9')
		return false;
	
	return true;
}

void writeDebug(const char *s, ...)
{
	bool shouldFree = true;
	char *temp = (char *)malloc(8192);
	
	if(!temp)
	{
		shouldFree = false;
		temp = "Out of memory in writeDebug";
	}
	else
	{
		va_list marker;
		va_start(marker,s);
		vsprintf(temp,s,marker);
		va_end(marker);
	}
	
	DRAGON_preserveVars();
	
	DRAGON_FILE *fp = NULL;
	if(debug_FileExists((const char*)"debug.txt",51) == FT_FILE){
		fp = DRAGON_fopen("debug.txt", "a");	//debug_FileExists index: 51
	}
	
	DRAGON_fputs(temp, fp);
	DRAGON_fputc('\r', fp);
	DRAGON_fputc('\n', fp);
	DRAGON_fclose(fp);
	DRAGON_restoreVars();
	
	if(shouldFree)
		free(temp);
}

//todo
#ifdef DEBUG_MODE
void debugInit()
{
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG);

	BG0_CR = BG_MAP_BASE(8);
	BG_PALETTE[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	consoleInitDefault((u16*)SCREEN_BASE_BLOCK(8), (u16*)CHAR_BASE_BLOCK(0), 16);
}
#else
/*
void dprintf(const char *d1, ...)
{
	// dummy function to call instead of iprintf
}
*/
#endif

// format of %sx, %dx, %cx where x is the variable number
// %c0 for control reset
// maximum of 9 ordered arguments, 1-9
void osprintf(char *out, const char* fmt, ...)
{
	void *vaArray[11];
	int numArgs = 0;
	int pos = 0;
	void *args;
	va_list marker;
	
	va_start(marker,fmt);

	args = va_arg(marker, void *);
	while(numArgs < 10)
	{
		vaArray[numArgs] = args;
		args = va_arg(marker, void *);		
		
		numArgs++;
	}
	
	va_end(marker);
	
	char c = 'A'; // A is for Apple!
	
	while(c != 0)
	{
		c = *fmt;
		
		fmt++;
		
		if(c == 0)
		{
			out[pos] = 0;
			break;
		}
		
		if(c == '%')
		{
			int i = 0;
			switch(*fmt)
			{
				case 'c':
				case 'C':
					fmt++;
					if(*fmt == '0')
					{
						out[pos] = CONTROL_RESET;
						pos++;
					}
					else
					{
						i = (*fmt) - '1';			
						if((u32)vaArray[i] != 0)
						{
							out[pos] = (u32)vaArray[i];
							pos++;
						}
					}
					
					break;
				case 'd':
				case 'D':
				{
					fmt++;
					i = (*fmt) - '1';
					
					char strTmp[18];
					sprintf(strTmp, "%d", (u32)vaArray[i]);
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}							
					
					break;
				}
				case 's':
				case 'S':
				{							
					fmt++;
					i = (*fmt) - '1';
					
					char *strPtr = (char *)vaArray[i];
					
					while(*strPtr != 0)
					{								
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}				
					
					break;
				}
				case 'h':
				{
					bool milT = false;
					
					fmt++;
					switch(*fmt)
					{
						case 'm':
						case 'M':
							milT = true;
					}
					
					char strTmp[3];
					sprintf(strTmp, "%d", getHour(milT));
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'H':
				{
					bool milT = false;
					
					fmt++;
					switch(*fmt)
					{
						case 'm':
						case 'M':
							milT = true;
					}
					
					char strTmp[3];
					sprintf(strTmp, "%02d", getHour(milT));
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'm':
				{
					char strTmp[3];
					sprintf(strTmp, "%d", getMinute());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'M':
				{
					char strTmp[3];
					sprintf(strTmp, "%02d", getMinute());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 't':
				{
					char strTmp[3];
					sprintf(strTmp, "%d", getSecond());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'T':
				{
					char strTmp[3];
					sprintf(strTmp, "%02d", getSecond());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}								
				case 'a':				
				case 'A':
				{
					char *strPtr = l_ampm[isPM()];
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}							
				case 'k':				
				case 'K':
				{
					out[pos] = CONTROL_COLOR;
					pos++;
					
					break;
				}						
				case 'b':				
				case 'B':
				{
					out[pos] = CONTROL_BOLD;
					pos++;
					
					break;
				}				
				case 'u':				
				case 'U':
				{
					out[pos] = CONTROL_UNDERLINE;
					pos++;
					
					break;
				}
			}
			
			fmt++;
		}
		else if(c == '\\')
		{
			switch(*fmt)
			{
				case '\\':
					out[pos] = '\\';
					pos++;
					break;
				case 't':
					out[pos] = '\t';
					pos++;
					break;
				case 'n':
					out[pos] = '\n';
					pos++;
					break;
				case '%':
					out[pos] = '\%';
					pos++;
					break;
			}
			
			fmt++;
		}
		else
		{
			out[pos] = c;
			pos++;
		}
	}
}

int cursorPosCall(int pos, u32 c, int pass, int xPos, int yPos)
{
	if(!(keysPressed() & KEY_TOUCH))
		return 0;
	
	if(pass == 1)
		return 0;
	if(!pos)
	{
		foundCursor = false;
		tCursor = -1;
	}
	
	if(foundCursor)
		return 0;
		
	struct touchScr t = touchReadXYNew();	
	int tWidth = (getCharWidth(c) >> 1)+1;
	
	if((t.touchXpx >= (xPos - tWidth)) && (t.touchXpx <= (xPos + tWidth)))
	{
		tCursor = pos;
		foundCursor = true;
	}
	
	return 0;
}

int getTouchCursor()
{
	int tRet = -2;
	
	if(foundCursor)
		tRet = tCursor;
	
	tCursor = -1;
	foundCursor = false;
	
	return tRet;
}

// Not written by me.  Can you tell by the proper use of comments?
char *cistrstr(char *haystack, char *needle)
{
	if ( !*needle )
	{
		return haystack;
	}
	
	for(;*haystack;++haystack)
	{
		if(toupper(*haystack) == toupper(*needle))
		{
			/*
			* Matched starting char -- loop through remaining chars.
			*/
			const char *h, *n;
			
			for(h = haystack, n = needle;*h && *n;++h, ++n)
			{
				if(toupper(*h) != toupper(*n))
				{
					break;
				}
			}
			
			if(!*n) /* matched all of 'needle' to null termination */
			{
				return haystack; /* return the start of the match */
			}
		}
	}
	
	return 0;
}

void takeScreenshot()
{
	if(keysPressed() & KEY_SELECT)
	{
		static PICTURE_DATA ssPicture;
		
		initImage(&ssPicture, 0, 0);
		createImage(&ssPicture, 256, 192 * 2, 0xFFFF);
		sprintf(ssPicture.fileName, "screenshot%d%d%d%d%d%d.png", getDay(), getMonth(), getYear(), getHour(true), getMinute(), getSecond());
		
		if(screensFlipped() || configFlipped())
		{
			memcpy((uint16 *)ssPicture.rawData, fb_backBuffer(), 256*192*2);
			memcpy((uint16 *)ssPicture.rawData + 256*192, bg_backBuffer(), 256*192*2);
		}
		else
		{
			memcpy((uint16 *)ssPicture.rawData, fb_backBuffer(), 256*192*2);
			memcpy((uint16 *)ssPicture.rawData + 256*192, bg_backBuffer(), 256*192*2);
		}
		
		saveImageFromHandle(&ssPicture);				
		freeImage(&ssPicture);
	}
}
