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
#include <libdt/libdt.h>
#include "font_arial_11.h"
#include "font_gautami_10.h"
#include "font_arial_9.h"
#include "controls.h"
#include "general.h"
#include "language.h"
#include "colors.h"
#include "keyboard.h"
#include "graphics.h"
#include "settings.h"
#include "globals.h"
#include "drawing/calculator.h"
#include "drawing/soundplayer.h"

int default_scroll_left = DEFAULT_SCROLL_LEFT_R;
int list_left = LIST_LEFT_R;
int list_right = LIST_RIGHT_R;

extern bool reverseDate;
extern bool milTime;

// for keyboard drawing
extern char me_upper[42];
extern char me_lower[42];
extern char me_number[42];
extern char me_upperspecial[42];
extern char me_lowerspecial[42];

static bool dispScrollHandle = true;
static char *lButtonPtr = NULL;
static char *rButtonPtr = NULL;

// for testing scroll
static int scrollTop = 0;
static int scrollBottom = 0;
static int scrollLeft = 0;

// for testing list
static int listTop = 0;
static int listBottom = 0;
static int listLeft = 0;
static int listRight = 0;
static int listCur = 0;
static int listMax = 0;

// for left/right handed
static int handMode = HAND_RIGHT;

// for orientation
static int curOrientation = -1;
static int oldOrientations[10];
static int oldOPosition = 0;

void pushOrientation(int orientation)
{
	if(oldOPosition == MAX_ORIENTATIONS)
	{
		return;
	}
	
	oldOrientations[oldOPosition] = orientation;
	oldOPosition++;
}

int popOrientation()
{
	if(oldOPosition == 0)
	{
		return ORIENTATION_0;
	}
	
	oldOPosition--;
	return oldOrientations[oldOPosition];
}

void setOrientation(int orientation)
{
	// Setting while already in a mode causes flicker
	if(curOrientation == orientation)
		return;
	
	fb_setOrientation(orientation);
	bg_setOrientation(orientation);
	
	curOrientation = orientation;
}

void setNewOrientation(int orientation)
{
	// Setting while already in a mode causes flicker
	if(curOrientation == orientation)
		return;
	
	if(curOrientation != -1)
	{
		// Push old orientation on the stack
		pushOrientation(curOrientation);
	}
	
	// Set orientation
	setOrientation(orientation);
}

void setOldOrientation()
{
	// Pop orientation off the stack and set it
	setOrientation(popOrientation());
}

int getOrientation()
{
	return curOrientation;
}

int getLLocation()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (O0_FORWARDRIGHT + O0_FORWARDLEFT) / 2;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (O90_FORWARDRIGHT + O90_FORWARDLEFT) / 2;
	}
	
	return 0;
}


int getRLocation()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (O0_BACKLEFT + O0_BACKRIGHT) / 2;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (O90_BACKLEFT + O90_BACKRIGHT) / 2;
	}
	
	return 0;
}

int getScreenCenter()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return O0_CENTER;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return O90_CENTER;
	}
	
	return 0;
}

int getScreenWidth()
{
	return getScreenCenter() * 2;
}

int getScreenHeight()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return O0_HEIGHT;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return O90_HEIGHT;
	}
	
	return 0;
}

void drawTimeDate()
{
	char str[128];
	setFont((uint16 **)font_arial_11);
	
	setColor(homeDateTextColor);	
	fb_drawRect(0,0, 255, 32, homeDateFillColor);
	
	if(reverseDate)
		sprintf(str,"%s, %d %s, %d", l_days[getDayOfWeek()], getDay(), l_months[getMonth()-1], getYear());
	else
		sprintf(str,"%s, %s %d, %d", l_days[getDayOfWeek()], l_months[getMonth()-1], getDay(), getYear());
	
	fb_dispString(3,3-2,str);			
	
	if(milTime)
		sprintf(str, "%02d%c%02d%c%02d", getHour(true), ':', getMinute(), ':', getSecond());
	else
		sprintf(str, "%d%c%02d%c%02d %s", getHour(false), ':', getMinute(), ':', getSecond(), l_ampm[isPM()]);
	fb_dispString(3,18-2,str);	
	
	fb_drawRect(0,33,255,33,homeDateBorderColor);
	fb_drawRect(0,34,255,35,homeDateBorderFillColor);
	fb_drawRect(0,36,255,36,homeDateBorderColor);
}

void drawHome()
{
	char str[64];
	
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);
	sprintf(str,"%c\a\a%s", BUTTON_START, l_home);
	
	bg_dispString(centerOnPt(getScreenCenter(),str,(uint16 **)font_gautami_10), 3, str);
}

