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
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include <libpicture.h>
#include "scribble.h"
#include "drawtools.h"
#include "address.h"
#include "home.h"
#include "texteditor.h"
#include "pictureviewer.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../filerout.h"
#include "../general.h"
#include "../globals.h"
#include "../graphics.h"
#include "../settings.h"
#include "../font_arial_11.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../keyboard.h"
#include "../language.h"
#include "../controls.h"
#include "specific_shared.h"
#include "browser.h"
#include "drawtools.h"

uint16 customColors[27] = { 	0x0000, 0x4210, 0x0010, 0x0110, 0x0210, 0x0200, 0x4A49, 0x4000, 0x4010,
										0xFFFF, 0x6318, 0x001F, 0x221F, 0x03FF, 0x03F0, 0x7FE0, 0x7C00, 0x7C1F, 
										0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

uint16 *toolSprites[MAX_TOOLS] = { tool_pencil, tool_pen, tool_colorgrab, tool_erasor, tool_floodfill, tool_line, tool_thickline, tool_rect, tool_thickrect, tool_circle, tool_thickcircle, tool_spray, tool_replace, tool_text };

uint16 *drawBuffer;
uint16 *tmpBuffer;
int saveFormat = SAVE_BMP;
uint16 saveColor = 0;

int colorCursorscribble = 0;
uint16 curColor = 0;
int toolsCursor = 0;
int curTool = TOOL_PENCIL;
int queuedTool = -1;
int lastXscribble = -1;
int lastYscribble = -1;
int curX = -1;
int curY = -1;
int slidePosition;
bool dragging = false;
bool dragging2 = false;
int box_x;
int box_y;
uint16 tmpColor;
char textEntry[SCRIBBLE_LEN];
bool ignoreInput = false;
int scribbleMode = 0;
bool isFlipped = false;
bool isHeld = false;
bool s_isPopulated = false;
PICTURE_DATA scribblePicture;
uint16 scribbleEntries;
int curScribble = 0;

int getScribbleEntries()
{
	return scribbleEntries;
}

void initScribble()
{
	initImage(&scribblePicture, 0, 0);
}

void scribbleSetUnPopulated()
{
	s_isPopulated = false;
}

uint16 *getCustomColors()
{
	return customColors;
}

void scribbleListCallback(int pos, int x, int y)
{
	char str[256];
	
	strcpy(str, scribbleList[pos].shortName);
	abbreviateString(str, list_right - (list_left + 15), (uint16 **)font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

void drawScribbleList()
{
	drawHome();
	
	if(isFlipped)
	{
		SWAP_LCDS();
		isFlipped = false;
	}
	
	if(!s_isPopulated)
	{	
		scribbleEntries = populateScribbleList(d_scribble);
		
		strcpy(copyFrom, "");
		s_isPopulated = true;		
	}
	
	switch(scribbleMode)
	{
		case 0: // normal
			if(scribbleEntries == 0)
				drawButtonTexts(NULL, l_create);
			else
				drawButtonTexts(l_editselected, l_create);
			
			break;
		case 1: // delete
			drawButtonTexts(l_delete, l_back);
			
			break;
	}
	
	if(getCursor() >= scribbleEntries)
	{
		moveCursorAbsolute(0);
	}
	
	drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, getCursor(), scribbleEntries, l_noscribble, scribbleListCallback);
	drawScrollBar(getCursor(), scribbleEntries, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT);
}

void deleteCurrentScribble()
{
	if(scribbleEntries == 0)
		return;
	
	scribbleMode = 1;
}

void freeScribbleList()
{	
	freeImage(&scribblePicture);
	freeScribble();
}

void initTools()
{
	colorCursorscribble = 0;
	curColor = 0 | BIT(15);
	toolsCursor = 0;
	curTool = TOOL_PENCIL;
	lastXscribble = -1;
	lastYscribble = -1;
	penWidth = 1;
}

void memCpySet(void *dst, void *src, int size)
{
	uint16 *tDst = (uint16 *)dst;
	uint16 *tSrc = (uint16 *)src;
	
	for(int i = 0; i < (size >> 1); i++)
	{
		tDst[i] = tSrc[i] | BIT(15);
	}
}

void drawScribbleScreen()
{
	if(!isFlipped)
	{
		isFlipped = true;
		
		setBGColor(0xFFFF, &scribblePicture);
		setLargeDimensions(256,192); // normal thumbnail
		loadImage(scribbleList[curScribble].fileName, &scribblePicture, 0, 0);
		memCpySet(fb_backBuffer(),scribblePicture.picData+2, 256*192*2);
		drawBuffer = fb_backBuffer();
		fb_swapBuffers();
		
		memset(fb_backBuffer(), 0xFF, 256*192*2);
		
		SWAP_LCDS();
		initTools();
	}
	
	if(queuedTool != -1) // for tools that need preview
	{
		memCpySet(drawBuffer, fb_backBuffer(), 256*192*2);
		
		switch(queuedTool)
		{
			case TOOL_LINE:
				penWidth = 1;
				s_drawLine(lastXscribble, lastYscribble, curX, curY, curColor);
				break;				
			case TOOL_THICKLINE:
				penWidth = 3;
				s_drawLine(lastXscribble, lastYscribble, curX, curY, curColor);
				break;
			case TOOL_RECT:
				penWidth = 1;
				s_drawLine(lastXscribble, lastYscribble, lastXscribble, curY, curColor);
				s_drawLine(lastXscribble, lastYscribble, curX, lastYscribble, curColor);
				s_drawLine(curX, lastYscribble, curX, curY, curColor);
				s_drawLine(lastXscribble, curY, curX, curY, curColor);
				break;
			case TOOL_THICKRECT:
				penWidth = 3;
				s_drawLine(lastXscribble, lastYscribble, lastXscribble, curY, curColor);
				s_drawLine(lastXscribble, lastYscribble, curX, lastYscribble, curColor);
				s_drawLine(curX, lastYscribble, curX, curY, curColor);
				s_drawLine(lastXscribble, curY, curX, curY, curColor);
				break;						
			case TOOL_CIRCLE:
			{
				penWidth = 1;
				
				int cX = (curX + lastXscribble) / 2;
				int cY = (curY + lastYscribble) / 2;
				
				int radiusX = 0;
				int radiusY = 0;
				
				if(curX > lastXscribble)
					radiusX = (curX - lastXscribble) / 2;
				else
					radiusX = (lastXscribble - curX) / 2;
				
				if(curY > lastYscribble)
					radiusY = (curY - lastYscribble) / 2;
				else
					radiusY = (lastYscribble - curY) / 2;
					
				if(radiusX == 0 || radiusY == 0)
					s_drawLine(lastXscribble, lastYscribble, curX, curY, curColor);
				else
					s_drawEllipse(cX, cY, radiusX, radiusY, curColor);
				break;					
			}
			case TOOL_THICKCIRCLE:
			{
				penWidth = 3;
				
				int cX = (curX + lastXscribble) / 2;
				int cY = (curY + lastYscribble) / 2;
				
				int radiusX = 0;
				int radiusY = 0;
				
				if(curX > lastXscribble)
					radiusX = (curX - lastXscribble) / 2;
				else
					radiusX = (lastXscribble - curX) / 2;
				
				if(curY > lastYscribble)
					radiusY = (curY - lastYscribble) / 2;
				else
					radiusY = (lastYscribble - curY) / 2;
					
				if(radiusX == 0 || radiusY == 0)
					s_drawLine(lastXscribble, lastYscribble, curX, curY, curColor);
				else
					s_drawEllipse(cX, cY, radiusX, radiusY, curColor);
				break;
			}
			case TOOL_TEXT:
				penWidth = 1;
				drawCheckered = true;
				s_drawLine(lastXscribble, lastYscribble, lastXscribble, curY, 0);
				s_drawLine(lastXscribble, lastYscribble, curX, lastYscribble, 0);
				s_drawLine(curX, lastYscribble, curX, curY, 0);
				s_drawLine(lastXscribble, curY, curX, curY, 0);
				drawCheckered = false;
				break;
		}
	}
}

void drawToolsScreen()
{	
	setFont((uint16 **)font_arial_9);
	bg_setDefaultClipping();
	setColor(widgetTextColor);
	
	if(getMode() == EDITSCRIBBLE)
		drawButtonTextsEX(BUTTON_L, l_save, BUTTON_R, l_back);
	
	if(getMode() == CHOOSECOLOR)
		drawButtonTexts(l_confirm, l_back);
	
	// colors
	
	for(int x=0;x<27;x++)
	{
		int xp = x % 9;
		int yp = x / 9;
		
		if(x == colorCursorscribble)
			bg_drawFilledRect(13 + 26*xp, 90 + 23*yp, 17 + 26*xp + 15, 94 + 23*yp + 15, widgetHighlightColor, widgetFillColor);
		else
			bg_drawFilledRect(13 + 26*xp, 90 + 23*yp, 17 + 26*xp + 15, 94 + 23*yp + 15, widgetBorderColor, widgetFillColor);
		
		bg_drawRect(15 + 26*xp, 92 + 23*yp, 15 + 26*xp + 15, 92 + 23*yp + 15, customColors[x]);
	}
	
	// tools	
	
	for(int x=0;x<MAX_TOOLS;x++)
	{
		int xp = x % 9;
		int yp = x / 9;
		
		if(x == toolsCursor)
			bg_drawFilledRect(13 + 26*xp, 13 + 23*yp, 17 + 26*xp + 15, 17 + 23*yp + 15, widgetHighlightColor, widgetFillColor);
		else
			bg_drawFilledRect(13 + 26*xp, 13 + 23*yp, 17 + 26*xp + 15, 17 + 23*yp + 15, widgetBorderColor, widgetFillColor);
		
		bg_dispSprite(15 + 26*xp, 15 + 23*yp, toolSprites[x], 31775);
	}
}

void drawScribbleText()
{
	int a,b,c,d;
	
	if(lastXscribble > curX)
	{
		a = curX;
		c = lastXscribble;
	}
	else
	{
		c = curX;
		a = lastXscribble;
	}
	
	if(lastYscribble > curY)
	{
		b = curY;
		d = lastYscribble;
	}
	else
	{
		d = curY;
		b = lastYscribble;
	}
	
	a+=3;
	b+=3;
	c-=3;
	d-=3;
	
	setColor(curColor);
	setFont((uint16 **)font_arial_9);
	fb_setClipping(a,b,c,d);
	
	setWrapToBorder();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, 2, 0, 0);
		else
			setCursorProperties(cursorOverwriteColor, 2, 0, 0);
			
		showCursor();
		setCursorPos(getCursor());		
	}
	
	setFakeHighlight();
	fb_dispString(0,-3,textEntry);
	
	clearHighlight();
	setWrapNormal();
	hideCursor();	
}

