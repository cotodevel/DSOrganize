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
 
#ifndef _KEYBOARD_INCLUDED
#define _KEYBOARD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define K_C		100
#define K_S		101
#define K_D		102
#define K_B		103
#define K_R		104
#define K_W		105
#define K_T		106

#define NUMBERS 0
#define SYMBOLS 1
#define LOWERCASE 2
#define UPPERCASE 3
#define SPECIAL0 4
#define SPECIAL1 5

#define CLEAR_KEY 27

#define BSP	0x8 // Backspace
#define CAP	0x2 // Caps
#define RET	'\n' // Enter
#define SHF	0x4 // Shift
#define SPC	0x20 // Space
#define DEL 0x5 // Delete
#define SPL 0x1 // Special

#define KB_QWERTY 0
#define KB_MESSAGEASE 1

#define ACTIVE_NONE -1
#define ACTIVE_LOWERCASE 0
#define ACTIVE_UPPERCASE 1
#define ACTIVE_NUMBERS 2
#define ACTIVE_LOWERCASE_SPECIAL 3
#define ACTIVE_UPPERCASE_SPECIAL 4

#define CURSOR_FORWARD 1
#define CURSOR_BACKWARD -1

char translateCode(int inCode, int keyBoard);
char executeClick(int px, int py);
void executeMove(int px, int py);
char executeUp();
void toggleKeyboard();
void resetKeyboard();
void resetCursor();
void moveCursorAbsolute(int newPos);
void moveCursorRelative(int newPos);
int getCursor();
bool blinkOn();
void pushCursor();
void popCursor();
void incrimentBCount();
void genericAction(char *tmpBuffer, int size, char c);
void setFakeHighlight();
void setFakeHighlightEX(int subAmount);
void clearFakeHighlight();
void clearSelect();
void clickSound();

// important state functions
void setPressedChar(char c);
uint16 isCaps();
uint16 isShift();
uint16 isInsert();
uint16 isSpecial();
uint16 isSelect();
uint16 isControl();
char pressedChar();
int activeView();
int activeKeyboard();
void toggleCaps();
void toggleShift();
void toggleIns();
void toggleSpecial();
void toggleSelect();
void toggleControl();
void clearPressedChar();


extern int downX;
extern int downY;
extern int upX;
extern int upY;
extern int whichKeyboard;
extern int isNumbersKeyboard;
extern uint16 caps;
extern uint16 shift;
extern uint16 insert;
extern uint16 control;
extern uint16 selectKeyboard;
extern char curChar;
extern uint16 special;
extern int cursor;
extern uint16 blink;
extern uint16 bCount;
extern int oldCursor;
extern int beginHighlight;
extern int endHighlight;
extern int keyboard_Hit[60];
extern char me_upper[43];
extern char me_lower[43];
extern char me_number[43];
extern char me_lowerspecial[43];
extern char me_upperspecial[43];


#ifdef __cplusplus
}
#endif

#endif