bool isHome(int px, int py)
{
	if(!isTopArea(px, py))
		return false;
	
	char str[128];
	
	switch(getMode())
	{
		case VIEWER:
		case SOUNDRECORDER:
			sprintf(str,"%c\a\a%s", BUTTON_START, l_back);	
			break;
		case CONFIGURATION:
			sprintf(str,"%c\a\a%s", BUTTON_START, l_more);	
			break;
		case EDITSCRIBBLE:
		case CHOOSECOLOR:
		case EDITREMINDER:
		case EDITDAYVIEW:
		case EDITADDRESS:
		case BROWSERRENAME:
		case TEXTEDITOR:
		case EDITTODOLIST:
		case SCRIBBLETEXT:
			sprintf(str,"%c\a\a%s", BUTTON_START, l_swap);	
			break;
		case SOUNDPLAYER:
			sprintf(str,"%c\a\a%s", BUTTON_START, l_hold);	
			break;
		default:
			sprintf(str,"%c\a\a%s", BUTTON_START, l_home);	
			break;
	}
	
	int offsetPt = 0;
	int centerPt = 0;
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			centerPt = O0_CENTER;
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			centerPt = O90_CENTER;
			break;
	}
	
	offsetPt = centerPt - centerOnPt(centerPt, str, (uint16 **)font_gautami_10);	
	return (px >= centerPt - offsetPt) && (px <= centerPt + offsetPt);
}

void drawFilledRect(int x, int y, int bx, int by, uint16 borderColor, uint16 fillColor, uint16 *buffer, int size)
{
	// Draw sides
	drawRect(x, y, bx, y, borderColor, buffer, size);
	drawRect(x, by, bx, by, borderColor, buffer, size);
	drawRect(x, y, x, by, borderColor, buffer, size);
	drawRect(bx, y, bx, by, borderColor, buffer, size);
	
	// Draw center
	drawRect(x + 1, y + 1, bx - 1, by - 1, fillColor, buffer, size);
}

void drawKeyboard()
{
	drawKeyboardSpecific(3, BUTTON_START, l_swap, bg_backBuffer());
}

void drawKeyboardSpecific(int y, char button, char *text, uint16 *bb)
{
	drawKeyboardSpecificEX(0, y, button, text, bb);
}

