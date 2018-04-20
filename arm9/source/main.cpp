/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

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
 
#include "socket.h"
#include "in.h"
#include <netdb.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "main.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"

#include "devoptab_devices.h"
#include "fsfatlayerTGDSNew.h"
#include "usrsettingsTGDS.h"

#include "videoTGDS.h"
#include "keypadTGDS.h"
#include "guiTGDS.h"

#include <stdio.h>
#include <stdlib.h>
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include <string.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include <libpicture.h>
#include "fatwrapper.h"
#include "keyboard.h"
#include "font_arial_11.h"
#include "font_gautami_10.h"
#include "settings.h"
#include "graphics.h"
#include "filerout.h"
#include "globals.h"
#include "settings.h"
#include "language.h"
#include "general.h"
#include "sound.h"
#include "wifi.h"
#include "colors.h"
#include "lcdfunctions.h"
#include "controls.h"
#include "clipboard.h"
#include "resources.h"
#include "splash.h"
#include "html.h"
#include "cyg-profile.h"
#include "drawing/home.h"
#include "drawing/configuration.h"
#include "drawing/calendar.h"
#include "drawing/dayview.h"
#include "drawing/address.h"
#include "drawing/browser.h"
#include "drawing/calculator.h"
#include "drawing/texteditor.h"
#include "drawing/pictureviewer.h"
#include "drawing/help.h"
#include "drawing/about.h"
#include "drawing/todo.h"
#include "drawing/scribble.h"
#include "drawing/soundplayer.h"
#include "drawing/database.h"
#include "drawing/viewer.h"
#include "drawing/soundrecorder.h"
#include "drawing/irc.h"
#include "drawing/webbrowser.h"

#include "keypadTGDS.h"
#include "about.h"
#include "powerTGDS.h"
#include "general.h"
#include "fatwrapper.h"

// internals
int yThreshold = 0;
int queuedCommand = -1;
int queuedData = -1;
bool hasTouched = false;
bool held = false;
char lChar = 0;
char kChar = 0;
uint16 repeatCount = 0;
uint16 editField = 0;



//-------------
// main program
//-------------

TransferSound Snd;
TransferSoundData SndDat =		{ (void *)0 , 0, 11025, 64, 64, 1 };
 
//---------------------------------------------------------------------------------
void setGenericSound( u32 rate, u8 vol, u8 pan, u8 format) {
//---------------------------------------------------------------------------------
 
	SndDat.rate		= rate;
	SndDat.vol		= vol;
	SndDat.pan		= pan;
	SndDat.format	= format;
 }
 
void initProgram()
{
	//------------------------
	// start initializing code
	//------------------------
	
	powerON(POWER_2D_A | POWER_2D_B | POWER_SWAP_LCDS);	// turn on everything
	
	#ifndef DEBUG_MODE
	fb_init(); // initialize top screen video
	#else
	debugInit();
	#endif
	bg_init(); // initialize bottom screen video
	
	setNewOrientation(ORIENTATION_0);
	
	// set up extra vram banks to be scratch memory	
	//vramSetBankE(VRAM_E_LCD);
	VRAMBLOCK_SETBANK_E(VRAM_E_LCDC_MODE);
	
	//vramSetBankF(VRAM_F_LCD);
	VRAMBLOCK_SETBANK_F(VRAM_F_LCDC_MODE);
	
	//vramSetBankG(VRAM_G_LCD);
	VRAMBLOCK_SETBANK_G(VRAM_G_LCDC_MODE);
	
	//vramSetBankH(VRAM_H_LCD);
	VRAMBLOCK_SETBANK_H(VRAM_H_LCDC_MODE);
	
	//vramSetBankI(VRAM_I_LCD);
	VRAMBLOCK_SETBANK_I(VRAM_I_LCDC_MODE);
	
	//custom Handler
	setupCustomExceptionHandler((uint32*)&CustomDebugHandler);
	
	fb_setBGColor(30653);	
	bg_setBGColor(0);
	drawStartSplash();
	
	SET_MAIN_TOP_LCD();	 // set fb to top screen
	fb_swapBuffers();
	bg_swapBuffers();
	
	// out of order for competition
	//irqInit(); // initialize irqs
	enableVBlank(); // initialize vblank irq
	
	setMode(INITFAT);
	setSoundInterrupt(); // initialize fifo irq
	invalidateHomeLoaded();
	
	setGenericSound(11025, 127, 64, 1);
	waitForInit();			//  wait until arm7 has loaded and gone to sleep
	
	initComplexSound(); // initialize sound variables
	initWifi();
	fixGautami();
	
	setCursorProperties(0, 2, 0, 0);
	initCalc();
	initScribble();
	initCapture();
	initClipboard();
	
	// set defaults to english in case we can't load the langauge file for some reason
	// also takes care of partial translations.	
	initLanguage();
	initRandomList();
	fixAndTags();
	resetKeyboard();
	
	curDay = getDay();
	curMonth = getMonth();
	curYear = getYear();
	
	oldDay = 0;
	oldYear = 0;
	oldMonth = 0;
	
	if(DRAGON_InitFiles() == false)
	{
		// oops, no cf card!
		setMode(DISPLAYCOW);
		
		setFont((uint16 **)font_arial_11);
		setColor(0xFFFF);
		
		bg_dispSprite(96, 5, errmsg, 0);
		bg_setClipping(5,25,250,181);
		bg_dispString(0,0,l_nofat);
		bg_swapBuffers();
		
		while(1)
		{
			// wee, la la la!
		}
	}
	//otherwise card init correctly.
	else{
		
	}
	//--------------------------------------------------------------------
	//finished init, now check to make sure the DSOrganize dir is there...
	//--------------------------------------------------------------------
	
	findDataDirectory();
	makeDirectories();
	
	if(DRAGON_FileExists("DSOrganize") != FT_DIR)
	{
		setMode(DISPLAYCOW);
		
		// oops, not there, we must create!
		DRAGON_mkdir("DSOrganize");
		DRAGON_chdir("DSOrganize");
		DRAGON_mkdir("DAY");
		DRAGON_mkdir("HELP");
		DRAGON_mkdir("LANG");
		DRAGON_mkdir("RESOURCES");
		DRAGON_mkdir("REMINDER");
		DRAGON_mkdir("SCRIBBLE");
		DRAGON_mkdir("TODO");
		DRAGON_mkdir("VCARD");
		DRAGON_mkdir("ICONS");
		DRAGON_mkdir("CACHE");
		DRAGON_mkdir("HOME");
		DRAGON_chdir("/");
		
		makeDefaultSettings();
		
		setFont((uint16 **)font_arial_11);
		setColor(0xFFFF);
		
		bg_dispSprite(96, 5, errmsg, 0);
		bg_setClipping(5,25,250,181);
		bg_dispString(0,0, l_createdir);
		bg_swapBuffers();
		
		//while(!keysPressed())
		//{
		//}
	}
	else{
		
	}
	setMode(INITPLUGIN);
	
	//-------------------------------------------------------------------
	//finished creating dirs, now check to make sure if they extracted it
	//did their extracting program actually get all the dirs?
	//-------------------------------------------------------------------	
	
	DRAGON_chdir(getDefaultDSOrganizeFolder("").c_str());
	
	if(DRAGON_FileExists("DAY") != FT_DIR)
	{
		DRAGON_mkdir("DAY");
	}
	if(DRAGON_FileExists("HELP") != FT_DIR)
	{
		DRAGON_mkdir("HELP");
	}
	if(DRAGON_FileExists("LANG") != FT_DIR)
	{
		DRAGON_mkdir("LANG");
	}
	if(DRAGON_FileExists("REMINDER") != FT_DIR)
	{
		DRAGON_mkdir("REMINDER");
	}
	if(DRAGON_FileExists("SCRIBBLE") != FT_DIR)
	{
		DRAGON_mkdir("SCRIBBLE");
	}
	if(DRAGON_FileExists("TODO") != FT_DIR)
	{
		DRAGON_mkdir("TODO");
	}
	if(DRAGON_FileExists("VCARD") != FT_DIR)
	{
		DRAGON_mkdir("VCARD");
	}
	if(DRAGON_FileExists("ICONS") != FT_DIR)
	{
		DRAGON_mkdir("ICONS");
	}
	if(DRAGON_FileExists("CACHE") != FT_DIR)
	{
		DRAGON_mkdir("CACHE");
	}
	if(DRAGON_FileExists("HOME") != FT_DIR)
	{
		DRAGON_mkdir("HOME");
	}
	
	DRAGON_chdir("/");
	
	
	//-------------------------------------------
	//how about we load the settings for them eh?
	//-------------------------------------------
	loadSettings();
	fb_setBGColor(genericFillColor);
	bg_setBGColor(genericFillColor);
	
	DRAGON_chdir(getDefaultDSOrganizeFolder("").c_str());
	
	std::string PathFix = getDefaultDSOrganizePath(string("startup.wav"));
	if(DRAGON_FileExists(PathFix.c_str()) == FT_FILE)
	{
		char tStr[MAX_TGDSFILENAME_LENGTH+1] = {0};
		sprintf(tStr, "%s", PathFix.c_str());
		loadWavToMemory();
		loadSound(tStr);
	}
	DRAGON_chdir("/");
}

