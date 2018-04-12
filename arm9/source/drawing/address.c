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
#include <vcard.h>
#include "address.h"
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
extern VCARD_FILE *fileList;

static int addressMode = 0;
static bool a_isPop = false;
static int curAddress = 0;
static uint16 addressEntries;

void dispCursor(uint16 x)
{
	if(x != editField || getMode() != EDITADDRESS)
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

void addressSetUnPopulated()
{
	a_isPop = false;
}

uint16 getMaxCursor()
{
	int whichAddress = 0;
	
	if(fileList[curAddress].workAddrPresent == true)
		whichAddress = 1;	
	if(fileList[curAddress].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(fileList[curAddress].workPhone[0] != 0)
		whichPhone = 1;
	if(fileList[curAddress].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;
	
	switch(editField)
	{
case 0:
			return strlen(fileList[curAddress].firstName);
			break;
		case 1:
			return strlen(fileList[curAddress].lastName);
			break;
		case 2:
			return strlen(fileList[curAddress].nickName);
			break;
		case 3:
			if(whichPhone == 0)
				return strlen(fileList[curAddress].homePhone);
			else
				return strlen(fileList[curAddress].workPhone);
			
			break;
		case 4:
			return strlen(fileList[curAddress].cellPhone);
			break;
		case 5:
			if(whichAddress == 0)
				return strlen(fileList[curAddress].homeAddr.addrLine);
			else
				return strlen(fileList[curAddress].workAddr.addrLine);
			
			break;
		case 6:
			if(whichAddress == 0)
				return strlen(fileList[curAddress].homeAddr.city);
			else
				return strlen(fileList[curAddress].workAddr.city);
			
			break;
		case 7:
			if(whichAddress == 0)
				return strlen(fileList[curAddress].homeAddr.state);
			else
				return strlen(fileList[curAddress].workAddr.state);
			
			break;
		case 8:
			if(whichAddress == 0)
				return strlen(fileList[curAddress].homeAddr.zip);
			else
				return strlen(fileList[curAddress].workAddr.zip);
			
			break;
		case 9:
			return strlen(fileList[curAddress].email);
			break;
		case 10:
			return strlen(fileList[curAddress].comment);
			break;
	}
	return 0;
}

void addressListCallback(int pos, int x, int y)
{
	char str[70];
	
	calculateListName(&fileList[pos], str);
	abbreviateString(str, list_right - (list_left + 15), (uint16 **)font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

void drawAddressList()
{
	drawHome();
	
	if(!a_isPop)
	{
		separateMultiples(d_vcard);
		addressEntries = populateList(d_vcard);
		a_isPop = true;
		
		if(strlen(fileName) > 0) // we have to jump to a specific one		
		{
			for(uint16 it=0;it<addressEntries;it++)
			{
				if(strcmp(fileName, fileList[it].fileName) == 0)
				{
					moveCursorAbsolute(it);
					break;
				}
			}
		}
	}
	
	if(getCursor() > addressEntries - 1)
		resetCursor();
	
	switch(addressMode)
	{
		case 0:
		{
			if(addressEntries == 0)
				drawButtonTexts(NULL, l_create);
			else			
				drawButtonTexts(l_editselected, l_create);
			break;
		}
		case 1:
			drawButtonTexts(l_delete, l_back);
			break;
	}
	
	drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM, getCursor(), addressEntries, l_novcard, addressListCallback);
	drawScrollBar(getCursor(), addressEntries, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT);
}

void drawCurrentAddress()
{	
	int whichAddress = 0;
	int wAddr = getCursor();
	
	uint16 colors[11] = { widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor };
	
	if(getMode() == EDITADDRESS)
	{
		colors[editField] = textEntryHighlightColor; 
		wAddr = curAddress;
	}
	
	if(fileList[wAddr].workAddrPresent == true)
		whichAddress = 1;	
	if(fileList[wAddr].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(fileList[wAddr].workPhone[0] != 0)
		whichPhone = 1;
	if(fileList[wAddr].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;

	fb_setClipping(0,0,255,191);
	setFont((uint16 **)font_gautami_10);
	setColor(genericTextColor);
	
	fb_dispString(3, 3, l_firstname);
	fb_dispString(131, 3, l_lastname);	
	fb_drawFilledRect(3, 3 + 8, 125, 3 + 23, colors[0], textEntryFillColor);
	fb_drawFilledRect(131, 3 + 8, 252, 3 + 23, colors[1], textEntryFillColor);

	fb_dispString(3, 30, l_nickname);	
	fb_drawFilledRect(3, 30 + 8, 252, 30 + 23, colors[2], textEntryFillColor);
	
	if(whichPhone == 0)
		fb_dispString(3, 57, l_homephone);
	else
		fb_dispString(3, 57, l_workphone);
	
	fb_dispString(131, 57, l_cellphone);
	fb_drawFilledRect(3, 57 + 8, 125, 57 + 23, colors[3], textEntryFillColor);
	fb_drawFilledRect(131, 57+ 8, 252, 57 + 23, colors[4], textEntryFillColor);
	
	fb_dispString(3, 84, l_address);	
	fb_drawFilledRect(3, 84 + 8, 252, 84 + 23, colors[5], textEntryFillColor);
	
	fb_dispString(3, 111, l_city);
	fb_dispString(131, 111, l_state);
	fb_dispString(177, 111, l_zip);
	fb_drawFilledRect(3, 111 + 8, 125, 111 + 23, colors[6], textEntryFillColor);
	fb_drawFilledRect(131, 111 + 8, 171, 111 + 23, colors[7], textEntryFillColor);
	fb_drawFilledRect(177, 111 + 8, 252, 111 + 23, colors[8], textEntryFillColor);
	
	fb_dispString(3, 138, l_email);	
	fb_drawFilledRect(3, 138 + 8, 252, 138 + 23, colors[9], textEntryFillColor);
	
	fb_dispString(3, 165, l_comments);	
	fb_drawFilledRect(3, 165 + 8, 252, 165 + 23, colors[10], textEntryFillColor);	
	
	if(addressEntries == 0 || a_isPop == false)
		return;
		
	// draw data here
	
	setColor(textEntryTextColor);
	
	fb_setClipping(3, 3 + 8, 125, 3 + 26);	
	dispCursor(0);
	fb_dispString(3, 5, fileList[wAddr].firstName);
	fb_setClipping(131, 3 + 8, 252, 3 + 26);
	dispCursor(1);
	fb_dispString(3, 5, fileList[wAddr].lastName);
	
	fb_setClipping(3, 30 + 8, 252, 30 + 26);
	dispCursor(2);
	fb_dispString(3, 5, fileList[wAddr].nickName);
	
	fb_setClipping(3, 57 + 8, 125, 57 + 26);
	dispCursor(3);
	
	if(whichPhone == 0)
		fb_dispString(3, 5, fileList[wAddr].homePhone);
	else	
		fb_dispString(3, 5, fileList[wAddr].workPhone);
	
	fb_setClipping(131, 57+ 8, 252, 57 + 26);
	dispCursor(4);
	fb_dispString(3, 5, fileList[wAddr].cellPhone);
		
	fb_setClipping(3, 84 + 8, 252, 84 + 26);
	dispCursor(5);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, fileList[wAddr].homeAddr.addrLine);
	else
		fb_dispString(3, 5, fileList[wAddr].workAddr.addrLine);
	
	fb_setClipping(3, 111 + 8, 125, 111 + 26);
	dispCursor(6);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, fileList[wAddr].homeAddr.city);
	else
		fb_dispString(3, 5, fileList[wAddr].workAddr.city);
	
	fb_setClipping(131, 111 + 8, 171, 111 + 26);
	dispCursor(7);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, fileList[wAddr].homeAddr.state);
	else
		fb_dispString(3, 5, fileList[wAddr].workAddr.state);
	
	fb_setClipping(177, 111 + 8, 252, 111 + 26);
	dispCursor(8);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, fileList[wAddr].homeAddr.zip);
	else
		fb_dispString(3, 5, fileList[wAddr].workAddr.zip);
	
	fb_setClipping(3, 138 + 8, 252, 138 + 26);
	dispCursor(9);
	fb_dispString(3, 5, fileList[wAddr].email);
	
	fb_setClipping(3, 165 + 8, 252, 165 + 26);
	dispCursor(10);
	fb_dispString(3, 5, fileList[wAddr].comment);
	hideCursor();
	clearHighlight();
}

void drawEditAddress()
{	
	char str[70];
	
	drawLR(l_prevfield, l_nextfield);
	
	if(strlen(fileList[curAddress].fileName) > 0)
	{
		// it's not new, we can delete if we want
		drawDelete(bg_backBuffer());
	}

	drawKeyboard();
	
	switch(addressMode)
	{
		case 0:
			drawButtonTexts(l_save, l_back);
			break;
		case 1:
			drawButtonTexts(l_delete, l_back);
			break;
	}
	
	setFont((uint16 **)font_arial_11);
	
	calculateListName(&fileList[curAddress], str);
	setColor(genericTextColor);
	abbreviateString(str, 230, (uint16 **)font_arial_11);	
	bg_dispString(13,20-2,str);
}

bool isNumbers(char *str)
{
	while(*str != 0)
	{
		if(*str < '0' || *str > '9')
			return false;
		
		str++;
	}

	return true;
}

void formatCell(int eF)
{
	if(eF == 3)
	{
		char *phone;
		int whichPhone = 0;
		
		if(fileList[curAddress].workPhone[0] != 0)
			whichPhone = 1;
		if(fileList[curAddress].homePhone[0] != 0) // default to home phone no matter what
			whichPhone = 0;
		
		if(whichPhone == 0)
			phone = fileList[curAddress].homePhone;
		else
			phone = fileList[curAddress].workPhone;
		
		// applicable, phone numbers!
		if(isNumbers(phone) && strlen(phone) == 7)
		{
			phone[7] = phone[6];
			phone[6] = phone[5];
			phone[5] = phone[4];
			phone[4] = phone[3];
			phone[3] = '-';
			phone[8] = 0;
			
			return;
		}
		
		if(isNumbers(phone) && strlen(phone) == 10)
		{
			phone[13] = phone[9];
			phone[12] = phone[8];
			phone[11] = phone[7];
			phone[10] = phone[6];
			phone[8] = phone[5];
			phone[7] = phone[4];
			phone[6] = phone[3];
			phone[3] = phone[2];
			phone[2] = phone[1];
			phone[1] = phone[0];
			
			phone[0] = '(';
			phone[4] = ')';
			phone[5] = ' ';
			phone[9] = '-';
			phone[14] = 0;
			
			return;
		}
	}
	
	if(eF == 4)
	{
		// applicable, phone numbers!
		if(isNumbers(fileList[curAddress].cellPhone) && strlen(fileList[curAddress].cellPhone) == 7)
		{
			fileList[curAddress].cellPhone[7] = fileList[curAddress].cellPhone[6];
			fileList[curAddress].cellPhone[6] = fileList[curAddress].cellPhone[5];
			fileList[curAddress].cellPhone[5] = fileList[curAddress].cellPhone[4];
			fileList[curAddress].cellPhone[4] = fileList[curAddress].cellPhone[3];
			fileList[curAddress].cellPhone[3] = '-';
			fileList[curAddress].cellPhone[8] = 0;
			
			return;
		}
		
		if(isNumbers(fileList[curAddress].cellPhone) && strlen(fileList[curAddress].cellPhone) == 10)
		{
			fileList[curAddress].cellPhone[13] = fileList[curAddress].cellPhone[9];
			fileList[curAddress].cellPhone[12] = fileList[curAddress].cellPhone[8];
			fileList[curAddress].cellPhone[11] = fileList[curAddress].cellPhone[7];
			fileList[curAddress].cellPhone[10] = fileList[curAddress].cellPhone[6];
			fileList[curAddress].cellPhone[8] = fileList[curAddress].cellPhone[5];
			fileList[curAddress].cellPhone[7] = fileList[curAddress].cellPhone[4];
			fileList[curAddress].cellPhone[6] = fileList[curAddress].cellPhone[3];
			fileList[curAddress].cellPhone[3] = fileList[curAddress].cellPhone[2];
			fileList[curAddress].cellPhone[2] = fileList[curAddress].cellPhone[1];
			fileList[curAddress].cellPhone[1] = fileList[curAddress].cellPhone[0];
			
			fileList[curAddress].cellPhone[0] = '(';
			fileList[curAddress].cellPhone[4] = ')';
			fileList[curAddress].cellPhone[5] = ' ';
			fileList[curAddress].cellPhone[9] = '-';
			fileList[curAddress].cellPhone[14] = 0;
			
			return;
		}
	}
	
	if(eF == 8)
	{
		// applicable, zip code!
		
		char *zip;
		
		int whichAddress = 0;
		
		if(fileList[curAddress].workAddrPresent == true)
			whichAddress = 1;	
		if(fileList[curAddress].homeAddrPresent == true) // default to home address no matter what
			whichAddress = 0;
		
		if(whichAddress == 0)
			zip = fileList[curAddress].homeAddr.zip;
		else
			zip = fileList[curAddress].workAddr.zip;
		
		if(isNumbers(zip) && strlen(zip) == 9)
		{
			zip[9] = zip[8];
			zip[8] = zip[7];
			zip[7] = zip[6];
			zip[6] = zip[5];
			zip[5] = '-';
			zip[10] = 0;
		}
	}
}

void editAddressAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	int whichAddress = 0;
	
	if(fileList[curAddress].workAddrPresent == true)
		whichAddress = 1;	
	if(fileList[curAddress].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(fileList[curAddress].workPhone[0] != 0)
		whichPhone = 1;
	if(fileList[curAddress].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;
	
	if(c == RET)
	{
		if(editField < 10)
		{
			clearSelect();
			formatCell(editField);
			editField++;
			resetCursor();
		}
		return;
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
			buf = fileList[curAddress].firstName;
			size = 30;
			break;
		case 1:
			buf = fileList[curAddress].lastName;
			size = 30;
			break;
		case 2:
			buf = fileList[curAddress].nickName;
			size = 80;
			break;
		case 3:
			if(whichPhone == 0)
				buf = fileList[curAddress].homePhone;
			else
				buf = fileList[curAddress].workPhone;
			
			size = 14;
			break;
		case 4:
			buf = fileList[curAddress].cellPhone;
			size = 14;
			break;
		case 5:
			if(whichAddress == 0)
				buf = fileList[curAddress].homeAddr.addrLine;
			else
				buf = fileList[curAddress].workAddr.addrLine;
				
			size = 60;
			break;
		case 6:
			if(whichAddress == 0)
				buf = fileList[curAddress].homeAddr.city;
			else
				buf = fileList[curAddress].workAddr.city;
			
			size = 30;
			break;
		case 7:
			if(whichAddress == 0)
				buf = fileList[curAddress].homeAddr.state;
			else
				buf = fileList[curAddress].workAddr.state;
			
			size = 3;
			break;
		case 8:
			if(whichAddress == 0)
				buf = fileList[curAddress].homeAddr.zip;
			else
				buf = fileList[curAddress].workAddr.zip;
			
			size = 10;
			break;
		case 9:
			buf = fileList[curAddress].email;
			size = 60;
			break;
		case 10:
			buf = fileList[curAddress].comment;
			size = 100;
			break;
	}
	
	if(c == CLEAR_KEY)
	{
		memset(buf, 0, size + 2); // extra byte per entry
		resetCursor();
		return;
	}	
	
	char *tmpBuffer = (char *)trackMalloc(size + 1,"address kbd");
	memcpy(tmpBuffer,buf,size + 1);
	int oldCursor = getCursor();	
	
	genericAction(tmpBuffer, size, c);
	
	int *pts = NULL;
	int numPts = 0;
	
	switch(editField)
	{
		case 0:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 3 + 8, 125, 3 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 1:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 3 + 8, 252, 3 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 2:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 30 + 8, 252, 30 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 3:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 57 + 8, 125, 57 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 4:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 57+ 8, 252, 57 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 5:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 84 + 8, 252, 84 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 6:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 111 + 8, 125, 111 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 7:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 111 + 8, 171, 111 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 8:
			numPts = getWrapPoints(3, 5, tmpBuffer, 177, 111 + 8, 252, 111 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 9:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 138 + 8, 252, 138 + 26, &pts, (uint16 **)font_gautami_10);
			break;
		case 10:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 165 + 8, 252, 165 + 26, &pts, (uint16 **)font_gautami_10);
			break;
	}
	
	free(pts);
	
	if(numPts == 1)
		memcpy(buf,tmpBuffer,size);
	else
		moveCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}

void deleteAddress()
{
	if(addressEntries > 0)
	{
		if(strlen(fileList[getCursor()].fileName) > 0)
		{
			addressMode = 1;
		}
	}
}

void editAddressForward()
{
	switch(addressMode)
	{
		case 0: // save
			saveVCard(&fileList[curAddress], d_vcard);
			
			sortList(addressEntries);			
			strcpy(fileName,"");
			setMode(ADDRESS);
			popCursor();
			break;
		case 1: // delete
			DRAGON_chdir(d_vcard);
			DRAGON_remove(fileList[curAddress].fileName);
			DRAGON_chdir("/");
			a_isPop = false;
			strcpy(fileName,"");
			setMode(ADDRESS);
			resetCursor();
			addressMode = 0;
			break;
	}
}

void addressForward()
{
	switch(addressMode)
	{
		case 0: // edit
			if(addressEntries > 0)
			{
				setMode(EDITADDRESS);
				curAddress = getCursor();
				editField = 0;
				pushCursor();
			}
			break;
		case 1: // delete
			DRAGON_chdir(d_vcard);
			DRAGON_remove(fileList[getCursor()].fileName);
			DRAGON_chdir("/");
			a_isPop = false;
			strcpy(fileName,"");
			if(getCursor() > 0)
				moveCursorRelative(CURSOR_BACKWARD);
			addressMode = 0;
			break;
	}
}

void editAddressBack()
{
	switch(addressMode)
	{
		case 0: // back		
			a_isPop = false;
			strcpy(fileName,"");
			setMode(ADDRESS);
			break;
		case 1: // cancel delete
			addressMode = 0;
			break;
	}
}

void addressBack()
{
	switch(addressMode)
	{
		case 0: // back	
			setMode(EDITADDRESS);
			editField = 0;
			moveCursorAbsolute(addressEntries);
			
			curAddress = addVCard();
			clearVCard(&fileList[curAddress]);
			addressEntries++;
			pushCursor();
			break;
		case 1: // cancel delete
			addressMode = 0;
			break;
	}
}

int getAddressEntries()
{
	return addressEntries;
}

