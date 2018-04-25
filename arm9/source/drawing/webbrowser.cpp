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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libfb/libcommon.h>
#include <libpicture.h>
#include "webbrowser.h"
#include "browser.h"
#include "irc.h"
#include "database.h"
#include "viewer.h"
#include "pictureviewer.h"
#include "../sidetabs.h"
#include "../resources.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../font_arial_11.h"
#include "../general.h"
#include "../keyboard.h"
#include "../globals.h"
#include "../graphics.h"
#include "../html.h"
#include "../wifi.h"
#include "../language.h"
#include "../shortcuts.h"
#include "../settings.h"
#include "../controls.h"
#include "../errors.h"
#include "specific_shared.h"
#include "fatwrapper.h"

static HTML_RENDERED htmlPage;
static u32 bState = WEBSTATE_INIT;
static int bPosition = 0;
static bool keyboardShowing = true;
static bool nextClears = false;
static bool wbDrag = false;
static bool ignoreUntilUp = false;
static bool doURL = false;
static bool drawURL = false;
static char *lastURL = NULL;
static int textLength = 0;
static bool maskInput = false;
static bool alreadyCaptured = false;
static char anchor[64];
static char lastanchor[64];
static bool dontScan = false;
static char curHTMLPage[256];
static char specialKey[18][8];
static int curSpecialKey = -1;
static int specialCountDown = 0;
static FAVORITE_TYPE *favorites = NULL;
static FAVORITE_TYPE *ircCaptured = NULL;
static int listCursor = 0;
static bool nextForward = false;
static int dlQueueCount = 0;
static int dlQueuePos = 0;
static DLQUEUE_TYPE *dlQueue = NULL;
static bool loadHomePage = false;
static bool isImage = false;
static bool isFileDownload = false;
static bool temporaryStart = false;
static char curExtension[MAX_EXT];
static char curFileName[FILENAME_SIZE];
static uint16 *htmlRender = NULL;
static bool oldImageLandscape = false;
static bool needsRedraw = false;

static char unreachable[] = { "<html><head><title>Server Unreachable</title></head><body><p>The server was unreachable! This could be due to one of the following reasons:<br /><ul><li>The server is down</li><li>You misspelled the URL</li><li>The WIFI connection is flaky</li></ul></p></body></html>" };
static char incomplete[] = { "<html><head><title>Incomplete Download</title></head><body><p>The server sent an incomplete reply back.  This can usually be fixed by simply refreshing the page.  This can also be caused by file corruption or a bad DLDI driver.</p></body></html>" };
static char badsearch[] = { "<html><head><title>Bad Search Identifier</title></head><body><p>The search identifier you used is not defined.  Please make sure you have correctly configured searchprefs.txt located inside the DSOrganize data directory.</p></body></html>" };

extern bool copying;
extern bool rendering;
extern u32 maxSize;
extern u32 curSize;
extern char downFile[256];
extern DRAGON_FILE *downFP;
extern char *pointedText;
extern uint16 mode;
extern char fileName[256];

extern bool strUpdated;
extern uint16 insert;
extern char curChar;
extern uint16 caps;
extern uint16 editCursor;
extern char *inputBuffer;
extern int textBlock;

// for history
static int historyPos = -1;
static int historyMaxFake = 0;
static int historyMax = 0;
static HISTORY_TYPE *history = NULL;

// for quicksearch
static int searchMax = 0;
static SEARCH_TYPE *searchStrings;

// prototypes
void loadPage(char *file, bool toHistory);
void constructValidURL(char *tURL, char *finalURL);
void escapeIllegals(char *str);
bool historyIsNext();
bool historyIsPrev();
void cancelQueuedDownloads();
extern void drawScrollBarIRC(int pos, int max);
void finishFileDownload();

void mallocHtmlRenderer()
{
	if(htmlRender != NULL)
	{
		// Already malloc'd, don't go again
		return;
	}
	
	// Malloc memory
	setGlobalError(ERROR_WEB_RENDERSPACE);
	htmlRender = (uint16 *)trackMalloc((256*(576 + 16))*2, "temp html render");
	setGlobalError(ERROR_NONE);
}

void freeHtmlRenderer()
{
	if(htmlRender == NULL)
	{
		// Already free'd, don't go again
		return;
	}
	
	// Free memory
	trackFree(htmlRender);	
	htmlRender = NULL;
}

void setTemporaryHome(char *tHome)
{
	strcpy(inputBuffer, tHome);
	temporaryStart = true; 
}

void initCapture()
{
	ircCaptured = NULL;
}

bool isShowingImage()
{
	return isImage;
}

void strccat(char *str, char c)
{
	char tStr[2];
	
	tStr[0] = c;
	tStr[1] = 0;
	
	strcat(str, tStr);
}

void jumpToAnchor(char *toJump)
{
	if(htmlPage.maxURLs == 0)
		return;
	
	strlwr(toJump);
	
	for(int x=0;x<htmlPage.maxURLs;x++)
	{
		if(strcmp(htmlPage.urlCodes[x].name, toJump) == 0)
		{
			bPosition = htmlPage.urlCodes[x].yPos;
			return;
		}
	}
}

void eraseAnchors(char *toErase)
{
	for(int x=strlen(toErase)-1;x>0;x--)
	{
		if(toErase[x] == '/') // no urls
			return;
		
		if(toErase[x] == '#') // found one
		{
			toErase[x] = 0;
			return;
		}
	}
}

void recalcTextAreaJump()
{
	if(controlFocus() && htmlPage.formCodes[controlFocus()-1].whatType == FORM_TEXTAREA)
	{
		// make sure currently typing text is always visible
		
		u32 *skipAmount = (u32 *)htmlPage.formCodes[controlFocus()-1].state;
		int maxLines = (htmlPage.formCodes[controlFocus()-1].height - 3) / 12;
		
		int *pts = NULL;
		int numPts = getWrapPoints(3, 4, pointedText, 0, 0, htmlPage.formCodes[controlFocus()-1].width, htmlPage.formCodes[controlFocus()-1].height, &pts, (uint16 **)font_gautami_10);
		
		*skipAmount = 0;
		if(numPts > maxLines)
		{
			if(pts[numPts-1] < getCursor())
			{
				int tmpPt = numPts - maxLines;
				
				if(tmpPt < 0)
					tmpPt = 0;
				
				*skipAmount = pts[tmpPt];
			}
			else
			{
				for(int i=0;i<numPts;i++)
				{
					if(pts[i] >= getCursor()) // this line is after the cursor
					{
						int tmpPt = i - (maxLines - 1);
						
						if(tmpPt < 0)
							tmpPt = 0;
						
						*skipAmount = pts[tmpPt];
						break;
					}
				}
			}
		}
		
		free(pts);
	}
}

void addToHistory(char *url, char *fName, int content)
{
	if(historyMax == 0)
	{
		historyPos = 0;
		historyMax = 1;
		
		setGlobalError(ERROR_WEB_HISTORY);
		history = (HISTORY_TYPE *)trackMalloc(sizeof(HISTORY_TYPE), "history struct");
		setGlobalError(ERROR_NONE);
		memset(&history[0], 0, sizeof(HISTORY_TYPE)); // just to get the point across
	}
	else
	{
		historyPos++;
		
		if(historyPos == historyMax)
		{
			historyMax++;
			history = (HISTORY_TYPE *)trackRealloc(history, sizeof(HISTORY_TYPE) * historyMax);
		}
		
		memset(&history[historyPos], 0, sizeof(HISTORY_TYPE));
	}	
	
	strcpy(history[historyPos].url, url);
	strcpy(history[historyPos].file, fName);
	history[historyPos].position = 0;
	history[historyPos].content = content;
	
	historyMaxFake = historyPos+1;
}

bool matchesHistory(char *url)
{
	if(historyMax == 0)
		return false;
	
	if(strcmp(history[historyPos].url, url) == 0)
		return true;
	
	return false;
}

void historyBack()
{
	if(!historyIsPrev())
		return;
	
	history[historyPos].position = bPosition;
	
	historyPos--;
	
	memset(inputBuffer, 0, MAX_URL+1);
	strcpy(inputBuffer, history[historyPos].url);
	strcpy(lastURL, inputBuffer);
	eraseAnchors(lastURL);
	strUpdated = true;	
	
	setContentType(history[historyPos].content);
	loadPage(history[historyPos].file, false);
	bPosition = history[historyPos].position;
	
	clearFocus();					
	
	pointedText = inputBuffer;
	textLength = MAX_URL;
}

void historyForward()
{
	if(!historyIsNext())
		return;
	
	history[historyPos].position = bPosition;
	
	historyPos++;
	
	memset(inputBuffer, 0, MAX_URL+1);
	strcpy(inputBuffer, history[historyPos].url);
	strcpy(lastURL, inputBuffer);
	eraseAnchors(lastURL);
	strUpdated = true;	
	
	setContentType(history[historyPos].content);
	loadPage(history[historyPos].file, false);
	bPosition = history[historyPos].position;
	
	clearFocus();					
	
	pointedText = inputBuffer;
	textLength = MAX_URL;
}

bool historyIsNext()
{
	if(historyPos < historyMaxFake - 1)
		return true;
	
	return false;
}

bool historyIsPrev()
{
	if(historyPos > 0)
		return true;
	
	return false;
}

void clearHistory()
{
	if(history)
		trackFree(history);
	
	history = NULL;
	historyPos = -1;
	historyMax = 0;
	historyMaxFake = 0;
}

char quickLower(char c)
{
	if(c >= 'A' && c <= 'Z')
		return c + ('a' - 'A');
	
	return c;
}

void loadSearchPreferences()
{
	searchStrings = NULL;
	searchMax = 0;
	
	if(DRAGON_FileExists("searchprefs.txt") != FT_FILE)
		return;
	
	DRAGON_FILE *df = NULL;
	
	if(debug_FileExists((const char*)"searchprefs.txt",35) == FT_FILE){
		df = DRAGON_fopen("searchprefs.txt", "r");	//debug_FileExists index: 35
	}
	while(!DRAGON_feof(df))
	{
		setGlobalError(ERROR_WEB_SEARCHTEMP);
		char *tStr = (char *)safeMalloc(MAX_SEARCH_INPUT + 1);
		setGlobalError(ERROR_NONE);
		
		DRAGON_fgets(tStr, MAX_SEARCH_INPUT, df);
		
		if(tStr[1] == ' ') // validate it
		{
			searchMax ++;
			
			setGlobalError(ERROR_WEB_SEARCHPREF);
			
			if(!searchStrings)
				searchStrings = (SEARCH_TYPE *)trackMalloc(sizeof(SEARCH_TYPE), "search strings");
			else
				searchStrings = (SEARCH_TYPE *)trackRealloc(searchStrings, sizeof(SEARCH_TYPE) * searchMax);
			
			setGlobalError(ERROR_NONE);
			
			memset(&searchStrings[searchMax-1], 0, sizeof(SEARCH_TYPE));
			
			searchStrings[searchMax-1].triggerChar = quickLower(tStr[0]);
			strcpy(searchStrings[searchMax-1].url, tStr+2);
		}
		
		free(tStr);
	}
	
	DRAGON_fclose(df);
}

