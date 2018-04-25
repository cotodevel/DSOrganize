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
#include "viewer.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../font_arial_11.h"
#include "../general.h"
#include "../keyboard.h"
#include "../globals.h"
#include "../html.h"
#include "../language.h"
#include "../controls.h"
#include "../resources.h"

static uint ptCount = 0;
static uint bufferLength = 0;
static int *pts = NULL;
static char *textFile = NULL;
static int filePos = 0;
static int textType = 0;
static int loadType = PLAINTEXT;
static HTML_RENDERED htmlPage;

static int bookmarks[10];
static int whichBookmark = 0;
static char bookmarkFile[256];
static bool hasRendered = false;
static int pHeight = 0;

extern bool copying;
extern u32 maxSize;
extern u32 curSize;
extern uint16 insert;
extern char fileName[256];
extern char curChar;
extern uint16 caps;
extern char curDir[256];
extern bool needsRefresh;
extern uint16 editCursor;

void initViewer()
{
	hasRendered = false;
}

void saveBookmarks()
{
	bool sBookmarks = false;
	
	for(int i=0;i<10;i++)
	{
		if(bookmarks[i] != -1)
		{
			sBookmarks = true;
			break;
		}
	}
	
	if(!sBookmarks)
	{
		DRAGON_remove(bookmarkFile);
		return;
	}
	
	if(debug_FileExists((const char*)bookmarkFile,32) == FT_FILE){
		DRAGON_FILE *fp = DRAGON_fopen(bookmarkFile, "w");	//debug_FileExists index: 32
		DRAGON_fwrite(bookmarks, 4, 10, fp);
		DRAGON_fclose(fp);	
	}
	
	// hide file
	DRAGON_SetFileAttributes(bookmarkFile, ATTRIB_HID, ATTRIB_HID);
}

void freeViewerText()
{
	if(textFile != NULL)
		trackFree(textFile);
	
	textFile = NULL;
	
	if(pts != NULL)
		free(pts);
	
	pts = NULL;
	
	if(loadType == HTML)
	{
		freeHTML(&htmlPage);
	}
	
	saveBookmarks();
}

void wrapCalculate()
{
	ptCount = getWrapPoints(0, 5, textFile, 3, 3, 240, 187, &pts, (uint16 **)font_arial_9);		
}

void viewScrollCallback()
{	
	for(int i=0;i<whichBookmark;i++)
	{
		double z = (double)(155 - ((SCROLL_ARROW_HEIGHT + 2) * 2)) * (double)bookmarks[i];
		z = z / (double)pHeight;
		
		bg_drawRect(default_scroll_left + 2, (int)z + (SCROLL_ARROW_HEIGHT + 5 + (SCROLL_HANDLE_HEIGHT / 2)), default_scroll_left + DEFAULT_SCROLL_WIDTH - 2, (int)z + (SCROLL_ARROW_HEIGHT + 7 + (SCROLL_HANDLE_HEIGHT / 2)), viewerBookmarkColor);
	}
}

