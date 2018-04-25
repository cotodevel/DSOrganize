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
#include <libcommon.h>
#include <libdt.h>
#include <m_apm.h>
#include "calculator.h"
#include "../keyboard.h"
#include "../general.h"
#include "../colors.h"
#include "../globals.h"
#include "../font_gautami_10.h"
#include "../general.h"
#include "../language.h"
#include "../controls.h"

char lastKey = 0;
bool working = false;

static int curKey = -1;
static M_APM ans;
static M_APM mem;
static M_APM stack[10];
static char stackKey[10];
static M_APM curNumber;
static int stackPos = -1;
static bool belowDecimal = false;
static char enterKey = 0;
static char enterBuffer[64];
static bool clearOnType = false;
static bool deg = true;
static int isMore = 0;
static bool wasLastPress = false;
static int eraseChar = 0;

static char l_rad[] = { "RAD" };
static char l_deg[] = { "DEG" };

static char *calcString[NUM_CONTROLS*NUM_SCREENS] = { "sqrt", "^2", 	" ^ ", 	"(", 	")", 	"/", // ^
									"x!",	"sin",	"7",	"8",	"9",	"*",
									"1/x",	"cos",	"4",	"5",	"6",	"-",
									"ln",	"tan",	"1",	"2",	"3",	"+",
									"e^x",	"pi",	"0",	".",	"(-)",	"=",
									"log",	"MS",	"MR",	"MC",	"CE",	"C",
									
									"", 	"", 	" ^ ", 	"(", 	")", 	"/", // ^
									"gcd",	"asin",	"7",	"8",	"9",	"*",
									"lcm",	"acos",	"4",	"5",	"6",	"-",
									"int",	"atan",	"1",	"2",	"3",	"+",
									"",		"rand",	"0",	".",	"(-)",	"=",
									"",		"MS",	"MR",	"MC",	"CE",	"C" };

static uint16 calcColor[NUM_CONTROLS*NUM_SCREENS];

char *screen = NULL;
char *memory = NULL;

void toggleDeg()
{
	deg = !deg;
}

void toggleMore()
{
	isMore = 1 - isMore;
}

int getMore()
{
	return isMore;
}

bool getDeg()
{
	return deg;
}

void handleKeyHighlighting()
{		
	if(eraseChar > 0)
	{
		--eraseChar;		
		if(!eraseChar && pressedChar())
			clearPressedChar();
		if(!eraseChar && curKey != -1)			
			curKey = -1;
	}
	else if(pressedChar())
		eraseChar = 1;
}