void loadSpecialKeys()
{
	memset(specialKey, 0, 18*8);
	
	if(DRAGON_FileExists("specialkeys.txt") != FT_FILE)
		return;
	
	DRAGON_FILE *df = NULL;
	if(debug_FileExists((const char*)"specialkeys.txt",36) == FT_FILE){
		df = DRAGON_fopen("specialkeys.txt", "r");	//debug_FileExists index: 36
	}
	
	int numKeys = 0;
	
	while(!DRAGON_feof(df) && numKeys < 18)
	{
		char tStr[64];
		
		DRAGON_fgets(tStr, 63, df);
		strncpy(specialKey[numKeys], tStr, 7);
		
		numKeys++;
	}
	
	DRAGON_fclose(df);
}

void loadFavorites()
{
	favorites = NULL;
	
	if(DRAGON_FileExists("favorites.txt") != FT_FILE)
		return;
	
	DRAGON_FILE *df = NULL;
	
	if(debug_FileExists((const char*)"favorites.txt",37) == FT_FILE){
		df = DRAGON_fopen("favorites.txt", "r");	//debug_FileExists index: 37
	}
	while(!DRAGON_feof(df))
	{
		if(!favorites)
		{
			setGlobalError(ERROR_WEB_FAVORITES);
			favorites = (FAVORITE_TYPE *)trackMalloc(sizeof(FAVORITE_TYPE), "favorites");
			setGlobalError(ERROR_NONE);
			
			memset(favorites, 0, sizeof(FAVORITE_TYPE));
			favorites->numEntries = 1;
		}
		else
		{
			favorites->numEntries++;
			favorites = (FAVORITE_TYPE *)trackRealloc(favorites, sizeof(FAVORITE_TYPE) * favorites->numEntries);
			memset(&favorites[favorites->numEntries-1], 0, sizeof(FAVORITE_TYPE));
		}
		
		setGlobalError(ERROR_WEB_FAVORITESTEMP);
		char *tStr = (char *)safeMalloc(MAX_FAVORITES_INPUT + 1);
		setGlobalError(ERROR_NONE);
		
		DRAGON_fgets(tStr, MAX_FAVORITES_INPUT, df);
		strncpy(favorites[favorites->numEntries-1].caption, tStr, 63);
		DRAGON_fgets(tStr, MAX_FAVORITES_INPUT, df);
		strncpy(favorites[favorites->numEntries-1].url, tStr, 511);
		
		free(tStr);
	}
	
	DRAGON_fclose(df);
}

void bookmarkCurrent()
{
	// appends the current file and title to the favorites.txt database and addds to the end of the favorites list.
	
	if(!isLoaded(&htmlPage))
	{
		// exit if nothing loaded
		return;
	}
	
	if(!favorites)
	{
		setGlobalError(ERROR_WEB_ADDFAVORITES);
		favorites = (FAVORITE_TYPE *)trackMalloc(sizeof(FAVORITE_TYPE), "favorites");
		setGlobalError(ERROR_NONE);
		
		memset(favorites, 0, sizeof(FAVORITE_TYPE));
		favorites->numEntries = 1;
	}
	else
	{
		favorites->numEntries++;
		favorites = (FAVORITE_TYPE *)trackRealloc(favorites, sizeof(FAVORITE_TYPE) * favorites->numEntries);
		memset(&favorites[favorites->numEntries-1], 0, sizeof(FAVORITE_TYPE));
	}
	
	if(strlen(htmlPage.title) == 0)
	{
		strncpy(favorites[favorites->numEntries-1].caption, inputBuffer, 63);
	}
	else
	{
		strncpy(favorites[favorites->numEntries-1].caption, htmlPage.title, 63);
	}
	
	strncpy(favorites[favorites->numEntries-1].url, inputBuffer, 511);
	
	DRAGON_FILE *df = NULL;
	
	if(debug_FileExists((const char*)"favorites.txt",38) == FT_FILE){
		df = DRAGON_fopen("favorites.txt", "w");	//debug_FileExists index: 38
	}
	
	for(int i=0;i < favorites->numEntries;i++)
	{
		DRAGON_fputs(favorites[i].caption, df);
		DRAGON_fputs("\r\n", df);
		DRAGON_fputs(favorites[i].url, df);
		DRAGON_fputs("\r\n", df);
	}
	
	DRAGON_fclose(df);
	
	setSideTabEnabled(TAB_FAVORITES, true);
}

void addURLToCapture(char *url)
{
	if(!ircCaptured)
	{
		setGlobalError(ERROR_WEB_CAPTURED);
		ircCaptured = (FAVORITE_TYPE *)trackMalloc(sizeof(FAVORITE_TYPE), "irc captured");
		setGlobalError(ERROR_NONE);
		
		memset(ircCaptured, 0, sizeof(FAVORITE_TYPE));
		ircCaptured->numEntries = 1;
	}
	else
	{
		for(int i=0;i<ircCaptured->numEntries;i++)
		{	
			if(strcmp(url, ircCaptured[i].url) == 0) // exit if the url is the same
				return;
		}
		
		ircCaptured->numEntries++;
		ircCaptured = (FAVORITE_TYPE *)trackRealloc(ircCaptured, sizeof(FAVORITE_TYPE) * ircCaptured->numEntries);
		memset(&ircCaptured[ircCaptured->numEntries-1], 0, sizeof(FAVORITE_TYPE));
	}

	strncpy(ircCaptured[ircCaptured->numEntries-1].url, url, 511);
	
	char *tURL = ircCaptured[ircCaptured->numEntries-1].url;
	
	if(tURL[strlen(tURL)-1] == '.')
		tURL[strlen(tURL)-1] = 0; // kill trailing . for periods at the end of sentences
}

void initWebBrowser()
{
	// clear cache
	deleteFAT((char*)getDefaultDSOrganizeCacheFolder("").c_str(), false);
	
	loadSearchPreferences();
	loadSpecialKeys();
	loadFavorites();
	
	initSideTabs();
	
	addSideTab(TAB_KEYBOARD, tab_keyboard);
	addSideTab(TAB_SPECIALKEYS, tab_special);
	addSideTab(TAB_FAVORITES, tab_favorites);
	addSideTab(TAB_IRCURLS, tab_irccapture);
	
	setDefaultSideTab(TAB_KEYBOARD);
	
	if(!ircCaptured)
		setSideTabEnabled(TAB_IRCURLS, false);
	
	if(!favorites)
		setSideTabEnabled(TAB_FAVORITES, false);
	
	setGlobalError(ERROR_WEB_INPUTBUF);
	inputBuffer = (char *)trackMalloc(MAX_URL+1, "inbuffer");
	lastURL = (char *)trackMalloc(MAX_URL+1,"lasturl");
	setGlobalError(ERROR_NONE);
	
	memset(anchor, 0, 64);
	memset(lastanchor, 0, 64);
	strcpy(inputBuffer, "about:blank");
	memset(&htmlPage, 0, sizeof(HTML_RENDERED));
	memset(curExtension, 0, MAX_EXT);
	memset(curFileName, 0, FILENAME_SIZE);
	
	clearFocus();
	
	bState = WEBSTATE_INIT;
	keyboardShowing = true;
	strUpdated = false;
	nextClears = true;	
	htmlPage.height = -1;
	ignoreUntilUp = false;
	doURL = false;
	drawURL = false;
	maskInput = false;
	alreadyCaptured = false;
	dontScan = false;
	loadHomePage = false;
	isImage = false;
	isFileDownload = false;
	needsRedraw = false;
	
	oldImageLandscape = isImageLandscape();
	
	// modify this to give landscape to browser
	if(isImageLandscape() != true)
	{
		toggleImageLandscape();
	}
	
	dlQueue = NULL;
	history = NULL;
	pointedText = inputBuffer;
	textLength = MAX_URL;
	
	curSpecialKey = -1;
	specialCountDown = 0;
}

void setWebState(int s)
{
	if(bState == WEBSTATE_DOWNLOADING)
	{
		switch(s)
		{
			case 0:
				bState = WEBSTATE_FINISHED;
				
				if(isFileDownload)
				{
					bState = WEBSTATE_CONNECTED;
					finishFileDownload();
				}
				
				break;
			case 1:
				bState = WEBSTATE_FAILEDSERVER;
				break;
		}
	}
}

void addCSS()
{
	// check if we have a css source
	
	if(htmlPage.cssLocation == NULL)
	{
		fixBadColor(&htmlPage);
		
		return;
	}
	
	if(!dlQueue)
	{
		setGlobalError(ERROR_WEB_DLQUEUECSS);
		dlQueue = (DLQUEUE_TYPE *)trackMalloc(sizeof(DLQUEUE_TYPE), "dl queue");
		setGlobalError(ERROR_NONE);
		memset(dlQueue, 0, sizeof(DLQUEUE_TYPE));
		dlQueueCount = 1;
	}
	else
	{
		dlQueueCount++;
		dlQueue = (DLQUEUE_TYPE *)trackRealloc(dlQueue, sizeof(DLQUEUE_TYPE) * dlQueueCount);
		memset(&dlQueue[dlQueueCount - 1], 0, sizeof(DLQUEUE_TYPE));
	}
	
	dlQueue[dlQueueCount - 1].type = DLQ_CSS;
	dlQueue[dlQueueCount - 1].pos = 0; // don't need this
	dlQueue[dlQueueCount - 1].isDownloading = false;
}

