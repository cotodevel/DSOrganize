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
#include "texteditor.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../font_arial_11.h"
#include "../general.h"
#include "../keyboard.h"
#include "../globals.h"
#include "../language.h"
#include "../settings.h"
#include "../controls.h"

static uint ptCount = 0;
static uint bufferLength = 0;
static int *pts = NULL;
static char *textFile = NULL;
static int filePos = 0;
static bool cursorUpdate = false;
static int textType = 0;
static bool enableSyntax = false;
static char lChar;
static u32 textLines = 11;

static uint16 textColor;
static int hilightType = INI;
static bool isTag = false;
static bool isQuote = false;
static bool hadQuote = false;
static bool justDid = true;
static bool isDoctype = false;
static bool hasLoaded = false;

extern bool copying;
extern u32 maxSize;
extern u32 curSize;
extern char fileName[256];
extern char curDir[256];

void freeText()
{
	if(textFile != NULL)
		trackFree(textFile);
	
	textFile = NULL;
	
	if(pts != NULL)
		free(pts);
	
	pts = NULL;
}

void initTextEditor()
{
	hasLoaded = false;
}

void wrapCalculateTextEditor()
{
	ptCount = getWrapPoints(0, 5, textFile, LEFT, TOP, RIGHT, BOTTOM + (isTextEditorFixed() ? FIXEDPAD : 0), &pts, setTextEditorFont());		
}

int getLocation()
{
	for(uint x=0;x<ptCount-1;x++)
	{
		if(filePos == pts[x])
			return x;
	}
	return -1;
}

int getVirtualLine()
{
	for(uint x = 1;x < ptCount;x++)
	{
		if(getCursor() >= pts[x-1] && getCursor() < pts[x])
			return x-1;
	}
	
	return ptCount-1;
}

int getVirtualColumn()
{
	return getCursor() - pts[getVirtualLine()];
}

int callback(int pos, u32 c, int pass, int xpos, int ypos)
{
	switch(hilightType)
	{
		case INI:			
			if(lChar == '\r' || lChar == '\n') // new line
			{		
				if(c == '[') // section header
				{
					textColor = texteditorSection;
					setBold(true);
					setColor(textColor);
				}
				else if(c == ';') // comment
				{
					textColor = texteditorComment;
					setBold(false);
					setColor(textColor);
				}
				else // property
				{
					textColor = texteditorProperty;
					setBold(true);
					setColor(textColor);
				}
			}
			else if(lChar == '=')
			{
				setBold(false);
				if(textColor == texteditorProperty)
				{
					textColor = texteditorSetting;
					setColor(textColor);
				}
			}
			
			lChar = c;
			break;
		case HTML:
			if(textColor == texteditorComment)
			{
				if(lChar == '!' && c != '>') // figure oot if it's a doctype or a normal comment 
				{
					if(c == '-') // normal comment
						isDoctype = false;
					else
						isDoctype = true;
				}
				
				if(c == '>')
				{
					if(!isDoctype && lChar != '-')
					{						
						lChar = c;
						return 0;
					}
					
					textColor = textAreaTextColor;
					setBold(false);
					setColor(textColor);
					isTag = false;
					isQuote = false;
					hadQuote = false;
					justDid = false;
				}				
				
				lChar = c;
				return 0;
			}
			
			if(lChar == 34)
			{	
				if(isQuote && !justDid)
				{
					textColor = textAreaTextColor;
					setBold(true);
					setColor(textColor);
					isQuote = false;
					hadQuote = false;
					
					if(c == '>')
					{
						if(isTag && !isQuote)
						{
							isTag = false;
							isQuote = false;
							hadQuote = false;
							
							textColor = textAreaTextColor;
							setColor(textColor);
							setBold(false);
							justDid = false;
						}
					}
					lChar = c;
					return 0;
				}
			}		
			
			if(c == '>')
			{
				if(isTag && (!isQuote || (isQuote && hadQuote)))
				{
					isTag = false;
					isQuote = false;
					hadQuote = false;
					
					textColor = textAreaTextColor;
					setColor(textColor);
					setBold(false);
					justDid = false;
					lChar = c;
					return 0;
				}
			}
			
			if(lChar == '<' && !isQuote)
			{
				isTag = true;
				isQuote = false;
				hadQuote = false;
				
				if(c == '!') // comment
				{
					textColor = texteditorComment;
					setColor(textColor);
					setBold(false);
				}
				else // tag
				{
					textColor = texteditorSection;
					setColor(textColor);
					setBold(true);
				}
			}
			
			if(isTag && isQuote && hadQuote)
			{
				if(lChar == ' ' || lChar == '	' || lChar == 13 || lChar == 10 || lChar == 27)
				{
					isQuote = false;
					hadQuote = false;
					textColor = textAreaTextColor;
					setBold(true);
					setColor(textColor);
				}
			}
			
			if(isTag) // hilight individual properties, etc
			{
				if(!isQuote || hadQuote)
				{
					if(c == ' ' || c == '	' || c == 13 || c == 10 || c == 27)
					{
						isQuote = false;
						hadQuote = false;
						textColor = textAreaTextColor;
						setBold(true);
						setColor(textColor);
					}
				}
				
				if(!isQuote)
				{
					if(c == '=')
					{
						textColor = textAreaTextColor;
						setColor(textColor);
						setBold(false);
					}				
				}
				
				if(c == 34 || lChar == '=')
				{
					if(!isQuote)
					{
						if(c == 34)
							hadQuote = false;
						else
							hadQuote = true;
						
						isQuote = true;
						textColor = texteditorSetting;
						setBold(false);
						setColor(textColor);
						justDid = true;
						lChar = c;
						return 0;
					}
				}		
			}
			
			justDid = false;
			lChar = c;
			break;
	}
	
	return 0;
}