void resetColors()
{
	calcColor[0] = calculatorUnaryFillColor;
	calcColor[1] = calculatorUnaryFillColor;
	calcColor[2] = calculatorBinaryFillColor;
	calcColor[3] = calculatorSpecialFillColor;
	calcColor[4] = calculatorSpecialFillColor;
	calcColor[5] = calculatorBinaryFillColor;
	calcColor[6] = calculatorUnaryFillColor;
	calcColor[7] = calculatorUnaryFillColor;
	calcColor[8] = calculatorNormalFillColor;
	calcColor[9] = calculatorNormalFillColor;
	calcColor[10] = calculatorNormalFillColor;
	calcColor[11] = calculatorBinaryFillColor;
	calcColor[12] = calculatorUnaryFillColor;
	calcColor[13] = calculatorUnaryFillColor;
	calcColor[14] = calculatorNormalFillColor;
	calcColor[15] = calculatorNormalFillColor;
	calcColor[16] = calculatorNormalFillColor;
	calcColor[17] = calculatorBinaryFillColor;
	calcColor[18] = calculatorUnaryFillColor;
	calcColor[19] = calculatorUnaryFillColor;
	calcColor[20] = calculatorNormalFillColor;
	calcColor[21] = calculatorNormalFillColor;
	calcColor[22] = calculatorNormalFillColor;
	calcColor[23] = calculatorBinaryFillColor;
	calcColor[24] = calculatorUnaryFillColor;
	calcColor[25] = calculatorNormalFillColor;
	calcColor[26] = calculatorNormalFillColor;
	calcColor[27] = calculatorNormalFillColor;
	calcColor[28] = calculatorSpecialFillColor;
	calcColor[29] = calculatorSpecialFillColor;
	calcColor[30] = calculatorUnaryFillColor;
	calcColor[31] = calculatorMemoryFillColor;
	calcColor[32] = calculatorMemoryFillColor;
	calcColor[33] = calculatorMemoryFillColor;
	calcColor[34] = calculatorSpecialFillColor;
	calcColor[35] = calculatorSpecialFillColor;					
	calcColor[36] = calculatorUnaryFillColor;
	calcColor[37] = calculatorUnaryFillColor;
	calcColor[38] = calculatorBinaryFillColor;
	calcColor[39] = calculatorSpecialFillColor;
	calcColor[40] = calculatorSpecialFillColor;
	calcColor[41] = calculatorBinaryFillColor;
	calcColor[42] = calculatorBinaryFillColor;
	calcColor[43] = calculatorUnaryFillColor;
	calcColor[44] = calculatorNormalFillColor;
	calcColor[45] = calculatorNormalFillColor;
	calcColor[46] = calculatorNormalFillColor;
	calcColor[47] = calculatorBinaryFillColor;
	calcColor[48] = calculatorBinaryFillColor;
	calcColor[49] = calculatorUnaryFillColor;
	calcColor[50] = calculatorNormalFillColor;
	calcColor[51] = calculatorNormalFillColor;
	calcColor[52] = calculatorNormalFillColor;
	calcColor[53] = calculatorBinaryFillColor;
	calcColor[54] = calculatorUnaryFillColor;
	calcColor[55] = calculatorUnaryFillColor;
	calcColor[56] = calculatorNormalFillColor;
	calcColor[57] = calculatorNormalFillColor;
	calcColor[58] = calculatorNormalFillColor;
	calcColor[59] = calculatorBinaryFillColor;
	calcColor[60] = calculatorUnaryFillColor;
	calcColor[61] = calculatorNormalFillColor;
	calcColor[62] = calculatorNormalFillColor;
	calcColor[63] = calculatorNormalFillColor;
	calcColor[64] = calculatorSpecialFillColor;
	calcColor[65] = calculatorSpecialFillColor;
	calcColor[66] = calculatorUnaryFillColor;
	calcColor[67] = calculatorMemoryFillColor;
	calcColor[68] = calculatorMemoryFillColor;
	calcColor[69] = calculatorMemoryFillColor;
	calcColor[70] = calculatorSpecialFillColor;
	calcColor[71] = calculatorSpecialFillColor;
}

void initCalc()
{
	deg = true;
	lastKey = 0;
	isMore = 0;
	wasLastPress = false;
}

void startScreen()
{
	ans = m_apm_init();
	mem = m_apm_init();
	curNumber = m_apm_init();
	
	isMore = 0;
	wasLastPress = false;
	
	screen = (char *)trackMalloc(DIGITS+1,"calculator screen");
	memory = (char *)trackMalloc(100,"calculator memory");
	
	strcpy(screen, "0");
	strcpy(memory, "0");
}

void closeScreen()
{
	trackFree(screen);
	trackFree(memory);
	
	screen = NULL;
	memory = NULL;
	
	m_apm_free(ans);
	m_apm_free(mem);
	m_apm_free(curNumber);
}

void clearCalc()
{	
	if(screen == NULL)
		startScreen();
	
	if(stackPos > -1)
	{
		for(int x=stackPos;x>-1;x--)
			m_apm_free(stack[x]);
	}
	
	stackPos = -1;
	
	m_apm_set_long(ans, 0);
	m_apm_set_long(curNumber, 0);
	strcpy(screen,"0");	
	lastKey = 0;
	
	belowDecimal = false;
	wasLastPress = false;
	
	char str[64];
	sprintf(str, "%d%d%d", getHour(true), getMinute(), getSecond());
	m_apm_set_random_seed(str);
	
	resetColors();
}

void pushCalcStack()
{
	if(stackPos > 9)
		return;
	stackPos++;
	stack[stackPos] = m_apm_init();
	m_apm_copy(stack[stackPos], ans);	
	stackKey[stackPos] = lastKey;
	lastKey = 0;
	
	m_apm_set_long(ans, 0);
	strcpy(screen,"0");	
}	