void drawViewerScreens()
{			
	if(!hasRendered)
	{
		// hasn't been loaded yet
		
		whichBookmark = 0;
		for(int i=0;i<10;i++)
			bookmarks[i] = -1;
		
		char fName[256];
		char ext[256];
		strcpy(fName,curDir);
		separateExtension(fName,ext);
		strlwr(ext);
		
		// load bookmarks here
		
		strcpy(bookmarkFile, fName);
		strcat(bookmarkFile, ".bmf");
		
		
		if(debug_FileExists((const char*)bookmarkFile,33) == FT_FILE)
		{
			DRAGON_FILE *fp = DRAGON_fopen(bookmarkFile, "r");	//debug_FileExists index: 33
			DRAGON_fread(bookmarks, 4, 10, fp);
			DRAGON_fclose(fp);	
			
			for(int i=0;i<10;i++)
			{
				if(bookmarks[i] != -1)
					whichBookmark = i + 1;
			}
		}
		
		// end load bookmarks		
		
		if(strcmp(ext,".htm") == 0 || strcmp(ext,".html") == 0)
			loadType = HTML;
		else
			loadType = PLAINTEXT;
		
		if(loadType == PLAINTEXT)
		{
			char readBuffer[102];
			char *tmpBuf = NULL;
			char crChar = 0;
			filePos = 0;
			
			freeViewerText();
			
			if(debug_FileExists((const char*)curDir,34) == FT_FILE){
				DRAGON_FILE *fFile = DRAGON_fopen(curDir, "r");		//debug_FileExists index: 34
				
				bufferLength = DRAGON_flength(fFile) + 100;
				
				copying = true;
				maxSize = bufferLength;
				curSize = 0;
				
				textFile = (char *)trackMalloc(bufferLength,"viewer src");
				memset(textFile, 0, bufferLength);
				
				// figure out if its windows/unix/mac
				
				DRAGON_fgets(textFile, bufferLength+1, fFile);
				
				if(strlen(textFile) == bufferLength) // we have no returns in this document
					textType = WINDOWS; // assume windows cuz thats how we are
				else
				{
					DRAGON_fseek(fFile, strlen(textFile), 0);
					
					char c = DRAGON_fgetc(fFile);
					
					if(c == 0x0A) // LF
						textType = UNIX;
					else
					{
						if(DRAGON_feof(fFile))
							textType = MAC; // has to be mac, its a CR at the end of the file
						else
						{
							c = DRAGON_fgetc(fFile);
							if(c == 0x0A) //LF
								textType = WINDOWS;
							else
								textType = MAC;
						}
					}
					
					memset(textFile, 0, strlen(textFile));
					DRAGON_fseek(fFile, 0, 0);
					uint tmpLocation = 0;
					
					switch(textType)
					{
						case WINDOWS:
							crChar = 0x0D; // CR
							break;
						case UNIX:
							crChar = 0x0A; // LF
							break;
						case MAC:
							crChar = 0x0D; // CR
							break;
					}
					
					// read the file
					
					while(!DRAGON_feof(fFile))
					{	
						memset(readBuffer, 0, 102);
						curSize = DRAGON_ftell(fFile);
						DRAGON_fread(readBuffer, 1, 100, fFile);
						
						tmpBuf = strchr(readBuffer, crChar);
						
						if(tmpBuf == NULL) // not found, just tack on to the end
						{
							if(strlen(readBuffer) > 0)
							{						
								memcpy(textFile + tmpLocation, readBuffer, strlen(readBuffer));
								tmpLocation += strlen(readBuffer);
								textFile[tmpLocation] = 0;
							}
						}
						else
						{
							filePos = tmpBuf - readBuffer; // position
							
							if(filePos > 0)
							{
								memcpy(textFile + tmpLocation, readBuffer, filePos);
								tmpLocation += filePos;
							}
							
							textFile[tmpLocation] = RET;
							tmpLocation++;
							textFile[tmpLocation] = 0;
							
							if(textType == WINDOWS)
								filePos+=2;
							else
								filePos+=1;
							
							DRAGON_fseek(fFile, curSize + filePos, 0);					
						}
					}
				}
				
				DRAGON_fclose(fFile);
			}
			copying = false;
			wrapCalculate();
		}
		
		if(loadType == HTML)
		{
			copying = true;
			curSize = 0;
			maxSize = 1;
			
			setProgressVars(&curSize, &maxSize, &htmlPage);			
			setHTMLWidth(240, &htmlPage);
			
			disableURLS();
			setEncoding(0, &htmlPage);
			loadHTMLFromFile(curDir, &htmlPage);
			
			copying = false;
		}
		
		hasRendered = true;
		filePos = 0;
	}
	
	if(loadType == PLAINTEXT)
	{
		setFont((uint16 **)font_arial_9);
		setColor(genericTextColor);
		fb_setClipping(getHand() == HAND_RIGHT ? 3 : (default_scroll_left + DEFAULT_SCROLL_WIDTH + 5), 3, getHand() == HAND_RIGHT ? 240 : (default_scroll_left + DEFAULT_SCROLL_WIDTH + 240), 187);
		bg_setClipping(getHand() == HAND_RIGHT ? 3 : (default_scroll_left + DEFAULT_SCROLL_WIDTH + 5), 3, getHand() == HAND_RIGHT ? 240 : (default_scroll_left + DEFAULT_SCROLL_WIDTH + 240), 187-16);
		
		setWrapToBorder();
		fb_dispString(0,5,textFile + pts[filePos]);
		
		if(ptCount > 11)
			bg_dispString(0, 5, textFile + pts[filePos+11]);
		setWrapNormal();
		
		pHeight = ptCount-21;
		
		if(ptCount > 21)
		{
			// draw scrollbar
			displayScrollHandle(true);
			drawScrollBarCustom(filePos, pHeight, viewScrollCallback, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 173);
		}
		else
		{
			// draw skeleton
			displayScrollHandle(false);
			drawScrollBarCustom(0, 1, viewScrollCallback, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 173);
		}
		
		char str[25];
		bg_setDefaultClipping();
		setFont((uint16 **)font_gautami_10);
		setColor(genericTextColor);
		sprintf(str,"%c\a\a%s", BUTTON_B, l_back);	
		bg_dispString(centerOnPt(128,str, (uint16 **)font_gautami_10), 181, str);
		
		// display bookmarks
		
		for(int i=0;i<whichBookmark;i++)
		{
			if(bookmarks[i] >= filePos && bookmarks[i] <= (filePos + 20))
			{
				// bookmark is visible
				if(bookmarks[i] < (filePos + 11))
				{
					fb_drawRect(0, 14 + ((bookmarks[i] - filePos) * 16), 1, 16 + ((bookmarks[i] - filePos) * 16), viewerBookmarkColor);
					fb_setPixel(2, 15 + ((bookmarks[i] - filePos) * 16), RGB15(31,0,0));
				}
				else
				{
					bg_drawRect(0, 14 + ((bookmarks[i] - (filePos + 11)) * 16), 1, 16 + ((bookmarks[i] - (filePos + 11)) * 16), viewerBookmarkColor);
					bg_setPixel(2, 15 + ((bookmarks[i] - (filePos + 11)) * 16), RGB15(31,0,0));
				}
			}
		}
	}
	
	if(loadType == HTML)
	{
		uint16 *htmlRender = (uint16 *)trackMalloc((256*(384+16))*2, "temp html render");
		
		disableBackgroundDraw();
		displayHTML(htmlRender, &htmlPage, filePos - 192, 384);
		
		int xOffset = getHand() == HAND_RIGHT ? 0 : (default_scroll_left + DEFAULT_SCROLL_WIDTH);
		
		if(xOffset > 0)
		{
			// fill extra bytes to make it look nice
			for(int h = 0;h < xOffset;h++)
			{
				fb_backBuffer()[h] = htmlPage.colors[C_BG] | BIT(15);
				bg_backBuffer()[h] = htmlPage.colors[C_BG] | BIT(15);
			}
		}
		
		for(int h = 0;h < 256 * 192 - xOffset;h++)
		{
			fb_backBuffer()[h + xOffset] = htmlRender[h];
			bg_backBuffer()[h + xOffset] = htmlRender[h + 49152];
		}
		
		trackFree(htmlRender);
		
		if(filePos < 192)
		{
			uint16 *tRender = (uint16 *)trackMalloc(256*192*2, "temp title render");
			
			for(int i=0;i<256*192;++i)
				tRender[i] = 0xFFFF;
			
			drawRect(3, 3, 252, 3, 0, tRender, 192);
			drawRect(3, 188, 252, 188, 0, tRender, 192);
			
			drawRect(3, 3, 3, 188, 0, tRender, 192);
			drawRect(252, 3, 252, 188, 0, tRender, 192);
			
			setFont((uint16 **)font_arial_11);
			dispString(0, 0, htmlPage.title, tRender, 1, 5, 5, 250, 186);
			
			for(int i=filePos;i<192;++i)
			{
				int tY = i-filePos;
				
				for(int j=0;j<256;++j)
					fb_backBuffer()[j + (256*tY)] = tRender[j + (256*i)];
			}
			
			trackFree(tRender);
		}
		
		// draw bookmarks
		
		for(int i=0;i<whichBookmark;i++)
		{
			int yLoc = 8 + (bookmarks[i] - filePos);
			
			if(yLoc >= -2 && yLoc <= 192+177)
			{
				if(yLoc < 192)
				{			
					fb_drawRect(0, yLoc, 1, 2 + yLoc, viewerBookmarkColor);
					fb_setPixel(2, yLoc + 1, viewerBookmarkColor);
				}
				else
				{
					yLoc -= 192;
					bg_drawRect(0, yLoc, 1, 2 + yLoc, viewerBookmarkColor);
					bg_setPixel(2, yLoc + 1, viewerBookmarkColor);
				}
			}
		}
		
		// end bookmarks
		
		bg_drawRect(0,177,255,191, genericFillColor);
		
		char str[25];
		bg_setDefaultClipping();
		setFont((uint16 **)font_gautami_10);
		setColor(genericTextColor);
		sprintf(str,"%c\a\a%s", BUTTON_B, l_back);	
		bg_dispString(centerOnPt(128,str, (uint16 **)font_gautami_10), 181, str);
		
		pHeight = htmlPage.height - 192;
		
		if(htmlPage.height > 192 - 16)
		{
			// draw scrollbar
			displayScrollHandle(true);			
			drawScrollBarCustom(filePos, pHeight, viewScrollCallback, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 173);
		}
		else
		{
			displayScrollHandle(false);
			drawScrollBarCustom(0, 1, viewScrollCallback, default_scroll_left, 3, DEFAULT_SCROLL_WIDTH, 173);
		}
	}
}