void identifyImages()
{
	for(int x=0;x<htmlPage.maxFormControls;++x)
	{
		if(htmlPage.formCodes[x].formID == DUMMYFORM_IMAGES && htmlPage.formCodes[x].whatType == FORM_IMAGE)
		{	
			if(!dlQueue)
			{
				setGlobalError(ERROR_WEB_DLQUEUEIMG);
				dlQueue = (DLQUEUE_TYPE *)trackMalloc(sizeof(DLQUEUE_TYPE), "dl queue");
				setGlobalError(ERROR_NONE);
				memset(dlQueue, 0, sizeof(DLQUEUE_TYPE));
				dlQueueCount = 1;
			}
			else
			{
				dlQueueCount++;
				dlQueue = (DLQUEUE_TYPE *)trackRealloc(dlQueue, sizeof(DLQUEUE_TYPE) * dlQueueCount);
				memset(&dlQueue[dlQueueCount - 1], 0, sizeof(DLQUEUE_TYPE));
			}
			
			dlQueue[dlQueueCount - 1].type = DLQ_IMAGE;
			dlQueue[dlQueueCount - 1].pos = x;
			dlQueue[dlQueueCount - 1].isDownloading = false;
		}
	}
	
	dlQueuePos = 0;
}

bool isQueued()
{
	if(!dlQueue)
		return false;
	
	return true;
}

void memcpyimg(uint16 *dst, uint16 *src, u32 size)
{
	for(u32 i=0;i<(size>>1);i++)
	{
		if(*src == 0)
		{
			*dst++ = BIT(15) | 1; // for no transparent
			src++;
		}
		else
			*dst++ = *src++;
	}	
}

void freeDLQueue()
{
	if(dlQueue)
		trackFree(dlQueue);
	
	dlQueue = NULL;
	dlQueueCount = 0;
}

void checkDLQueue()
{	
	if(!dlQueue)
		return;
	
	if(!isDownloading()) // no current download
	{			
		if(dlQueue[dlQueuePos].isDownloading) // we were fetching this one
		{
			dlQueue[dlQueuePos].isDownloading = false;
			
			switch(dlQueue[dlQueuePos].type)
			{
				case DLQ_IMAGE:
				{
					// render image
					setGlobalError(ERROR_WEB_RENDERIMAGE);
					
					PICTURE_DATA tPicture;
					memset(&tPicture, 0, sizeof(PICTURE_DATA));
					setBGColor(getBGColor(&htmlPage), &tPicture);	
					
					disableGIFFileStreaming();
					setLargeDimensions(htmlPage.formCodes[dlQueue[dlQueuePos].pos].width,htmlPage.formCodes[dlQueue[dlQueuePos].pos].height); // normal thumbnail
					loadImage(downFile, &tPicture, htmlPage.formCodes[dlQueue[dlQueuePos].pos].width, htmlPage.formCodes[dlQueue[dlQueuePos].pos].height);
					
					IMAGE_TYPE *tImg = (IMAGE_TYPE *)htmlPage.formCodes[dlQueue[dlQueuePos].pos].state;
					
					tImg->state = IMAGE_CANTLOAD; // default
					tImg->aniData.aniDelays = NULL; // to ensure proper memory freeing
					tImg->aniData.curAni = 0;
					tImg->aniData.maxAni = 0;
					
					if(tPicture.error)
					{
						if(tPicture.max_X == 1 && tPicture.max_Y == 1) // special case for spacer pictures, create blank space here
						{
							// start with enough room for 1bpp
							u32 imageSize = ((htmlPage.formCodes[dlQueue[dlQueuePos].pos].width * htmlPage.formCodes[dlQueue[dlQueuePos].pos].height) >> 3) + 1;
							
							// round to the next u16
							if(imageSize & 0x01)
							{
								imageSize++;
							}
							
							// add in header size
							imageSize += 10;
							
							// create temporary sprite
							disableHalt();
							tImg->graphicsData = (uint16 *)trackMalloc(imageSize, "picture");
							
							if(tImg->graphicsData != NULL)
							{							
								// make sure it's set to the default color
								memset(tImg->graphicsData, 0, imageSize);
								
								// set up the header
								tImg->graphicsData[0] = 0xFFFF;
								tImg->graphicsData[1] = htmlPage.formCodes[dlQueue[dlQueuePos].pos].width - 1;
								tImg->graphicsData[2] = htmlPage.formCodes[dlQueue[dlQueuePos].pos].height - 1;
								tImg->graphicsData[3] = 1;
								tImg->graphicsData[4] = htmlPage.colors[C_BG];
								
								// make sure it renders
								tImg->state = IMAGE_READYFORDISPLAY;
							}
						}
					}
					else
					{
						if(tPicture.picType == PIC_JPEG || tPicture.picType == PIC_PNG || tPicture.picType == PIC_BMP || tPicture.picType == PIC_GIF)
						{	
							if(tPicture.picData != NULL)
							{								
								// sanity check
								if(tPicture.picData[0] + 1 <= 256 && tPicture.picData[1] + 1 <= 384)
								{
									// set width so browser knows proper wrapping
									htmlPage.formCodes[dlQueue[dlQueuePos].pos].width = tPicture.picData[0] + 1;
									htmlPage.formCodes[dlQueue[dlQueuePos].pos].height = tPicture.picData[1] + 1;
									
									u32 size = (((tPicture.picData[0] + 1) * (tPicture.picData[1] + 1)) * 2) + 4; // just in case the size is different than stated
									
									disableHalt();
									tImg->graphicsData = (uint16 *)trackMalloc(size, "picture");
									
									if(tImg->graphicsData)
									{					
										memcpyimg(tImg->graphicsData, tPicture.picData, size);
										
										tImg->state = IMAGE_READYFORDISPLAY;
									}
								}
							}
						}
						
						if(tPicture.picType == PIC_ANI)
						{
							if(tPicture.aniPointers != NULL)
							{
								// handle setup for anigif here
								uint16 *curSprite = (uint16 *)(tPicture.aniPointers[0].data);	
								u32 size = (((curSprite[0] + 1) * (curSprite[1] + 1)) * 2) + 4; // just in case the size is different than stated
								
								// sanity check
								if(curSprite[0] + 1 <= 256 && curSprite[1] + 1 <= 384)
								{
									// set width so browser knows proper wrapping							
									htmlPage.formCodes[dlQueue[dlQueuePos].pos].width = curSprite[0] + 1;
									htmlPage.formCodes[dlQueue[dlQueuePos].pos].height = curSprite[1] + 1;
									
									disableHalt();
									tImg->graphicsData = (uint16 *)trackMalloc(size * tPicture.aniCount, "picture");
									
									if(tImg->graphicsData)
									{
										disableHalt();
										tImg->aniData.aniDelays = (int *)trackMalloc(sizeof(int) * tPicture.aniCount, "ani data");
										
										if(tImg->aniData.aniDelays)
										{
											memcpy(tImg->aniData.aniDelays, tPicture.aniDelays, sizeof(int) * tPicture.aniCount);
											// Copy all images to single range
											for(int whichImage=0;whichImage<tPicture.aniCount;whichImage++)
											{
												curSprite = (uint16 *)(tPicture.aniPointers[whichImage].data);
												memcpyimg(tImg->graphicsData + ((size >> 1) * whichImage), curSprite, size);
											}
											
											tImg->state = IMAGE_READYFORDISPLAY;
											tImg->aniData.curAni = 0;
											tImg->aniData.maxAni = tPicture.aniCount;
											tImg->aniData.vBlankStart = getAniHeartBeat();
										}
										else
										{
											trackFree(tImg->graphicsData);
											tImg->graphicsData = 0;
										}
									}
								}
							}
						}
					}
					
					freeImage(&tPicture);
					
					setGlobalError(ERROR_NONE);
					
					break;
				}
				case DLQ_CSS:
				{	
					DRAGON_FILE *df = NULL;
					
					if(debug_FileExists((const char*)downFile,39) == FT_FILE){
						df = DRAGON_fopen(downFile, "r");	//debug_FileExists index: 39
					}
					
					u32 fLen = DRAGON_flength(df);
					
					if(fLen > 0)
					{
						disableHalt();
						char *tCSS = (char *)safeMalloc(fLen + 1);
						
						if(tCSS)
						{
							DRAGON_fread(tCSS, 1, fLen, df);
							
							parseCSS(tCSS, &htmlPage);
							
							free(tCSS);
						}
					}
					
					fixBadColor(&htmlPage);
					
					DRAGON_fclose(df);
					
					break;
				}
			}
			
			dlQueuePos++;
			
			if(dlQueuePos >= dlQueueCount)
			{
				freeDLQueue();
				
				return;
			}
		}
		
		char *tURL = NULL;
		char tEXT[64];
		memset(tEXT, 0, 64);
		
		switch(dlQueue[dlQueuePos].type)
		{
			case DLQ_IMAGE:				
			{	
				disableHalt();
				tURL = (char *)safeMalloc(MAX_URL+1);
				
				if(!tURL)
				{
					// no more memory to load anything, back out of the image queue
					freeDLQueue();
					
					return;
				}
				
				memcpy(tURL, inputBuffer, MAX_URL+1);
				
				IMAGE_TYPE *tImg = (IMAGE_TYPE *)htmlPage.formCodes[dlQueue[dlQueuePos].pos].state;
				
				disableHalt();
				char *tData = (char *)safeMalloc(tImg->url.endPos - tImg->url.startPos + 1);
				
				if(!tData)
				{
					// no more memory to load anything, back out of the image queue
					
					freeDLQueue();
					
					return;
				}
				
				getURLFromPosition(curHTMLPage, tImg->url, tData);
				
				constructValidURL(tData, tURL);
				free(tData);
				
				escapeIllegals(tURL);
				
				// get extension
				
				for(int gEXT=strlen(tURL);gEXT>0;gEXT--)
				{
					switch(tURL[gEXT])
					{
						case '.':
						{
							for(u32 fEXT=gEXT+1;fEXT<strlen(tURL);fEXT++)
							{
								tEXT[fEXT-(gEXT+1)] = tURL[fEXT];
								
								if(strlen(tEXT) == 63)
								{
									break;
								}
							}
							
							goto exitFor;
						}
						case '\\':
						case '/':
						case ':':
						case '*':
						case '?':
						case '\"':
						case '<':
						case '>':
						case '|':
						{
							// found illegal char, no extension
							goto exitFor;
						}
					}
					
					if((strlen(tURL) - gEXT) >= 64)
					{
						// we overran how big the buffer can be, assume no ext;
						break;
					}
				}
exitFor:		// because you cant break out of a for while in a switch
				
				if(strlen(tEXT) == 0)
				{
					strcpy(tEXT, "JPG"); // try an extension
				}
				
				// Ensure that the extension is uppercase and shortest possible
				strupr(tEXT);
				
				// special case
				if(strcmp(tEXT, "JPEG") == 0)
				{
					// No need to force lfn
					strcpy(tEXT, "JPG");
				}
				
				break;
			}
			case DLQ_CSS:
			{
				if(htmlPage.cssLocation) // in case it wasn't filled
				{	
					disableHalt();
					tURL = (char *)safeMalloc(MAX_URL+1);
					
					if(!tURL)
					{
						// no more memory to load anything, back out of the image queue						
						freeDLQueue();
						
						return;
					}
					
					memcpy(tURL, inputBuffer, MAX_URL+1);
					
					constructValidURL(htmlPage.cssLocation, tURL);
					
					free(htmlPage.cssLocation);
					htmlPage.cssLocation = NULL;
					
					escapeIllegals(tURL);
					strcpy(tEXT, "TMP");
				}
				
				break;
			}
		}
		
		if(tURL != NULL)
		{	
			// force 8.3 for most cases
			tEXT[3] = 0;
			strupr(tEXT);
			
			std::string PathFix = getDefaultDSOrganizeCachePath(std::to_string((u32)CalcCRC32(tURL)) +"."+ string(tEXT));
			
			if(debug_FileExists((const char*)PathFix.c_str(),40) == FT_FILE){
				downFP = DRAGON_fopen(PathFix.c_str(), "w+");	//debug_FileExists index: 40
				resetRCount();
				
				setReferrer(lastURL);
				getFile(tURL, res_bAgent);
			}
			
			dlQueue[dlQueuePos].isDownloading = true;
			
			free(tURL);
		}
	}
	else // just do the loop
	{
		checkFile();
	}
}

