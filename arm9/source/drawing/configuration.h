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
 
#ifndef _CONFIG_INCLUDED
#define _CONFIG_INCLUDED

#include "language.h"
#include "graphics.h"
#include "settings.h"

#define PAGE1 0
#define PAGE2 6
#define PAGE3 10
#define PAGE4 11
#define PAGE5 12
#define PAGE6 13
#define PAGE7 14
#define PAGE8 15
#define PAGE9 19
#define PAGE10 24
#define PAGE11 29

// normal entries

// page 1
#define LANGUAGE (PAGE1 + 0)
#define HOMESCREEN (PAGE1 + 1)
#define TIMEFORMAT (PAGE1 + 2)
#define NAMEFORMAT (PAGE1 + 3)
#define DATEFORMAT (PAGE1 + 4)
#define ICONSET (PAGE1 + 5)

// page 2
#define SECONDCLICK (PAGE2 + 0)
#define SWAPAB (PAGE2 + 1)
#define LEFTHANDED (PAGE2 + 2)
#define KEYCLICK (PAGE2 + 3)

// page 3
#define HOMEARRANGE (PAGE3 + 0)

// page 4
#define SAVEFORMAT (PAGE4 + 0)

// page 5
#define AUTOBULLET (PAGE5 + 0)

// page 6
#define VIEWERFIXED (PAGE6 + 0)

// page 7
#define IMAGESIDEWAYS (PAGE7 + 0)

// page 8
#define SHOWHIDDEN (PAGE8 + 0)
#define DEFAULTSAVE (PAGE8 + 1)
#define NORMALBOOT (PAGE8 + 2)
#define ALTBOOT (PAGE8 + 3)

// page 9
#define AUTOCONNECT (PAGE9 + 0)
#define IRCSERVER (PAGE9 + 1)
#define NICKNAME (PAGE9 + 2)
#define ALTNICK (PAGE9 + 3)
#define IRCFIXED (PAGE9 + 4)

// page 10
#define HTMLMODE (PAGE10 + 0)
#define PROXY (PAGE10 + 1)
#define HOMEPAGE (PAGE10 + 2)
#define DOWNDIR (PAGE10 + 3)
#define AUTOHIDE (PAGE10 + 4)

// page 11
#define WIFISWITCH (PAGE11 + 0)

// max modes, etc
#define NUMOPTIONS 30

// for the rearranging homescreen
#define MAXDRAG 10

// for home screen selection
#define MAX_STARTUP_SELECTION 10

#define Y_OFFSET 35
#define Y_FIRST (Y_OFFSET)
#define Y_SECOND (25 + Y_OFFSET)
#define Y_THIRD (50 + Y_OFFSET)
#define Y_FOURTH (75 + Y_OFFSET)
#define Y_FIFTH (100 + Y_OFFSET)

#define Y_WIFIOFFSET 15
#define Y_SUBTRACT (Y_OFFSET - Y_WIFIOFFSET)

#define DRAG_CLIP_Y 44
#define X_JUMP 48
#define Y_JUMP 40

#define ROUNDBUTTON_WIDTH 73
#define INBETWEEN_WIDTH 4

#define WIFI_NORMAL 0
#define WIFI_EDITPROFILE 1
#define WIFI_EDITSETTING 2


#ifdef __cplusplus
extern "C" {
#endif

extern LANG_LIST *aLanguages;
extern ICON_LIST *cIcons;
extern char *descText;
extern int lState;
extern int iState;
extern int mLanguages;
extern int mIcons;
extern bool cLoadedPage1;
extern bool isCFlipped;
extern bool isDragging;
extern int dragItem;
extern int lastX;
extern int lastY;
extern int offsetX;
extern int offsetY;
extern int lastLoaded;
extern int lastLoadedWifi;
extern uint16 *sprites[MAXDRAG];
extern int configCursor;
extern int sState;
extern int configInitialized;
extern WIFI_PROFILE *wifiInfo;
extern WIFI_PROFILE *wifiSave;
extern int wifiState;
extern int wifiCursor;
extern int wifiEditProfile;
extern char wifiInput[64];

extern void drawConfiguration();
extern void leftAction();
extern void rightAction();
extern void configureAction(int px, int py);
extern bool configurationDrag(int px, int py);
extern void configurationDrop();
extern void configurationSwitchPages();
extern void drawTopConfiguration();
extern bool configFlipped();
extern void resetScreenFlip();
extern void editConfigAction(char c);
extern void resetProxy();
extern bool isTextEntryScreen();
extern void configDecCursor();
extern void configIncCursor();
extern void configurationResetCursor();
extern void clearHelpMemory();
extern void configurationSetLanguages();
extern void configurationSetStartMode();
extern void destroyConfiguration();
extern void configurationForward();
extern void configurationBack();
extern bool isAPScreen();
extern void configurationUp();
extern void configurationDown();
extern void cloneIfNeeded(void *wTmp);

#ifdef __cplusplus
}
#endif

#endif
