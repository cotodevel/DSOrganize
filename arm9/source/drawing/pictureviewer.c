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
#include <string.h>
#include <stdio.h>
#include <libcommon.h>
#include <libpicture.h>
#include "pictureviewer.h"
#include "browser.h"
#include "../general.h"
#include "../colors.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../globals.h"
#include "../settings.h"
#include "../filerout.h"
#include "../keyboard.h"
#include "../language.h"
#include "../controls.h"

#include "specific_shared.h"

extern uint16 browserMode;
extern char fileName[256];
extern int vblCount;

int loc_X = 0;
int loc_Y = 0;

PICTURE_DATA curPicture;
bool picLoaded = false;

extern bool loading;
extern u32 curSize;

// defined functions for picture.c callbacks
void resetCounter()
{
	vblCount = 0;
}

// end defines

bool getNextPic()
{
	int fType = UNKNOWN;
	int tmpCur = getCursor();
	
	while(fType != PICFILE)
	{
		tmpCur = nextFile(fileName, getBrowserEntries());
		
		if(tmpCur == -1)
			return false;
		
		fType = getFileType(fileName);
	}
	
	moveCursorAbsolute(tmpCur);
	return true;
}

bool getPrevPic()
{
	int fType = UNKNOWN;
	int tmpCur = getCursor();
	
	while(fType != PICFILE)
	{
		tmpCur = prevFile(fileName);
		
		if(tmpCur == -1)
			return false;
		
		fType = getFileType(fileName);
	}
	
	moveCursorAbsolute(tmpCur);
	return true;
}

void getPrevPicture()
{
	freePicture();
	if(getPrevPic())
	{
		picLoaded = false;
	}
	else
	{
		exitPictureScreen();
		setMode(BROWSER);
	}
}

void getNextPicture()
{
	freePicture();
	if(getNextPic())
	{
		picLoaded = false;
	}
	else
	{
		exitPictureScreen();
		setMode(BROWSER);
	}
}

void setBinName(char *bN, int x, int y)
{
	strcpy(curPicture.binName, bN);
	curPicture.max_X = x;
	curPicture.max_Y = y;
}

void clearBinName()
{
	strcpy(curPicture.binName,"");
}

void freePicture()
{
	if(picLoaded)
		freeImage(&curPicture);
	
	picLoaded = false;
}

void resetScreen()
{
	loc_X = 0;
	loc_Y = 0;
	
	restartAnimation(&curPicture);
	restartGifFile(&curPicture);
}

void memcpySetBit15(void *dst, void *src, int size)
{
	uint16 *dTmp = (uint16 *)dst;
	uint16 *sTmp = (uint16 *)src;
	
	for(int i = 0;i < (size >> 1); i++)
	{
		*dTmp = *sTmp | BIT(15);
		
		dTmp++;
		sTmp++;
	}
}

void displayPicture(int offsetX, int offsetY, void *tmpData)
{
	if(curPicture.error == true)
		return;
	if(tmpData == NULL)
		return;
	
	int readLen = getScreenWidth();
	int loopLen = getScreenHeight();
	uint16 *dBuffer = fb_backBuffer();
	uint16 *sBuffer = (uint16 *)tmpData;
	
	if(curPicture.max_Y < getScreenHeight())
	{
		loopLen = curPicture.max_Y;
		dBuffer += 256*((getScreenHeight() - curPicture.max_Y)/2);
		offsetY = 0;
	}
	
	if(curPicture.max_X < getScreenWidth())
	{
		readLen = curPicture.max_X;
		dBuffer += (((getScreenWidth() - curPicture.max_X) / 2) >> 1) << 1;
		offsetX = 0;
	}	
	
	sBuffer += offsetY * curPicture.max_X;
	sBuffer += (offsetX >> 1) << 1;
	
	for(int i=0;i<loopLen;i++)
	{
		// old way was memcpy
		memcpySetBit15(dBuffer,sBuffer,readLen * 2);
		
		dBuffer += 256;
		sBuffer += curPicture.max_X;
	}
}

void exitPictureScreen()
{
	freePicture();
	setOldOrientation();
}