void drawScribbleKeyboard()
{
	drawKeyboard();
	drawButtonTexts(l_confirm, l_back);
}

void exitScribbleConfirm()
{
	fb_eraseBG();
	fb_swapBuffers();
	drawBuffer = fb_backBuffer();	
	trackFree(tmpBuffer);
	
	int a,b,c,d;
	
	if(lastXscribble > curX)
	{
		a = curX;
		c = lastXscribble;
	}
	else
	{
		c = curX;
		a = lastXscribble;
	}
	
	if(lastYscribble > curY)
	{
		b = curY;
		d = lastYscribble;
	}
	else
	{
		d = curY;
		b = lastYscribble;
	}
	
	a+=3;
	b+=3;
	c-=3;
	d-=3;	
	
	setMode(EDITSCRIBBLE);
	fb_swapBuffers();	
	
	memCpySet(drawBuffer, tmpBuffer, 256*192*2);
	setColor(curColor);
	setFont((uint16 **)font_arial_9);	
	dispString(0, -3, textEntry, drawBuffer, 1, a, b, c, d);
	
	SWAP_LCDS();
	ignoreInput = true;
}

void exitScribbleCancel()
{
	fb_eraseBG();
	fb_swapBuffers();
	drawBuffer = fb_backBuffer();	
	fb_swapBuffers();
	
	memCpySet(drawBuffer, tmpBuffer, 256*192*2);
	trackFree(tmpBuffer);
	
	setMode(EDITSCRIBBLE);
	SWAP_LCDS();
	
	ignoreInput = true;
}

void editScribbleAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == CAP)
	{
		toggleCaps();
		return;
	}	
	
	char *tmpBuf = (char *)trackMalloc(1024,"scribble kbd");
	memcpy(tmpBuf,textEntry,1024);
	int oldCursor = getCursor();
	
	genericAction(tmpBuf, 1024 - 1, c);
	
	int a,b,e,d;
	
	if(lastXscribble > curX)
	{
		a = curX;
		e = lastXscribble;
	}
	else
	{
		e = curX;
		a = lastXscribble;
	}
	
	if(lastYscribble > curY)
	{
		b = curY;
		d = lastYscribble;
	}
	else
	{
		d = curY;
		b = lastYscribble;
	}
	
	a+=3;
	b+=3;
	e-=3;
	d-=3;
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuf, a, b, e, d, &pts, (uint16 **)font_arial_9);
	free(pts);
	
	if(numPts <= ((d-b) / 15))
		memcpy(textEntry,tmpBuf,SCRIBBLE_LEN);
	else
		moveCursorAbsolute(oldCursor);
	
	trackFree(tmpBuf);
}

void scribbleDown(int x, int y)
{
	if(isHeld)
		return;
	if(x <= 0 || x > 255 || y <= 0 || y > 191)
		return;
	
	curTool = toolsCursor;
	penWidth = 1;
	queuedTool = -1;
	
	switch(curTool)
	{
		case TOOL_PEN:
			penWidth = 3;
		case TOOL_PENCIL:
			s_setPixel(x, y, curColor);
			lastXscribble = x;
			lastYscribble = y;
			break;
		case TOOL_COLORGRAB:			
			if(colorCursorscribble >= 18)
				customColors[colorCursorscribble] = drawBuffer[x + (y * 256)] | BIT(15);
			break;
		case TOOL_ERASOR:
			penWidth = 10;			
			s_setPixel(x, y, 0xFFFF);
			lastXscribble = x;
			lastYscribble = y;
			break;
		case TOOL_FLOODFILL:
			s_floodFill(x, y, curColor, drawBuffer[x + (y * 256)]);
			break;
		case TOOL_TEXT:
			tmpBuffer = (uint16 *)trackMalloc(256*192*2,"draw tmp buffer");	
			memcpy(tmpBuffer, drawBuffer, 256*192*2);	
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:	
			memCpySet(fb_backBuffer(), drawBuffer, 256*192*2);
			queuedTool = curTool;
			curX = x;
			curY = y;
			lastXscribble = x;
			lastYscribble = y;
			break;
		case TOOL_SPRAY:{
			s_spray(x, y, 10, curColor);
		}
			break;
		case TOOL_REPLACE:{
			uint16 tC = drawBuffer[x + (y * 256)] | BIT(15);
			
			for(int l=0;l<256*192;l++)
			{
				if(drawBuffer[l] == tC)
					drawBuffer[l] = curColor | BIT(15);
			}
		}
			break;
	}
	
	isHeld = true;
}