void drawKeyboardSpecificEX(int x, int y, char button, char *text, uint16 *bb)
{
	char tStr[64];
	
	setFont((uint16 **)font_gautami_10);
	
	if(button != 0)
	{
		setColor(genericTextColor);	
		sprintf(tStr,"%c\a\a%s", button, text);
		dispString(centerOnPt(128,tStr, (uint16 **)font_gautami_10), y, tStr, bb, 1, 0, 0, 255, 191);
	}
	
	setColor(keyboardTextColor);
	
	switch(activeKeyboard())
	{
		case KB_QWERTY:
		{
			drawFilledRect(x+KEYBOARD_LEFT+200, KEYBOARD_TOP+19, x+KEYBOARD_LEFT+228, KEYBOARD_TOP+38, keyboardBorderColor, pressedChar() == BSP ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+190, KEYBOARD_TOP+38, x+KEYBOARD_LEFT+228, KEYBOARD_TOP+57, keyboardBorderColor, pressedChar() == RET ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+76, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+171, KEYBOARD_TOP+95, keyboardBorderColor, pressedChar() == SPC ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+19, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+38, KEYBOARD_TOP+95, keyboardBorderColor, pressedChar() == DEL ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+38, x+KEYBOARD_LEFT+19, KEYBOARD_TOP+57, keyboardBorderColor, isCaps() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+57, x+KEYBOARD_LEFT+29, KEYBOARD_TOP+76, keyboardBorderColor, isShift() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+19, KEYBOARD_TOP+95, keyboardBorderColor, isSpecial() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			
			dispCustomSprite(x+13 + 02, 37 + 24 + 21, k_caps, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 04, 37 + 60 + 23, k_spl, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 05, 37 + 42 + 22, k_shift, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 23, 37 + 60 + 23, k_del, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 114, 37 + 60 + 23, k_space, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 208, 37 + 5 + 20, k_bsp, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 194, 37 + 22 + 21, k_ret, 31775, 0xFFFF, bb, 255, 191);
			
			char *str;
			bool doIt = false;
			
			if((!isCaps() && !isShift()) || (isCaps() && isShift()))
			{
				
				if(!isSpecial())
				{
					// lowercase
					str = l_lowercase;		
					doIt = true;
				}
				else
				{
					// special 0
					str = l_special0;		
					doIt = true;
				}
			}
			else
			{
				if(!isSpecial())
				{
					// uppercase
					str = l_uppercase;		
					doIt = true;
				}
				else
				{
					// special 1
					str = l_special1;
					doIt = true;
				}
			}
			
			if(doIt)
			{
				int j = 0;
				for(uint16 i=0;i<=9;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+10+(i*19), KEYBOARD_TOP+19, x+KEYBOARD_LEFT+10+(i*19)+19, KEYBOARD_TOP+20+18, keyboardBorderColor, tColor, bb, 192);						
					dispChar(x+13+11+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+18+8, str[i+j], bb, 255, 191);
				}
				
				j = 10;
				for(uint16 i=0;i<=8;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+19+(i*19), KEYBOARD_TOP+38, x+KEYBOARD_LEFT+19+(i*19)+19, KEYBOARD_TOP+39+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+21+(i*19)+8 - (getCharWidth(str[i+j])/2), 37+36+9, str[i+j], bb, 255, 191);
				}
				
				j = 19;
				for(uint16 i=0;i<=9;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+29+(i*19), KEYBOARD_TOP+57, x+KEYBOARD_LEFT+29+(i*19)+19, KEYBOARD_TOP+58+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+30+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+54+10, str[i+j], bb, 255, 191);
				}
				
				j = 29;
				for(uint16 i=0;i<=1;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+38+(i*19), KEYBOARD_TOP+76, x+KEYBOARD_LEFT+38+(i*19)+19, KEYBOARD_TOP+77+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+39+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+72+11, str[i+j], bb, 255, 191);
				}
				
				j = 31;
				for(uint16 i=0;i<=2;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+171+(i*19), KEYBOARD_TOP+76, x+KEYBOARD_LEFT+171+(i*19)+19, KEYBOARD_TOP+77+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+171+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+72+11, str[i+j], bb, 255, 191);
				}		
			}
			
			if(!isShift())
			{
				// numbers
				for(uint16 i=0;i<=11;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && (pressedChar() == l_numbers[i] || pressedChar() == l_symbols[i]))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+(i*19), KEYBOARD_TOP, x+KEYBOARD_LEFT+(i*19)+19, KEYBOARD_TOP+19, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+(i*19)+9 - (getCharWidth(l_numbers[i])/2), 37+7, l_numbers[i], bb, 255, 191);
				}
			}
			else
			{
				// symbols	
				for(uint16 i=0;i<=11;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && (pressedChar() == l_numbers[i] || pressedChar() == l_symbols[i]))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+(i*19), KEYBOARD_TOP, x+KEYBOARD_LEFT+(i*19)+19, KEYBOARD_TOP+19, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+(i*19)+9 - (getCharWidth(l_symbols[i])/2), 37+7, l_symbols[i], bb, 255, 191);
				}	
			}
			
			break;
		}
		case KB_MESSAGEASE:
		{
			uint16 tColor[17] = { 	keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardSpecialColor, keyboardSpecialColor, 
									keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardSpecialColor, keyboardSpecialColor, 
									keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardABCSwapColor };
			
			switch(pressedChar())
			{
				case BSP:
					tColor[5] = keyboardHighlightColor;
					break;
				case RET:
					tColor[11] = keyboardHighlightColor;
					break;
				case SPC:
					tColor[10] = keyboardHighlightColor;
					break;
				case DEL:
					tColor[4] = keyboardHighlightColor;
					break;
			}
			
			if(isCaps())
				tColor[0] = keyboardSpecialHighlightColor;
			if(isShift())
				tColor[6] = keyboardSpecialHighlightColor;
			if(isSpecial())
				tColor[12] = keyboardSpecialHighlightColor;
			
			for(int wB = 0;wB < 17; wB++)
			{
				int wbX = wB % 6;
				int wbY = wB / 6;
				
				drawFilledRect(32 + (wbX * 32), 37 + (wbY * 32), 32 + ((wbX + 1) * 32), 37 + ((wbY + 1) * 32), keyboardBorderColor, tColor[wB], bb, 192);
			}
			
			dispCustomSprite(32 + 8, 37 + 13, k_caps, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 7, 37 + 45, k_shift, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 10, 37 + 77, k_spl, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 170, 37 + 12, k_bsp, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 161, 37 + 43, k_ret, 31775, 0xFFFF, bb, 255, 191);	
			dispCustomSprite(32 + 138, 37 + 13, k_del, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 134, 37 + 45, k_space, 31775, 0xFFFF, bb, 255, 191);
			
			if(activeView() == ACTIVE_NUMBERS)
				dispString(32 + 133, 37 + 76, "ABC", bb, 1, 0, 0, 255, 191);
			else
				dispString(32 + 135, 37 + 76, "123", bb, 1, 0, 0, 255, 191);
			
			char *str = NULL;
			int adjVal = 0;
			
			switch(activeView())
			{
				case ACTIVE_LOWERCASE:
					str = me_lower;
					adjVal = 1;
					break;
				case ACTIVE_UPPERCASE:
					str = me_upper;
					break;
				case ACTIVE_NUMBERS:
					str = me_number;
					break;
				case ACTIVE_LOWERCASE_SPECIAL:
					str = me_lowerspecial;
					break;
				case ACTIVE_UPPERCASE_SPECIAL:
					str = me_upperspecial;
					break;
			}
			
			int j = 0;
			
			for(int i=0;i<9;i++)
			{
				int tX = i % 3;
				int tY = i / 3;
				
				dispChar(32+49+(tX * 32) - (getCharWidth(str[i+j])/2), 37+12+(tY * 32), str[i+j], bb, 255, 191);
			}
			
			setColor(keyboardMEDSTextColor);
			
			j = 9;
			dispChar(32+63 - (getCharWidth(str[j])), 37+23, str[j], bb, 255, 191);
			j = 10;
			dispChar(32+80 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 11;
			dispChar(32+99, 37+23, str[j], bb, 255, 191);
			j = 12;
			dispChar(32+63 - (getCharWidth(str[j])), 37+44, str[j], bb, 255, 191);
			
			j = 13;
			for(int i=0;i<9;i++)
			{
				int x = i % 3;
				int y = i / 3;
				int z = 0;
				
				if(y == 1 && adjVal == 1)
					z = 1;
				
				dispChar(32+66+(x * 11) + adjVal, 37+34+(y * 10) - adjVal + z, str[i+j], bb, 255, 191);
			}
			
			j = 22;
			dispChar(32+99, 37+44, str[j], bb, 255, 191);
			j = 23;
			dispChar(32+63 - (getCharWidth(str[j])), 37+65, str[j], bb, 255, 191);
			j = 24;
			dispChar(32+80 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);
			j = 25;
			dispChar(32+95 - (getCharWidth(str[j])), 37+76, str[j], bb, 255, 191);
			j = 26;
			dispChar(32+99, 37+65, str[j], bb, 255, 191);
			
			// extra shit that we are gonig to fill in anywhere
			// yea, yea, i know it's messy, so what?
			j = 27;
			dispChar(32+63 - (getCharWidth(str[j])), 37+12, str[j], bb, 255, 191);
			j = 28;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 29;
			dispChar(32+66, 37+12, str[j], bb, 255, 191);
			j = 30;
			dispChar(32+96 - (getCharWidth(str[j])), 37+12, str[j], bb, 255, 191);			
			j = 31;
			dispChar(32+99, 37+12, str[j], bb, 255, 191);
			j = 32;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 33;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+34, str[j], bb, 255, 191);
			j = 34;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+34, str[j], bb, 255, 191);			
			j = 35;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+54, str[j], bb, 255, 191);
			j = 36;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+54, str[j], bb, 255, 191);			
			j = 37;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);
			j = 38;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);			
			j = 39;
			dispChar(32+63 - (getCharWidth(str[j])), 37+76, str[j], bb, 255, 191);		
			j = 40;
			dispChar(32+66, 37+76, str[j], bb, 255, 191);
			j = 41;
			dispChar(32+99, 37+76, str[j], bb, 255, 191);
			j = 42;
			dispChar(32+120, 37+76, str[j], bb, 255, 191);
			
			break;
		}
	}
	
	drawSubKeys(bb);
}