void popCalcStack()
{
	if(stackPos < 0)
		return;
	performAction();
	m_apm_copy(ans, stack[stackPos]);		
	lastKey = stackKey[stackPos];
	
	m_apm_free(stack[stackPos]);
	stackPos--;
}

int getButton(int a, int b)
{
	for(int y = 0; y < NUM_BUTTONS_Y; y++)
	{
		for(int x = 0; x < NUM_BUTTONS_X; x++)
		{
			if(a >= (11 + (x*39)) && b >= (20 + (y*27)) && a <= (11 + (x*39)+35) && b <= (20 + (y*27)+23))
				return (x + (y * NUM_BUTTONS_X)) + 1;
		}
	}
	
	return -1;
}

void addToCalc(int number)
{
	if(strcmp(screen,"0")==0 || clearOnType)
		strcpy(screen,"");
	else
	{
		if(getStringWidth(screen, (uint16 **)font_gautami_10) > 240)
			return;
	}
	
	sprintf(screen,"%s%d", screen, number);	
	
	clearOnType = false;
}

void formatScreen(char *str)
{
	int x = 0;
	bool found = false;	
	
	while(str[x] != 0)
	{
		if(str[x] == '.')
			found = true;
		x++;
	}
	
	if(found)
	{
		while(str[strlen(str)-1] == '0')
			str[strlen(str)-1] = 0;
			
		if(str[strlen(str)-1] == '.')
			str[strlen(str)-1] = 0;
	}
}

void ansToScreen()
{	
	if(m_apm_fixpt_string_len(ACCURACY, ans) < DIGITS)
	{
		m_apm_to_fixpt_string(screen, ACCURACY, ans);		
		formatScreen(screen);
		
		if(getStringWidth(screen, (uint16 **)font_gautami_10) > 240)
			m_apm_to_string(screen, ACCURACY, ans);	
	}
	else // too many digits!
	{
		strcpy(screen,"ERR");
	}
}

void performAction()
{
	// this function will simply turn the answer into what the screen has
	// if there is no waiting binary operation, or else it performs said
	// operation and clears the binary flag.
	
	clearOnType = true;
	belowDecimal = false;
	
	if(lastKey == 0)
	{
		m_apm_set_string(ans,screen);
		formatScreen(screen);
		
		return;
	}
	
	working = true;
	
	enterKey = lastKey;
	strcpy(enterBuffer, screen);
	
	m_apm_set_string(curNumber,screen);	
	M_APM apmtmp = m_apm_init();
	M_APM apmlimit;
	
	switch(lastKey)
	{
		case '+':
			m_apm_add(apmtmp,ans,curNumber);
			break;
		case '-':
			m_apm_subtract(apmtmp,ans,curNumber);
			break;
		case '*':
			m_apm_multiply(apmtmp,ans,curNumber);
			break;
		case '/':
			apmlimit = m_apm_init();
			m_apm_set_long(apmlimit, 0);	
			if(m_apm_compare(curNumber, apmlimit) != 0)
			{
				m_apm_divide(apmtmp,ACCURACY,ans,curNumber);				
				m_apm_free(apmlimit);
			}
			else
			{				
				strcpy(screen,"ERR");
				lastKey = 0;
				
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				
				working = false;
				return;
			}	
			break;
		case '^':
			apmlimit = m_apm_init();
			m_apm_set_long(apmlimit, 9999);	
			if(m_apm_compare(curNumber, apmlimit) <= 0)
			{
				m_apm_pow(apmtmp,ACCURACY,ans,curNumber);			
				m_apm_free(apmlimit);
			}
			else
			{				
				strcpy(screen,"ERR");
				lastKey = 0;
				
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				
				working = false;
				return;
			}	
			break;
		case 'g':
			m_apm_gcd(apmtmp,ans,curNumber);
			break;
		case 'l':
			m_apm_lcm(apmtmp,ans,curNumber);
			break;
	}
	
	lastKey = 0;
	working = false;
	
	m_apm_copy(ans, apmtmp);	
	m_apm_free(apmtmp);
	m_apm_set_long(curNumber, 0);	
	ansToScreen();	
}