void calcCurrentColor()
{
	int x = getLocation();
	
	if(x <= 0)
	{
		textColor = textAreaTextColor;
		setColor(textColor);
		isQuote = false;
		isTag = false;
		hadQuote = false;
		setBold(false);
		justDid = false;
		
		lChar = '\r';
		return;
	}
	
	if(hilightType == HTML)
	{
		textColor = textAreaTextColor;
		isQuote = false;
		isTag = false;
		hadQuote = false;
		setBold(false);
		justDid = false;
		setColor(textColor);
		
		lChar = 0;
		int j = 0;
		
		for(int i=pts[x];i>0;i--)
		{
			if(textFile[i] == '<')
			{
				int p = i;
				
				if(p < 0)
					p = 0;
				
				j = p;
				break;
			}
		}
		
		for(int i=j;i<pts[x];i++)
			callback(0, textFile[i], 0, 0, 0);
	}
	else if(hilightType == INI)
	{
		int z = 0;
		int y = 0;
		
		for(y=x;y>0;y--)
		{
			if(pts[y] == 0) // we hit the start of the file
			{
				z = 0;
				break;
			}
			else
			{
				char c = textFile[pts[y]-1];
				
				if(c == '\r' || c == '\n')
				{
					z = pts[y]-1;
					break;
				}
			}
		}
		
		lChar = '\r';
		for(;z<pts[x];z++)
			callback(0, textFile[z], 0, 0, 0);
	}
}