void drawCurrentApp()
{
	//printfDebugger("drawCurrentApp() Mode:%d",getMode());	//while(1==1){} inside
	
	switch(getMode())
	{
		case SOUNDPLAYER:
			// we draw later
			break;
		case HOME:	//4 : OK
			drawSplash();
			drawHomeScreen();
			break;
		case HOMEMORE:	
			drawSplash();
			drawHomeScreenMore();
			break;
		case HOMESHORTCUTS:
			drawSplash();
			drawHomeScreenShortcuts();					
			break;
		case CALENDAR:	//0 : OK
			drawTimeDate();
			drawReminders();
			drawCalendar();
			break;
		case EDITREMINDER:	//todo
			drawTimeDate();
			drawReminder();
			drawEditReminder();
			break;
		case DAYVIEW:	//2 : OK
			drawTimeDate();
			drawCurrentEvent();
			drawDayView();
			break;			
		case EDITDAYVIEW://3 : OK
			drawTimeDate();
			drawEditDayView();
			drawEditCurrentEvent();
			break;
		case ADDRESS:	//5 : OK
			drawAddressList();
			drawCurrentAddress();
			break;		
		case EDITADDRESS: //6 : todo
			drawCurrentAddress();
			drawEditAddress();
			break;		
		case CONFIGURATION:	//7 : OK	//bugs could arise when read/writing system config files
			drawTopConfiguration();
			drawConfiguration();
			break;
		case BROWSER:
			drawFileBrowserScreen();
			drawFileInfoScreen();
			break;
		case BROWSERRENAME:
			drawEditFileName();
			drawEditFileControls();
			break;
		case CALCULATOR:
			drawAnswerScreen();
			drawCalculatorButtons();
			break;
		case TEXTEDITOR:
			drawEditScreen();
			drawEditControls();
			break;
		case PICTUREVIEWER:
			drawPictureScreen();
			drawZoomScreen();
			break;
		case ABOUT:
			drawAboutScreen();
			break;
		case TODOLIST:
			drawTodoList();
			drawCurrentTodo();
			break;	
		case EDITTODOLIST:
			drawCurrentTodo();
			drawEditTodo();
			break;
		case SCRIBBLEPAD:
			drawScribbleList();
			drawSplash();
			break;					
		case EDITSCRIBBLE:		
			drawScribbleScreen();
			drawToolsScreen();
			break;
		case CHOOSECOLOR:
			drawColorPicker();
			drawToolsScreen();
			break;
		case SCRIBBLETEXT:
			drawScribbleText();
			drawScribbleKeyboard();
			break;
		case HOMEBREWDATABASE:
			drawTopDatabaseScreen();
			drawBottomDatabaseScreen();
			break;
		case VIEWER:
			drawViewerScreens();
			break;
		case SOUNDRECORDER:
			drawTopRecordScreen();
			drawBottomRecordScreen();					
			break;
		case IRC:
			drawTopIRCScreen();
			drawBottomIRCScreen();
			break;
		case WEBBROWSER:
			drawWebBrowserScreens();
			break;
	}
}

void drawSoundScreen(int oldMode)
{
	if(getMode() == SOUNDPLAYER) // this is that 'later' thing i was talking about
	{
		if(oldMode == BROWSER || wasShortcutLaunched())
		{
			updateStreamLoop();
			bg_swapBuffers();
			
			updateStreamLoop();
			resetShortcutLaunchedFlag();
		}
		
		updateStreamLoop();
		checkEndSound();
		
		updateStreamLoop();
		drawTopSoundScreen();
		updateStreamLoop();
		drawBottomSoundScreen();
	}	
}

#ifdef PROFILING
unsigned int hblanks = 0;

void hblank_handler(void)
{
	hblanks++;
}
#endif

int main(int _argc, sint8 **_argv) {
	
	IRQInit();
	
	bool project_specific_console = false;	//set default console or custom console: default console
	GUI_init(project_specific_console);
	GUI_clear();
	
	initProgram();	//overrides TGDS project console
	
	sint32 fwlanguage = (sint32)getLanguage();
	
	//initProgram()->DRAGON_InitFiles() does it
	/*
	int ret=FS_init();
	if (ret == 0)
	{
		printf("FS Init ok.");
	}
	else if(ret == -1)
	{
		printf("FS Init error.");
	}
	*/
	
	//printfDebugger("initProgram() OK");	//while(1==1){} inside
	
	#ifdef PROFILING
	int counter = 0;
	
	irqSet(IRQ_HBLANK, hblank_handler);
	irqEnable(IRQ_HBLANK);
	
	cygprofile_begin();
	cygprofile_enable();
	#endif
	
	while(1) 
	{
		updateStreamLoop();
		
		if(!checkHelp())
		{
			int oldMode = 0;
			
			if(getLCDState() == LCD_ON)
			{
				updateStreamLoop();
				clearHelpScreen();
				
				updateStreamLoop();
				drawCurrentApp();
				
				// save the current mode before switching modes
				oldMode = getMode();
			}
			//so far here
			
			updateStreamLoop();
			checkKeys();
			
			updateStreamLoop();
			executeKeys();
			
			// Split here because the state can change in checkKeys
			if(getLCDState() == LCD_ON)
			{
				
				// draw sound screen if needed			
				updateStreamLoop();
				drawSoundScreen(oldMode);
				
				#ifdef SCREENSHOT_MODE
				takeScreenshot();			
				#endif
				
				updateStreamLoop();
				handleKeyHighlighting();
				
				updateStreamLoop();
				drawToScreen();
			}
			else
			{	
				updateStreamLoop();
				checkEndSound();
			}
		}
		
		#ifdef PROFILING
		counter++;
		
		if(counter == 700)
		{
			cygprofile_disable();
			cygprofile_end();
		}
		#endif
	}
	
	/*
	while (1)
	{
		IRQVBlankWait();
	}
	*/
	
}

bool isScrolling()
{
	return held;
}

//-----------------
// navigation stuff
//-----------------

void goForward()
{
	switch(getMode())
	{
		case CALENDAR:
			resetCursor();
			setMode(EDITREMINDER);
			break;
		case EDITREMINDER:
			saveReminder();
			setMode(CALENDAR);
			break;
		case DAYVIEW:
			resetCursor();
			loadRepeatMode();
			setMode(EDITDAYVIEW);
			break;
		case EDITDAYVIEW:
			saveDayView();
			setMode(DAYVIEW);
			break;
		case HOME:
		case HOMEMORE:
		case HOMESHORTCUTS:
			runHomeAction();
			break;
		case EDITADDRESS:
			editAddressForward();
			break;
		case ADDRESS:
			addressForward();
			break;
		case CONFIGURATION:
			configurationForward();
			break;
		case BROWSER:
			browserForward();
			break;
		case BROWSERRENAME:
			browserSafeFileName();
			
			if(getLastMode() == BROWSER)
			{
				renameFile();
				browserSetUnPopulated();
				popCursor();
				setMode(getLastMode());
			}
			if(getLastMode() == SCRIBBLEPAD)
			{
				createScribble();	
				popCursor();
			}
			
			break;
		case TEXTEDITOR:
			saveTextFile();
			popCursor();
			browserSetUnPopulated();
			fb_setBGColor(genericFillColor);
			resetCurDir();
			setMode(BROWSER);
			freeText();
			break;
		case PICTUREVIEWER:
			resetScreen();
			break;
		case TODOLIST:
			todoForward();
			break;
		case EDITTODOLIST:
			editTodoForward();			
			break;
		case SCRIBBLEPAD:
			scribbleForward();
			break;
		case EDITSCRIBBLE:
			saveScribble();
			scribbleSetUnPopulated();
			saveSettings(); // to save the custom color data
			popCursor();
			setMode(SCRIBBLEPAD);
			break;
		case CHOOSECOLOR:
			destroyColorPicker(true);
			break;
		case SCRIBBLETEXT:
			exitScribbleConfirm();
			popCursor();
			break;
		case SOUNDPLAYER:
			togglePause();
			break;
		case HOMEBREWDATABASE:
			getHomebrew();
			break;
		case VIEWER:
			toggleBookmark();
			break;
		case SOUNDRECORDER:
			recorderForward();
			break;
		case IRC:
			ircForward();
			break;
		case WEBBROWSER:
			webBrowserForward();
			break;
	}
}

