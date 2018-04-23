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
#include <libcommon.h>
#include <libfb.h>
#include "help.h"
#include "settings.h"
#include "html.h"
#include "general.h"
#include "globals.h"
#include "fsfatlayerTGDSNew.h"
#include "fatwrapper.h"

extern char curLang[30];
extern bool allowSwapBuffer;
extern u32 htmlStyle;

static bool helpLoaded = false;
static HTML_RENDERED htmlPage;

void drawHelpScreen()
{
	if(!helpLoaded)
	{
		//irqSet(IRQ_VBLANK, 0);
		char htmlFile[32];
		
		switch(getMode())
		{
			case HOME:
				strcpy(htmlFile, "%s%s/home.html");
				break;
			case HOMEMORE:
				strcpy(htmlFile, "%s%s/home.html");
				break;
			case CALENDAR:
				strcpy(htmlFile, "%s%s/calendar.html");
				break;
			case EDITREMINDER:
				strcpy(htmlFile, "%s%s/edit.html");
				break;
			case DAYVIEW:
				strcpy(htmlFile, "%s%s/dayplanner.html");
				break;			
			case EDITDAYVIEW:
				strcpy(htmlFile, "%s%s/edit.html");
				break;
			case ADDRESS:
				strcpy(htmlFile, "%s%s/address.html");
				break;		
			case EDITADDRESS:
				strcpy(htmlFile, "%s%s/editaddress.html");
				break;		
			case CONFIGURATION:
				strcpy(htmlFile, "%s%s/configuration.html");
				break;
			case BROWSER:
				strcpy(htmlFile, "%s%s/browser.html");
				break;
			case BROWSERRENAME:
				strcpy(htmlFile, "%s%s/renaming.html");
				break;
			case CALCULATOR:
				strcpy(htmlFile, "%s%s/calculator.html");
				break;
			case TEXTEDITOR:
				strcpy(htmlFile, "%s%s/texteditor.html");
				break;
			case PICTUREVIEWER:
				strcpy(htmlFile, "%s%s/pictureviewer.html");
				break;
			case TODOLIST:
				strcpy(htmlFile, "%s%s/todo.html");
				break;
			case EDITTODOLIST:
				strcpy(htmlFile, "%s%s/edittodo.html");
				break;
			case SCRIBBLEPAD:
				strcpy(htmlFile, "%s%s/scribble.html");
				break;
			case EDITSCRIBBLE:
				strcpy(htmlFile, "%s%s/editscribble.html");
				break;
			case CHOOSECOLOR:
				strcpy(htmlFile, "%s%s/color.html");
				break;
			case SCRIBBLETEXT:
				strcpy(htmlFile, "%s%s/edit.html");
				break;
			case SOUNDPLAYER:
				strcpy(htmlFile, "%s%s/sound.html");
				break;
			case HOMEBREWDATABASE:
				strcpy(htmlFile, "%s%s/db.html");
				break;
			case VIEWER:
				strcpy(htmlFile, "%s%s/viewer.html");
				break;
			case SOUNDRECORDER:
				strcpy(htmlFile, "%s%s/recorder.html");
				break;
			case IRC:
				strcpy(htmlFile, "%s%s/irc.html");
				break;
			case WEBBROWSER:
				strcpy(htmlFile, "%s%s/webbrowser.html");
				break;
		}
		
		u32 oldText = htmlStyle;
		htmlStyle = 1;
		
		char str[MAX_TGDSFILENAME_LENGTH+1] = {0};
		sprintf(str, htmlFile, getDefaultDSOrganizeHelpPath("").c_str(), curLang);
		
		setHTMLWidth(250, &htmlPage);
		setEncoding(0, &htmlPage);
		setContent(TYPE_HTML, &htmlPage);
		loadHTMLFromFile(str, &htmlPage);
		
		if(!isLoaded(&htmlPage))
		{
			sprintf(str, htmlFile, getDefaultDSOrganizeHelpPath("").c_str(), "english");
			loadHTMLFromFile(str, &htmlPage); // try the english one
			
			if(!isLoaded(&htmlPage))
			{
				char *t = "<html><head><title>Error Loading</title></head><body>There was an error loading the respective help file for this page.</body></html>";
				char *tSpace = (char *)trackMalloc(strlen(t)+1, "tmp render");
				memcpy(tSpace, t, strlen(t)+1);
				
				loadHTMLFromMemory(tSpace, &htmlPage);
			}
		}
		
		htmlStyle = oldText;
		helpLoaded = true;
	}
	
	if(getMode() == EDITSCRIBBLE || getMode() == CHOOSECOLOR)
	{
		allowSwapBuffer = false;
		disableBackgroundDraw();
		displayHTML(bg_backBuffer(), &htmlPage, 0, 192);
	}
	else
	{
		allowSwapBuffer = true;
		
		uint16 *htmlRender = (uint16 *)trackMalloc(256*(384+16)*2, "temp html render");
		
		disableBackgroundDraw();
		displayHTML(htmlRender, &htmlPage, 0, 384);
		
		for(int i=0;i<256*192;i++)
		{
			fb_backBuffer()[i] = htmlRender[i];
			bg_backBuffer()[i] = htmlRender[i + 49152];
		}
		
		trackFree(htmlRender);
	}
}

void clearHelpScreen()
{
	if(helpLoaded)
	{
		helpLoaded = false;
		freeHTML(&htmlPage);
		
		fb_setClipping(0,0,256,192);
		bg_setClipping(0,0,256,192);
		
		if(getMode() == SOUNDPLAYER)
			fb_swapBuffers();
		
		//irqSet(IRQ_VBLANK, vBlank);
	}
}