void scribbleMove(int x, int y)
{
	if(!isHeld)
		return;
	if(x <= 0 || x > 255 || y <= 0 || y > 191)
		return;
	
	switch(curTool)
	{
		case TOOL_PEN:
		case TOOL_PENCIL:
			if(lastXscribble <= 0 || lastYscribble <= 0 || lastXscribble > 255 || lastYscribble > 191)
				s_setPixel(x, y, curColor); // errant lines due to race conditions
			else				
				s_drawLine(lastXscribble, lastYscribble, x, y, curColor);
			
			lastXscribble = x;
			lastYscribble = y;
			break;
		case TOOL_COLORGRAB:			
			if(colorCursorscribble >= 18)
				customColors[colorCursorscribble] = drawBuffer[x + (y * 256)] | BIT(15);
			break;
		case TOOL_ERASOR:
			if(lastXscribble <= 0 || lastYscribble <= 0 || lastXscribble > 255 || lastYscribble > 191)
				s_setPixel(x, y, 0xFFFF); // errant lines due to race conditions
			else				
				s_drawLine(lastXscribble, lastYscribble, x, y, 0xFFFF);
			
			lastXscribble = x;
			lastYscribble = y;
			break;
		case TOOL_TEXT:		
			if(lastXscribble == -1 || lastYscribble == -1)
			{
				tmpBuffer = (uint16 *)trackMalloc(256*192*2,"text tmp buffer");	
				memcpy(tmpBuffer, drawBuffer, 256*192*2);		
			}
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:
			if(lastXscribble == -1 || lastYscribble == -1)
			{
				memCpySet(fb_backBuffer(), drawBuffer, 256*192*2);
				queuedTool = curTool;
				lastXscribble = x;
				lastYscribble = y;
			}
			curX = x;
			curY = y;
			break;
		case TOOL_SPRAY:
			s_spray(x, y, 10, curColor);
			break;
	}
}