void moveViewerCursor(int direction)
{	
	switch(loadType)
	{
		case PLAINTEXT:
			switch(direction)
			{
				case CURSOR_UP:
					if(filePos > 0)
						filePos--;
					break;
				case CURSOR_DOWN:			
					if(filePos < (int)(ptCount-21))
						filePos++;
					break;
				case CURSOR_PAGEUP:
					filePos -= 20; // last line visible
					
					if(filePos < 0)
						filePos = 0;
					break;
				case CURSOR_PAGEDOWN:
					filePos += 20; // last line visible
					
					if(filePos > (int)(ptCount - 21))
						filePos = ptCount - 21;	
					break;
				
				// bookmarks and such below
			}
			break;
		case HTML:
			if(htmlPage.height <= 192)
				return;
			
			switch(direction)
			{
				case CURSOR_UP:
					filePos-=16;					
					break;
				case CURSOR_DOWN:			
					filePos+=16;					
					break;
				case CURSOR_PAGEUP:
					filePos -= 352; // last line visible					
					break;
				case CURSOR_PAGEDOWN:
					filePos += 352; // last line visible					
					break;
				
				// bookmarks and such below
			}
			
			if(filePos < 0)
				filePos = 0;
			if(filePos > htmlPage.height - 192) // (384 - 16)
				filePos = htmlPage.height - 192;
			break;
	}
	
	switch(direction)
	{
		case CURSOR_LEFT: // prev bookmark
		{
			int distance = 0;
			
			for(int i=0;i<10;i++)
			{
				if(filePos - bookmarks[i] > 0 && bookmarks[i] != -1) // consider cuz it's above
				{
					if(distance == 0)
						distance = filePos - bookmarks[i];
					else
					{
						if((filePos - bookmarks[i]) < distance)
							distance = filePos - bookmarks[i];
					}
				}
			}
			
			filePos -= distance;
			break;
		}
		case CURSOR_RIGHT:
		{
			int distance = 0;
			
			for(int i=0;i<10;i++)
			{
				if(bookmarks[i] - filePos > 0 && bookmarks[i] != -1) // consider cuz it's below
				{
					if(distance == 0)
						distance = bookmarks[i] - filePos;
					else
					{
						if((bookmarks[i] - filePos) < distance)
							distance = bookmarks[i] - filePos;
					}
				}
			}
			
			filePos += distance;
			break;
		}
	}	
}