void goBack()
{
	switch(getMode())
	{
		case CALENDAR:
			resetDayView();
			destroyReminder();
			setMode(DAYVIEW);
			curTime = getCurTime();
			break;
		case EDITREMINDER:
			oldDay = 0;
			setMode(CALENDAR);
			resetCalLoaded();
			break;
		case DAYVIEW:
			resetDayView();
			resetCalLoaded();
			setMode(CALENDAR);
			break;
		case EDITDAYVIEW:
			resetDayView();
			setMode(DAYVIEW);
			break;
		case ADDRESS:
			addressBack();
			break;
		case EDITADDRESS:
			editAddressBack();
			break;
		case CONFIGURATION:	
			configurationBack();
			break;
		case BROWSER:
			browserBack();
			break;
		case BROWSERRENAME:
			popCursor();
			setMode(getLastMode());
			break;
		case TEXTEDITOR:
			popCursor();
			browserSetUnPopulated();
			fb_setBGColor(genericFillColor);
			resetCurDir();
			setMode(BROWSER);
			freeText();
			break;
		case PICTUREVIEWER:
			exitPictureScreen();
			browserSetUnPopulated();
			setMode(BROWSER);
			break;
		case TODOLIST:
			todoBack();
			break;
		case EDITTODOLIST:
			editTodoBack();
			break;
		case SCRIBBLEPAD:		
			scribbleBack();
			break;
		case EDITSCRIBBLE:
			fb_setBGColor(genericFillColor);
			saveSettings(); // to save the custom color data
			popCursor();
			setMode(SCRIBBLEPAD);
			break;		
		case CHOOSECOLOR:
			destroyColorPicker(false);
			break;
		case SCRIBBLETEXT:
			exitScribbleCancel();
			popCursor();
			break;
		case SOUNDPLAYER:
			destroySound();
			setMode(BROWSER);
			break;			
		case HOMEBREWDATABASE:
			refreshList();
			break;
		case VIEWER:
			popCursor();
			browserSetUnPopulated();
			resetCurDir();
			setMode(BROWSER);
			freeViewerText();
			break;
		case SOUNDRECORDER:
			recorderBack();
			break;
		case IRC:
			ircSwapButton();
			break;
		case WEBBROWSER:
			webBrowserBack();
			break;
	}
}

void touchDown(int px, int py)
{
	switch(getMode())
	{
		case EDITREMINDER:
		case EDITDAYVIEW:
		case BROWSERRENAME:
		case SCRIBBLETEXT:
		case EDITADDRESS:
		case TEXTEDITOR:
		case EDITTODOLIST:
			kChar = executeClick(px, py);
			break;
		case CONFIGURATION:
			if(isTextEntryScreen())
			{
				kChar = executeClick(px, py);
			}
			break;
		case IRC:
			kChar = ircHandleClick(px, py);
			break;
		case WEBBROWSER:
			kChar = webHandleClick(px, py);
			break;
		case SOUNDPLAYER:
			if(py >= 162 && py <= 182)
			{
				if(px >= 23 && px <= 126)
				{
					queueUpCommand(SND_GOFORWARD,0);
				}
				else if(px >= 130 && px <= 233)
				{
					queueUpCommand(SND_GOBACK,0);
				}
				
				return;
			}
			
			if(isHome(px, py))
			{
				queueUpCommand(SND_TOGGLEHOLD,0);
				
				hasTouched = true;
				return;
			}
			else if(isLButton(px, py))
			{
				queueUpCommand(SND_GETPREV,1);
				
				hasTouched = true;
				return;
			}
			else if(isRButton(px, py))
			{
				queueUpCommand(SND_GETNEXT,1);
				
				hasTouched = true;
				return;
			}
			
			if(py >=20 && py <= 40)
			{
				queueUpCommand(SND_SEEK, px);
				
				return;
			}
			
			break;
		case SOUNDRECORDER:
			if(py >= 162 && py <= 182)
			{
				if(px >= 23 && px <= 126)
				{
					queueUpCommand(SND_GOFORWARD,0);
				}
				else if(px >= 130 && px <= 233)
				{
					queueUpCommand(SND_GOBACK,0);
				}
				
				return;
			}
			
			if(isHome(px, py))
			{
				queueUpCommand(SND_TOGGLEHOLD,0);
				
				return;
			}
			
			if(py >=20 && py <= 40)
			{
				queueUpCommand(SND_SEEK, px);
			}
			
			break;
		case CALCULATOR:
			kChar = getButton(px, py);
			break;
	}
}

void touchUp()
{
	hasTouched = false;
	
	char kC = executeUp();
	
	if(kC != 0)
	{
		kChar = kC;
	}
	
	if(getMode() == WEBBROWSER)
	{
		webBrowserRelease();
	}
}

void checkKeyboards()
{
	if(!kChar)
	{
		return;
	}

	switch(getMode())
	{
		case SOUNDPLAYER:
		case SOUNDRECORDER:
			hasTouched = false;
			break;
		case EDITREMINDER:
			clickSound();
			editReminderAction(kChar);
			break;
		case EDITDAYVIEW:
			clickSound();
			editDayViewAction(kChar);
			break;
		case EDITADDRESS:
			clickSound();
			editAddressAction(kChar);
			break;
		case BROWSERRENAME:
			clickSound();
			editFileNameAction(kChar);
			break;
		case TEXTEDITOR:
			clickSound();
			editTextFileAction(kChar);
			break;
		case EDITTODOLIST:
			clickSound();
			editTodoAction(kChar);
			break;
		case SCRIBBLETEXT:
			clickSound();
			editScribbleAction(kChar);
			break;
		case IRC:
			clickSound();
			editIRCAction(kChar);
			break;
		case WEBBROWSER:
			clickSound();
			editWebBrowserAction(kChar);
			break;
		case CONFIGURATION:
			clickSound();
			editConfigAction(kChar);
			break;
		case CALCULATOR:
			clickSound();
			calcAction(kChar);
			break;
	}
	
	if(kChar == CLEAR_KEY || kChar == CAP || kChar == SHF || kChar == SPL)
	{
		lChar = 0;
	}
	else
	{
		lChar = kChar;
	}
	
	kChar = 0;
}

bool checkSleepState()
{
	if(getLCDState() == LCD_OFF)
	{
		if(!(keysPressed() & KEY_LID))
		{		
			setLCDState(LCD_ON);
			return true;
		}
	}
	else
	{
		if(keysPressed() & KEY_LID)
		{
			setLCDState(LCD_OFF);
			return true;
		}
	}
	
	return false;
}