void scribbleUp()
{
	if(!isHeld)
		return;
	
	switch(curTool)
	{
		case TOOL_PEN:
		case TOOL_PENCIL:
		case TOOL_ERASOR:
			lastXscribble = -1;
			lastYscribble = -1;
			break;
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:
			memset(fb_backBuffer(), 0xFF, 256*192*2);
			lastXscribble = -1;
			lastYscribble = -1;
			curX = -1;
			curY = -1;
			queuedTool = -1;
			break;			
		case TOOL_TEXT:
			if(abs(curX - lastXscribble) < 20 || abs(curY - lastYscribble) < 20) // no shenanigans
				return;	
			
			setMode(SCRIBBLETEXT);
			bg_swapBuffers();	
			resetCursor();
			queuedTool = -1;
			
			SWAP_LCDS();
			
			memset(textEntry,0,SCRIBBLE_LEN);
			
			fb_setBG(drawBuffer);
			memset(fb_backBuffer(), 0xFF, 256*192*2);			
			break;
	}
	
	isHeld = false;
}

void createScribble()
{	
	if(strlen(fileName) == 0)
	{
		popCursor();
		setMode(SCRIBBLEPAD);
	}
	else
	{
		if(!isFlipped)
		{
			isFlipped = true;
			
			fb_setBGColor(0xFFFF);
			createImage(&scribblePicture, 256, 192, 0xFFFF);
			strcpy(scribblePicture.fileName, d_scribble);
			strcat(scribblePicture.fileName, fileName);
			
			switch(saveFormat)
			{
				case SAVE_BMP:
					strcat(scribblePicture.fileName, ".bmp");
					break;
				case SAVE_PNG:
					strcat(scribblePicture.fileName, ".png");
					break;
			}
			
			fb_swapBuffers();
			drawBuffer = fb_backBuffer();
			fb_swapBuffers();
			pushCursor();
			setMode(EDITSCRIBBLE);
			SWAP_LCDS();
			
			initTools();
		}
		else
		{
			popCursor();
			setMode(SCRIBBLEPAD);
		}
	}
}
void saveScribble()
{
	memCpySet(scribblePicture.picData + 2, drawBuffer, 256*192*2);
	
	if(isFlipped)
	{
		SWAP_LCDS();
		isFlipped = false;
	}
	
	DRAGON_chdir("/");
	
	loading = true;
	saveImageFromHandle(&scribblePicture);
	loading = false;
	
	fb_setBGColor(genericFillColor);
	fb_swapBuffers();
}

void moveColor(int action)
{
	if(ignoreInput)
	{
		ignoreInput = false;
		return;
	}
	
	switch(action)
	{
		case CURSOR_UP:
			if(colorCursorscribble >= 9)
				colorCursorscribble-=9;
			break;
		case CURSOR_DOWN:
			if(colorCursorscribble < 27-9)
				colorCursorscribble+=9;
			break;
		case CURSOR_LEFT:
			if(colorCursorscribble > 0)
				colorCursorscribble--;
			break;
		case CURSOR_RIGHT:
			if(colorCursorscribble < 27-1)
				colorCursorscribble++;
			break;
	}
	
	curColor = customColors[colorCursorscribble] | BIT(15);
}

void moveTool(int action)
{	
	if(getMode() == EDITSCRIBBLE)
	{
		switch(action)
		{
			case CURSOR_UP:
			{
				int tCursor = toolsCursor-9;
				
				if(tCursor >= 0)
					toolsCursor = tCursor;
				break;
			}
			case CURSOR_DOWN:
			{
				int tCursor = toolsCursor+9;
				
				if(tCursor < MAX_TOOLS)
					toolsCursor = tCursor;
				break;
			}
			case CURSOR_LEFT:
				if(toolsCursor > 0)
					toolsCursor--;
				break;
			case CURSOR_RIGHT:
				if(toolsCursor < MAX_TOOLS-1)
					toolsCursor++;
				break;
		}
	}
	else
	{
		switch(action)
		{
			case CURSOR_UP:
				moveCursorAbsolute(getPositionAbove());
				break;
			case CURSOR_DOWN:
				moveCursorAbsolute(getPositionBelow());
				break;
			case CURSOR_LEFT:
				if(getCursor() > 0)
					moveCursorRelative(CURSOR_BACKWARD);
				break;
			case CURSOR_RIGHT:
				if(getCursor() < (int)strlen(textEntry))
					moveCursorRelative(CURSOR_FORWARD);
				break;
		}
	}
}

