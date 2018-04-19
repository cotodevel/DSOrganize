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

#include "language.h"
#include "graphics.h"
#include "irc.h"

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

extern uint16 startMode;
extern char lLanguage[LANG_SIZE];
extern char cIconSet[ICON_SIZE];
extern bool milTime;
extern bool firstLast;
extern bool showHidden;
extern u32 htmlStyle;
extern bool reverseDate;
extern bool disablePatching;
extern bool disableWrites;
extern char defaultSavePath[256];
extern char nickName[NICK_LEN];
extern char altnickName[NICK_LEN];
extern char ircServer[60];
extern bool autoConnect;
extern bool autoBullet;
extern int normalBoot;
extern int altBoot;
extern int wifiMode;

extern WIFI_PROFILE *dsoProfiles;
extern char proxy[256];
extern char dldiFile[256];
extern char homePage[256];
extern char downloadLocation[256];
extern int mode;
extern int lastMode;
extern bool swapAB;
extern bool textFixedWidth;
extern bool ircFixedWidth;
extern bool secondClickOpens;
extern bool autoHide;
extern bool imageLandscape;
extern bool keyClick;

extern uint16 widgetBorderColor;
extern uint16 widgetFillColor;
extern uint16 widgetTextColor;
extern uint16 widgetHighlightColor;
extern uint16 scrollFillColor;
extern uint16 scrollHighlightColor;
extern uint16 scrollNormalColor;
extern uint16 genericHighlightColor;
extern uint16 genericTextColor;
extern uint16 genericFillColor;
extern uint16 genericProgressColor; //0x6F7B
extern uint16 listBorderColor;
extern uint16 listFillColor;
extern uint16 listTextColor;
extern uint16 textEntryTextColor;
extern uint16 textEntryFillColor;
extern uint16 textEntryHighlightColor;
extern uint16 textEntryProgressColor; //0x6F7B
extern uint16 textAreaTextColor;
extern uint16 textAreaFillColor;
extern uint16 cursorNormalColor;
extern uint16 cursorOverwriteColor;
extern uint16 calendarWeekendColor;
extern uint16 calendarNormalColor;
extern uint16 calendarHasDayPlannerColor;
extern uint16 calendarCurrentDateColor;
extern uint16 calencarHasReminderColor;
extern uint16 calendarDayUnderlineColor;
extern uint16 dayPlannerBorderColor;
extern uint16 browserHiddenColor;
extern uint16 configurationArrowColor;
extern uint16 keyboardBorderColor;
extern uint16 keyboardFillColor;
extern uint16 keyboardSpecialColor;
extern uint16 keyboardABCSwapColor;
extern uint16 keyboardHighlightColor;
extern uint16 keyboardSpecialHighlightColor;
extern uint16 keyboardTextColor;
extern uint16 keyboardMEDSTextColor;
extern uint16 homeDateTextColor;
extern uint16 homeDateFillColor;
extern uint16 homeDateBorderColor;
extern uint16 homeDateBorderFillColor;
extern uint16 homeHighlightColor;
extern uint16 pictureSelectBorderColor;
extern uint16 soundSeekFillColor;
extern uint16 soundSeekForeColor;
extern uint16 soundSeekDisabledColor;
extern uint16 soundMetaTextColor;
extern uint16 soundHoldColor;
extern uint16 soundEQMedianColor;
extern uint16 soundEQDataColor;
extern uint16 soundArrowColor;
extern uint16 calculatorScreenFillColor;
extern uint16 calculatorScreenTextColor;
extern uint16 calculatorScreenBorderColor;
extern uint16 calculatorScreenFadeTextColor;
extern uint16 calculatorButtonTextColor;
extern uint16 calculatorButtonBorderColor;
extern uint16 calculatorNormalFillColor;
extern uint16 calculatorMemoryFillColor;
extern uint16 calculatorSpecialFillColor;
extern uint16 calculatorUnaryFillColor;
extern uint16 calculatorBinaryFillColor;
extern uint16 calculatorButtonHighlightColor;
extern uint16 texteditorComment;
extern uint16 texteditorProperty;
extern uint16 texteditorSection;
extern uint16 texteditorSetting;
extern uint16 viewerBookmarkColor;
extern uint16 loadingBorderColor;
extern uint16 loadingFillColor;
extern uint16 loadingBarColor;
extern uint16 ircTabFillColor;
extern uint16 ircTabBorderColor;
extern uint16 ircTabTextColor;
extern uint16 ircTabHighlightColor;
extern uint16 ircTabMoreTabsColor;
extern uint16 ircTabImprtantTextColor;
extern uint16 ircTabUnimportantTextColor;
extern uint16 ircScreenFillColor;
extern uint16 ircScreenBorderColor;
extern uint16 ircScreenTextColor;
extern char ircColorNOTICE;
extern char ircColorACTION;
extern char ircColorCTCP;
extern char ircColorNORMAL;
extern char ircColorSERVER;
extern char ircColorDSO;
extern char ircColorNICK;
extern char ircColorTOPIC;
extern char ircColorMODE;
extern char ircColorHIGHLIGHT;

#ifdef __cplusplus
}
#endif

#endif
