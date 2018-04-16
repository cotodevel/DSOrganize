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
#include <libfb/libcommon.h>
#include "about.h"
#include "home.h"
#include "../mikmod/include/mikmod_build.h"
#include "../fatwrapper.h"
#include "../font_arial_9.h"
#include "../font_arial_11.h"
#include "../general.h"
#include "../html.h"
#include "../settings.h"
#include "../graphics.h"
#include "../sound.h"
#include "../resources.h"
#include "../globals.h"
#include "../general.h"

// About source
//#include "about_html.h"
extern u8 about[];
static bool aboutLoaded = false;
static HTML_RENDERED aboutData;
static char *abScreen = NULL;

void drawAboutScreen()
{
	if(!aboutLoaded)
	{	
		setEncoding(CHARSET_ASCII, &aboutData);
		setHTMLWidth(ABOUT_HELP_WIDTH, &aboutData);
		setContent(TYPE_HTML, &aboutData);
		
		abScreen = (char *)trackMalloc(strlen((char *)&about)+1, "temp about");
		memcpy(abScreen, &about, strlen((char *)&about)+1);
		
		loadHTMLFromMemory(abScreen, &aboutData);
		aboutLoaded = true;
	}
	
	uint16 *htmlRender = (uint16 *)trackMalloc(SCREEN_WIDTH * (FULLPAGE_HEIGHT + 16) * SCREEN_BPP, "temp html render");
	displayHTML(htmlRender, &aboutData, 0, FULLPAGE_HEIGHT);
	
	disableBackgroundDraw();
	
	for(int i=0;i<SCREEN_WIDTH * SCREEN_HEIGHT;i++)
	{
		fb_backBuffer()[i] = htmlRender[i];
		bg_backBuffer()[i] = htmlRender[i + (SCREEN_WIDTH * SCREEN_HEIGHT)];
	}
	
	trackFree(htmlRender);
}

void freeabout()
{
	if(aboutLoaded)
	{
		aboutLoaded = false;
		freeHTML(&aboutData);
	}
}