void freeLastPage()
{
	freeHTML(&htmlPage);
}

void freeWebBrowser()
{
	if(isDownloading())
		cancelDownload();
	
	disconnectWifi();
	freeHtmlRenderer();
	freeWifiMem();
	freeLastPage();
	exitPictureScreen();
	freeDLQueue();
	clearHistory();
	destroySideTabs();
	
	if(isImageLandscape() != oldImageLandscape)
	{
		toggleImageLandscape();
	}
	
	if(searchStrings)
		trackFree(searchStrings);
	
	searchStrings = NULL;
	
	if(favorites)
		trackFree(favorites);
	
	favorites = NULL;
	
	if(lastURL)
		trackFree(lastURL);
	
	lastURL = NULL;
	
	if(inputBuffer)
		trackFree(inputBuffer);
	
	inputBuffer = NULL;
	
	clearFocus();
	
	// clear cache
	deleteFAT((char*)getDefaultDSOrganizeCacheFolder("").c_str(), false);	
	
}

void loadPage(char *file, bool toHistory)
{	
	freeLastPage();
	exitPictureScreen();
	freeDLQueue();
	memset(&htmlPage, 0, sizeof(HTML_RENDERED));
	htmlPage.height = -1;
	
	dlQueueCount = 0;
	
	strlwr(file);
	isImage = false;
	
	if(toHistory)
	{
		if(!matchesHistory(inputBuffer)) // don't add refreshes
			addToHistory(inputBuffer, file, getLastContentType());
	}
	
	if(getLastContentType() == TYPE_IMAGE)
	{
		strcpy(curHTMLPage, file);
		strcpy(fileName, file);
		
		isImage = true;
		return;
	}
	
	if(strncmp(file, "about:", 6) == 0)
	{	
		isImage = false;
		
		if(strcmp(file, "about:blank") == 0)
			return;
			
		if(strcmp(file, "about:unreachable") == 0)
		{			
			setHTMLWidth(WEB_WIDTH, &htmlPage);
			setContent(TYPE_HTML, &htmlPage);
			setEncoding(CHARSET_ASCII, &htmlPage);
			
			char *tSpace = (char *)trackMalloc(strlen(unreachable) + 2048 + 1, "tmp render");
			
			memset(tSpace + 2048, 0, strlen(unreachable) + 1);
			memset(tSpace, ' ', 2048);
			strcpy(tSpace + 2048, unreachable);
			
			loadHTMLFromMemory(tSpace, &htmlPage);
			
			bPosition = 0;
			return;	
		}
		
		if(strcmp(file, "about:incomplete") == 0)
		{
			setHTMLWidth(WEB_WIDTH, &htmlPage);
			setContent(TYPE_HTML, &htmlPage);
			setEncoding(CHARSET_ASCII, &htmlPage);
			
			char *tSpace = (char *)trackMalloc(strlen(incomplete) + 2048 + 1, "tmp render");
			
			memset(tSpace + 2048, 0, strlen(incomplete) + 1);
			memset(tSpace, ' ', 2048);
			strcpy(tSpace + 2048, incomplete);
			
			loadHTMLFromMemory(tSpace, &htmlPage);
			
			bPosition = 0;
			return;	
		}	
		
		if(strcmp(file, "about:badsearch") == 0)
		{
			setHTMLWidth(WEB_WIDTH, &htmlPage);
			setContent(TYPE_HTML, &htmlPage);
			setEncoding(CHARSET_ASCII, &htmlPage);
			
			char *tSpace = (char *)trackMalloc(strlen(badsearch) + 2048 + 1, "tmp render");
			
			memset(tSpace + 2048, 0, strlen(badsearch) + 1);
			memset(tSpace, ' ', 2048);
			strcpy(tSpace + 2048, badsearch);
			
			loadHTMLFromMemory(tSpace, &htmlPage);
			
			bPosition = 0;
			return;	
		}	
		
		return;
	}
	
	DRAGON_FILE *tFile = NULL;
	if(debug_FileExists((const char*)file,41) == FT_FILE){
		tFile = DRAGON_fopen(file, "r");	//debug_FileExists index: 41
	}
	
	if(DRAGON_flength(tFile) == 0)
	{
		DRAGON_fclose(tFile);
		loadPage("about:incomplete", false);
		
		return;
	}
	DRAGON_fclose(tFile);
	
	copying = true;
	curSize = 0;
	maxSize = 1;
	
	setProgressVars(&curSize, &maxSize, &htmlPage);			
	setHTMLWidth(WEB_WIDTH, &htmlPage);
	setEncoding(getLastCharset(), &htmlPage);
	setContent(getLastContentType(), &htmlPage);
	
	strcpy(curHTMLPage, file);
	
	enableURLS();
	loadHTMLFromFile(file, &htmlPage);
	
	copying = false;
	rendering = true;
	
	disableURLS();
	addCSS();
	identifyImages();
	
	bPosition = 0;
	
	if(strlen(anchor) > 0)
		jumpToAnchor(anchor);
	
	rendering = false;
}

void doRedirect(char *redir)
{
	if(dlQueueCount > 0)
	{
		// don't show redirects when loading advertisements, etc
		return;
	}
	
	strcpy(inputBuffer, redir);
	strcpy(lastURL, redir);
	eraseAnchors(lastURL);
	strUpdated = true;
	
	setPageFile(inputBuffer);
}

char *getCurrentURL()
{
	return inputBuffer;
}

void setDownloadFlag()
{
	isFileDownload = true;
}

void resetFileDownload()
{
	isFileDownload = false;
	
	memset(inputBuffer, 0, MAX_URL+1);
	strcpy(inputBuffer, history[historyPos].url);
	strcpy(lastURL, inputBuffer);
	strUpdated = true;	
	
	setContentType(history[historyPos].content);
	
	clearFocus();					
	
	pointedText = inputBuffer;
	textLength = MAX_URL;
}

void finishFileDownload()
{
	// Move file to proper download location here
	char tFile[256];
	sprintf(tFile, "%s%s", getDownloadDir(), curFileName);
	
	DRAGON_rename(downFile, tFile);
	
	resetFileDownload();
}

bool isDownloadingFile()
{
	return isFileDownload;
}

char *getPageExtension()
{
	return curExtension;
}

void drawDownloadScreen()
{
	drawHome();
	
    char statusStr[64];
	
	sprintf(statusStr, "%s:", l_status);
	
	drawButtonTexts(NULL, l_cancel);
		
	setFont((uint16 **)font_arial_9);
	setColor(genericTextColor);
	bg_dispString(10,20, statusStr);
	setBold(true);			
	bg_dispString(50,20,getDownStatus());
	setBold(false);
	
	runSpeed();
}

void setPageFile(char *tSearch)
{
	int endPos = strlen(tSearch);
	
	if(endPos == 0)
	{
		// No extension or filename
		strcpy(curExtension, "");
		strcpy(curFileName, "");
		
		return;
	}
	
	if(strchr(tSearch, '?') != NULL)
	{
		// Set the ending to the questionmark instead of the null
		endPos = strchr(tSearch, '?') - tSearch;
	}
	
	int curPos = endPos;
	bool foundExtension = false;
	
	while(curPos >= 0)
	{
		if(tSearch[curPos] == '/')
		{
			// Found slash before extension, so no ext
			if(!foundExtension)
			{
				strcpy(curExtension, "");
			}
			
			// Lets copy the rest of the filename
			int fileLen = endPos - (curPos + 1);
			
			if(fileLen >= FILENAME_SIZE)
			{
				// Too long, so no ext
				strcpy(curFileName, "untitled");
				
				if(strlen(curExtension) > 0)
				{
					strcat(curFileName, ".");
					strcat(curFileName, curExtension);
				}
				
				return;
			}
			
			memset(curFileName, 0, FILENAME_SIZE);
			strncpy(curFileName, tSearch + curPos + 1, fileLen);
			
			return;
		}
		
		if(tSearch[curPos] == '.' && !foundExtension)
		{
			// Found an extension marker
			int extLen = endPos - (curPos + 1);
			
			if(extLen >= MAX_EXT)
			{
				// Too long, so no ext
				strcpy(curExtension, "");
				
				return;
			}
			
			memset(curFileName, 0, MAX_EXT);
			strncpy(curExtension, tSearch + curPos + 1, extLen);
			strlwr(curExtension);
			
			foundExtension = true;
		}
		
		curPos--;
	}
	
	// Never found a dot, so no extension
	strcpy(curExtension, "");
	strcpy(curFileName, "");
	
	return;
}

/* This function will strip down a url to the 
   last found slash.  Fixed on 10/20/07 to 
   start before the ? character as that denotes
   inputs passed to the page. */
