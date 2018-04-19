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
 
#ifndef _WEBBROWSER_INCLUDED
#define _WEBBROWSER_INCLUDED

#include "html.h"

#define WEBSTATE_INIT 0
#define WEBSTATE_CONNECTING 1
#define WEBSTATE_CONNECTED 2
#define WEBSTATE_FAILEDCONNECT 3
#define WEBSTATE_DOWNLOADING 4
#define WEBSTATE_FINISHED 5
#define WEBSTATE_FAILEDSERVER 6
#define WEBSTATE_LOADHOMEPAGE 7

#define FAKE_RET 31
#define MAX_URL 16384

#define TAB_KEYBOARD 0
#define TAB_FAVORITES 1
#define TAB_IRCURLS 2
#define TAB_SPECIALKEYS 3

#define DLQ_IMAGE 0
#define DLQ_CSS 1

#define WEB_WIDTH 238

#define MAX_EXT 64
#define MAX_SEARCH_INPUT 514 // 512 + 2 for the character and space
#define MAX_FAVORITES_INPUT 512

typedef struct
{
	u32 pos;
	u32 type;
	bool isDownloading;
} DLQUEUE_TYPE;

typedef struct
{
	u32 position;
	int content;
	char url[512];
	char file[256];
} HISTORY_TYPE;

typedef struct
{
	char triggerChar;
	char url[512];
} SEARCH_TYPE;

typedef struct
{
	int numEntries; // only the first will ever be set
	char caption[64];
	char url[512];
} FAVORITE_TYPE;

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void drawWebBrowserScreens();
extern void initWebBrowser();
extern void webBrowserForward();
extern void webBrowserBack();
extern void freeWebBrowser();
extern void editWebBrowserAction(char c);
extern void setWebState(int s);
extern bool isShowingKeyboard();
extern void webBrowserStop();
extern void doRedirect(char *redir);
extern void webBrowserXButton();
extern bool isFav();
extern bool isShowingImage();
extern void initCapture();
extern void setTemporaryHome(char *tHome);
extern char *getCurrentURL();
extern void constructValidURL(char *tURL, char *outStr);
extern void escapeIllegals(char *str);
extern void setPageFile(char *tSearch);

// click events
extern void webBrowserClick(int px, int py);
extern void webBrowserDrag(int px, int py);
extern void webBrowserLButton();
extern void webBrowserRButton();
extern void webBrowserRelease();
extern void moveWebBrowserCursor(int direction);
extern void recalcTextAreaJump();
extern char webHandleClick(int px, int py);
extern bool isWebKeyboard();
extern void setScrollWeb(int py);
extern int getWebBrowserEntries();
extern char *getPageExtension();
extern void setDownloadFlag();
extern bool isDownloadingFile();

// extra
extern void strccat(char *str, char c);
extern void addURLToCapture(char *url);
extern int setWebBrowserCursor(int u, int v, int cCursor, int maxCur);

// prototypes
extern void loadPage(char *file, bool toHistory);
extern void constructValidURL(char *tURL, char *finalURL);
extern void escapeIllegals(char *str);
extern bool historyIsNext();
extern bool historyIsPrev();
extern void cancelQueuedDownloads();
extern void finishFileDownload();


extern HTML_RENDERED htmlPage;
extern u32 bState;
extern int bPosition;
extern bool keyboardShowing;
extern bool nextClears;
extern bool wbDrag;
extern bool ignoreUntilUp;
extern bool doURL;
extern bool drawURL;
extern char *lastURL;
extern int textLength;
extern bool maskInput;
extern bool alreadyCaptured;
extern char anchor[64];
extern char lastanchor[64];
extern bool dontScan;
extern char curHTMLPage[256];
extern char specialKey[18][8];
extern int curSpecialKey;
extern int specialCountDown;
extern FAVORITE_TYPE *favorites;
extern FAVORITE_TYPE *ircCaptured;
extern int listCursorIrc;
extern bool nextForward;
extern int dlQueueCount;
extern int dlQueuePos;
extern DLQUEUE_TYPE *dlQueue;
extern bool loadHomePage;
extern bool isImage;
extern bool isFileDownload;
extern bool temporaryStart;
extern char curExtension[MAX_EXT];
extern char curFileName[FILENAME_SIZE];
extern uint16 *htmlRender;
extern bool oldImageLandscape;
extern bool needsRedraw;

extern char unreachable[];
extern char incomplete[];
extern char badsearch[];


// for history
extern int historyPos;
extern int historyMaxFake;
extern int historyMax;
extern HISTORY_TYPE *historywebpage;

// for quicksearch
extern int searchMax;
extern SEARCH_TYPE *searchStrings;

#ifdef __cplusplus
}
#endif