void performUnaryAction(int action)
{
	performAction(); // flush out anything waiting
	enterKey = 0;
	
	if(strcmp(screen,"ERR") == 0)
		return;
	
	working = true;
	
	m_apm_set_string(curNumber,screen);	
	M_APM apmtmp = m_apm_init();	
	M_APM apmlimit = m_apm_init();
	
	switch(action)
	{
		case RECIPROCAL:
			m_apm_set_long(apmlimit, 0);	
			if(m_apm_compare(curNumber, apmlimit) != 0)
				m_apm_reciprocal(apmtmp,ACCURACY,curNumber);	
			else
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}		
			break;
		case FACTORIAL:	
			m_apm_set_long(apmlimit, 9999);			
			if(m_apm_compare(curNumber, apmlimit) <= 0) // make sure its not > limit, because this is where the DS crashes
				m_apm_factorial(apmtmp, curNumber);
			else
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			break;
		case SQRT:
			m_apm_set_long(apmlimit, 0);	
			if(m_apm_compare(curNumber, apmlimit) >= 0) // bounds check to make sure we are >= 0
				m_apm_sqrt(apmtmp, ACCURACY, curNumber);
			else
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			break;
		case SQUARE:
			m_apm_multiply(apmtmp,curNumber,curNumber);
			break;
		//case PI:
		//	m_apm_copy(apmtmp, MM_PI);
		//	break;
		case EXP: // e^x
			m_apm_pow(apmtmp, ACCURACY, MM_E, curNumber);
			break;
		case LN:
			m_apm_set_long(apmlimit, 0);	
			if(m_apm_compare(curNumber, apmlimit) > 0) // bounds check to make sure we are > 0
				m_apm_log(apmtmp, ACCURACY, curNumber);				
			else
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			break;
		case SIN:
			if(deg)
			{
				//must convert deg to rad! 2pi = 360º, 
				
				m_apm_multiply(apmtmp,curNumber,MM_2_PI);  // 2pi * degrees
				m_apm_copy(curNumber, apmtmp);
				
				M_APM apm360 = m_apm_init();
				m_apm_set_long(apm360, 360);					
				
				m_apm_divide(apmtmp,ACCURACY,curNumber,apm360); // divide by 360
				m_apm_copy(curNumber, apmtmp);				
				
				m_apm_free(apm360);
			}
			m_apm_sin(apmtmp, ACCURACY, curNumber);
			break;
		case COS:
			if(deg)
			{
				//must convert deg to rad! 2pi = 360º, 
				
				m_apm_multiply(apmtmp,curNumber,MM_2_PI);  // 2pi * degrees
				m_apm_copy(curNumber, apmtmp);
				
				M_APM apm360 = m_apm_init();
				m_apm_set_long(apm360, 360);					
				
				m_apm_divide(apmtmp,ACCURACY,curNumber,apm360); // divide by 360
				m_apm_copy(curNumber, apmtmp);			
				
				m_apm_free(apm360);
			}
			m_apm_cos(apmtmp, ACCURACY, curNumber);
			break;
		case TAN:
			if(deg)
			{
				//must convert deg to rad! 2pi = 360º, 
				
				m_apm_multiply(apmtmp,curNumber,MM_2_PI);  // 2pi * degrees
				m_apm_copy(curNumber, apmtmp);
				
				M_APM apm360 = m_apm_init();
				m_apm_set_long(apm360, 360);					
				
				m_apm_divide(apmtmp,ACCURACY,curNumber,apm360); // divide by 360
				m_apm_copy(curNumber, apmtmp);			
				
				m_apm_free(apm360);
			}
			m_apm_tan(apmtmp, ACCURACY, curNumber);
			break;
		case LOG:		
			m_apm_set_long(apmlimit, 0);	
			if(m_apm_compare(curNumber, apmlimit) > 0) // bounds check to make sure we are > 0
				m_apm_log10(apmtmp, ACCURACY, curNumber);
			else
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			break;
		case M_INT:
			m_apm_floor(apmtmp, curNumber);
			break;
		case ARCSIN: 
		{
			bool aS = false;
			
			m_apm_set_long(apmlimit, -1);	
			if(m_apm_compare(curNumber, apmlimit) >= 0) // bounds check
			{
				m_apm_set_long(apmlimit, 1);	
				if(m_apm_compare(curNumber, apmlimit) <= 0) // bounds check
				{
					m_apm_arcsin(apmtmp, ACCURACY, curNumber);					
					aS = true;
				}
			}
			
			if(aS == false)
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			else
			{
				if(deg)
				{
					//must convert rad to deg! 2pi = 360º, 
					
					M_APM apm360 = m_apm_init();
					m_apm_set_long(apm360, 360);	
					
					m_apm_multiply(curNumber,apmtmp,apm360);  // degrees * 360
					m_apm_free(apm360);
					
					m_apm_divide(apmtmp,ACCURACY,curNumber,MM_2_PI); // divide by 2pi
				}
			}
			
			break;
		}
		case ARCCOS:
		{
			bool aC = false;
			
			m_apm_set_long(apmlimit, -1);	
			if(m_apm_compare(curNumber, apmlimit) >= 0) // bounds check
			{
				m_apm_set_long(apmlimit, 1);	
				if(m_apm_compare(curNumber, apmlimit) <= 0) // bounds check
				{
					m_apm_arccos(apmtmp, ACCURACY, curNumber);
					aC = true;
				}
			}
			
			if(aC == false)
			{				
				strcpy(screen,"ERR");
				m_apm_free(apmtmp);
				m_apm_free(apmlimit);
				working = false;
				return;
			}
			else
			{
				if(deg)
				{
					//must convert rad to deg! 2pi = 360º, 
					
					M_APM apm360 = m_apm_init();
					m_apm_set_long(apm360, 360);	
					
					m_apm_multiply(curNumber,apmtmp,apm360);  // degrees * 360
					m_apm_free(apm360);
					
					m_apm_divide(apmtmp,ACCURACY,curNumber,MM_2_PI); // divide by 2pi
				}
			}
			
			break;
		}
		case ARCTAN:
			m_apm_arctan(apmtmp, ACCURACY, curNumber);	
			
			if(deg)
			{
				//must convert rad to deg! 2pi = 360º, 
				
				M_APM apm360 = m_apm_init();
				m_apm_set_long(apm360, 360);	
				
				m_apm_multiply(curNumber,apmtmp,apm360);  // degrees * 360
				m_apm_free(apm360);
				
				m_apm_divide(apmtmp,ACCURACY,curNumber,MM_2_PI); // divide by 2pi
			}
			break;
	}	
	
	working = false;
	
	m_apm_copy(ans, apmtmp);
	m_apm_free(apmtmp);
	m_apm_free(apmlimit);
	m_apm_set_long(curNumber, 0);
	ansToScreen();
}