void drawSubKeys(uint16 *bb)
{
	setFont((uint16 **)font_gautami_10);
	setColor(keyboardTextColor);
	
	char *insStr = NULL;
	
	if(isInsert())
	{
		insStr = "Ins";
	}
	else
	{
		insStr = "Ovr";
	}
	
	// draw insert
	drawRect(INS_LEFT, INS_TOP, INS_RIGHT, INS_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(INS_LEFT + 1, INS_TOP + 1, INS_RIGHT - 1, INS_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(INS_CENTER, insStr, (uint16 **)font_gautami_10), INS_TOP + 5, insStr, bb, 1, 0, 0, 255, 191);
	
	// draw clear
	if(getMode() != TEXTEDITOR)
	{
		drawRect(CLEAR_LEFT, CLEAR_TOP, CLEAR_RIGHT, CLEAR_BOTTOM, keyboardBorderColor, bb, 255);
		drawRect(CLEAR_LEFT + 1, CLEAR_TOP + 1, CLEAR_RIGHT - 1, CLEAR_BOTTOM - 1, keyboardFillColor, bb, 255);
		dispString(centerOnPt(CLEAR_CENTER, "Clear", (uint16 **)font_gautami_10), CLEAR_TOP + 5, "Clear", bb, 1, 0, 0, 255, 191);
	}
	
	// draw control
	drawRect(CTRL_LEFT, CTRL_TOP, CTRL_RIGHT, CTRL_BOTTOM, keyboardBorderColor, bb, 255);
	
	uint16 tColor = keyboardFillColor;	
	if(isControl())
		tColor = keyboardSpecialHighlightColor;

	drawRect(CTRL_LEFT + 1, CTRL_TOP + 1, CTRL_RIGHT - 1, CTRL_BOTTOM - 1, tColor, bb, 255);
	dispString(centerOnPt(CTRL_CENTER, "Ctrl", (uint16 **)font_gautami_10), CTRL_TOP + 5, "Ctrl", bb, 1, 0, 0, 255, 191);
	
	if(isSelect())
	{
		insStr = "Select";
	}
	else
	{
		insStr = "Move";
	}
	
	// draw select
	drawRect(SELECT_LEFT, SELECT_TOP, SELECT_RIGHT, SELECT_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(SELECT_LEFT + 1, SELECT_TOP + 1, SELECT_RIGHT - 1, SELECT_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(SELECT_CENTER, insStr, (uint16 **)font_gautami_10), SELECT_TOP + 5, insStr, bb, 1, 0, 0, 255, 191);
}

void drawDelete(uint16 *bb)
{
	setFont((uint16 **)font_gautami_10);
	setColor(keyboardTextColor);
	
	drawRect(DELETE_LEFT, DELETE_TOP, DELETE_RIGHT, DELETE_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(DELETE_LEFT + 1, DELETE_TOP + 1, DELETE_RIGHT - 1, DELETE_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(DELETE_CENTER, "Delete", (uint16 **)font_gautami_10), DELETE_TOP + 5, "Delete", bb, 1, 0, 0, 255, 191);
}

int drawLeftButton()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			bg_drawFilledRect(O0_FORWARDLEFT, O0_TOP, O0_FORWARDRIGHT, O0_BOTTOM, widgetBorderColor, widgetFillColor);
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			bg_drawFilledRect(O90_FORWARDLEFT, O90_TOP, O90_FORWARDRIGHT, O90_BOTTOM, widgetBorderColor, widgetFillColor);
			break;
	}
	
	return getLLocation();
}

int drawRightButton()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			bg_drawFilledRect(O0_BACKLEFT, O0_TOP, O0_BACKRIGHT, O0_BOTTOM, widgetBorderColor, widgetFillColor);	
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			bg_drawFilledRect(O90_BACKLEFT, O90_TOP, O90_BACKRIGHT, O90_BOTTOM, widgetBorderColor, widgetFillColor);	
			break;
	}
	
	return getRLocation();
}