void checkKeys()
{
	checkKeyboards();
	struct touchScr touch = touchReadXYNew();
	if(checkSleepState())
	{
		return;
	}
	
	if(((keysPressed() | keysPressed()) & KEY_TOUCH) && !hasTouched)
	{
		yThreshold = touch.touchYpx; // to stop shaking on some DS units.
		
		if(!touch.touchXpx || !touch.touchYpx)
		{
			return;
		}
		
		hasTouched = true;
		
		if(isButtons(touch.touchXpx, touch.touchYpx) && getMode() != HOME && getMode() != HOMEMORE && getMode() != HOMESHORTCUTS && getMode() != CALCULATOR && getMode() != EDITSCRIBBLE && getMode() != CHOOSECOLOR && getMode() != VIEWER && getMode() != SOUNDPLAYER && getMode() != SOUNDRECORDER && getMode() != BROWSER && getMode() != WEBBROWSER)
		{
			if(isForwardButton(touch.touchXpx, touch.touchYpx))
			{
				goForward();
			}
			else if(isBackButton(touch.touchXpx, touch.touchYpx))
			{
				goBack();
			}
		}		
		else
		{	
			switch(getMode())
			{
				case ABOUT:
				{
					if(touch.touchXpx >= 32 && touch.touchXpx <= 244 && touch.touchYpx >= 70 && touch.touchYpx <= 79)
					{
						// clicked on url to my site
						
						setMode(WEBBROWSER);
						initWebBrowser();
						resetCursor();
						
						setTemporaryHome("http://www.dragonminded.com/ndsdev/");
					}
					
					break;
				}
				case HOME:
				case HOMEMORE:
				case HOMESHORTCUTS:
				{
					int tmpCur = cursorHomeScreen(touch.touchXpx, touch.touchYpx);
					
					if(tmpCur != -1)
					{
						if(getMode() == HOMESHORTCUTS && tmpCur == 6)
						{
							break;
						}
						
						moveCursorAbsolute(tmpCur);
						goForward();
					}
					break;
				}
				case CALENDAR:
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						destroyReminder();
						returnHome();
					}
					else if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						--curMonth;
						if(curMonth < 1)
						{
							curMonth = 12;
							--curYear;
						}
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						++curMonth;
						if(curMonth > 12)
						{
							curMonth = 1;
							++curYear;
						}
					} 
					else
					{
						int z = getDayFromTouch(touch.touchXpx,touch.touchYpx);
						if(z != -1)
						{
							if(curDay == z && secondClickAction()) // second click
								goForward();
							curDay = z;
						}
					}
					break;
				case DAYVIEW:		
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						resetDayView();
						returnHome();
					}
					else if(isLButton(touch.touchXpx, touch.touchYpx))
					{	
						int cd = curDay;
						int cm = curMonth-1;
						int cy = curYear;
						
						subtractDay(&cd, &cm, &cy);
						resetDayView();	
						
						curDay = cd;
						curMonth = cm+1;
						curYear = cy;
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						int cd = curDay;
						int cm = curMonth-1;
						int cy = curYear;
						
						addDay(&cd, &cm, &cy);
						resetDayView();	
						
						curDay = cd;
						curMonth = cm+1;
						curYear = cy;
					}	
					else
					{
						int tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
						if(tmpCursor != -1)
						{
							if(curTime == tmpCursor && secondClickAction()) // double click
							{
								goForward();
							}
							curTime = tmpCursor;
						}
						
						if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
						{
							if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
							{
								if(curTime > 0)
								{
									--curTime;
								}
							}
							else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT))
							{
								if(curTime < 47)
								{
									++curTime;
								}
							}
							else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)))
							{
								held = true;
								curTime = getScroll(touch.touchYpx, 48);
							}
						}
					}
					break;
				case EDITDAYVIEW:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						decreaseDuration();
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						increaseDuration();
					}
					break;
				case ADDRESS:
				case BROWSER:
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						if(getMode() == BROWSER)
						{
							freeDirList();
							destroyRandomList();
							destroyPullUp();
							returnHome();
						}
						else
						{
							freeVCard();
							returnHome();
						}
						
						break;
					}
					
					if(isTopArea(touch.touchXpx, touch.touchYpx))
					{
						if(getMode() == BROWSER)
						{
							if(!isPullUp())
							{
								if(isLButton(touch.touchXpx, touch.touchYpx))
								{
									if(!browserMode)
									{
										browserMode = MAXBROWSERCYCLES;
									}
									
									--browserMode;
								}
								else if(isRButton(touch.touchXpx, touch.touchYpx))
								{
									++browserMode;
									
									if(browserMode == MAXBROWSERCYCLES)
									{
										browserMode = 0;
									}
								}
							}
							else
							{
								destroyPullUp();
							}
						}
					}
					else
					{		
						if(!testPullUp(touch.touchXpx, touch.touchYpx))
						{
							int tmpCursor = -1;
							int numEntries = 0;
							
							if(getMode() == BROWSER)
							{
								numEntries = getBrowserEntries();								
							}
							else if(getMode() == ADDRESS)
							{
								numEntries = getAddressEntries();
							}
							
							tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
							
							if(tmpCursor != -1)
							{
								bool setCursor = true;
								
								if(getMode() == BROWSER)
								{
									if(getCursor() == tmpCursor && secondClickAction())
									{
										// secondary click
										setCursor = false;
										loadFileWrapper(getCursor());
									}
								}								
								else if(getMode() == ADDRESS)
								{
									if(getCursor() == tmpCursor && secondClickAction())
									{
										// secondary click
										setCursor = false;
										addressForward();
									}
								}
								
								if(setCursor)
								{
									moveCursorAbsolute(tmpCursor);
								}
							}
							
							if(numEntries > 9)
							{
								if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
								{
									if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
									{
										if(getCursor() > 0)
										{
											moveCursorRelative(CURSOR_BACKWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT))
									{
										if(getCursor() < numEntries-1)
										{
											moveCursorRelative(CURSOR_FORWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)))
									{
										held = true;									
										moveCursorAbsolute(getScroll(touch.touchYpx, numEntries));
									}
								}
							}						
						}
					}
					break;
				case EDITADDRESS:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						if(editField > 0)
						{
							formatCell(editField);
							--editField;
							resetCursor();
							clearSelect();
						}
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						if(editField < 10)
						{
							formatCell(editField);
							++editField;
							resetCursor();
							clearSelect();
						}
					}
					else if(touch.touchXpx >= DELETE_LEFT && touch.touchXpx <= DELETE_RIGHT && touch.touchYpx >= DELETE_TOP && touch.touchYpx <= DELETE_BOTTOM)
					{
						deleteAddress();
					}
					break;
				case CONFIGURATION:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						configDecCursor();
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						configIncCursor();
					}
					else if(isHome(touch.touchXpx, touch.touchYpx) && !configFlipped())
					{
						configurationSwitchPages();
					}
					else
					{
						if(isAPScreen())
						{
							int tmpCursor = -1;
							int numap = Wifi_GetNumAP();
							
							tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
							
							if(tmpCursor != -1)
							{
								if(getCursor() == tmpCursor && secondClickAction())
								{
									// secondary click
									configurationForward();
									return;
								}
								
								moveCursorAbsolute(tmpCursor);
							}
							
							if(numap > 9)
							{
								if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
								{
									if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
									{
										if(getCursor() > 0)
										{
											moveCursorRelative(CURSOR_BACKWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT))
									{
										if(getCursor() < numap-1)
										{
											moveCursorRelative(CURSOR_FORWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)))
									{
										held = true;									
										moveCursorAbsolute(getScroll(touch.touchYpx, numap));
									}
								}
							}
						}
						else
						{
							configureAction(touch.touchXpx, touch.touchYpx);
						}
					}
					break;
				case CALCULATOR:
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						closeScreen();
						returnHome();
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						toggleDeg();
					}
					else if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						toggleMore();
					}
					break;
				case TEXTEDITOR:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						moveTextCursor(CURSOR_PAGEUP);
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						moveTextCursor(CURSOR_PAGEDOWN);
					}
					break;
				case PICTUREVIEWER:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						getPrevPicture();
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						getNextPicture();
					} 
					else
					{
						movePicture(touch.touchXpx, touch.touchYpx);
					}
					break;
				case TODOLIST:
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						freeTodo();
						returnHome();
					}
					else
					{
						int tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
						
						if(tmpCursor != -1)
						{
							if(getCursor() == tmpCursor && secondClickAction()) // double click
							{
								todoForward();
							}
							moveCursorAbsolute(tmpCursor);
						}
						
						if(getTodoEntries() > 9)
						{
							if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
							{
								if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
								{
									if(getCursor() > 0)
									{
										moveCursorRelative(CURSOR_BACKWARD);
									}
								}
								else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT))
								{
									if(getCursor() < getTodoEntries() - 1)
									{
										moveCursorRelative(CURSOR_FORWARD);
									}
								}
								else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)))
								{
									held = true;									
									moveCursorAbsolute(getScroll(touch.touchYpx, getTodoEntries()));
								}
							}
						}
					}
					break;
				case EDITTODOLIST:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						if(editField > 0)
						{
							--editField;
							resetCursor();
							clearSelect();
						}
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						if(editField < 1)
						{
							++editField;
							resetCursor();
							clearSelect();
						}
					}
					else if(touch.touchXpx >= DELETE_LEFT && touch.touchXpx <= DELETE_RIGHT && touch.touchYpx >= DELETE_TOP && touch.touchYpx <= DELETE_BOTTOM)
					{
						deleteTodo();
					}
					break;
				case SCRIBBLEPAD:
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						freeScribbleList();
						returnHome();
					}
					else
					{
						int tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
						
						if(tmpCursor != -1)
						{
							if(getCursor() == tmpCursor && secondClickAction()) // double click
								goForward();
							moveCursorAbsolute(tmpCursor);
						}
						
						if(getScribbleEntries() > 9)
						{
							if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
							{
								if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
								{
									if(getCursor() > 0)
									{
										moveCursorRelative(CURSOR_BACKWARD);
									}
								}
								else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT))
								{
									if(getCursor() < getScribbleEntries() - 1)
									{
										moveCursorRelative(CURSOR_FORWARD);
									}
								}
								else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)))
								{
									held = true;									
									moveCursorAbsolute(getScroll(touch.touchYpx, getScribbleEntries()));
								}
							}
						}
					}
					break;
				case EDITSCRIBBLE:
					scribbleDown(touch.touchXpx, touch.touchYpx);
					break;
				case CHOOSECOLOR:
					colorPickerDown(touch.touchXpx, touch.touchYpx);
					break;
				case HOMEBREWDATABASE:
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						lButtonDatabase();
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						rButtonDatabase();
					}	
					else if(isHome(touch.touchXpx, touch.touchYpx))
					{
						dbStartPressed();
					}
					else
					{
						if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 13 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) + 2)
						{
							if(touch.touchXpx >= 3 && touch.touchXpx <= 13)
							{
								prevCategory();
							}
							else if(touch.touchXpx >= 242 && touch.touchXpx <= 252)
							{
								nextCategory();
							}
						}
						else
						{
							int tmpCursor = getCursorFromTouch(touch.touchXpx, touch.touchYpx);
							
							if(tmpCursor != -1)
							{
								if(getCursor() == tmpCursor && secondClickAction()) // double click
								{
									goForward();
								}
								moveCursorAbsolute(tmpCursor);
							}	
							
							if(getHBDBEntries() > 8)
							{
								if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
								{
									if(touch.touchYpx >= DEFAULT_SCROLL_TOP && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT))
									{
										if(getCursor() > 0)
										{
											moveCursorRelative(CURSOR_BACKWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) - 15 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 15)
									{
										if(getCursor() < getHBDBEntries()-1)
										{
											moveCursorRelative(CURSOR_FORWARD);
										}
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)) - 15)
									{
										held = true;									
										moveCursorAbsolute(getScroll(touch.touchYpx, getHBDBEntries()));
									}
								}
							}
						}
					}
					break;
				case VIEWER:
					if(touch.touchYpx > 176)
					{
						goBack();
					}
					
					if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
					{
						if(touch.touchYpx >= 3 && touch.touchYpx <= (3 + SCROLL_ARROW_HEIGHT))								
						{
							moveViewerCursor(CURSOR_UP);
						}
						else if(touch.touchYpx >= (176 - SCROLL_ARROW_HEIGHT) && touch.touchYpx <= 176)
						{
							moveViewerCursor(CURSOR_DOWN);
						}
						else if(touch.touchYpx >= (3 + SCROLL_ARROW_HEIGHT + 2) && touch.touchYpx <= (176 - SCROLL_ARROW_HEIGHT - 2))
						{
							held = true;			
							setViewerPos(getScroll(touch.touchYpx, getViewerMax()));
						}
					}
					
					break;
				case IRC:
					if(isTopArea(touch.touchXpx, touch.touchYpx))
					{
						if(touch.touchXpx < 24)
						{
							ircLButton();
						}
						else if(touch.touchXpx > 24 && touch.touchXpx < 231)
						{
							ircActivateTab(touch.touchXpx);
						}
						else if(touch.touchXpx > 231)
						{
							ircRButton();
						}
					}
					else if(isNicks() && getIRCNickCount() > 7)
					{
						if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
						{
							if(touch.touchYpx >= DEFAULT_SCROLL_TOP + 32 - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT) + 32 - 9)								
							{
								ircUp();
							}
							else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 9)
							{
								ircDown();
							}
							else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) + 32 - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)) - 9)
							{
								held = true;			
								setScrollIRC(getScroll(touch.touchYpx, getIRCNickCount()));
							}
						}
					}
					
					break;
				case WEBBROWSER:
					if(isShowingKeyboard() || (!isShowingKeyboard() && (isShowingImage() || isDownloadingFile())))
					{
						if(isLButton(touch.touchXpx, touch.touchYpx))
						{
							webBrowserLButton();
						}
						else if(isRButton(touch.touchXpx, touch.touchYpx))
						{
							webBrowserRButton();
						}	
						else if(isHome(touch.touchXpx, touch.touchYpx))
						{
							freeWebBrowser();
							returnHome();
						}
						else
						{
							if(touch.touchXpx >= 229 && touch.touchYpx >= 19 && touch.touchXpx < 241 && touch.touchYpx < 33)
							{
								if(!isShowingImage())
								{
									webBrowserStop();
								}
							}
							else if(touch.touchYpx >= 162 && touch.touchYpx <= 182)
							{
								if(touch.touchXpx >= 23 && touch.touchXpx <= 126)
								{
									goForward();
								}
								else if(touch.touchXpx >= 130 && touch.touchXpx <= 233)
								{
									goBack();
								}
							}
							
							if(isFav() && getWebBrowserEntries() > 7)
							{
								if(touch.touchXpx >= getScrollLeft() && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH))
								{
									if(touch.touchYpx >= DEFAULT_SCROLL_TOP + 32 - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT) + 32 - 9)
									{
										moveWebBrowserCursor(CURSOR_UP);
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - SCROLL_ARROW_HEIGHT) - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 9)
									{
										moveWebBrowserCursor(CURSOR_DOWN);
									}
									else if(touch.touchYpx >= (DEFAULT_SCROLL_TOP + SCROLL_ARROW_HEIGHT + 2) + 32 - 9 && touch.touchYpx <= (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT - (SCROLL_ARROW_HEIGHT + 2)) - 9)
									{
										held = true;									
										setScrollWeb(getScroll(touch.touchYpx, getWebBrowserEntries()));
									}
								}
							}
						}
					}
					else
					{
						webBrowserClick(touch.touchXpx, touch.touchYpx);
					}
					
					break;
			}
		}
		
		return;
	}
	
	if((keysPressed() & KEY_TOUCH) && hasTouched)
	{
		if(abs(yThreshold - touch.touchYpx) > 1)
		{
			yThreshold = touch.touchYpx;
			
			if(held)
			{
				if(touch.touchXpx >= (getScrollLeft() - 40) && touch.touchXpx <= (getScrollLeft() + DEFAULT_SCROLL_WIDTH + 40))
				{
					switch(getMode())
					{
						case DAYVIEW:
							curTime = getScroll(touch.touchYpx, 48);
							break;
						case ADDRESS:
							moveCursorAbsolute(getScroll(touch.touchYpx, getAddressEntries()));
							break;
						case BROWSER:
							moveCursorAbsolute(getScroll(touch.touchYpx, getBrowserEntries()));
							break;
						case TODOLIST:
							moveCursorAbsolute(getScroll(touch.touchYpx, getTodoEntries()));
							break;
						case SCRIBBLEPAD:
							moveCursorAbsolute(getScroll(touch.touchYpx, getScribbleEntries()));
							break;
						case HOMEBREWDATABASE:
							moveCursorAbsolute(getScroll(touch.touchYpx, getHBDBEntries()));
							break;
						case VIEWER:
							setViewerPos(getScroll(touch.touchYpx, getViewerMax()));
							break;
						case CONFIGURATION:
							moveCursorAbsolute(getScroll(touch.touchYpx, Wifi_GetNumAP()));
							break;
					}
				}
			}
		}
		
		switch(getMode())
		{
			case PICTUREVIEWER:
				movePicture(touch.touchXpx, touch.touchYpx);
				break;
			case EDITSCRIBBLE:
				scribbleMove(touch.touchXpx, touch.touchYpx);
				break;
			case CHOOSECOLOR:
				colorPickerMove(touch.touchXpx, touch.touchYpx);
				break;
			case CONFIGURATION:
				if(!configurationDrag(touch.touchXpx, touch.touchYpx))
				{
					if(getRepCount() >= 4 && lChar != 0)
					{
						touchDown(touch.touchXpx, touch.touchYpx);
						if(kChar != lChar)
						{
							kChar = 0;
						}
						
						lChar = 0;
						
						setRepCount(0);
					}
				}
				
				break;
			case BROWSER:
				trackPullUp(touch.touchXpx, touch.touchYpx);
				break;
			case SOUNDPLAYER:
			case SOUNDRECORDER:
			case CALCULATOR:
				break;
			case IRC:
				if(isIRCKeyboard())
				{
					if(getRepCount() >= 4 && lChar != 0)
					{
						touchDown(touch.touchXpx, touch.touchYpx);
						if(kChar != lChar)
						{
							kChar = 0;
						}
						
						lChar = 0;
						
						setRepCount(0);
					}
				}
				else
				{
					if(held)
					{
						setScrollIRC(getScroll(touch.touchYpx, getIRCNickCount()));
					}
				}
				break;				
			case WEBBROWSER:
				if(!isShowingKeyboard())
				{
					webBrowserDrag(touch.touchXpx, touch.touchYpx);
				}
				else
				{
					if(isWebKeyboard())
					{
						if(getRepCount() >= 4 && lChar != 0)
						{
							touchDown(touch.touchXpx, touch.touchYpx);
							if(kChar != lChar)
							{
								kChar = 0;
							}
							
							lChar = 0;
							
							setRepCount(0);
						}
					}
					else
					{
						if(held)
						{
							setScrollWeb(getScroll(touch.touchYpx, getWebBrowserEntries()));
						}
					}
				}
				break;
			default:
				if(getRepCount() >= 4 && lChar != 0)
				{
					touchDown(touch.touchXpx, touch.touchYpx);
					if(kChar != lChar)
					{
						kChar = 0;
					}
					
					lChar = 0;
					
					setRepCount(0);
				}
				break;
		}
		
		return;
	}
	
	if(keysReleased() & KEY_TOUCH)
	{			
		hasTouched = false;
		
		switch(getMode())
		{
			case DAYVIEW:
			case ADDRESS:
			case TODOLIST:
			case SCRIBBLEPAD:
			case VIEWER:
			case IRC:
			case HOMEBREWDATABASE:
				held = false;
				break;
			case BROWSER:
				held = false;
				startPullUp();
				break;
			case EDITSCRIBBLE:
				scribbleUp();
				break;
			case CHOOSECOLOR:
				colorPickerUp();
				break;
			case CONFIGURATION:
				configurationDrop();
				break;
			case WEBBROWSER:
				held = false;
				webBrowserRelease();
				break;
		}
		
		return;
	}
	
	if(!(keysPressed() | keysPressed()))
	{
		// lets get out of here if nothing is being held
		return;
	}
	
	if(keysPressed() & KEY_UP)
	{
		switch(getMode())
		{
			case CALENDAR:
				if(curDay > 7)
				{
					curDay -= 7;
				}
				break;
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				setRepCount(-10);
				moveCursorAbsolute(getPositionAbove());			
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_UP);
				break;
			case VIEWER:
				setRepCount(-10);
				moveViewerCursor(CURSOR_UP);
				break;
			case WEBBROWSER:
				setRepCount(-10);
				moveWebBrowserCursor(CURSOR_UP);
				break;
			case CONFIGURATION:
				setRepCount(-10);
				configurationUp();
				break;
			case DAYVIEW:
				setRepCount(-10);
				if(curTime > 0)
				{
					--curTime;
				}
				break;
			case HOME:
			case HOMEMORE:
			case HOMESHORTCUTS:
				if(getCursor() >= 3)
				{
					moveCursorRelative(-3);
				}
				break;
			case BROWSER:
				if(confirmDelete())
				{
					break;
				}
				
				// fallthrough
			case ADDRESS:
			case TODOLIST:
			case SCRIBBLEPAD:
			case HOMEBREWDATABASE:
				setRepCount(-10);
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);
				}
				break;
			case EDITTODOLIST:
				if(!editField)
				{
					incrementUrgency();
					setRepCount(-10);
				}
				else
				{
					setRepCount(-10);
					moveCursorAbsolute(getPositionAbove());
				}
				break;
			case EDITSCRIBBLE:
			case SCRIBBLETEXT:
				moveTool(CURSOR_UP);
				break;
			case IRC:
				setRepCount(-10);
				ircUp();
				break;
			case SOUNDPLAYER:
				volumeUp();
				break;
		}
		
		return;
	}

	// repeat code
	if((keysPressed() & KEY_UP) && (getRepCount() >= 4))
	{	
		switch(getMode())
		{
			case DAYVIEW:
				setRepCount(0);;
				if(curTime > 0)
				{
					--curTime;
				}
				break;
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				moveCursorAbsolute(getPositionAbove());
				setRepCount(0);;
				break;
			case TEXTEDITOR:
				setRepCount(0);
				moveTextCursor(CURSOR_UP);
				break;
			case VIEWER:
				setRepCount(0);
				moveViewerCursor(CURSOR_UP);
				break;
			case WEBBROWSER:
				setRepCount(0);
				moveWebBrowserCursor(CURSOR_UP);
				break;
			case CONFIGURATION:
				setRepCount(0);
				configurationUp();
				break;
			case BROWSER:
				if(confirmDelete())
				{
					break;
				}
				
				// fallthrough
			case ADDRESS:
			case TODOLIST:
			case SCRIBBLEPAD:
			case HOMEBREWDATABASE:
				setRepCount(0);
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);
				}
				break;
			case EDITTODOLIST:
				if(!editField)
				{
					incrementUrgency();
					setRepCount(0);
				}
				else
				{
					setRepCount(0);
					moveCursorAbsolute(getPositionAbove());
				}
				break;
			case IRC:
				setRepCount(0);
				ircUp();
				break;
		}
		
		return;
	}	
	
	if(keysPressed() & KEY_DOWN)
	{
		switch(getMode())
		{
			case CALENDAR:
				if(curDay < daysInMonth(curMonth-1,curYear)-6)
				{
					curDay += 7;
				}
				break;
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				setRepCount(-10);
				moveCursorAbsolute(getPositionBelow());
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_DOWN);
				break;
			case VIEWER:
				setRepCount(-10);
				moveViewerCursor(CURSOR_DOWN);
				break;
			case WEBBROWSER:
				setRepCount(-10);
				moveWebBrowserCursor(CURSOR_DOWN);
				break;
			case CONFIGURATION:
				setRepCount(-10);
				configurationDown();
				break;
			case DAYVIEW:
				setRepCount(-10);
				if(curTime < 47)
				{
					++curTime;
				}
				break;
			case HOME:
			case HOMEMORE:
				moveCursorRelative(3);
				break;
			case HOMESHORTCUTS:
				if(getCursor() <= 2)
				{				
					moveCursorRelative(3);
				}
				break;
			case ADDRESS:
				setRepCount(-10);
				if(getCursor() < getAddressEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case BROWSER:
				if(confirmDelete())
				{
					break;
				}
				
				setRepCount(-10);
				if(getCursor() < getBrowserEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case TODOLIST:
				setRepCount(-10);
				if(getCursor() < getTodoEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case SCRIBBLEPAD:
				setRepCount(-10);
				if(getCursor() < getScribbleEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case HOMEBREWDATABASE:
				setRepCount(-10);
				if(getCursor() < getHBDBEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);		
				}
				break;
			case EDITTODOLIST:
				if(!editField)
				{
					decrementUrgency();
					setRepCount(-10);
				}
				else
				{
					setRepCount(-10);
					moveCursorAbsolute(getPositionBelow());
				}
				break;
			case EDITSCRIBBLE:
			case SCRIBBLETEXT:
				moveTool(CURSOR_DOWN);
				break;
			case IRC:
				setRepCount(-10);
				ircDown();
				break;
			case SOUNDPLAYER:
				volumeDown();
				break;
		}
		
		return;
	}
	
	// repeat code
	if((keysPressed() & KEY_DOWN) && (getRepCount() >= 4))
	{
		switch(getMode())
		{
			case DAYVIEW:
				setRepCount(0);
				if(curTime < 47)
				{
					++curTime;
				}
				break;
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				moveCursorAbsolute(getPositionBelow());
				setRepCount(0);
				break;
			case TEXTEDITOR:
				setRepCount(0);
				moveTextCursor(CURSOR_DOWN);
				break;
			case VIEWER:
				setRepCount(0);
				moveViewerCursor(CURSOR_DOWN);
				break;
			case WEBBROWSER:
				setRepCount(0);
				moveWebBrowserCursor(CURSOR_DOWN);
				break;
			case CONFIGURATION:
				setRepCount(0);
				configurationDown();
				break;
			case ADDRESS:
				setRepCount(0);
				if(getCursor() < getAddressEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case BROWSER:
				if(confirmDelete())
				{
					break;
				}
				
				setRepCount(0);
				if(getCursor() < getBrowserEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case TODOLIST:
				setRepCount(0);
				if(getCursor() < getTodoEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case SCRIBBLEPAD:
				setRepCount(0);
				if(getCursor() < getScribbleEntries() - 1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case HOMEBREWDATABASE:
				setRepCount(0);
				if(getCursor() < getHBDBEntries()-1)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case EDITTODOLIST:
				if(!editField)
				{
					decrementUrgency();
					setRepCount(0);
				}
				else
				{
					setRepCount(0);
					moveCursorAbsolute(getPositionBelow());
				}
				break;
			case IRC:
				setRepCount(0);
				ircDown();
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_LEFT)
	{
		switch(getMode())
		{
			case CALENDAR:
				--curDay;
				if(curDay < 1)
				{
					curDay = 1;
				}
				break;
			case DAYVIEW:				
				dvBookMarkUp();
				break;
			case IRC:
				invalidateInput();
				
				// fallthrough
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				setRepCount(-10);
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);
				}
				break;
			case WEBBROWSER:
				setRepCount(-10);
				
				if(isShowingKeyboard())
				{
					if(getCursor() > 0)
					{
						moveCursorRelative(CURSOR_BACKWARD);
						
						invalidateInput();
						recalcTextAreaJump();
					}
				}
				else
				{
					moveWebBrowserCursor(CURSOR_PAGEUP);
				}
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_LEFT);
				break;
			case VIEWER:
				moveViewerCursor(CURSOR_LEFT);
				break;
			case HOME:
			case HOMEMORE:
			case HOMESHORTCUTS:
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);
				}
				break;
			case EDITADDRESS:
			case EDITTODOLIST:
				setRepCount(-10);
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);
				}
				break;
			case CONFIGURATION:
				leftAction();
				
				if(isTextEntryScreen())
				{
					setRepCount(-10);
				}
				break;
			case EDITSCRIBBLE:
			case SCRIBBLETEXT:
				moveTool(CURSOR_LEFT);
				break;
			case BROWSER:
				moveCursorRelative(-8);
				setRepCount(-10);
				break;
			case HOMEBREWDATABASE:
				prevCategory();
				break;
			case SOUNDPLAYER:
				soundPrevTrack();
				break;
		}
		
		return;
	}
	
	// repeat code
	if((keysPressed() & KEY_LEFT) && (getRepCount() >= 4))
	{
		switch(getMode())
		{
			case IRC:
				invalidateInput();
				
				// fallthrough
			case EDITREMINDER:
			case EDITDAYVIEW:
			case BROWSERRENAME:
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);	
					setRepCount(0);
				}
				break;
			case WEBBROWSER:
				setRepCount(0);
				
				if(isShowingKeyboard())
				{
					if(getCursor() > 0)
					{
						moveCursorRelative(CURSOR_BACKWARD);
						
						invalidateInput();
						recalcTextAreaJump();
					}
				}
				else
				{
					moveWebBrowserCursor(CURSOR_PAGEUP);
				}
				break;
			case BROWSER:
				moveCursorRelative(-8);
				setRepCount(0);
				break;
			case TEXTEDITOR:
				setRepCount(0);
				moveTextCursor(CURSOR_LEFT);
				break;
			case EDITADDRESS:
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);				
					setRepCount(0);
				}
				break;
			case EDITTODOLIST:
				if(getCursor() > 0)
				{
					moveCursorRelative(CURSOR_BACKWARD);				
					setRepCount(0);
				}
				break;
			case CONFIGURATION:
				if(isTextEntryScreen())
				{
					leftAction();
					setRepCount(0);
				}
				break;
		}
		
		return;
	}		
	
	if(keysPressed() & KEY_RIGHT)
	{
		switch(getMode())
		{
			case CALENDAR:
				++curDay;
				if(curDay > daysInMonth(curMonth-1,curYear))
				{
					curDay = daysInMonth(curMonth-1,curYear);
				}
				break;
			case DAYVIEW:
				dvBookMarkDown();
				break;
			case EDITREMINDER:
				setRepCount(-10);
				if(!isReminderEnd(getCursor()))
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case EDITDAYVIEW:
				setRepCount(-10);
				if(dayViewCursorMove(curTime, getCursor()))
				{
					moveCursorRelative(CURSOR_FORWARD);	
				}
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_RIGHT);
				break;
			case VIEWER:
				moveViewerCursor(CURSOR_RIGHT);
				break;
			case BROWSERRENAME:
				setRepCount(-10);
				if(getCursor() < getMaxCursorBrowser())
				{
					moveCursorRelative(CURSOR_FORWARD);	
				}
				break;
			case HOME:
			case HOMEMORE:
				moveCursorRelative(CURSOR_FORWARD);
				break;
			case HOMESHORTCUTS:
				if(getCursor() < 5)
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case EDITADDRESS:
				setRepCount(-10);
				if(getCursor() < getMaxCursor())
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case EDITTODOLIST:
				setRepCount(-10);
				if(getCursor() < getMaxCursorTodo())
				{
					moveCursorRelative(CURSOR_FORWARD);
				}
				break;
			case CONFIGURATION:
				rightAction();	
				
				if(isTextEntryScreen())
				{
					setRepCount(-10);
				}
				break;
			case EDITSCRIBBLE:
			case SCRIBBLETEXT:
				moveTool(CURSOR_RIGHT);
				break;
			case IRC:
				setRepCount(-10);
				if(getCursor() < inputLength())
				{
					moveCursorRelative(CURSOR_FORWARD);	
				}
				
				invalidateInput();
				break;
			case WEBBROWSER:
				setRepCount(-10);
				
				if(isShowingKeyboard())
				{
					if(getCursor() < inputLength())
					{
						moveCursorRelative(CURSOR_FORWARD);
						
						invalidateInput();
						recalcTextAreaJump();
					}
				}
				else
				{
					moveWebBrowserCursor(CURSOR_PAGEDOWN);
				}
				
				break;
			case BROWSER:
				moveCursorRelative(8);
				setRepCount(-10);
				
				if(getCursor() >= getBrowserEntries() - 1)
				{
					moveCursorAbsolute(getBrowserEntries() - 1);
				}
				break;
			case HOMEBREWDATABASE:
				nextCategory();
				break;
			case SOUNDPLAYER:
				soundNextTrack();
				break;
		}
		
		return;
	}
	
	// repeat code
	if((keysPressed() & KEY_RIGHT) && (getRepCount() >= 4))
	{
		switch(getMode())
		{
			case EDITREMINDER:
				if(!isReminderEnd(getCursor()))
				{
					moveCursorRelative(CURSOR_FORWARD);	
					setRepCount(0);
				}
				break;
			case EDITDAYVIEW:
				if(dayViewCursorMove(curTime, getCursor()))
				{
					moveCursorRelative(CURSOR_FORWARD);	
					setRepCount(0);
				}
				break;
			case BROWSERRENAME:
				if(getCursor() < getMaxCursorBrowser())
				{
					moveCursorRelative(CURSOR_FORWARD);				
					setRepCount(0);
				}
				break;
			case TEXTEDITOR:
				setRepCount(0);
				moveTextCursor(CURSOR_RIGHT);
				break;
			case EDITADDRESS:
				if(getCursor() < getMaxCursor())
				{						
					moveCursorRelative(CURSOR_FORWARD);				
					setRepCount(0);
				}
				break;
			case EDITTODOLIST:
				if(getCursor() < getMaxCursorTodo())
				{						
					moveCursorRelative(CURSOR_FORWARD);				
					setRepCount(0);
				}
				break;
			case IRC:
				if(getCursor() < inputLength())
				{
					moveCursorRelative(CURSOR_FORWARD);	
					setRepCount(0);
				}
				
				invalidateInput();
				break;
			case WEBBROWSER:
				setRepCount(0);
				
				if(isShowingKeyboard())
				{
					if(getCursor() < inputLength())
					{
						moveCursorRelative(CURSOR_FORWARD);
						
						invalidateInput();
						recalcTextAreaJump();
					}
				}
				else
				{
					moveWebBrowserCursor(CURSOR_PAGEDOWN);				
				}				
				break;
			case BROWSER:
				moveCursorRelative(8);
				setRepCount(0);
				
				if(getCursor() >= getBrowserEntries() - 1)
				{
					moveCursorAbsolute(getBrowserEntries() - 1);
				}				
				break;
			case CONFIGURATION:
				if(isTextEntryScreen())
				{
					rightAction();
					setRepCount(0);
				}
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_L)
	{	
		switch(getMode())
		{
			case CALENDAR:
				--curMonth;
				if(curMonth < 1)
				{
					curMonth = 12;
					--curYear;
				}
				break;
			case EDITADDRESS:
				if(editField > 0)
				{
					formatCell(editField);
					--editField;
					resetCursor();
					clearSelect();
				}
				break;
			case EDITDAYVIEW:
				decreaseDuration();
				break;
			case EDITTODOLIST:
				if(editField > 0)
				{
					--editField;
					resetCursor();
					clearSelect();
				}
				break;
			case CONFIGURATION:
				configDecCursor();
				break;
			case DAYVIEW:
			{
				int cd = curDay;
				int cm = curMonth-1;
				int cy = curYear;
				
				subtractDay(&cd, &cm, &cy);
				
				curDay = cd;
				curMonth = cm+1;
				curYear = cy;		
				
				resetDayView();			
				break;
			}
			case BROWSER:
				if(browserMode != CONFIRMDELETE && !isPullUp())
				{
					if(!browserMode)
					{
						browserMode = MAXBROWSERCYCLES;
					}
					--browserMode;
				}
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_PAGEUP);
				repeatCount = 0;
				break;
			case VIEWER:
				setRepCount(-10);
				moveViewerCursor(CURSOR_PAGEUP);
				repeatCount = 0;
				break;
			case CALCULATOR:
				toggleMore();
				break;
			case EDITSCRIBBLE:
				goForward();	
				break;
			case SOUNDPLAYER:
				getPrevSound(0);
				break;
			case PICTUREVIEWER:
				getPrevPicture();
				break;
			case HOMEBREWDATABASE:
				lButtonDatabase();
				break;
			case IRC:
				ircLButton();
				break;
			case WEBBROWSER:
				webBrowserLButton();
				break;
			case HOMEMORE:
				setMode(HOME);
				break;
			case HOMESHORTCUTS:
				setMode(HOMEMORE);
				break;
		}
		
		return;
	}
	
	// repeat code
	if((keysPressed() & KEY_L) && (getRepCount() >= 4))
	{
		switch(getMode())
		{
			case TEXTEDITOR:
				if(repeatCount < 10)
				{
					setRepCount(0);
				}
				else if(repeatCount < 20)
				{
					setRepCount(1);
				}
				else if(repeatCount < 30)
				{
					setRepCount(3);
				}
				else
				{
					setRepCount(4);
				}
				
				++repeatCount;
				
				if(getRepCount() == 4)
				{
					moveTextCursor(CURSOR_PAGEUP);
					setRepCount(3);
				}
				
				moveTextCursor(CURSOR_PAGEUP);
				break;
			case VIEWER:
				if(repeatCount < 10)
				{
					setRepCount(0);
				}
				else if(repeatCount < 20)
				{
					setRepCount(1);
				}
				else if(repeatCount < 30)
				{
					setRepCount(3);
				}
				else
				{
					setRepCount(4);
				}
				
				++repeatCount;
				
				if(getRepCount() == 4)
				{
					moveViewerCursor(CURSOR_PAGEUP);
					setRepCount(3);
				}
				
				moveViewerCursor(CURSOR_PAGEUP);
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_R)
	{
		switch(getMode())
		{
			case CALENDAR:
				++curMonth;
				if(curMonth > 12)
				{
					curMonth = 1;
					++curYear;
				}
				break;
			case EDITADDRESS:
				if(editField < 10)
				{
					formatCell(editField);
					++editField;
					clearSelect();
					resetCursor();
				}
				break;
			case EDITDAYVIEW:
				increaseDuration();
				break;
			case EDITTODOLIST:
				if(editField < 1)
				{
					++editField;
					clearSelect();
					resetCursor();
					
					addBulletToTodo();
				}
				break;
			case CONFIGURATION:
				configIncCursor();
				break;
			case DAYVIEW:
			{
				int cd = curDay;
				int cm = curMonth-1;
				int cy = curYear;
				
				addDay(&cd, &cm, &cy);
				
				curDay = cd;
				curMonth = cm+1;
				curYear = cy;	
				
				resetDayView();	
				break;
			}
			case BROWSER:
				if(browserMode != CONFIRMDELETE && !isPullUp())
				{
					++browserMode;
					if(browserMode == MAXBROWSERCYCLES)
					{
						browserMode = 0;
					}
				}
				break;
			case CALCULATOR:
				toggleDeg();
				break;
			case TEXTEDITOR:
				setRepCount(-10);
				moveTextCursor(CURSOR_PAGEDOWN);
				repeatCount = 0;
				break;
			case VIEWER:
				setRepCount(-10);
				moveViewerCursor(CURSOR_PAGEDOWN);
				repeatCount = 0;
				break;
			case EDITSCRIBBLE:
				goBack();
				break;
			case SOUNDPLAYER:
				getNextSound(0);
				break;
			case PICTUREVIEWER:
				getNextPicture();
				break;
			case HOMEBREWDATABASE:
				rButtonDatabase();
				break;
			case IRC:
				ircRButton();
				break;
			case WEBBROWSER:
				webBrowserRButton();
				break;
			case HOME:
				setMode(HOMEMORE);
				break;
			case HOMEMORE:
				setMode(HOMESHORTCUTS);
				break;
		}
		
		return;
	}
	
	// repeat code
	if((keysPressed() & KEY_R) && (getRepCount() >= 4))
	{
		switch(getMode())
		{
			case TEXTEDITOR:
				if(repeatCount < 10)
				{
					setRepCount(0);
				}
				else if(repeatCount < 20)
				{
					setRepCount(1);
				}
				else if(repeatCount < 30)
				{
					setRepCount(3);
				}
				else
				{
					setRepCount(4);
				}
				
				++repeatCount;
				
				if(getRepCount() == 4)
				{
					moveTextCursor(CURSOR_PAGEDOWN);
					setRepCount(3);
				}
				
				moveTextCursor(CURSOR_PAGEDOWN);
				break;
			case VIEWER:
				if(repeatCount < 10)
				{
					setRepCount(0);
				}
				else if(repeatCount < 20)
				{
					setRepCount(1);
				}
				else if(repeatCount < 30)
				{
					setRepCount(3);
				}
				else
				{
					setRepCount(4);
				}
				
				++repeatCount;
				
				if(getRepCount() == 4)
				{
					moveViewerCursor(CURSOR_PAGEDOWN);
					setRepCount(3);
				}
				
				moveViewerCursor(CURSOR_PAGEDOWN);
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_START)
	{
		switch(getMode())
		{
			case CALENDAR:
				destroyReminder();
				returnHome();
				break;
			case DAYVIEW:
				resetDayView();
				returnHome();
				break;
			case ADDRESS:				
				freeVCard();
				returnHome();
				break;
			case CALCULATOR:
				closeScreen();
				returnHome();
				break;
			case BROWSER:
				freeDirList();
				destroyRandomList();
				destroyPullUp();
				returnHome();
				break;
			case HOME:
			case HOMEMORE:
			case HOMESHORTCUTS:
				setLastMode(getMode());
				setMode(ABOUT);
				break;
			case ABOUT:{
				setMode(getLastMode());
				freeabout();
			}
			break;
			case TODOLIST:
				freeTodo();
				returnHome();
				break;
			case SCRIBBLEPAD:
				freeScribbleList();
				returnHome();
				break;
			case WEBBROWSER:
				freeWebBrowser();
				returnHome();
				break;
			case EDITSCRIBBLE:
				loadColorPicker();
				break;
			case SOUNDPLAYER:
				toggleHold();
				break;
			case HOMEBREWDATABASE:				
				dbStartPressed();
				break;
			case SOUNDRECORDER:
				recorderStartPressed();
				break;
			case CONFIGURATION:
				configurationSwitchPages();
				break;
			case IRC:
				ircBack();
				break;
			case EDITREMINDER:
			case EDITDAYVIEW:
			case EDITADDRESS:
			case BROWSERRENAME:
			case TEXTEDITOR:
			case EDITTODOLIST:
			case SCRIBBLETEXT:
				toggleKeyboard();
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_X)
	{
		switch(getMode())
		{
			case TEXTEDITOR:
				moveTextCursor(CURSOR_END);
				break;
			case BROWSER:
				toggleHidden(getCursor());
				break;
			case EDITSCRIBBLE:
				moveColor(CURSOR_UP);
				break;
			case SCRIBBLEPAD:
				deleteCurrentScribble();
				break;
			case SOUNDPLAYER:
				switchSoundMode();
				break;
			case SOUNDRECORDER:
				switchRecordingMode();
				break;
			case ADDRESS:
				deleteAddress();
				break;
			case TODOLIST:
				deleteTodo();
				break;
			case CONFIGURATION:
				if(isTextEntryScreen())
				{
					toggleKeyboard();
				}
				break;
			case WEBBROWSER:
				webBrowserXButton();
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_Y)
	{
		switch(getMode())
		{
			case TEXTEDITOR:
				moveTextCursor(CURSOR_HOME);
				break;
			case EDITSCRIBBLE:
				moveColor(CURSOR_LEFT);
				break;
			case BROWSER:
				alternateLoadFile(getCursor());
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_A)
	{
		switch(getMode())
		{
			case EDITSCRIBBLE:
				moveColor(CURSOR_RIGHT);
				break;
			default:
				if(isABSwapped())
				{
					goBack();
				}
				else
				{
					goForward();
				}
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_B)
	{
		switch(getMode())
		{
			case EDITSCRIBBLE:
				moveColor(CURSOR_DOWN);
				break;
			default:
				if(isABSwapped())
				{
					goForward();
				}
				else
				{
					goBack();
				}
				break;
		}
		
		return;
	}
}

void queueUpCommand(int qc, int qd)
{
	queuedCommand = qc;
	queuedData = qd;
}

void checkJustKeys()
{
	if(getMode() != SOUNDPLAYER && getMode() != SOUNDRECORDER && getMode() != HOMEBREWDATABASE)
	{
		return;
	}
	
	struct touchScr touch = touchReadXYNew();
	
	if(keysPressed() & KEY_TOUCH)
	{
		yThreshold = touch.touchYpx; // to stop shaking on some DS units.
		
		switch(getMode())
		{
			case SOUNDPLAYER:
				if(touch.touchYpx >= 162 && touch.touchYpx <= 182)
				{
					if(touch.touchXpx >= 23 && touch.touchXpx <= 126)
					{
						queueUpCommand(SND_GOFORWARD,0);
					}
					else if(touch.touchXpx >= 130 && touch.touchXpx <= 233)
					{
						queueUpCommand(SND_GOBACK,0);
					}
				}		
				else
				{	
					if(isLButton(touch.touchXpx, touch.touchYpx))
					{
						queueUpCommand(SND_GETPREV,1);
					}
					else if(isRButton(touch.touchXpx, touch.touchYpx))
					{
						queueUpCommand(SND_GETNEXT,1);
					}	
					else if(isHome(touch.touchXpx, touch.touchYpx))
					{
						queueUpCommand(SND_TOGGLEHOLD,0);
					}
					else if(touch.touchYpx >=20 && touch.touchYpx <= 40)
					{
						queueUpCommand(SND_SEEK, touch.touchXpx);
						
						return;
					}
				}
				break;
			case SOUNDRECORDER:
				if(touch.touchYpx >= 162 && touch.touchYpx <= 182)
				{
					if(touch.touchXpx >= 23 && touch.touchXpx <= 126)
					{
						queueUpCommand(SND_GOFORWARD,0);
					}
					else if(touch.touchXpx >= 130 && touch.touchXpx <= 233)
					{
						queueUpCommand(SND_GOBACK,0);
					}
				}		
				else
				{	
					if(isHome(touch.touchXpx, touch.touchYpx))
					{
						queueUpCommand(SND_TOGGLEHOLD,0);
					}
					else if(touch.touchYpx >=20 && touch.touchYpx <= 40)
					{
						queueUpCommand(SND_SEEK, touch.touchXpx);
						
						return;
					}
				}
				break;
		}
		
		return;
	}
	
	if(keysPressed() & KEY_L)
	{
		queueUpCommand(SND_GETPREV,0);
	}
	
	if(keysPressed() & KEY_R)
	{
		queueUpCommand(SND_GETNEXT,0);
	}
	
	if(keysPressed() & KEY_START)
	{
		queueUpCommand(SND_TOGGLEHOLD,0);
	}
	
	if(keysPressed() & KEY_X)
	{
		queueUpCommand(SND_MODE,0);
	}
		
	if(keysPressed() & KEY_UP)
	{
		queueUpCommand(SND_VOLUMEUP,0);
	}
	
	if(keysPressed() & KEY_DOWN)
	{
		queueUpCommand(SND_VOLUMEDN,0);
	}
	
	if(keysPressed() & KEY_LEFT)
	{
		queueUpCommand(SND_PREVINTERNAL,0);
	}
	
	if(keysPressed() & KEY_RIGHT)
	{
		queueUpCommand(SND_NEXTINTERNAL,0);
	}
	
	if(keysPressed() & KEY_A)
	{
		if(isABSwapped())
		{
			queueUpCommand(SND_GOBACK,0);
		}
		else
		{
			queueUpCommand(SND_GOFORWARD,0);
		}
	}
	
	if(keysPressed() & KEY_B)
	{
		if(isABSwapped())
		{
			queueUpCommand(SND_GOFORWARD,0);
		}
		else
		{
			queueUpCommand(SND_GOBACK,0);
		}
	}
}

void executeKeys()
{
	if(getMode() != SOUNDPLAYER && getMode() != SOUNDRECORDER)
	{
		if(getState() == STATE_STOPPED)
		{
			freeSound();
		}
	}
	
	if(queuedCommand == -1)
	{
		return;
	}
	
	switch(getMode())
	{
		case SOUNDPLAYER:
			switch(queuedCommand)
			{
				case SND_GOFORWARD:
					goForward();
					break;
				case SND_GOBACK:
					goBack();
					break;
				case SND_GETPREV:
					getPrevSound(queuedData);
					break;
				case SND_GETNEXT:
					getNextSound(queuedData);
					break;
				case SND_TOGGLEHOLD:		
					toggleHold();
					break;
				case SND_MODE:			
					switchSoundMode();
					break;
				case SND_VOLUMEUP:			
					volumeUp();
					break;
				case SND_VOLUMEDN:			
					volumeDown();
					break;
				case SND_PREVINTERNAL:
					soundPrevTrack();
					break;
				case SND_NEXTINTERNAL:
					soundNextTrack();
					break;
				case SND_SEEK:
					seekSound(queuedData);
					break;
			}
			break;
		case SOUNDRECORDER:
			switch(queuedCommand)
			{
				case SND_GOFORWARD:
					goForward();
					break;
				case SND_GOBACK:
					goBack();
					break;
				case SND_MODE:
					switchRecordingMode();
					break;
				case SND_TOGGLEHOLD:
					recorderStartPressed();
					break;
				case SND_SEEK:
					seekRecordSound(queuedData);
					break;
			}
			break;
		case HOMEBREWDATABASE:
			switch(queuedCommand)
			{
				case SND_GOBACK:
					goBack();
					break;
			}
			break;
	}
	
	queuedCommand = -1;
	queuedData = -1;
}