void drawEditScreen()
{			
	if(!hasLoaded)
	{
		// hasn't been loaded yet
		
		if(isTextEditorFixed())
		{
			textLines = 12;
		}
		else
		{
			textLines = 11;
		}
		
		fb_setBGColor(textAreaFillColor);
		
		char fName[FILENAME_SIZE];
		char ext[FILENAME_SIZE];
		strcpy(fName,curDir);
		separateExtension(fName,ext);
		strlwr(ext);
		
		if(strcmp(ext,".ini") == 0 || strcmp(ext,".lng") == 0 || strcmp(ext,".dss") == 0)
		{
			hilightType = INI;
			enableSyntax = true;
		}
		else if(strcmp(ext,".htm") == 0 || strcmp(ext,".html") == 0 || strcmp(ext,".xml") == 0)
		{
			hilightType = HTML;
			enableSyntax = true;		
		}
		else
			enableSyntax = false;
		
		char readBuffer[READ_BUFFER_SIZE + 1];
		char *tmpBuf = NULL;
		char crChar = 0;
		filePos = 0;
		
		freeText();
		if(debug_FileExists((const char*)curDir,29) == FT_FILE){
			DRAGON_FILE *fFile = DRAGON_fopen(curDir, "r");		//debug_FileExists index: 29
			
			bufferLength = DRAGON_flength(fFile) + PAD_SIZE;
			
			copying = true;
			maxSize = bufferLength;
			curSize = 0;
			
			textFile = (char *)trackMalloc(bufferLength,"text editor file");
			memset(textFile, 0, bufferLength);
			
			// figure out if its windows/unix/mac
			
			DRAGON_fgets(textFile, bufferLength+1, fFile);
			
			if(strlen(textFile) == bufferLength) // we have no returns in this document
				textType = WINDOWS; // assume windows cuz thats how we are
			else
			{
				DRAGON_fseek(fFile, strlen(textFile), 0);
				
				char c = DRAGON_fgetc(fFile);
				
				if(c == LF) // LF
					textType = UNIX;
				else
				{
					if(DRAGON_feof(fFile))
						textType = MAC; // has to be mac, its a CR at the end of the file
					else
					{
						c = DRAGON_fgetc(fFile);
						if(c == LF) //LF
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
						crChar = CR; // CR
						break;
					case UNIX:
						crChar = LF; // LF
						break;
					case MAC:
						crChar = CR; // CR
						break;
				}
				
				// read the file
				
				while(!DRAGON_feof(fFile))
				{	
					memset(readBuffer, 0, READ_BUFFER_SIZE + 1);
					curSize = DRAGON_ftell(fFile);
					DRAGON_fread(readBuffer, 1, READ_BUFFER_SIZE, fFile);
					
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
						textFile[tmpLocation] = EOS;
						
						if(textType == WINDOWS)
							filePos+=2;
						else
							filePos+=1;
						
						DRAGON_fseek(fFile, curSize + filePos, 0);					
					}
				}
			}
			
			DRAGON_fclose(fFile);
			
			copying = false;		
			hasLoaded = true;
			
			wrapCalculateTextEditor();
			
			filePos = 0;
			resetCursor();
		}
	}
	
	fb_setClipping(LEFT, TOP, RIGHT, BOTTOM + (isTextEditorFixed() ? FIXEDPAD : 0));
	
	setWrapToBorder();
	
	if(cursorUpdate == true)
	{
		setCursorProperties(textAreaFillColor, 2, 0, 0);
		
		showCursor();
		setCursorPos(getCursor() - filePos);	
	}
	else 	
	{
		if(blinkOn())
		{
			if(isInsert())
				setCursorProperties(cursorNormalColor, isTextEditorFixed() ? 0 : 2, 0, 0);
			else
				setCursorProperties(cursorOverwriteColor, isTextEditorFixed() ? 0 : 2, 0, 0);
				
			showCursor();
			setCursorPos(getCursor() - filePos);		
		}
	}
	
	setFont(setTextEditorFont());
	setColor(textAreaTextColor);
	
	if(enableSyntax == true)
	{
		lChar = 0;
		setCallBack(callback);
		calcCurrentColor();
	}
	
	setFakeHighlightEX(filePos);
	fb_dispString(0,5,textFile + filePos);
	
	if(enableSyntax == true)
		clearCallBack();
	
	setBold(false);
	setItalics(false);
	setUnderline(false);
	
	setWrapNormal();
	hideCursor();
	clearHighlight();
	
	if(ptCount > textLines)
	{
		// draw scrollbar
		
		double z;
		
		fb_drawFilledRect(242, 3, 252, 188, widgetBorderColor, scrollFillColor);
		
		// we have 170 pixels of vertical movement
		
		z = (double)170 * (double)getLocation();
		z = z / (double)(ptCount-textLines);
		
		fb_drawRect(244, 5 + (int)z, 250, 5 + 11 + (int)z, scrollNormalColor);		
	}
}