void drawButtonTextsEX(char lBut, char *lText, char rBut, char *rText)
{
	setFont((uint16 **)font_arial_9);
	setColor(widgetTextColor);
	
	bg_setDefaultClipping();
	
	char str[128];
	int bottomLoc = O0_TOP;
	
	if(curOrientation == ORIENTATION_90 || curOrientation == ORIENTATION_270)
	{
		bottomLoc = O90_TOP;
	}
	
	if(lText != NULL)
	{
		sprintf(str, "%c\a%s", lBut, lText);
		bg_dispString(centerOnPt(drawLeftButton(), str, (uint16 **)font_arial_9),bottomLoc + 3,str);	
	}
	
	if(rText != NULL)
	{
		sprintf(str, "%c\a%s", rBut, rText);
		bg_dispString(centerOnPt(drawRightButton(), str, (uint16 **)font_arial_9),bottomLoc + 3,str);		
	}
}

void drawButtonTexts(char *lText, char *rText)
{	
	char aButton = BUTTON_A;
	char bButton = BUTTON_B;
	
	if(isABSwapped())
	{
		aButton = BUTTON_B;
		bButton = BUTTON_A;
	}
	
	drawButtonTextsEX(aButton, lText, bButton, rText);
}

bool isButtons(int tx, int ty)
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (ty >= O0_TOP && ty <= O0_BOTTOM);
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (ty >= O90_TOP && ty <= O90_BOTTOM);
	}
	
	return false;
}

bool isForwardButton(int tx, int ty)
{
	if(!isButtons(tx, ty))
		return false;
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (tx >= O0_FORWARDLEFT && tx <= O0_FORWARDRIGHT);
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (tx >= O90_FORWARDLEFT && tx <= O90_FORWARDRIGHT);
	}
	
	return false;
}