void stripToLastSlash(char *tSlash)
{
	int limit; // Limit to where it can search backards
	
	if(tSlash[4] == ':')
	{
		// http://
		limit = 7;
	}
	else
	{
		// https://
		limit = 8;
	}
	
	if((int)strlen(tSlash) <= limit)
		return;
	
	int x = limit;
	int y = 0;
	
	while(tSlash[x] != 0 && tSlash[x] != '?')
	{
		if(tSlash[x] == '/')
			y = x;
		
		x++;
	}
	
	if(y > 0)
		tSlash[y+1] = 0;
}

void stripToFirstSlash(char *tSlash)
{
	int limit; // Limit to where it can search backards
	
	if(tSlash[4] == ':')
	{
		// http://
		limit = 7;
	}
	else
	{
		// https://
		limit = 8;
	}
	
	if((int)strlen(tSlash) <= limit)
		return;
	
	int x = limit;
	
	while(tSlash[x] != 0)
	{
		if(tSlash[x] == '/')
		{
			tSlash[x+1] = 0;
			return;
		}
		
		x++;
	}
}

// only escapes spaces for now because i think that's all it should need.
// always operates on the large size so its safe to assume virtuall infinite
void escapeIllegals(char *str)
{
	int curLen = strlen(str);
	
	for(int y = (int)strlen(str);y >= 0;--y)
	{	
		if(str[y] == 0x20)
		{
			// Move all memory forward two places and insert space
			memmove(str + y + 2, str + y, curLen - y);
			
			// Insert the space char
			str[y] = '%';
			str[y+1] = '2';
			str[y+2] = '0';
			
			// make sure to reflect that the string is now two larger
			curLen += 2;
		}
	}
}

void strescapecat(char *dst, char *src)
{	
	for(uint y=0;y<strlen(src);++y)
	{	
		switch(src[y])
		{
			case 0x20:
				strcat(dst, "+");
				break;
			case 0x22:
			case 0x3C:
			case 0x3E:
			case 0x23:
			case 0x25:
			case 0x7B:
			case 0x7D:
			case 0x7C:
			case 0x5C:
			case 0x5E:
			case 0x7E:
			case 0x5B:
			case 0x5D:
			case 0x60:
			case 0x24:
			case 0x26:
			case 0x2B:
			case 0x2C:
			case 0x2F:
			case 0x3A:
			case 0x3B:
			case 0x3D:
			case 0x3F:
			case 0x40:{
				char tStr[20];
				
				sprintf(tStr, "%c%02x", '%', src[y]);
				strcat(dst, tStr);
			}
				break;
			default:
				strccat(dst, src[y]);
				break;
		}
	}
}

/* constructValidURL will take a url as passed from a link
   and the last url parsed as inputs.  If the current url
   is fully qualified, then it will just paste that to the
   final url.  If not, it uses the last url as a base point
   to figure out recursive directories.

 tURL: Url from link that needs parsing/fixing
 finalURL: initially contains last url, will finally
   contain the corrected url link */
void constructValidURL(char *tURL, char *finalURL)
{
	setGlobalError(ERROR_WEB_CREATEURL);
	char *tStr = (char *)safeMalloc(MAX_URL+1);
	setGlobalError(ERROR_NONE);
	
	strcpy(tStr, tURL);
	strlwr(tStr);
	
	if(strncmp(tStr, "http://", 7) == 0)
	{
		strcpy(finalURL, tURL);
		
		free(tStr);
		return;
	}
	
	if(strncmp(tStr, "https://", 8) == 0)
	{
		strcpy(finalURL, tURL);
		
		free(tStr);
		return;
	}
	
	if(tStr[0] == '#')
	{
		// its an anchor
		strcpy(finalURL, lastURL);
		strcat(finalURL, tURL);
		
		free(tStr);
		return;
	}
	
	if(tStr[0] == '/') // root
	{
		if(tStr[1] == '/')
		{
			// slashdot tard-url
			
			strcpy(tStr, "http:");
			strcat(tStr, tURL);
			
			memset(finalURL, 0, MAX_URL+1);
			strcpy(finalURL, tStr);
		}
		else
		{		
			stripToFirstSlash(finalURL);
			strcat(finalURL, tURL+1);
		}
		
		free(tStr);
		return;
	}
	
	if(tStr[0] == '.') // either current or up one
	{
		if(tStr[1] == '.' && tStr[2] == '/') // up one
		{
			stripToLastSlash(finalURL);
			
			int x = strlen(finalURL)-1;
			
			if(finalURL[x] == '/')
			{
				finalURL[x] = 0;
				stripToLastSlash(finalURL);
			}
			
			strcat(finalURL, tURL+3);
			
			free(tStr);
			return;
		}
		
		if(tStr[1] == '/')
		{
			stripToLastSlash(finalURL);
			strcat(finalURL, tURL+2);
			
			free(tStr);
			return;	
		}
	}
	
	// default = on current directory
	
	stripToLastSlash(finalURL);
	strcat(finalURL, tURL);
	free(tStr);
}

void drawTextBoxWEB()
{
	// textbox code from irc
	if(keyboardShowing)
	{	
		setColor(textEntryTextColor);
		setFont((uint16 **)font_gautami_10);
		
		bg_drawFilledRect(13, 19, 241, 33, widgetBorderColor, textEntryFillColor);
		
		if(isDownloading())
		{
			double z;
			
			z = (double)(226 - 15) * (double)curSize; // room for stop button
			z = z / (double)(maxSize);	
			bg_drawRect(14, 20, (int)z + 14, 32, textEntryProgressColor);
		}
		
		bg_setClipping(13, 19, 235, 33);
		
		char *holdText = pointedText;
		
		if(maskInput)
		{
			int tSize = strlen(holdText);
			setGlobalError(ERROR_WEB_MASKPASSWORD);
			pointedText = (char *)trackMalloc(tSize+1, "temp text");
			setGlobalError(ERROR_NONE);
			memset(pointedText, '*', tSize);
			pointedText[tSize] = 0;
		}
		else
		{
			for(int i=0;i<(int)strlen(pointedText);i++)
			{
				if(pointedText[i] == RET)
					pointedText[i] = FAKE_RET;
			}
			
			wb_ret[4] = textEntryFillColor;
			wb_ret[5] = textEntryTextColor;
			
			font_gautami_10[FAKE_RET] = wb_ret;
		}
		
		char tmpText[128];
		int tmpCur = calcString(tmpText); // figure out where to start and end and where the cursor goes
		
		bool noBlink = false;
		
		if(keysPressed() & KEY_TOUCH && !ignoreUntilUp)
		{
			struct touchScr t = touchReadXYNew();
			
			if(t.touchXpx > 13 & t.touchYpx > 19 & t.touchXpx < 241 & t.touchYpx < 33)
			{
				int tCur = getTouchCursor();
				
				if(tCur == -2)
					tmpCur = strlen(tmpText);
				else if(tCur == -1)
					tmpCur = 0;
				else
					tmpCur = tCur;
				
				moveCursorAbsolute(tmpCur + textBlock);
				
				noBlink = true;
			}
			
			setCallBack(cursorPosCall);
		}
		
		if(!(keysPressed() & KEY_TOUCH))
			ignoreUntilUp = false;
		
		if(blinkOn() || noBlink)
		{
			if(isInsert())
				setCursorProperties(cursorNormalColor, -2, -3, -1);
			else
				setCursorProperties(cursorOverwriteColor, -2, -3, -1);
				
			showCursor();
			setCursorPos(tmpCur);		
		}
		
		bg_disableClipping();
		
		setFakeHighlightEX(textBlock);		
		bg_dispString(3, 4, tmpText);
		font_gautami_10[FAKE_RET] = NULL;
		
		for(int i=0;i<(int)strlen(pointedText);i++)
		{
			if(pointedText[i] == FAKE_RET)
				pointedText[i] = RET;
		}
		
		clearCallBack();
		bg_enableClipping();
		
		hideCursor();
		clearHighlight();
		
		if(maskInput)
		{
			trackFree(pointedText);
			pointedText = holdText;
		}
		
		if(isDownloading() || isQueued())
			bg_dispSprite(227, 19, wb_stop, 31775);
	}
}