void drawPictureScreen()
{
	int wX = 256;
	int wY = 192;
	
	if(!isImageLandscape())
	{
		wX = 192;
		wY = 256;
	}
	
	if(!picLoaded)
	{
		setFont((uint16 **)font_gautami_10);
		curPicture.resetFunction = resetCounter;
		setBGColor(genericFillColor, &curPicture);
		enableGIFFileStreaming();
		setLargeDimensions(wX,wY); // normal thumbnail
		loading = true;
		loadImage(fileName, &curPicture, wX - 6, wY - 57);
		loading = false;
		
		picLoaded = true;
		
		loc_X = 0;
		loc_Y = 0;
	}
	
	if(isImageLandscape())
	{
		setNewOrientation(ORIENTATION_0);
	}
	else
	{
		switch(getHand())
		{
			case HAND_LEFT:
				setNewOrientation(ORIENTATION_90);
				break;
			case HAND_RIGHT:
				setNewOrientation(ORIENTATION_270);
				break;
		}	
	}
	
	int tX;
	int tY;
	
	if(curPicture.max_X < getScreenWidth())
		tX = (getScreenWidth() - curPicture.max_X) / 2;
	else
		tX = loc_X;
	
	if(curPicture.max_Y < getScreenHeight())
		tY = (getScreenHeight() - curPicture.max_Y) / 2;
	else
		tY = loc_Y;
	
	if(!curPicture.isLarge)
	{
		if(curPicture.picType == PIC_JPEG || curPicture.picType == PIC_PNG || curPicture.picType == PIC_BMP || curPicture.picType == PIC_BIN || curPicture.picType == PIC_GIF)
		{
			if(curPicture.picData != NULL)
				fb_dispSprite(tX,tY,curPicture.picData,genericFillColor);
			
			return;
		}
		
		if(curPicture.picType == PIC_ANI)
		{
			if(!curPicture.usingFile)
			{
				fb_dispSprite(tX,tY,(uint16 *)(curPicture.aniPointers[curPicture.curAni].data),genericFillColor);
				return;
			}
			
			copyFileData(&curPicture);
			fb_dispSprite(tX,tY,(uint16 *)curPicture.rawData,genericFillColor);			
		}
		
		return;
	}

	if(curPicture.picType == PIC_JPEG || curPicture.picType == PIC_PNG || curPicture.picType == PIC_BMP || curPicture.picType == PIC_BIN || curPicture.picType == PIC_GIF)
	{
		displayPicture(tX,tY, curPicture.rawData);
		return;
	}
	
	if(curPicture.picType == PIC_ANI)
	{
		if(!curPicture.usingFile)
		{
			displayPicture(tX,tY,curPicture.aniPointers[curPicture.curAni].data);
			return;
		}
		
		copyFileData(&curPicture);
		displayPicture(tX,tY,curPicture.rawData);
	}
}

void movePicture(int x, int y)
{
	if(x < curPicture.p_X || y < (curPicture.p_Y + 19))
		return;
	
	if(x > (int)(curPicture.p_X + getSpriteWidth(curPicture.picData)) || y > (int)((curPicture.p_Y + 19) + getSpriteHeight(curPicture.picData)))
		return;
	
	int fX = x - curPicture.p_X; // set top left to 0,0
	int fY = y - (curPicture.p_Y + 19); // set top left to 0,0
	int eX = getSpriteWidth(curPicture.picData);
	int eY = getSpriteHeight(curPicture.picData);
	
	if(curPicture.max_X - curPicture.adjust > getScreenWidth())
	{
		int lX = (int)((double)(curPicture.max_X * fX) / (double)eX);
		
		lX -= getScreenWidth() / 2;
		
		if(lX < 0)
			lX = 0;
		if(lX > curPicture.max_X - getScreenWidth() - curPicture.adjust)
			lX = curPicture.max_X - getScreenWidth() - curPicture.adjust;
		
		loc_X = lX;
	}	
	
	if(curPicture.max_Y > getScreenHeight())
	{
		int lY = (int)((double)(curPicture.max_Y * fY) / (double)eY);		
		
		lY -= getScreenHeight() / 2;
		
		if(lY < 0)
			lY = 0;
		if(lY > curPicture.max_Y - getScreenHeight())
			lY = curPicture.max_Y - getScreenHeight();
		
		loc_Y = lY;
	}		
}

void drawZoomScreen()
{
	setColor(genericTextColor);
	setFont((uint16 **)font_arial_9);
	
	if(curPicture.error)
	{
		bg_setClipping(10, 30, getScreenWidth() - 10, 135);
		bg_dispString(0,0,l_rendererror);	
	}
	else
	{
		char str[64];
		sprintf(str,"%dx%d", curPicture.max_X - curPicture.adjust, curPicture.max_Y);
		bg_dispString(centerOnPt(getScreenCenter(), str, (uint16 **)font_arial_9),3, str);
		
		if(curPicture.isLarge && curPicture.picData != NULL)
		{
			int tX;
			int tY;
			
			if(curPicture.max_X < getScreenWidth())
				tX = 0;
			else
				tX = loc_X;
			
			if(curPicture.max_Y < getScreenHeight())
				tY = 0;
			else
				tY = loc_Y;
			
			bg_dispSprite(curPicture.p_X, curPicture.p_Y + 19, curPicture.picData, genericFillColor);
			
			int t_X = (int)((double)tX * curPicture.ratX);
			int t_Y = (int)((double)tY * curPicture.ratY);
			int b_X = 0;
			int b_Y = 0;
			
			if(curPicture.max_X > getScreenWidth())
				b_X = (int)((double)getScreenWidth() * curPicture.ratX);
			else
				b_X = (int)((double)curPicture.max_X * curPicture.ratX);
			
			if(curPicture.max_Y > getScreenHeight())	
				b_Y = (int)((double)getScreenHeight() * curPicture.ratY);
			else
				b_Y = (int)((double)curPicture.max_Y * curPicture.ratY);
			
			t_X += curPicture.p_X;
			t_Y += curPicture.p_Y + 19;
			
			bg_drawBox(t_X, t_Y, t_X + b_X, t_Y + b_Y, pictureSelectBorderColor);
		}
		else
		{
			setColor(genericTextColor);
			bg_setClipping(10, 30, getScreenWidth() - 10, 135);
			bg_dispString(0,0,l_preview);
		}
	}
	
	if(getMode() != PICTUREVIEWER)
		return;
	
	bg_setDefaultClipping();
	
	drawButtonTexts(l_reset, l_back);
	drawLR(l_prevfile, l_nextfile);
}
