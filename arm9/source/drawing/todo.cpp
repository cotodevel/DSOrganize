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
#include "todo.h"
#include "address.h"
#include "browser.h"
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

extern uint16 editField;
extern char fileName[256];
extern TODO_FILE *todoList;
extern bool autoBullet;

static int todoMode = 0;
static bool t_isPop = false;
static int curTodo = 0;
static uint16 todoEntries;

static uint16 *sprList[6] = { onhold, normal, normalsemi, urgent, urgentsemi, finished };

void addBulletToTodo()
{	
	if(autoBullet && strlen(todoList[curTodo].message) == 0)
	{
		editTodoAction('-');
		editTodoAction(' ');
		clearPressedChar();
	}
}

void todoSetUnPopulated()
{
	t_isPop = false;
}

void saveTodo(TODO_FILE *vf, char *dir)
{
	char filename[MAX_TGDSFILENAME_LENGTH+1] = {0};	
	if(strlen(vf->fileName) == 0)
	{	// new todo
		
		sprintf(filename, "%s.todo", vf->title);
		if(DRAGON_FileExists(filename) != FT_NONE)
			sprintf(filename, "%s%d%d%d%d%d.todo", vf->title, getDay(), getMonth(), getYear(), getHour(true), getMinute());
		
		safeFileName(filename);
		
		strcpy(vf->fileName,filename);
	}
	else
		strcpy(filename, vf->fileName);
	
	DRAGON_FILE *fFile = NULL;
	if(debug_FileExists((const char*)filename,31) == FT_FILE){
		fFile = DRAGON_fopen(filename, "w");	//debug_FileExists index: 31
	}
	
	uint16 sz;
	
	DRAGON_fputc(vf->urgency, fFile);
	
	sz = strlen(vf->title) + 1; // catch zero based strings
	DRAGON_fputc((char)(sz & 0xFF), fFile);
	DRAGON_fputc((char)((sz >> 8) & 0xFF), fFile);			
	DRAGON_fwrite(vf->title, 1, sz, fFile); // write the entry itself
	
	sz = strlen(vf->message) + 1; // catch zero based strings
	DRAGON_fputc((char)(sz & 0xFF), fFile);
	DRAGON_fputc((char)((sz >> 8) & 0xFF), fFile);			
	DRAGON_fwrite(vf->message, 1, sz, fFile); // write the entry itself
	
	DRAGON_fclose(fFile);
}

void fixGautami()
{
	for(int i=0;i<6;i++)
	{
		font_gautami_10[i+20] = sprList[i];
		((int16 *)font_gautami_10[259])[i+20] = -4;
	}
}

void loadCurrentTodos(char **rStr)
{
	int tmpEntries = populateTodoList((char*)getDefaultDSOrganizeTodoFolder("").c_str());
	
	if(tmpEntries == 0)
	{
		freeTodo();
		*rStr = (char *)trackMalloc(1, "tmp reminder");
		*rStr[0] = 0;
	}
	else
	{
		*rStr = (char *)trackMalloc(1024, "tmp reminder");
		memset(*rStr, 0, 1024);
		
		for(int i=0;i<=2;i++)
		{
			if(i == tmpEntries)
				break;
			
			char str[256];
			sprintf(str, "%c %s\n\v\v\v\v\v", todoList[i].urgency+20, todoList[i].title);
			strcat(*rStr, str);
		}
		
		freeTodo();
	}
}

void dispCursorTodo(uint16 x)
{
	if(x != editField || getMode() != EDITTODOLIST)
	{
		hideCursor();
		clearHighlight();
		return;
	}
	
	setFakeHighlight();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, -2, -3, -1);
		else
			setCursorProperties(cursorOverwriteColor, -2, -3, -1);
			
		showCursor();
		setCursorPos(getCursor());		
	}
}

uint16 getMaxCursorTodo()
{
	switch(editField)
	{
		case 0:
			return strlen(todoList[curTodo].title);
			break;
		case 1:
			return strlen(todoList[curTodo].message);
			break;
	}
	return 0;
}