void moveTextCursor(int direction)
{
	int x = 0;
	int y = 0;
	int z = 0;
	
	switch(direction)
	{
		case CURSOR_LEFT:
			if(getCursor() > 0)
			{
				moveCursorRelative(CURSOR_BACKWARD);
				if(getCursor() < filePos && ptCount > textLines)
					filePos = pts[getLocation() - 1];
			}
			break;
		case CURSOR_RIGHT: 
			if(textFile[getCursor()] != 0) 
			{
				moveCursorRelative(CURSOR_FORWARD);
				if(getCursor() >= pts[getLocation() + textLines] && ptCount > textLines) // 11 lines on screen at once
				{
					if((getLocation() + textLines) >= ptCount) // make sure we dont step over the edge					
						return;
					
					filePos = pts[getLocation() + 1];					
				}
			}
			break;
		case CURSOR_UP:
			x = getLocation();
			
			if(getCursor() < pts[x + 1] && ptCount > textLines) // we are on the first line of the screen, scroll up first then display			
			{
				if(x == 0) // cant go up if we are on the first line
					break;
				
				filePos = pts[x-1];
				
				cursorUpdate = true;
				
				fb_swapBuffers();	
				drawEditScreen();
				
				fb_swapBuffers();	
				drawEditScreen();
				
				cursorUpdate = false;
			}
			
			moveCursorAbsolute(getPositionAbove() + filePos);
			break;
		case CURSOR_DOWN:
			x = getLocation();
			
			if(getCursor() >= pts[x + (textLines - 1)] && ptCount > textLines) // we are on the last line of the screen, scroll down first then display
			{
				if((x + textLines) >= ptCount) // cant go down if we are at the end
					break;
				
				filePos = pts[x+1];
				
				cursorUpdate = true;
				
				fb_swapBuffers();	
				drawEditScreen();
				
				cursorUpdate = false;
			}
			
			moveCursorAbsolute(getPositionBelow() + filePos);
			break;
		case CURSOR_PAGEUP:
			x = getLocation();
			y = getVirtualLine() - x;
			z = getVirtualColumn();
			
			x -= textLines;
			
			if(x < 0)
				x = 0;
				
			filePos = pts[x];				
			moveCursorAbsolute(pts[x + y] + z); // we are lazy so we just set to row/column instead of exact position
			
			if(getCursor() >= pts[x + y + 1]) // oops went too far for this line
				moveCursorAbsolute(pts[x + y + 1] - 1); // set it back to the end of the line
			break;
		case CURSOR_PAGEDOWN:
			x = getLocation();
			y = getVirtualLine() - x;
			z = getVirtualColumn();
			
			x += textLines;
			
			if(x > (int)(ptCount - textLines))
				x = ptCount - textLines;
			if(x < 0)
				x = 0;
				
			filePos = pts[x];				
			moveCursorAbsolute(pts[x + y] + z); // we are lazy so we just set to row/column instead of exact position
			
			// we have to check to make sure we didn't go over the end!
			
			if((x + y + 1) >= (int)ptCount)
			{
				if(getCursor() > (int)strlen(textFile))
					moveCursorAbsolute(strlen(textFile));
			}
			else if(getCursor() >= pts[x + y + 1]) // oops went too far for this line
				moveCursorAbsolute(pts[x + y + 1] - 1); // set it back to the end of the line
				
			break;
		case CURSOR_HOME:
			moveCursorAbsolute(pts[getVirtualLine()]);
			break;
		case CURSOR_END:			
			if(getVirtualLine() == (int)(ptCount-1))
			{
				moveCursorAbsolute(strlen(textFile));
			}
			else
			{
				// check for weird wordwraps
				char tC = textFile[pts[getVirtualLine()+1] - 1];
				if(tC != '\n' && tC != '\r' && tC != ' ')
				{
					moveCursorAbsolute(pts[getVirtualLine()+1]);
				}
				else
				{
					moveCursorAbsolute(pts[getVirtualLine()+1]-1);
				}
			}
			break;
	}
}