void calcAction(int action)
{
	if(action <= 0)
		return;
		
	if(strcmp(screen,"ERR") == 0 && action != 34 && action != 35)
		return;

	curKey = action - 1;
	eraseChar = 2;
	drawCalculatorButtons();
	IRQVBlankWait();
	bg_swapBuffers();
	drawCalculatorButtons();
	
	switch(action - 1)
	{
		case 0: // sqrt
			if(isMore == 1)
				return;
			wasLastPress = false;
			performUnaryAction(SQRT);
			break;
		case 1: // squared
			if(isMore == 1)
				return;
			wasLastPress = false;
			performUnaryAction(SQUARE);
			break;
		case 2: // ^
			if(!wasLastPress)
			{
				wasLastPress = true;
				performAction();
			}
			
			lastKey = '^';
			break;	
		case 3: // (
			wasLastPress = false;
			pushCalcStack();
			break;
		case 4: // )
			wasLastPress = false;
			popCalcStack();
			break;
		case 5: // /
			if(!wasLastPress)
			{
				wasLastPress = true;
				performAction();
			}
			
			lastKey = '/';
			break;	
		case 6: // x!
			if(isMore == 0)
			{				
				wasLastPress = false;
				performUnaryAction(FACTORIAL);
			}
			else
			{
				if(!wasLastPress)
				{
					wasLastPress = true;
					performAction();
				}
				
				lastKey = 'g';
			}
			break;
		case 7: // sin
			wasLastPress = false;
			if(isMore == 0)
				performUnaryAction(SIN);
			else
				performUnaryAction(ARCSIN);
			break;
		case 8: // 7
			wasLastPress = false;
			addToCalc(7);
			break;
		case 9: // 8
			wasLastPress = false;
			addToCalc(8);
			break;
		case 10: // 9
			wasLastPress = false;
			addToCalc(9);
			break;
		case 11: // *
			if(!wasLastPress)
			{
				wasLastPress = true;
				performAction();
			}
			
			lastKey = '*';
			break;	
		case 12: // 1/x	
			if(isMore == 0)
			{
				performUnaryAction(RECIPROCAL);
				wasLastPress = false;
			}
			else
			{
				if(!wasLastPress)
				{
					wasLastPress = true;
					performAction();
				}
				
				lastKey = 'l';
			}
			break;
		case 13: // cos
			wasLastPress = false;
			if(isMore == 0)
				performUnaryAction(COS);
			else
				performUnaryAction(ARCCOS);
			break;
		case 14: // 4
			wasLastPress = false;
			addToCalc(4);
			break;
		case 15: // 5
			wasLastPress = false;
			addToCalc(5);
			break;
		case 16: // 6
			wasLastPress = false;
			addToCalc(6);
			break;
		case 17: // -
			if(!wasLastPress)
			{
				wasLastPress = true;
				performAction();
			}
			
			lastKey = '-';
			break;	
		case 18: // ln
			wasLastPress = false;
			if(isMore == 0)
				performUnaryAction(LN);
			else
				performUnaryAction(M_INT);			
			break;
		case 19: // tan		
			wasLastPress = false;
			if(isMore == 0)
				performUnaryAction(TAN);
			else
				performUnaryAction(ARCTAN);
			break;
		case 20: // 1
			wasLastPress = false;
			addToCalc(1);
			break;
		case 21: // 2
			wasLastPress = false;
			addToCalc(2);
			break;
		case 22: // 3
			wasLastPress = false;
			addToCalc(3);
			break;
		case 23: // +
			if(!wasLastPress)
			{
				wasLastPress = true;
				performAction();
			}
			
			lastKey = '+';
			break;	
		case 24: // e^x
			wasLastPress = false;
			if(isMore == 1)
				return;
			performUnaryAction(EXP);
			break;
		case 25: // pi
			wasLastPress = false;
			if(isMore == 0)
				m_apm_to_fixpt_string(screen, ACCURACY, MM_PI);		
			else
			{
				M_APM mTmp = m_apm_init();
				m_apm_get_random(mTmp);
				m_apm_to_fixpt_string(screen, ACCURACY, mTmp);	
			}
			
			formatScreen(screen);
			
			clearOnType = true;
			
			break;
		case 26: // 0
			wasLastPress = false;
			addToCalc(0);
			break;
		case 27: // .
			wasLastPress = false;
			if(belowDecimal == false)
			{
				belowDecimal = true;
				if(clearOnType)
				{
					strcpy(screen,"0.");
					clearOnType = false;
				}
				else
					sprintf(screen,"%s.", screen);					
			}
			break;
		case 28: // (-)
			// first check for zero
			
			if(strcmp(screen,"0")==0)
				break;
			
			// not zero, we can flip sign
			
			wasLastPress = false;
			char tStr[100];
			
			strcpy(tStr,screen);
			
			if(screen[0] == '-')
				sprintf(screen, "%s", tStr+1);
			else
				sprintf(screen, "-%s", tStr);
			break;
		case 29: // =
			wasLastPress = false;
			
			if(enterKey != 0 && lastKey == 0)
			{
				strcpy(screen, enterBuffer);
				lastKey = enterKey;
			}
			
			performAction();
			break;
		case 30: // log
			wasLastPress = false;
			if(isMore == 1)
				return;
			performUnaryAction(LOG);
			break;	
		case 31: // M+		
			wasLastPress = false;
			performAction();
			m_apm_copy(mem, ans);
			strcpy(memory, screen);
			break;
		case 32: // M-
		{
			wasLastPress = false;
			M_APM apmtmp = m_apm_init();			
			m_apm_copy(apmtmp, ans);
			m_apm_copy(ans, mem);
			
			ansToScreen();
			
			m_apm_copy(ans, apmtmp);			
			m_apm_free(apmtmp);
			clearOnType = true;
			break;
		}
		case 33: // MC
			wasLastPress = false;
			m_apm_set_long(mem, 0);
			strcpy(memory,"0");
			break;
		case 34: // CE
			wasLastPress = false;
			m_apm_set_long(curNumber, 0);
			strcpy(screen,"0");
			belowDecimal = false;
			break;
		case 35: // C
			wasLastPress = false;
			clearCalc();
			belowDecimal = false;
			break;
	}
}