void todoListCallback(int pos, int x, int y)
{
	char str[512];
	
	strcpy(str, todoList[pos].title);
	abbreviateString(str, list_right - (list_left + 17), (uint16 **)font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_dispSprite(x, y - 1, sprList[(int)todoList[pos].urgency],0);	
}

void drawTodoList()
{
	drawHome();
	
	if(!t_isPop)
	{
		todoEntries = populateTodoList((char*)getDefaultDSOrganizeTodoFolder("").c_str());
		t_isPop = true;
		
		if(strlen(fileName) > 0) // we have to jump to a specific one		
		{
			for(uint16 it=0;it<todoEntries;it++)
			{
				if(strcmp(fileName, todoList[it].fileName) == 0)
				{
					moveCursorAbsolute(it);
					break;
				}
			}
		}
	}
	
	if(getCursor() > todoEntries - 1)
		resetCursor();
	
	switch(todoMode)
	{
		case 0:			
			if(todoEntries == 0)
				drawButtonTexts(NULL, l_create);
			else
				drawButtonTexts(l_editselected, l_create);
			break;
		case 1:
			drawButtonTexts(l_delete, l_back);
			break;
	}
	
	drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, getCursor(), todoEntries, l_notodo, todoListCallback);
	drawScrollBar(getCursor(), todoEntries, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT);
}

void drawCurrentTodo()
{	
	uint16 colors[2] = { widgetBorderColor, widgetBorderColor };
	int wTodo = getCursor();
	
	if(getMode() == EDITTODOLIST)
	{
		colors[editField] = textEntryHighlightColor; 
		wTodo = curTodo;
	}

	fb_setClipping(0,0,255,191);
	setFont((uint16 **)font_gautami_10);
	
	fb_dispSprite(3,12,sprList[(int)todoList[wTodo].urgency],0);
	
	setColor(genericTextColor);
	fb_dispString(18, 3, l_title);	
	fb_drawFilledRect(18, 3 + 8, 252, 3 + 23, colors[0], textEntryFillColor);

	fb_drawFilledRect(3, 30, 252, 188, colors[1], textEntryFillColor);
	
	if(todoEntries == 0 || !t_isPop)
		return;
	
	// draw data here
	
	setColor(textEntryTextColor);
	
	fb_setClipping(18, 3 + 8, 250, 3 + 26);	
	dispCursorTodo(0);
	fb_dispString(3, 5, todoList[wTodo].title);
	
	fb_setClipping(3, 30, 250, 188);
	dispCursorTodo(1);
	fb_dispString(3, 4, todoList[wTodo].message);
	
	hideCursor();
	clearHighlight();
}

void drawEditTodo()
{	
	drawLR(l_prevfield, l_nextfield);
	
	if(strlen(todoList[curTodo].fileName) > 0)
	{
		// it's not new, we can delete if we want.
		drawDelete(bg_backBuffer());
	}

	drawKeyboard();
	
	setFont((uint16 **)font_arial_9);
	setColor(genericTextColor);
	
	switch(todoMode)
	{
		case 0:	
			drawButtonTexts(l_save, l_back);
			break;
		case 1:
			drawButtonTexts(l_delete, l_back);
			break;
	}
	
	setFont((uint16 **)font_arial_11);
	
	char str[128];
	strcpy(str,todoList[curTodo].title);	
	setColor(genericTextColor);
	abbreviateString(str, 230, (uint16 **)font_arial_11);	
	bg_dispString(13,20-2,str);
}

void editTodoAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == RET)
	{ 
		if(editField == 0)
		{
			editField++;
			resetCursor();
			clearSelect();
			
			if(autoBullet && strlen(todoList[curTodo].message) == 0)
			{
				editTodoAction('-');
				editTodoAction(' ');
			}
			
			setPressedChar(RET);
			
			return;
		}
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	// figure out which buffer to go to, how big....
	char *buf = 0; // get rid of stupid warning message
	int size = 0;
	
	switch(editField)
	{
		case 0:
			buf = todoList[curTodo].title;
			size = 40;
			break;
		case 1:
			buf = todoList[curTodo].message;
			size = 1024;
			break;
	}
	
	size++;
	
	if(c == CLEAR_KEY)
	{
		memset(buf, 0, size);
		resetCursor();
		return;
	}		
	
	char *tmpBuffer = (char *)trackMalloc(size + 1,"todo kbd");
	memcpy(tmpBuffer,buf,size + 1);
	int oldCursor = getCursor();
	
	genericAction(tmpBuffer, size, c);
	
	int *pts = NULL;
	
	switch(editField)
	{
		case 0:
		{
			int numPts = getWrapPoints(3, 4, tmpBuffer, 18, 3 + 8, 250, 3 + 26, &pts, (uint16 **)font_gautami_10);
			free(pts);
			
			if(numPts == 1)
				memcpy(buf,tmpBuffer,size);
			else
				moveCursorAbsolute(oldCursor);
			
			break;
		}
		case 1:
		{
			int numPts = getWrapPoints(3, 4, tmpBuffer, 3, 30, 250, 188, &pts, (uint16 **)font_gautami_10);
			free(pts);
			
			if(numPts <= 12)
				memcpy(buf,tmpBuffer,size);
			else
				moveCursorAbsolute(oldCursor);
			
			break;
		}
	}
	
	trackFree(tmpBuffer);
	
	if(c == RET && autoBullet && getCursor() != oldCursor)
	{
		editTodoAction('-');
		editTodoAction(' ');
		setPressedChar(RET);
	}
}

void incrementUrgency()
{
	if(todoList[curTodo].urgency < FINISHED)
		todoList[curTodo].urgency++;
}

void decrementUrgency()
{
	if(todoList[curTodo].urgency > ONHOLD)
		todoList[curTodo].urgency--;
}

void deleteTodo()
{	
	if(todoEntries > 0)
	{
		if(strlen(todoList[getCursor()].fileName) > 0)
		{
			todoMode = 1;
		}
	}
}

void editTodoForward()
{
	switch(todoMode)
	{
		case 0: // save
			saveTodo(&todoList[curTodo], (char*)getDefaultDSOrganizeTodoFolder("").c_str());
			
			sortTodoList(todoEntries);			
			strcpy(fileName,"");
			setMode(TODOLIST);
			popCursor();
			break;
		case 1: // delete
			DRAGON_remove(todoList[curTodo].fileName);
			t_isPop = false;
			strcpy(fileName,"");
			setMode(TODOLIST);
			resetCursor();
			todoMode = 0;
			break;
	}
}

void todoForward()
{
	switch(todoMode)
	{
		case 0: // forward	
			if(todoEntries > 0)
			{
				setMode(EDITTODOLIST);
				editField = 0;
				curTodo = getCursor();
				pushCursor();
			}
			break;
		case 1: // delete
			DRAGON_remove(todoList[getCursor()].fileName);
			t_isPop = false;
			strcpy(fileName,"");
			if(getCursor() > 0)
			{
				moveCursorRelative(CURSOR_BACKWARD);
			}
			todoMode = 0;
			break;
	}
}

void editTodoBack()
{
	switch(todoMode)
	{
		case 0: // back
			t_isPop = false;
			strcpy(fileName,"");
			setMode(TODOLIST);
			popCursor();
			break;
		case 1: // cancel delete
			todoMode = 0;
			break;
	}
}

void todoBack()
{
	switch(todoMode)
	{
		case 0: // back
			setMode(EDITTODOLIST);
			editField = 0;
			moveCursorAbsolute(todoEntries);
			
			curTodo = addTodo();
			clearTodo(&todoList[curTodo]);
			todoEntries++;
			pushCursor();			
			break;
		case 1: // cancel delete
			todoMode = 0;
			break;
	}
}

int getTodoEntries()
{
	return todoEntries;
}

