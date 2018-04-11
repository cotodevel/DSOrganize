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

#ifndef __main9_h__
#define __main9_h__

#include "typedefsTGDS.h"
#include "dsregs.h"

#endif


#ifdef __cplusplus
extern "C" {
#endif

// day stuff
extern uint16 curDay;
extern uint16 curMonth;
extern uint16 curYear;
extern uint16 curTime;
extern uint16 oldYear;
extern uint16 oldDay;
extern uint16 oldMonth;

// for addresses, text editor and fileViewers
extern uint16 browserMode;
// internals
extern int yThreshold;
extern int queuedCommand;
extern int queuedData;
extern bool hasTouched;
extern bool held;
extern char lChar;
extern char kChar;
extern uint16 repeatCount;
extern uint16 editField;

// Prototypes
extern void queueUpCommand(int qc, int qd);

extern bool isScrolling();
extern void goForward();
extern void goBack();
extern void touchDown(int px, int py);
extern void touchUp();
extern void checkKeyboards();
extern bool checkSleepState();
extern void checkKeys();
extern void queueUpCommand(int qc, int qd);
extern void checkJustKeys();
extern void executeKeys();
extern void initProgram();
extern void drawCurrentApp();
extern void drawSoundScreen(int oldMode);

#ifdef PROFILING
unsigned int hblanks;
extern void hblank_handler(void);
#endif

extern int main(int _argc, sint8 **_argv);

#ifdef __cplusplus
}
#endif