void drawAnswerScreen()
{	
	if(screen == NULL)
		clearCalc();

	char str[256];
	int x = 0;
	bool found = false;
	
	setFont((uint16 **)font_gautami_10);
	setColor(calculatorScreenTextColor);
	
	fb_drawFilledRect(3, 165 + 8, 252, 165 + 23, calculatorScreenBorderColor, calculatorScreenFillColor);	
	
	if(working)	
	{
		strcpy(str, l_working);
		switch(getSecond()%4)
		{
			case 1:
				strcat(str," .");
				break;
			case 2:
				strcat(str," . .");
				break;
			case 3:
				strcat(str," . . .");
				break;
		}
		
		fb_dispString(7, 165 + 8 + 5, str);
	}
	else
	{
		strcpy(str,screen);
		
		while(str[x] != 0)
		{
			if(str[x] == '.')
				found = true;
			x++;
		}
		
		if(!found)
			strcat(str,".");
		
		fb_dispString(252 - getStringWidth(str, (uint16 **)font_gautami_10), 165 + 8 + 5, str);
	}
	
	setColor(genericTextColor);
	
	strcpy(str,memory);	
	x = 0;
	found = false;
	
	while(str[x] != 0)
	{
		if(str[x] == '.')
			found = true;
		x++;
	}
	
	if(!found)
		strcat(str,".");	
	
	fb_dispString(3, 138, l_memory);	
	fb_drawFilledRect(3, 138 + 8, 252, 138 + 23, calculatorScreenBorderColor, calculatorScreenFillColor);
	fb_dispString(252 - getStringWidth(str, (uint16 **)font_gautami_10), 138 + 8 + 5, str);

	if(deg)
	{
		setColor(genericTextColor);
		fb_dispString(3, 138 + 23 + 3, l_deg);
		setColor(calculatorScreenFadeTextColor);
		fb_dispString(40, 138 + 23 + 3, l_rad);
	}
	else
	{
		setColor(calculatorScreenFadeTextColor);
		fb_dispString(3, 138 + 23 + 3, l_deg);
		setColor(genericTextColor);
		fb_dispString(40, 138 + 23 + 3, l_rad);	
	}
}

