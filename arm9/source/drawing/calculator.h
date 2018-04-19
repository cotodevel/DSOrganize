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
 
#ifndef _CALCULATOR_INCLUDED
#define _CALCULATOR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <m_apm.h>


#define RECIPROCAL 0
#define FACTORIAL 1
#define SQRT 2
#define PI 3
#define EXP 4
#define LN 5
#define SIN 6
#define COS 7
#define TAN 8
#define LOG 9
#define SQUARE 10
#define ARCSIN 11
#define ARCCOS 12
#define ARCTAN 13
#define M_INT 14

#define ACCURACY 20
#define DIGITS 199999

#define NUM_CONTROLS 36
#define NUM_SCREENS 2
#define NUM_BUTTONS_X 6
#define NUM_BUTTONS_Y 6

void initCalc();
void startScreen();
void closeScreen();
void clearCalc();
void pushCalcStack();
void popCalcStack();
int getButton(int a, int b);
void addToCalc(int number);
void formatScreen(char *str);
void ansToScreen();
void performAction();
void performUnaryAction(int action);
void calcAction(int action);
void drawAnswerScreen();
void drawCalculatorButtons();
void toggleDeg();
void toggleMore();
int getMore();
bool getDeg();
void handleKeyHighlighting();

extern char lastKey;
extern bool working;
extern int curKey;
extern M_APM ans;
extern M_APM mem;
extern M_APM stack[10];
extern char stackKey[10];
extern M_APM curNumber;
extern int stackPos;
extern bool belowDecimal;
extern char enterKey;
extern char enterBuffer[64];
extern bool clearOnType;
extern bool deg;
extern int isMore;
extern bool wasLastPress;
extern int eraseChar;
extern char l_rad[];
extern char l_deg[];
extern char *calculatorString[NUM_CONTROLS*NUM_SCREENS];
extern uint16 calcColor[NUM_CONTROLS*NUM_SCREENS];
extern char *screen;
extern char *memory;

#ifdef __cplusplus
}
#endif

#endif
