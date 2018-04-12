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
#include <libfb/libcommon.h>
#include "lcdfunctions.h"
#include "settings.h"
#include "sound.h"
#include "drawing/configuration.h"
#include "drawing/scribble.h"

static bool lcdOn = true;
static bool needsRefresh = false;

void flagRefresh()
{
	needsRefresh = true;
}

bool refreshFlagged()
{
	return needsRefresh;
}

bool getLCDState()
{
	return lcdOn;
}

void setLCDState(bool state)
{
	if(state == LCD_ON && !lcdOn)
	{
		powerON(POWER_2D_A | POWER_2D_B | POWER_SWAP_LCDS); // turn on everything
		
		lcdMainOnTop();	
		
		if(screensFlipped() || configFlipped())
			lcdSwap();
		
		needsRefresh = true;
		lcdOn = true;
		
		return;
	}
	
	if(state == LCD_OFF && lcdOn)
	{
		if(getMode() == SOUNDRECORDER) // force recorder to work when top is shut
		{
			powerOFF(POWER_2D_A | POWER_2D_B); // turn off some stuff
		}
		else
		{
			powerOFF(POWER_2D_A | POWER_2D_B | POWER_SWAP_LCDS);	//powerOFF(POWER_ALL_2D); // turn off everything
		}
		
		lcdOn = false;
		
		return;
	}
}

void drawToScreen()
{
	updateStreamLoop();
	drawColorCursor(); // hack so color cursor doesn't sometimes make color white
	
	if(getMode() == SOUNDPLAYER)
	{
		if(needsRefresh)
		{
			updateStreamLoop();
			fb_swapBuffers();
			needsRefresh = false;
		}
	}
	else
	{
		if(getMode() != EDITSCRIBBLE)
		{
			updateStreamLoop();
			fb_swapBuffers();
		}
	}
	
	updateStreamLoop();
	bg_swapBuffers();
}