bool isBackButton(int tx, int ty)
{
	if(!isButtons(tx, ty))
		return false;
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (tx >= O0_BACKLEFT && tx <= O0_BACKRIGHT);
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (tx >= O90_BACKLEFT && tx <= O90_BACKRIGHT);
	}
	
	return false;
}

void displayScrollHandle(bool t)
{
	dispScrollHandle = t;
}

void drawScrollBarEXEX(int pos, int max, bool isHeld, void (*callBack)(), int x, int y, int width, int height)
{
	int bottom = y + height;
	int right = x + width;
	double z;
	
	if(max <= 0)
	{
		pos = 0;
		max = 1;
	}
	
	bg_drawFilledRect(x, y, right, y + SCROLL_ARROW_HEIGHT, widgetBorderColor, widgetFillColor);
	bg_drawFilledRect(x, bottom - SCROLL_ARROW_HEIGHT, right, bottom, widgetBorderColor, widgetFillColor);
	bg_drawFilledRect(x, y + SCROLL_ARROW_HEIGHT + 2, right, bottom - SCROLL_ARROW_HEIGHT - 2, widgetBorderColor, scrollFillColor);

	bg_dispCustomSprite(x + 2, y + 4, up_arrow, 31775, scrollNormalColor);
	bg_dispCustomSprite(x + 2, bottom - SCROLL_ARROW_HEIGHT + 5, down_arrow, 31775, scrollNormalColor);
	
	// set the metrics for the test
	scrollTop = y + SCROLL_ARROW_HEIGHT + 4;
	scrollBottom = bottom - SCROLL_ARROW_HEIGHT - 4;
	scrollLeft = x;
	
	// mark where we have stuff already

	if(callBack)
		callBack();
	
	if(!dispScrollHandle)
		return;
	
	int verticalPlay = height - (2 * (SCROLL_ARROW_HEIGHT + 4));
	verticalPlay -= SCROLL_HANDLE_HEIGHT;
	
	z = (double)verticalPlay * (double)pos;
	z = z / (double)(max-1);
	
	int yLoc = y + SCROLL_ARROW_HEIGHT + 4 + (int)z;
	
	if(yLoc + SCROLL_HANDLE_HEIGHT > bottom - SCROLL_ARROW_HEIGHT - 4)
		yLoc = bottom - SCROLL_ARROW_HEIGHT - 4 - SCROLL_HANDLE_HEIGHT;
	
	if(!isHeld)
		bg_drawRect(x + 2, yLoc, right - 2, yLoc + SCROLL_HANDLE_HEIGHT, scrollNormalColor);
	else
		bg_drawRect(x + 2, yLoc, right - 2, yLoc + SCROLL_HANDLE_HEIGHT, scrollHighlightColor);
}

void drawScrollBar(int pos, int max, int x, int y, int width, int height)
{
	drawScrollBarEXEX(pos, max, isScrolling(), 0, x, y, width, height);
}

void drawScrollBarEX(int pos, int max, bool isHeld, int x, int y, int width, int height)
{
	drawScrollBarEXEX(pos, max, isHeld, 0, x, y, width, height);
}

void drawScrollBarCustom(int pos, int max, void (*callBack)(), int x, int y, int width, int height)
{
	drawScrollBarEXEX(pos, max, isScrolling(), callBack, x, y, width, height);
}

int getScroll(int py, int max)
{
	// handle being too far off
	if(py < scrollTop)
		return 0;
	if(py > scrollBottom)
		return max - 1;
	
	// relative position in relation to the top of the scroll area
	int relPos = py - (scrollTop + (SCROLL_HANDLE_HEIGHT / 2));
	int verticalPlay = (scrollBottom - scrollTop) - SCROLL_HANDLE_HEIGHT;
	
	double z = (double)(relPos) * (double)(max-1);
	z = z / (double)(verticalPlay);
	
	if(z < 0)
		return 0;
	if(z > (max - 1))
		return max - 1;
	
	return (int)z;
}

bool isTopArea(int px, int py)
{
	return (py < TOPAREA);
}

bool isLButton(int px, int py)
{
	if(lButtonPtr == 0)
		return false;
	if(!isTopArea(px, py))
		return false;
	
	char str[128];
	sprintf(str,"%c\a\a%s", BUTTON_L, lButtonPtr);	
	
	return (px <= getStringWidth(str, (uint16 **)font_gautami_10) && px != 0);
}

bool isRButton(int px, int py)
{
	if(rButtonPtr == 0)
		return false;
	if(!isTopArea(px, py))
		return false;
	
	char str[128];
	sprintf(str,"%s\a\a%c", rButtonPtr, BUTTON_R);
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (px >= (O0_RBUTTON - getStringWidth(str, (uint16 **)font_gautami_10)));
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (px >= (O90_RBUTTON - getStringWidth(str, (uint16 **)font_gautami_10)));
	}
	
	return false;	
}

