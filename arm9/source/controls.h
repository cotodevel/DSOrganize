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
 
#ifndef _CONTROLS_INCLUDED
#define _CONTROLS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// scrollbar metrics
#define DEFAULT_SCROLL_LEFT_L 5
#define DEFAULT_SCROLL_LEFT_R 240
#define DEFAULT_SCROLL_TOP 14
#define DEFAULT_SCROLL_WIDTH 10
#define DEFAULT_SCROLL_HEIGHT 144

#define SCROLL_ARROW_HEIGHT 12
#define SCROLL_HANDLE_HEIGHT 12

// list location
#define LIST_LEFT_L 17
#define LIST_LEFT_R 5
#define LIST_RIGHT_L 250
#define LIST_RIGHT_R 238
#define LIST_TOP 14
#define LIST_BOTTOM 158

#define LIST_STEP 16

// for handedness
#define HAND_RIGHT 0
#define HAND_LEFT 1

// for keys on keyboard
#define DEFAULT_TOP 138
#define DEFAULT_BOTTOM 155

#define INS_LEFT 13
#define INS_TOP DEFAULT_TOP
#define INS_RIGHT 44
#define INS_BOTTOM DEFAULT_BOTTOM
#define INS_CENTER ((INS_LEFT + INS_RIGHT) / 2)

#define CLEAR_LEFT 200
#define CLEAR_TOP DEFAULT_TOP
#define CLEAR_RIGHT 242
#define CLEAR_BOTTOM DEFAULT_BOTTOM
#define CLEAR_CENTER ((CLEAR_LEFT + CLEAR_RIGHT) / 2)

#define DELETE_LEFT 156
#define DELETE_TOP DEFAULT_TOP
#define DELETE_RIGHT 198
#define DELETE_BOTTOM DEFAULT_BOTTOM
#define DELETE_CENTER ((DELETE_LEFT + DELETE_RIGHT) / 2)

#define CTRL_LEFT 46
#define CTRL_TOP DEFAULT_TOP
#define CTRL_RIGHT 88
#define CTRL_BOTTOM DEFAULT_BOTTOM
#define CTRL_CENTER ((CTRL_LEFT + CTRL_RIGHT) / 2)

#define SELECT_LEFT 90
#define SELECT_TOP DEFAULT_TOP
#define SELECT_RIGHT 132
#define SELECT_BOTTOM DEFAULT_BOTTOM
#define SELECT_CENTER ((SELECT_LEFT + SELECT_RIGHT) / 2)

// for button locations
#define O0_TOP 162
#define O0_BOTTOM 182
#define O0_FORWARDLEFT 10
#define O0_FORWARDRIGHT 126
#define O0_BACKLEFT 130
#define O0_BACKRIGHT 244
#define O0_RBUTTON 253
#define O0_CENTER 128
#define O0_HEIGHT 192

#define O90_TOP 226
#define O90_BOTTOM 246
#define O90_FORWARDLEFT 10
#define O90_FORWARDRIGHT 94
#define O90_BACKLEFT 98
#define O90_BACKRIGHT 180
#define O90_RBUTTON 189
#define O90_CENTER 96
#define O90_HEIGHT 256

#define TOPAREA 15

#define KEYBOARD_LEFT 13
#define KEYBOARD_TOP 37

#define MAX_ORIENTATIONS 10

// global stuff
void drawHome();
bool isHome(int px, int py);
bool isTopArea(int px, int py);
void drawTimeDate();
int getScreenWidth();
int getScreenHeight();
int getScreenCenter();

// keyboard stuff
void drawKeyboard();
void drawKeyboardSpecific(int y, char button, char *text, uint16 *bb);
void drawKeyboardSpecificEX(int x, int y, char button, char *text, uint16 *bb);
void drawSubKeys(uint16 *bb);
void drawDelete(uint16 *bb);

// bottom buttons
void drawButtonTexts(char *lText, char *rText);
void drawButtonTextsEX(char lBut, char *lText, char rBut, char *rText);
bool isButtons(int tx, int ty);
bool isForwardButton(int tx, int ty);
bool isBackButton(int tx, int ty);
int getLLocation();
int getRLocation();

// scroll stuff
void displayScrollHandle(bool t);
void drawScrollBar(int pos, int max, int x, int y, int width, int height);
void drawScrollBarEX(int pos, int max, bool isHeld, int x, int y, int width, int height);
void drawScrollBarCustom(int pos, int max, void (*callBack)(), int x, int y, int width, int height);
void drawScrollBarEXEX(int pos, int max, bool isHeld, void (*callBack)(), int x, int y, int width, int height);
int getScroll(int py, int max);
int getScrollLeft();

// listbox stuff
void drawListBox(int tx, int ty, int bx, int by, int curEntry, int maxEntries, char *noString, void (*callback)(int, int, int));
int getCursorFromTouch(int u, int v);

// left and right buttons
void drawLR(char *lText, char *rText);
void drawLREX(char *lText, bool lEnabled, char *rText, bool rEnabled);
bool isLButton(int px, int py);
bool isRButton(int px, int py);

// rounded buttons
void drawRoundedButton(int x, int y, int bx, int by, char *text, bool select, bool highlight);

// textboxes
void drawTextBox(int x, int y, int bx, int by, const char *text, bool centered, bool selected, bool italics);

// left handed settings
void setHand(int m);
int getHand();
void swapHands();

// for orientation
void setNewOrientation(int orientation);
void setOldOrientation();
int getOrientation();

// for left-handed mode
extern int default_scroll_left;
extern int list_left;
extern int list_right;

#ifdef __cplusplus
}
#endif

#endif