void drawEditControls()
{
	char str[128];
	
	drawLR(l_pageup, l_pagedown);
	
	//line,column
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);	
	sprintf(str,"%d, %d", getVirtualLine() + 1, getVirtualColumn() + 1);
	bg_dispString(centerOnPt(128, str, (uint16 **)font_gautami_10),12, str);	
	
	drawKeyboard();
	drawButtonTexts(l_save, l_back);
	
	setFont((uint16 **)font_arial_11);
	
	strcpy(str, fileName);
	abbreviateString(str, 210, (uint16 **)font_arial_11);
	setColor(genericTextColor);	
	bg_dispString(23,20-2,str);
}

void checkCursorPosition()
{
	if(ptCount <= textLines)
	{
		// Don't do anything if the line is short enough.
		filePos = 0;
		
		return;
	}
	
	int x = getLocation();
	
	if(x + textLines >= ptCount && x > 0)
	{
		// Ensure the end of the file is never in the middle of the screen
		filePos = pts[(ptCount - textLines)];
		return;
	}
	
	if(getCursor() >= pts[x + textLines] && x != -1) // we are below the last line of the screen
	{
		// Ensure the cursor is always visible
		filePos = pts[x+1];	
		return;
	}
	
	if(getCursor() < filePos) // we are above the first line of the screen
	{
		// Ensure the cursor is always visible
		filePos = pts[getVirtualLine()];
		
		return;
	}
}

void editTextFileAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	uint textLen = strlen(textFile);
	
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	if(textLen >= bufferLength-1)
	{
		bufferLength += PAD_SIZE;
		
		textFile = (char *)trackRealloc(textFile, bufferLength);
		
		for(uint x=textLen;x<bufferLength - 1;x++)
			textFile[x] = 0;
	}
	
	genericAction(textFile, bufferLength - 1, c);
	wrapCalculateTextEditor();
	checkCursorPosition();
}

void saveTextFile()
{
	uint strPosition = 0;
	uint textLen = strlen(textFile);
	int writeLen = 0;
	char *tmpPtr = NULL;
	
	resetCursor();
	filePos = 0;
	drawEditScreen();
	IRQVBlankWait();
	fb_swapBuffers();
	
	DRAGON_chdir("/");
	DRAGON_remove(curDir);
	
	if(curDir[0] != '/')
	{
		char strTmp[FILENAME_SIZE];
		strcpy(strTmp,"/");
		strcat(strTmp,curDir);
		strcpy(curDir,strTmp);
	}
	
	DRAGON_FILE *fFile = DRAGON_fopen(curDir, "w");	
	
	copying = true;
	
	maxSize = textLen;
	curSize = 0;
	
	while(strPosition < textLen)
	{
		tmpPtr = strchr(textFile + strPosition, RET);
		curSize = strPosition;
		
		if(tmpPtr != NULL)
		{
			writeLen = tmpPtr - (textFile + strPosition);
			
			if(writeLen > 0)
				DRAGON_fwrite(textFile + strPosition, 1, writeLen, fFile);
			strPosition += writeLen + 1;
			
			switch(textType)
			{
				case WINDOWS:
					DRAGON_fputc(CR, fFile); // CR
					DRAGON_fputc(LF, fFile); // LF
					break;
				case UNIX:
					DRAGON_fputc(LF, fFile); // LF
					break;
				case MAC:
					DRAGON_fputc(CR, fFile); // CR
					break;
			}
		}
		else
		{
			tmpPtr = strchr(textFile + strPosition, 0);
			
			writeLen = tmpPtr - (textFile + strPosition);
			
			if(writeLen > 0)
				DRAGON_fwrite(textFile + strPosition, 1, writeLen, fFile);
			strPosition += writeLen + 1;
		}
	}
	
	DRAGON_fclose(fFile);
	
	copying = false;
}