void webListCallback(int pos, int x, int y)
{
	char str[512];
	FAVORITE_TYPE *curList = ircCaptured; // set default, so we can fall through on favorites later

	if(getActiveSideTab() == TAB_FAVORITES)
		curList = favorites;
	
	if(!strlen(curList[pos].caption))
		strcpy(str, curList[pos].url);
	else
		strcpy(str, curList[pos].caption);
	
	abbreviateString(str, list_right - (list_left + 15 + 18), (uint16 **)font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

void drawWebBrowserScreens()
{	
	switch(bState)
	{
		case WEBSTATE_INIT:
			connectWifi(); // request connect
			bState = WEBSTATE_CONNECTING;
			
			setFont((uint16 **)font_arial_11);
			setColor(genericTextColor);
			
			bg_dispString(13,20-2,l_associating);
			break;
		case WEBSTATE_CONNECTING: // test status
		{
			int i = Wifi_AssocStatus(); // check status
			
			if(i == ASSOCSTATUS_ASSOCIATED)
				bState = WEBSTATE_LOADHOMEPAGE;
			
			if(i == ASSOCSTATUS_CANNOTCONNECT) 
				bState = WEBSTATE_FAILEDCONNECT;
			
			setFont((uint16 **)font_arial_11);
			setColor(genericTextColor);
			
			bg_dispString(13,20-2,l_associating);
			break;
		}
		case WEBSTATE_FAILEDCONNECT:
			setFont((uint16 **)font_arial_11);
			setColor(genericTextColor);
			
			bg_dispString(13,20-2,l_ircfailed);
			
			break;
		case WEBSTATE_FAILEDSERVER:
			loadPage("about:unreachable", true);
			
			bState = WEBSTATE_CONNECTED;
			
			goto normalRender;
			
			break;
		case WEBSTATE_FINISHED:
			bState = WEBSTATE_CONNECTED;
			loadPage(downFile, true);
			
			goto normalRender;
			
			break;
		case WEBSTATE_LOADHOMEPAGE:
			if(!temporaryStart)
			{
				strcpy(inputBuffer, getHomePage());
			}
			
			loadHomePage = true;
			temporaryStart = false;
			
			webBrowserForward();
			break;			
		case WEBSTATE_DOWNLOADING:
			checkFile();
			
			// fallthrough
		case WEBSTATE_CONNECTED:
normalRender:
		{
			checkDLQueue(); // for downloading images
			drawTextBoxWEB();
			
			if(isImage)
			{
				freeHtmlRenderer();
				drawPictureScreen();
				
				if(!keyboardShowing)
				{
					drawZoomScreen();
					
					drawButtonTexts(l_go, l_hide);
					drawLREX(l_back, historyIsPrev(), l_forward, historyIsNext());
				}
				
				break;
			}
			
			// display html page
			
			bool noRaceURL = doURL && !dontScan;
			
			if(isLoaded(&htmlPage))
			{
				runAnimatedImages(&htmlPage);
				mallocHtmlRenderer();
				
				if(keyboardShowing || isFileDownload)
				{		
					displayHTML(htmlRender + 49152, &htmlPage, bPosition, 192);
					
					#ifndef DEBUG_MODE
					int xOffset = ((getHand() == HAND_RIGHT) ? 0 : (default_scroll_left + DEFAULT_SCROLL_WIDTH));
					
					if(xOffset > 0)
					{	
						// fill extra bytes to make it look nice
						for(int h = 0;h < xOffset;h++)
						{
							fb_backBuffer()[h] = htmlPage.colors[C_BG] | BIT(15);
						}
					}
					
					for(int h=0;h<256*192 - xOffset;h++)
					{
						fb_backBuffer()[h + xOffset] = htmlRender[h + 49152];
					}
					#endif
					
					if(htmlPage.height > 192)
					{
						// draw scrollbar
						
						double z;
						
						fb_drawFilledRect(default_scroll_left, 3, default_scroll_left + DEFAULT_SCROLL_WIDTH, 188, widgetBorderColor, scrollFillColor);
						
						z = (double)(155 + 15) * (double)bPosition;
						z = z / (double)(htmlPage.height - 192);	
						if(wbDrag)
							fb_drawRect(default_scroll_left + 2, 5 + (int)z, default_scroll_left + DEFAULT_SCROLL_WIDTH - 2, 5 + 11 + (int)z, scrollHighlightColor);
						else
							fb_drawRect(default_scroll_left + 2, 5 + (int)z, default_scroll_left + DEFAULT_SCROLL_WIDTH - 2, 5 + 11 + (int)z, scrollNormalColor);
					}
				}
				else
				{
					if(noRaceURL && !drawURL)
						enableURLS();
					
					displayHTML(htmlRender + 49152, &htmlPage, bPosition - 192, 384);
					int xOffset = getHand() == HAND_RIGHT ? 0 : (default_scroll_left + DEFAULT_SCROLL_WIDTH);
					
					if(xOffset > 0)
					{
						// fill extra bytes to make it look nice
						for(int h = 0;h < xOffset;h++)
						{
							#ifndef DEBUG_MODE
							fb_backBuffer()[h] = htmlPage.colors[C_BG] | BIT(15);
							#endif
							bg_backBuffer()[h] = htmlPage.colors[C_BG] | BIT(15);
						}
					}
					
					for(int h=0;h<256*192 - xOffset;h++)
					{
						#ifndef DEBUG_MODE
						fb_backBuffer()[h + xOffset] = htmlRender[h + 49152];
						#endif
						bg_backBuffer()[h + xOffset] = htmlRender[h + (49152 << 1)];
					}
					
					if(bPosition < 192)
					{
						// Use the spare banks as a render point to save on memory
						uint16 *tRender = VRAM_E;
						
						for(int i=0;i<256*192;++i)
							tRender[i] = 0xFFFF;
						
						drawRect(3, 3, 252, 3, 0, tRender, 192);
						drawRect(3, 188, 252, 188, 0, tRender, 192);
						
						drawRect(3, 3, 3, 188, 0, tRender, 192);
						drawRect(252, 3, 252, 188, 0, tRender, 192);
						
						setFont((uint16 **)font_arial_11);
						dispString(0, 0, htmlPage.title, tRender, 1, 5, 5, 250, 186);
						
						for(int i=bPosition;i<192;++i)
						{
							#ifndef DEBUG_MODE
							int tY = i-bPosition;
							
							for(int j=0;j<256;++j)
							{
								fb_backBuffer()[j + (256*tY)] = tRender[j + (256*i)];
							}
							#endif
						}
					}
					
					if(htmlPage.height > 192 - 16)
					{
						// draw scrollbar
						displayScrollHandle(true);			
						drawScrollBarEX(bPosition, htmlPage.height - 192, wbDrag, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 171);
					}
					else
					{
						displayScrollHandle(false);			
						drawScrollBar(0, 1, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 171);
					}
				}
				
				if(!isFileDownload)
				{
					if(drawURL)
					{
						drawURL = false;
						webBrowserForward();
					}
					
					else if(noRaceURL)
					{					
						doURL = false;
						disableURLS();
						
						int tmpURL = getLastURL();
						
						if(tmpURL)
						{	
							--tmpURL;
							
							if(htmlPage.urlCodes[tmpURL].url.endPos > htmlPage.urlCodes[tmpURL].url.startPos)
							{
								if(isDownloading() || isQueued()) //stop downloading images and clear download queue
								{
									cancelQueuedDownloads();			
								}
								
								char *tURL = (char *)safeMalloc(htmlPage.urlCodes[tmpURL].url.endPos - htmlPage.urlCodes[tmpURL].url.startPos + 1);
								getURLFromPosition(curHTMLPage, htmlPage.urlCodes[tmpURL].url, tURL);
								
								setReferrer(lastURL);
								
								memcpy(inputBuffer, lastURL, MAX_URL+1);
								constructValidURL(tURL, inputBuffer);
								strUpdated = true;
								
								free(tURL);
								
								memset(lastanchor, 0, 64);
								memset(anchor, 0, 64);
								
								setClicking(tmpURL, C_ALINK, &htmlPage);						
								drawURL = true;
							}
							else
							{
								drawURL = false;
							}
						}
						
						if(!drawURL)
						{
							char *oldText = NULL;
							maskInput = false;
							
							if(controlFocus())
							{	
								if(isDownloading() || isQueued()) //stop downloading images and clear download queue
								{
									cancelQueuedDownloads();			
								}
								
								switch(htmlPage.formCodes[controlFocus()-1].whatType)
								{
									case FORM_TEXTAREA:
										oldText = pointedText;
										pointedText = (char *)htmlPage.formCodes[controlFocus()-1].state + 4;
										textLength = htmlPage.formCodes[controlFocus()-1].maxLength;
										
										resetSideTabs();
										webBrowserBack();
										
										break;
									case FORM_TEXT:
									case FORM_PASSWORD:
										oldText = pointedText;
										pointedText = (char *)htmlPage.formCodes[controlFocus()-1].state;
										textLength = htmlPage.formCodes[controlFocus()-1].maxLength;
										
										resetSideTabs();
										webBrowserBack();
										
										if(htmlPage.formCodes[controlFocus()-1].whatType == FORM_PASSWORD)
											maskInput = true;
										break;
									case FORM_SUBMIT:
										oldText = pointedText;						
										pointedText = inputBuffer;
										textLength = MAX_URL;
										
										drawURL = true;
										break;
									default:
										oldText = pointedText;						
										pointedText = inputBuffer;
										textLength = MAX_URL;
								}
							}
							else
							{
								oldText = pointedText;						
								pointedText = inputBuffer;
								textLength = MAX_URL;
							}
							
							if(oldText != pointedText)
							{
								resetCursor();
								
								if(pointedText == inputBuffer)
									nextClears = true;
								
								strUpdated = true;
							}
						}
					}
				}
				else
				{
					drawDownloadScreen();
				}
			}
			
			if(!keyboardShowing && !isFileDownload)
			{
				char str[25];
				
				bg_drawRect(0,177,255,191, genericFillColor);
				
				bg_setDefaultClipping();
				setFont((uint16 **)font_gautami_10);
				setColor(genericTextColor);
				
				if(isDownloading())
				{
					double z;
					
					z = (double)(255) * (double)curSize;
					z = z / (double)(maxSize);	
					bg_drawRect(0,177,(int)z,191, genericProgressColor);
				}
				
				if(dlQueueCount > 0)
				{				
					sprintf(str,"(%d/%d)", dlQueuePos + 1, dlQueueCount);
					bg_dispString(5, 181, str);
				}
				
				sprintf(str,"%c\a\a%s", BUTTON_B, l_hide);	
				bg_dispString(centerOnPt(128,str, (uint16 **)font_gautami_10), 181, str);
			}
			
			break;
		}
	}
	
	if(keyboardShowing && !isFileDownload)
	{
		displaySideTabs();
		
		FAVORITE_TYPE *curList = ircCaptured; // set default, so we can fall through on favorites later
		
		switch(getActiveSideTab())
		{
			case TAB_KEYBOARD:		
				drawKeyboardSpecificEX(10, 143, 0, NULL, bg_backBuffer());		
				
				char tBStr[64];
				bg_setDefaultClipping();
				setColor(genericTextColor);	
				sprintf(tBStr,"%c\a\a%s", BUTTON_X, l_swap);				
				bg_dispString(145, 143, tBStr);
				
				break;
			case TAB_SPECIALKEYS:
				
				setColor(keyboardTextColor);
				setFont((uint16 **)font_gautami_10);
				bg_setDefaultClipping();
				
				for(int i=0;i<18;i++)
				{
					int x = i % 6;
					int y = i / 6;
					
					int tOff = 0;
					int bOff = 0;
					int lOff = 0;
					int rOff = 0;
					
					if(x >= 4)
						rOff = -1;
					if(x == 5)
						lOff = -1;
					
					x = 32 + 16 + (x * 32);					
					y = 37 + 12 + (y * 32);
					
					if(i >= 6)
						bOff = -1;
					
					if(i >= 12)
						tOff = -1;
					
					uint16 tColor = keyboardFillColor;
						
					if(curSpecialKey == i)
					{
						tColor = keyboardHighlightColor;					
					}
					
					bg_drawFilledRect(x - 16 + lOff, y - 12 + tOff, x + 16 + rOff, y + 20 + bOff, keyboardBorderColor, tColor);
					
					x = centerOnPt(x, specialKey[i], (uint16 **)font_gautami_10) + 1;
					
					bg_dispString(x, y, specialKey[i]);
				}
				
				if(specialCountDown)
				{
					specialCountDown--;
					
					if(!specialCountDown)
						curSpecialKey = -1;
				}
				
				char tStr[64];
				
				setFont((uint16 **)font_gautami_10);
				setColor(genericTextColor);
				sprintf(tStr,"%c\a\aBKSP", BUTTON_X);
				bg_dispString(145, 143, tStr);
				
				drawSubKeys(bg_backBuffer());
				
				break;
			case TAB_FAVORITES: // fall through
				curList = favorites;
			case TAB_IRCURLS:
			{	
				if(!curList)
				{				
					displayScrollHandle(false);
					drawScrollBarIRC(0, 1);
					break;
				}
				if(!curList->numEntries)
				{
					displayScrollHandle(false);
					drawScrollBarIRC(0, 1);
					break;
				}
				
				drawListBox(list_left + 18, LIST_TOP + 23, list_right, LIST_BOTTOM - 9, listCursor, curList->numEntries, NULL, webListCallback);
				drawScrollBarIRC(listCursor, curList->numEntries);
			}
		}
		
		drawButtonTexts(l_go, l_hide);
		drawHome();
		drawLREX(l_back, historyIsPrev(), l_forward, historyIsNext());
	}
	
	if(nextForward)
	{
		nextForward = false;
		webBrowserForward();
	}
	
	needsRedraw = false;
}

void webBrowserForward()
{
	if(isFileDownload)
	{
		// Can't do anything while downloading
		return;
	}
	
	switch(bState)
	{
		case WEBSTATE_INIT:
		case WEBSTATE_CONNECTING:
		case WEBSTATE_FAILEDCONNECT:
			// can't do anything in this state
			
			return;
	}
	
	cancelQueuedDownloads();
	
	if(keyboardShowing && !loadHomePage) // disable loading from favorites if homepage is being loaded
	{
		FAVORITE_TYPE *curList = ircCaptured; // set default, so we can fall through on favorites later
		
		switch(getActiveSideTab())
		{
			case TAB_FAVORITES: // fall through
				curList = favorites;
			case TAB_IRCURLS:
				if(!curList)
					return;
				if(!curList->numEntries)
					return;
				
				strcpy(inputBuffer, curList[listCursor].url);
				resetCursor();
				strUpdated = true;
		}
	}
	
	loadHomePage = false;
	bool needsEscape = true;
	
	if(controlFocus())
	{	
		int tForm = htmlPage.formCodes[controlFocus()-1].formID;
		int whichSubmit = -1;
		
		if(htmlPage.formCodes[controlFocus()-1].whatType == FORM_SUBMIT)
			whichSubmit = controlFocus()-1;
		
		setReferrer(lastURL);
		
		char *tData = (char *)safeMalloc(htmlPage.formData[tForm-1].url.endPos - htmlPage.formData[tForm-1].url.startPos + 1);
		getURLFromPosition(curHTMLPage, htmlPage.formData[tForm-1].url, tData);
		
		constructValidURL(tData, lastURL);
		
		free(tData);
		
		escapeIllegals(lastURL);
		needsEscape = false;
		
		clearFocus();					
		
		pointedText = inputBuffer;
		textLength = MAX_URL;
		maskInput = false;
		
		memcpy(inputBuffer, lastURL, MAX_URL+1);
		eraseAnchors(lastURL);
		strUpdated = true;
		
		bool needsAnd = false;
		
		char *tFormData = (char *)safeMalloc(MAX_URL+1);
		
		for(int x=0;x<htmlPage.maxFormControls;++x)
		{
			if((int)htmlPage.formCodes[x].formID == tForm)
			{	
				switch(htmlPage.formCodes[x].whatType)
				{
					case FORM_TEXTAREA:
						if(needsAnd)
							strcat(tFormData, "&");
						
						needsAnd = true;
						strescapecat(tFormData, htmlPage.formCodes[x].name);
						strcat(tFormData, "=");
						strescapecat(tFormData, (char *)htmlPage.formCodes[x].state + 4);						
						break;
					case FORM_TEXT:
					case FORM_PASSWORD:
					case FORM_HIDDEN:
						if(needsAnd)
							strcat(tFormData, "&");
						
						needsAnd = true;
						strescapecat(tFormData, htmlPage.formCodes[x].name);
						strcat(tFormData, "=");
						strescapecat(tFormData, (char *)htmlPage.formCodes[x].state);						
						break;
					case FORM_CHECK:							
						if(htmlPage.formCodes[x].state)
						{								
							if(needsAnd)
								strcat(tFormData, "&");
							
							needsAnd = true;
							strescapecat(tFormData, htmlPage.formCodes[x].name);
							strcat(tFormData, "=on");
						}
						break;
					case FORM_RADIO:							
						if(htmlPage.formCodes[x].maxLength)
						{								
							if(needsAnd)
								strcat(tFormData, "&");
							
							needsAnd = true;
							strescapecat(tFormData, htmlPage.formCodes[x].name);
							strcat(tFormData, "=");
							strescapecat(tFormData, (char *)htmlPage.formCodes[x].state);
						}
						break;
					case FORM_SUBMIT:
						if(whichSubmit != -2)
						{
							if(whichSubmit != -1 && x != whichSubmit)
								break;
								
							if(strlen(htmlPage.formCodes[x].name) > 0)
							{							
								if(needsAnd)
									strcat(tFormData, "&");
								
								needsAnd = true;
								
								strescapecat(tFormData, htmlPage.formCodes[x].name);
								strcat(tFormData, "=");
								strescapecat(tFormData, (char *)htmlPage.formCodes[x].resetState);
							}
							
							whichSubmit = -2;
						}
						break;
					case FORM_SELECT:
						if(htmlPage.formCodes[x].maxLength)
						{
							FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)htmlPage.formCodes[x].state;
							
							if(fsi->type == 0) // dropdown
							{
								if(needsAnd)
									strcat(tFormData, "&");
								
								needsAnd = true;
								
								strescapecat(tFormData, htmlPage.formCodes[x].name);
								strcat(tFormData, "=");
								strescapecat(tFormData, fsi->items[fsi->cursor].value);
							}
							else // multiselect
							{
								for(int i=0;i<htmlPage.formCodes[x].maxLength;i++)
								{
									if(fsi->items[i].depth >= 0 && fsi->items[i].highlighted)
									{
										if(needsAnd)
											strcat(tFormData, "&");
										
										needsAnd = true;
										
										strescapecat(tFormData, htmlPage.formCodes[x].name);
										strcat(tFormData, "=");
										strescapecat(tFormData, fsi->items[i].value);
									}
								}
							}
						}
						break;
				}
			}
		}
		
		if(htmlPage.formData[tForm-1].method == M_GET)
		{
			strcat(inputBuffer, "?");
			strcat(inputBuffer, tFormData);
		}
		else
		{
			setPostData(tFormData);
		}
		
		free(tFormData);
	}
	
	if(strlen(inputBuffer) == 0)
		return;
	
	// check for anchors
	
	memset(anchor, 0, 64);
	
	for(int x=strlen(inputBuffer)-1;x>0;x--)
	{
		if(inputBuffer[x] == '/') // no urls
			break;
		
		if(inputBuffer[x] == '#') // found one
		{
			strncpy(anchor, &inputBuffer[x+1], 63);
			inputBuffer[x] = 0;
			break;
		}
	}
	
	char *tStr = (char *)safeMalloc(MAX_URL+1);
	strcpy(tStr, inputBuffer);
	strlwr(tStr);
	
	if(strcmp(tStr, "/.") == 0)
	{
		memset(inputBuffer, 0, MAX_URL+1);
		strcpy(inputBuffer, "http://slashdot.org/");
		
		strcpy(tStr, inputBuffer);
		strlwr(tStr);
		
		strUpdated = true;
	}
	
	// Check for search string format
	if(tStr[0] != ' ' && tStr[1] == ' ' && tStr[2] != ' ' && tStr[2] != 0)
	{
		bool found = false;
		
		// It's the right format, look for the search url
		if(searchStrings)
		{
			for(int i=0;i<searchMax;++i)
			{
				if(searchStrings[i].triggerChar == tStr[0])
				{
					// found the url
					
					found = true;
					
					char *tSearch = (char *)safeMalloc(MAX_URL+1);
					
					memset(tSearch, 0, MAX_URL+1);					
					strescapecat(tSearch, inputBuffer+2);					
					
					memset(inputBuffer, 0, MAX_URL+1);
					strcpy(inputBuffer, searchStrings[i].url);
					
					strcat(inputBuffer, tSearch);
					strUpdated = true;
					needsEscape = false;
					
					free(tSearch);
				}
			}
		}
		
		if(!found)
		{
			// The url wasn't in the list
			strcpy(inputBuffer, "about:badsearch");
			strUpdated = true;
		}
		
		strcpy(tStr, inputBuffer);
		strlwr(tStr);
	}
	
	if(strncmp(tStr, "about:", 6) != 0)
	{	
		if(strncmp(tStr, "http://", 7) != 0)
		{
			if(strncmp(tStr, "https://", 8) != 0)
			{
				char *tURLStr = (char *)safeMalloc(MAX_URL+1);
				
				strcpy(tURLStr, "http://");
				strcat(tURLStr, inputBuffer);
				
				memset(inputBuffer, 0, MAX_URL+1);
				strcpy(inputBuffer, tURLStr);
				
				strUpdated = true;
				
				free(tURLStr);
			}
		}
		
		strcpy(tStr, inputBuffer);
		strlwr(tStr);
		
		char *tSearch = NULL;
		
		if(strncmp(tStr, "https://", 8) == 0)
			tSearch = tStr + 8;
		else
			tSearch = tStr + 7;
		
		if(!strchr(tSearch, '/'))
		{
			strcat(inputBuffer, "/");
			strUpdated = true;
		}
		
		if(needsEscape)
		{
			escapeIllegals(inputBuffer);
			strUpdated = true;
		}
	}
	else
	{
		memcpy(lastanchor, anchor, 64);
		
		loadPage(tStr, true);
		
		resetCursor();
		
		bState = WEBSTATE_CONNECTED;
		
		free(tStr);
		
		return;
	}
	
	free(tStr);
	
	// if the anchor changed, its the same url, and the anchor isn't nothing
	if(strcmp(lastURL, inputBuffer) == 0 && strlen(anchor) > 0 && strcmp(lastanchor, anchor) != 0)
	{
		// just jump to the anchor
		
		jumpToAnchor(anchor);
		
		bState = WEBSTATE_CONNECTED;
		
		resetCursor();
		nextClears = true;
	}
	else
	{	
		setPageFile(inputBuffer);
		std::string PathFix = getDefaultDSOrganizeCachePath( string( std::to_string((u32)CalcCRC32(inputBuffer)) + string(".TMP") ));
		
		if(debug_FileExists((const char*)PathFix.c_str(),42) == FT_NONE){
			downFP = DRAGON_fopen(PathFix.c_str(), "w+");	//debug_FileExists index: 42
		}
		resetRCount();
		
		strcpy(lastURL, inputBuffer);
		getFile(inputBuffer, res_bAgent);
		
		bState = WEBSTATE_DOWNLOADING;
		
		resetCursor();
		nextClears = true;
		
		if(isAutoHide())
		{
			if(keyboardShowing)
			{
				// Hide keyboard if it's showing
				webBrowserBack();
			}
		}
	}
	
	memcpy(lastanchor, anchor, 64);

	if(strlen(anchor) > 0)
	{
		strcat(inputBuffer, "#");
		strcat(inputBuffer, anchor);
		strUpdated = true;
	}
}

void webBrowserBack()
{
	if(isFileDownload)
	{
		cancelDownload();
		resetFileDownload();
		return;
	}
	
	switch(bState)
	{
		case WEBSTATE_INIT:
		case WEBSTATE_CONNECTING:
		case WEBSTATE_FAILEDCONNECT:	
			return;
	}
	
	dontScan = true;
	keyboardShowing = !keyboardShowing;
	
	if(!keyboardShowing)
	{
		if(htmlPage.height < 384 - 16)
			bPosition = 0;
	}	
}

void webBrowserLButton()
{
	if(isFileDownload)
		return;
	
	historyBack();
}

void webBrowserRButton()
{
	if(isFileDownload)
		return;
	
	historyForward();
}

void cancelQueuedDownloads()
{
	if(isDownloading())
		cancelDownload();
	
	freeDLQueue();
}

void webBrowserStop()
{
	if(bState == WEBSTATE_DOWNLOADING)
	{
		cancelDownload();
		bState = WEBSTATE_CONNECTED;
		ignoreUntilUp = true;
		resetCursor();
		resetFileDownload();
	}
	else
	{
		if(isDownloading() || isQueued()) //stop downloading images and clear download queue
		{
			cancelQueuedDownloads();			
		}
	}
}

bool isWebKeyboard()
{
	return getActiveSideTab() == TAB_KEYBOARD;
}

bool isFav()
{
	if(getActiveSideTab() != TAB_FAVORITES && getActiveSideTab() != TAB_IRCURLS)
		return false;
	
	if(needsRedraw)
		return false;
	
	return true;
}

int getWebBrowserEntries()
{
	if(!isFav())
		return 0;
	
	FAVORITE_TYPE *curList = ircCaptured;
	
	if(getActiveSideTab() == TAB_FAVORITES)
		curList = favorites;
	
	if(!curList)
		return 0;
	
	return curList->numEntries;
}

void setScrollWeb(int py)
{
	listCursor = py;
}

void webBrowserClick(int px, int py)
{	
	if(px == 0 && py == 0)
		return;
	
	if(alreadyCaptured)
		return;
	
	alreadyCaptured = true;
	
	if(px >= default_scroll_left && px <= default_scroll_left + DEFAULT_SCROLL_WIDTH)
	{
		if(py >= 3 && py <= (3 + SCROLL_ARROW_HEIGHT))								
		{
			moveWebBrowserCursor(CURSOR_UP);
		}
		else if(py >= (174 - SCROLL_ARROW_HEIGHT) && py <= 174)
		{
			moveWebBrowserCursor(CURSOR_DOWN);
		}
		else if(py >= (3 + SCROLL_ARROW_HEIGHT + 2) && py <= (174 - SCROLL_ARROW_HEIGHT - 2))
		{
			bPosition = getScroll(py, (htmlPage.height - 192) + 1);
			wbDrag = true;
		}
		
		return;
	}
	
	if(py > 176)
	{
		webBrowserBack();
		return;
	}
	
	wbDrag = false;
	setClickPosition(getHand() == HAND_RIGHT ? px : px - (default_scroll_left + DEFAULT_SCROLL_WIDTH), py + 192);
	doURL = true;
	
	drawURL = false;
}

void webBrowserDrag(int px, int py)
{
	if(!isShowingKeyboard() && isImage)
	{
		movePicture(px, py);
		return;
	}
	
	if(!alreadyCaptured)
	{
		webBrowserClick(px, py);
		return;
	}
	
	if(px >= default_scroll_left && px <= default_scroll_left + DEFAULT_SCROLL_WIDTH && wbDrag)
	{
		bPosition = getScroll(py, (htmlPage.height - 192) + 1);
		return;
	}
}

void webBrowserRelease()
{
	wbDrag = false;
	alreadyCaptured = false;
	dontScan = false;
}

char webHandleClick(int px, int py)
{
	if(isFileDownload || needsRedraw)
	{
		return 0;
	}
	
	if(isShowingKeyboard())
	{
		if(clickSideTab(px, py)) // if it changed
		{
			needsRedraw = true;		
			listCursor = 0;
		}
		else
		{		
			switch(getActiveSideTab())
			{
				case TAB_KEYBOARD:	
				{
					int offset = 0;
					
					if(activeKeyboard() == KB_QWERTY && py < 138)
						offset = 10;
					
					if(py >= 138 && py <= 138 + 17 && px > 135 && px < 135 + 64)
						toggleKeyboard();
					
					if(py > TOPAREA && px > 20)
						return executeClick(px - offset, py);
					
					break;
				}
				case TAB_SPECIALKEYS:
				{
					if(py >= 138)
					{
						if(py <= 138 + 17)
						{
							if(px > 135 && px < 135 + 64)
							{
								editWebBrowserAction(BSP);							
							}
							else
							{
								return executeClick(px, py);
							}
						}
						break;
					}
					
					for(int i=0;i<18;i++)
					{
						int x = i % 6;
						int y = i / 6;
						
						int tOff = 0;
						int bOff = 0;
						int lOff = 0;
						int rOff = 0;
						
						if(x >= 4)
							rOff = -1;
						if(x == 5)
							lOff = -1;
						
						y = 37 + 12 + (y * 32);
						x = 32 + 16 + (x * 32);
						
						if(i >= 6)
							bOff = -1;
						
						if(i >= 12)
							tOff = -1;
						
						if(px >= (x - 15 + lOff) && py >= (y - 11 + tOff) && px <= (x + 15 + rOff) && py <= (y + 19 + bOff))					
						{
							if(strlen(specialKey[i]) > 0)
							{
								specialCountDown = 2;
								curSpecialKey = i;
								
								for(int j=0;j<(int)strlen(specialKey[i]);j++)
									editWebBrowserAction(specialKey[i][j]);
							}
						}
					}
					
					
					break;
				}
				case TAB_IRCURLS:
				case TAB_FAVORITES:
				{
					if(px > 20)
					{
						int tmpCursor = getCursorFromTouch(px, py);
						
						if(tmpCursor != -1)
						{
							if(listCursor == tmpCursor && secondClickAction()) // double click
								nextForward = true;
							listCursor = tmpCursor;
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		if(isImage)
		{
			movePicture(px, py);
		}
		else
		{
			webBrowserClick(px, py);
		}
	}
	
	return 0;
}

void ensureNoRunover()
{
	if(bPosition < 0)
		bPosition = 0;
	if(bPosition > (int)(htmlPage.height - 192))
		bPosition = htmlPage.height - 192;
}

void moveWebBrowserCursor(int direction)
{	
	switch(direction)
	{
		case CURSOR_UP:
		{
			int x = getActiveSideTab();
			
			if(!keyboardShowing)
				x = TAB_KEYBOARD;
			
			switch(x)
			{
				case TAB_KEYBOARD:
				case TAB_SPECIALKEYS:					
					if(htmlPage.height <= 192)
						return;
					
					bPosition-=16;
					
					ensureNoRunover();
					
					break;
				case TAB_FAVORITES:
				case TAB_IRCURLS:
					if(listCursor > 0)
						listCursor--;
					break;
			}
			
			break;
		}
		case CURSOR_DOWN:
		{
			FAVORITE_TYPE *curList = ircCaptured;
			int x = getActiveSideTab();
			
			if(!keyboardShowing)
				x = TAB_KEYBOARD;			
			
			switch(x)
			{
				case TAB_KEYBOARD:
				case TAB_SPECIALKEYS:					
					if(htmlPage.height <= 192)
						return;
					
					bPosition+=16;
					
					ensureNoRunover();
					
					break;
				case TAB_FAVORITES:
					curList = favorites;
				case TAB_IRCURLS:
					if(!curList)
						break;
					
					if(listCursor < curList->numEntries - 1)
						listCursor++;
					break;
			}
			
			break;
		}
		case CURSOR_PAGEUP:
			if(htmlPage.height <= 192)
				return;
			
			bPosition -= 192; // last line visible
			
			ensureNoRunover();
			
			break;
		case CURSOR_PAGEDOWN:		
			if(htmlPage.height <= 192)
				return;
			
			bPosition += 192; // last line visible
			
			ensureNoRunover();
			
			break;
		
		// bookmarks and such below
	}
}

void editWebBrowserAction(char c)
{
	if(c == 0) return;
	switch(bState)
	{
		case WEBSTATE_INIT:
		case WEBSTATE_CONNECTING:
		case WEBSTATE_FAILEDCONNECT:
			return;
	}
	
	if(isControl())
	{	
		// control character
		switch(c)
		{
			case 'b': // bookmark
				toggleControl();
				setPressedChar(c);
				bookmarkCurrent();
				return;
		}
	}
	
	if(c == RET)
	{
		if(controlFocus() && htmlPage.formCodes[controlFocus()-1].whatType == FORM_TEXTAREA)
			setPressedChar(RET);
		else
		{
			webBrowserForward();
			setPressedChar(RET);
			return;
		}
	}
	else
		setPressedChar(c);
	
	if(c == CLEAR_KEY || (nextClears && getCursor() == 0))
	{
		memset(pointedText, 0, textLength+1);
		strUpdated = true;
		nextClears = false;
		resetCursor();
		
		if(c == CLEAR_KEY)
			return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	genericAction(pointedText, textLength - 1, c);	
	strUpdated = true;	
	recalcTextAreaJump();
}

void webBrowserXButton()
{
	if(!isShowingKeyboard() || isFileDownload)
		return;
	
	switch(getActiveSideTab())
	{
		case TAB_KEYBOARD:	
			toggleKeyboard();
			break;
		case TAB_SPECIALKEYS:
			editWebBrowserAction(BSP);
			break;
	}
}

bool isShowingKeyboard()
{
	return keyboardShowing;
}

