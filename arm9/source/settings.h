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
 
#ifndef _SETTINGS_INCLUDED
#define _SETTINGS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define CALENDAR 0
#define EDITREMINDER 1
#define DAYVIEW 2
#define EDITDAYVIEW 3
#define HOME 4
#define ADDRESS 5
#define EDITADDRESS 6
#define CONFIGURATION 7
#define BROWSER 8
#define HOMEMORE 9
#define BROWSERRENAME 10
#define CALCULATOR 11
#define TEXTEDITOR 12
#define PICTUREVIEWER 13
#define ABOUT 14
#define TODOLIST 15
#define EDITTODOLIST 16
#define SCRIBBLEPAD 17
#define EDITSCRIBBLE 18
#define CHOOSECOLOR 19
#define SCRIBBLETEXT 20
#define SOUNDPLAYER 21
#define HOMEBREWDATABASE 22
#define VIEWER 23
#define SOUNDRECORDER 24
#define IRC 25
#define INITFAT 26
#define INITPLUGIN 27
#define HOMESHORTCUTS 28
#define WEBBROWSER 29
#define DISPLAYCOW 30

#define BOOT_INTERNAL 0
#define BOOT_MIGHTYMAX 1
#define BOOT_CHISHM 2

#define WIFI_FIRMWARE 0
#define WIFI_DSORGANIZE 1

typedef struct
{
	bool enabled;
	char ssid[33];
	char wepKey[16];
	int wepMode;
	bool dhcp;
	bool autodns;
	u32 ip;
	u32 subnet;
	u32 gateway;
	u32 primarydns;
	u32 secondarydns;
} WIFI_PROFILE;

void loadSettings();
void makeDefaultSettings();
void saveSettings();
void setStartMode(int m);
void loadColors();
void loadExternalDLDI();
void loadWifi();
void saveWifi();
WIFI_PROFILE *readWifi();

// access routines
char *getProxy();
char *getDLDI();
char *getHomePage();
char *getDownloadDir();

int getMode();
void setMode(int newMode);
int getLastMode();
void setLastMode(int oldMode);

bool isABSwapped();
void toggleABSwap();

bool secondClickAction();
void toggleSecondClickOption();

bool isAutoHide();
void toggleAutoHide();

void toggleImageLandscape();
bool isImageLandscape();

bool keyClickEnabled();
void toggleKeyClick();

// for fixed width fonts
bool isTextEditorFixed();
bool isIRCFixed();
uint16 **setTextEditorFont();
uint16 **setIRCFont();
void toggleTextFixed();
void toggleIRCFixed();


#ifdef __cplusplus
}
#endif

#endif