void drawCalculatorButtons()
{	
	drawHome();
	
	setFont((uint16 **)font_gautami_10);
	setColor(calculatorButtonTextColor);
	
	for(int y = 0;y < NUM_BUTTONS_Y; y++)
	{
		for(int x = 0; x < NUM_BUTTONS_X; x++)
		{
			if(strlen(calcString[(x + (y * NUM_BUTTONS_X)) + (isMore * NUM_CONTROLS)]) > 0)
			{
				if(curKey == (x + (y * NUM_BUTTONS_X)))
				{
					bg_drawFilledRect(11 + (x*39), 20 + (y*27), 11 + (x*39)+35, 20 + (y*27)+23, calculatorButtonHighlightColor, calcColor[(x + (y * NUM_BUTTONS_X)) + (isMore * NUM_CONTROLS)]);
				}
				else
				{
					bg_drawFilledRect(11 + (x*39), 20 + (y*27), 11 + (x*39)+35, 20 + (y*27)+23, calculatorButtonBorderColor, calcColor[(x + (y * NUM_BUTTONS_X)) + (isMore * NUM_CONTROLS)]);
				}
				
				bg_dispString(11 + centerOnPt((x*39) + 17, calcString[(x + (y * NUM_BUTTONS_X)) + (isMore * NUM_CONTROLS)], (uint16 **)font_gautami_10), 20 + (y*27) + 8, calcString[(x + (y * NUM_BUTTONS_X)) + (isMore * NUM_CONTROLS)]);
			}
		}	
	}
	
	char *strPtr;
	
	if(isMore == 0)
	{
		strPtr = l_more;
	}
	else
	{
		strPtr = l_back;
	}
	
	if(deg)
	{
		drawLR(strPtr, l_rad);
	}
	else
	{
		drawLR(strPtr, l_deg);
	}
}