void drawLREX(char *lText, bool lEnabled, char *rText, bool rEnabled)
{
	// for metrics
	lButtonPtr = lText;
	rButtonPtr = rText;
	
	char str[128];
	
	setFont((uint16 **)font_gautami_10);
	
	if(lEnabled)
		setColor(genericTextColor);
	else
		setColor(soundHoldColor);
		
	sprintf(str,"%c\a\a%s", BUTTON_L, lText);	
	bg_dispString(3, 3, str);
	
	if(rEnabled)
		setColor(genericTextColor);
	else
		setColor(soundHoldColor);
		
	sprintf(str,"%s\a\a%c", rText, BUTTON_R);
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			bg_dispString(O0_RBUTTON-getStringWidth(str, (uint16 **)font_gautami_10),3, str);
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			bg_dispString(O90_RBUTTON-getStringWidth(str, (uint16 **)font_gautami_10),3, str);
			break;
	}
}

void drawLR(char *lText, char *rText)
{
	drawLREX(lText, true, rText, true);
}

int getCursorFromTouch(int u, int v)
{
	int z = 0;
	int y = 0;
	int tc = -1;
	
	if(!(u >= listLeft && u <= listRight))
		return -1;
	
	// metrics
	uint16 numEntries = (((listBottom - listTop) - 4) / LIST_STEP) + 1;
	uint16 lowThreshold = numEntries / 2;
	uint16 highThreshold = lowThreshold + 1;
	
	if(listMax <= numEntries)
	{
		y = listMax;
		z = 0;
	}
	else
	{
		if(listCur < lowThreshold)
			z = 0;
		else if(listCur > listMax - highThreshold)
			z = listMax - numEntries;
		else
			z = listCur - lowThreshold;
		
		y = z + numEntries;
	}
	
	// in case it ever flows over
	if(y > listMax)
		y = listMax;
	
	for(uint16 x=z;x<y;x++)
	{
		if(v >= (listTop + ((x - z) * LIST_STEP)) && v <= ((listTop + LIST_STEP) + ((x - z) * LIST_STEP)))
			tc = x;
	}
	
	if(tc > listMax)
		tc = -1;
	
	return tc;
}

void drawListBox(int tx, int ty, int bx, int by, int curEntry, int maxEntries, char *noString, void (*callback)(int, int, int))
{
	// make sure we are inside the bounds and set up for defaults
	setFont((uint16 **)font_arial_9);
	setColor(genericTextColor);
	bg_setClipping(tx + 2, ty + 3, bx - 2, by);
	
	// set metrics for testing
	listTop = ty;
	listBottom = by;
	listLeft = tx;
	listRight = bx;
	
	listCur = curEntry;
	listMax = maxEntries;
	
	if(maxEntries == 0)
	{
		displayScrollHandle(false);
		
		setColor(genericTextColor);
		bg_dispString(0, 0, noString);
		return;
	}	
	
	bg_drawFilledRect(tx, ty, bx, by, listBorderColor, listFillColor);
	
	// temporary locations
	uint16 z = 0;
	uint16 y = 0;
	
	// metrics
	uint16 numEntries = (((by - ty) - 4) / LIST_STEP) + 1;
	uint16 lowThreshold = numEntries / 2;
	uint16 highThreshold = lowThreshold + 1;
	
	if(maxEntries <= numEntries)
	{
		y = maxEntries;
		z = 0;
	}
	else
	{
		if(curEntry < lowThreshold)
			z = 0;
		else if(curEntry > maxEntries - highThreshold)
			z = maxEntries - numEntries;
		else
			z = curEntry - lowThreshold;
		
		y = z + numEntries;
	}
	
	// in case it ever flows over
	if(y > maxEntries)
		y = maxEntries;
	
	for(uint16 x=z;x<y;x++)
	{	
		if(x == curEntry)
			bg_drawRect(tx + 2, (ty + 2) + ((x - z) * LIST_STEP), bx - 2, (ty + 14) + ((x - z) * LIST_STEP), genericHighlightColor);
		
		// set clipping so a lazy callback can just display at 0,0
		bg_setClipping(tx + 2, (ty + 1) + ((x - z) * LIST_STEP), bx - 2, (ty + 15) + ((x - z) * LIST_STEP));
		
		// set up stuff for a generic callback
		setColor(listTextColor);
		callback(x, tx + 2, (ty + 2) + ((x - z) * LIST_STEP));
	}
	
	if(maxEntries <= numEntries)
		displayScrollHandle(false);
	else
		displayScrollHandle(true);
	
	bg_setDefaultClipping();
}

