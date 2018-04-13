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
 
#ifndef _GENERAL_INCLUDED
#define _GENERAL_INCLUDED

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "keypadTGDS.h"

// sound stuff

#define SND_GOFORWARD 0
#define SND_GOBACK 1
#define SND_GETPREV 2
#define SND_GETNEXT 3
#define SND_TOGGLEHOLD 5
#define SND_MODE 6
#define SND_VOLUMEUP 7
#define SND_VOLUMEDN 8
#define SND_PREVINTERNAL 9
#define SND_NEXTINTERNAL 10
#define SND_SEEK 11

// going to screens
#define GO_CONFIG 12

//#define SCREENSHOT_MODE
//#define MALLOC_TRACK
//#define DEBUG_MODE
//#define PROFILING

typedef struct
{
	u32 memLocation;
	u32 size;
	char description[32];
} MALLOC_LIST;



#define assert(x,y) { \
	if(x) { \
		writeDebug(y); \
	} \
}

#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifdef DEBUG_MODE
//#define dprintf iprintf
#else
//extern void dprintf(const char *d1, ...);
#endif
	
extern void *trackMalloc(u32 length, char *desc);
extern void trackFree(void *tmp);
extern void *trackRealloc(void *tmp, u32 length);

extern void *safeMalloc(size_t size);
extern void safeFree(void *p);

extern void makeDirectories();
extern void vBlank();
extern void launchNDS(char *file);
extern void launchNDSAlt(char *file);
extern uint16 getCurTime();
extern int centerOnPt(int pt, char *str, uint16 **FP);
extern int format12(int x);
extern char properCaps(char c);
extern void abbreviateString(char *str, int maxLen, uint16 **FP);
extern void separateExtension(char *str, char *ext);
extern void enableVBlank();
extern void disableVBlank();
extern bool checkHelp();
extern void returnHome();
extern void quickSwap(int *x, int *y);
extern void gotoApplication(int app, bool transPos);
extern void defaultLocations();
extern void fakeConfiguration();
extern bool isDataDir();
extern bool isNumerical(char c);
extern void osprintf(char *out, const char* fmt, ...);
extern int cursorPosCall(int pos, u32 c, int pass, int xPos, int yPos);
extern int getTouchCursor();
extern char *cistrstr(char *haystack, char *needle);
extern u32 getAniHeartBeat();
extern void disableHalt();
extern void takeScreenshot();
extern int getRepCount();
extern void setRepCount(int newVal);
extern void findDataDirectory();

// debug only
extern void setGlobalError(u32 sE);
extern void debugPrint(char *str);
extern void debugPrintHalt(char *str);
extern void writeDebug(const char *s, ...);
extern void debugInit();

#ifdef MALLOC_TRACK
extern void printMallocList();
#endif

// in arm9.cpp
extern void checkJustKeys();
extern bool isScrolling();
extern void touchDown(int px, int py);
extern void touchUp();
extern void goForward();

// fix for games & music
extern struct touchScr touchReadXYNew();

#ifdef __cplusplus
}
#endif