void loadColorPicker()
{
	if(colorCursorscribble < 18) // don't edit pallate
		return;
	
	memCpySet(scribblePicture.picData + 2, drawBuffer, 256*192*2); // save current scribble
	saveColor = customColors[colorCursorscribble] | BIT(15);
	slidePosition = 31*3;
	dragging = false;
	dragging2 = false;
	
	box_x = 3;
	box_y = 3;
	
	drawChooser(3, 3, fb_backBuffer());
	fb_setBG(fb_backBuffer());
	
	uint16 *bg = fb_backBuffer();
	tmpColor = bg[3 + (3 * 256)] | BIT(15);
	
	setMode(CHOOSECOLOR);
}

void drawColorPicker()
{
	customColors[colorCursorscribble] = drawSlider(200, 3, tmpColor, slidePosition, fb_backBuffer()) | BIT(15);	
	
	fb_drawRect(195, slidePosition + 3, 198, slidePosition + 3, 0x0);
	fb_drawRect(212, slidePosition + 3, 215, slidePosition + 3, 0x0);
}

void drawColorCursor()
{	
	if(getMode() == CHOOSECOLOR)
		fb_drawBox(box_x - 2, box_y - 2, box_x + 2, box_y + 2, 0xFFFF);
}

void colorPickerDown(int x, int y)
{
	if((x >= 3 && x <= (3 + 31 * 6)) && (y >= 3 && y <= (3 + 31 * 6)))
	{
		uint16 *bg = fb_backBuffer();
		
		dragging = true;
		tmpColor = bg[x + (y * 256)] | BIT(15);
		
		box_x = x;
		box_y = y;
	}
	
	if((x >= 200 && x <= 210) && (y >= 3 && y <= (3 + 31 * 6)))
	{
		dragging2 = true;
		slidePosition = y - 3;
	}
}

void colorPickerMove(int x, int y)
{
	if(dragging)
	{	
		if(x < 3)
			x = 3;
		if(x >= (3 + 31 * 6))
			x = (3 + 31 * 6) - 1;
			
		if(y < 3)
			y = 3;
		if(y >= (3 + 31 * 6))
			y = (3 + 31 * 6) - 1;
		
		uint16 *bg = fb_backBuffer();
		
		tmpColor = bg[x + (y * 256)] | BIT(15);
		
		box_x = x;
		box_y = y;
	}
	
	if(dragging2)
	{
		dragging2 = true;
		slidePosition = y - 3;
		
		if(slidePosition < 0)
			slidePosition = 0;
		if(slidePosition >= (31 * 6))
			slidePosition = (31 * 6) - 1;				
	}
}

void colorPickerUp()
{
	dragging = false;
	dragging2 = false;
}

void destroyColorPicker(bool toSaveColor)
{
	if(!toSaveColor)
		customColors[colorCursorscribble] = saveColor | BIT(15);
	curColor = customColors[colorCursorscribble] | BIT(15);
		
	fb_eraseBG();
	
	fb_swapBuffers();
	fb_dispSprite(0,0,scribblePicture.picData, genericFillColor);
	drawBuffer = fb_backBuffer();
	fb_swapBuffers();
	setMode(EDITSCRIBBLE);
	ignoreInput = true;
}

void scribbleForward()
{
	switch(scribbleMode)
	{
		case 0:	 // open file		
			if(scribbleEntries > 0)
			{
				curScribble = getCursor();
				pushCursor();
				setMode(EDITSCRIBBLE);
			}
			
			break;			
		case 1: // delete file
			DRAGON_remove(scribbleList[getCursor()].fileName);
			moveCursorRelative(CURSOR_BACKWARD);
			
			s_isPopulated = false;
			
			scribbleMode = 0;
			break;
	}
}

void scribbleBack()
{	
	switch(scribbleMode)
	{
		case 0:	 // create file
			strcpy(fileName, "");
			pushCursor();
			setLastMode(SCRIBBLEPAD);
			setMode(BROWSERRENAME);
			
			break;
		case 1: // cancel delete			
			scribbleMode = 0;
			break;
	}
}

bool screensFlipped()
{
	if(getMode() == SCRIBBLETEXT)
		return !isFlipped;
	
	return isFlipped;
}