void setHand(int m)
{
	handMode = m;
	
	switch(handMode)
	{
		case HAND_LEFT:
			default_scroll_left = DEFAULT_SCROLL_LEFT_L;
			list_left = LIST_LEFT_L;
			list_right = LIST_RIGHT_L;
			
			break;
		case HAND_RIGHT:
			default_scroll_left = DEFAULT_SCROLL_LEFT_R;
			list_left = LIST_LEFT_R;
			list_right = LIST_RIGHT_R;
			
			break;
	}
}

int getHand()
{
	return handMode;
}

void swapHands()
{
	if(handMode == HAND_LEFT)
	{
		setHand(HAND_RIGHT);
	}
	else
	{
		setHand(HAND_LEFT);
	}
}

int getScrollLeft()
{
	return scrollLeft;
}

void drawRoundedButton(int x, int y, int bx, int by, char *text, bool select, bool highlight)
{
	// draw the sides
	uint16 borderColor = widgetBorderColor;	
	if(select)
	{
		borderColor = textEntryHighlightColor;
	}
	
	bg_drawRect(x + 2, y, bx - 2, y, borderColor);
	bg_drawRect(x + 2, by, bx - 2, by, borderColor);
	bg_drawRect(x, y + 2, x, by - 2, borderColor);
	bg_drawRect(bx, y + 2, bx, by - 2, borderColor);
	
	bg_setPixel(x + 1, y + 1, borderColor);
	bg_setPixel(bx - 1, y + 1, borderColor);
	bg_setPixel(x + 1, by - 1, borderColor);
	bg_setPixel(bx - 1, by - 1, borderColor);
	
	// fill the damn thing
	uint16 fillColor = widgetFillColor;
	if(highlight)
	{
		fillColor = genericHighlightColor;
	}
	
	bg_drawRect(x + 2, y + 1, bx - 2, by - 1, fillColor);
	bg_drawRect(x + 1, y + 2, x + 1, by - 2, fillColor);
	bg_drawRect(bx - 1, y + 2, bx - 1, by - 2, fillColor);
	
	// figure out how many lines the text is
	int *pts = NULL;
	int numPts = 0;
	numPts = getWrapPoints(0, 0, text, x + 2, y + 2, bx - 2, by - 2, &pts, (uint16 **)font_gautami_10);
	
	setFont((uint16 **)font_gautami_10);
	setColor(widgetTextColor);
	bg_setClipping(x, y + 2, bx, by - 2);
	
	int firstReturn = -1;
	if(strchr(text, '\n') != NULL)
	{
		firstReturn = strchr(text, '\n') - text;
	}
	
	// put text inside it
	for(int c=0;c<numPts;c++)
	{
		int nextPt = 0;
		if(c == (numPts - 1))
		{
			// last point
			nextPt = strlen(text);
		}
		else
		{
			// grab next
			nextPt = pts[c + 1];
		}
		
		char tmpText[256];
		memset(tmpText, 0, 256);
		memcpy(tmpText, text + pts[c], nextPt - pts[c]);
		
		if(firstReturn != 0 && pts[c] <= firstReturn)
		{
			setBold(true);
		}
		else
		{
			setBold(false);
		}
		
		bg_dispString(centerOnPt(((bx + x) / 2), tmpText, (uint16 **)font_gautami_10) - x, ((by + y) / 2) - y - ((numPts * 10) / 2) + (c * 10), tmpText);
	}
	
	free(pts);
	
	bg_setDefaultClipping();
}

void drawTextBox(int x, int y, int bx, int by, const char *text, bool centered, bool selected, bool italics)
{
	// set up graphics options
	setFont((uint16 **)font_gautami_10);
	setColor(textEntryTextColor);
	
	// draw the box and set up the clipping area
	bg_drawFilledRect(x, y, bx, by, selected ? textEntryHighlightColor : widgetBorderColor, textEntryFillColor);	
	bg_setClipping(x, y, bx, by + 3);
	
	// allocate temporary storage for string to be shortened
	char *tText = (char *)safeMalloc(strlen(text) + 4); // enough room for trailing zero and the "..."
	strcpy(tText, text);
	
	// shorten the string if needed to ensure it fits
	abbreviateString(tText, bx - x, (uint16 **)font_gautami_10);
	
	setItalics(italics);
	
	// display string with centered setting
	if(centered)
	{
		bg_dispString(centerOnPt((bx - x) >> 1, tText, (uint16 **)font_gautami_10), 4, tText);
	}
	else
	{
		bg_dispString(3, 4, tText);
	}
	
	bg_setDefaultClipping();
	setItalics(false);
}