void toggleBookmark()
{
	switch(loadType) // don't allow bookmarking if the file doesn't even have a frickin scrollbar.
	{
		case PLAINTEXT:
			if(ptCount <= 21)
				return;
			break;
		case HTML:
			if(htmlPage.height <= 192)
				return;
			break;
	}
	
	for(int i=0;i<10;i++)
	{
		if(bookmarks[i] == filePos) // remove
		{
			if(i < 9)
			{
				for(int j=i;j<8;j++)
					bookmarks[j] = bookmarks[j+1];
			}
			bookmarks[9] = -1;
			
			whichBookmark--;
			
			saveBookmarks();
			
			return;
		}
	}
	
	if(whichBookmark < 10)
	{
		bookmarks[whichBookmark] = filePos;
		whichBookmark++;
		
		saveBookmarks();
	}
	else
	{
		// delete the first bookmark
		
		for(int i=0;i<9;i++)
			bookmarks[i] = bookmarks[i+1];
		
		bookmarks[9] = filePos;
		
		saveBookmarks();
	}
}

void setViewerPos(int pos)
{
	filePos = pos;
}

int getViewerMax()
{
	// add one to both in order to force it to scroll to the last line
	switch(loadType)
	{
		case PLAINTEXT:
			return ptCount - 20;
		case HTML:
			return htmlPage.height - 192;
	}
	
	return 0;
}
