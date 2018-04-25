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
#include "resources.h"

#include "main.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "typedefsTGDS.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"

#include "devoptab_devices.h"
#include "fsfatlayerTGDSNew.h"
#include "usrsettingsTGDS.h"
#include "exceptionTGDS.h"

#include "videoTGDS.h"
#include "keypadTGDS.h"
#include "guiTGDS.h"

// hbdb
char *res_MOTD = { "http://www.dragonminded.com/dsorganize/?req=MOTD&ver=3" };
char *res_CARD = { "http://www.dragonminded.com/dsorganize/?req=HBLIST&ver=3&card=" };
char *res_PKG = { "http://www.dragonminded.com/dsorganize/?req=PACKAGE&ver=3&pkg=%s" };

// irc
char *res_aboutIRC = { ":%s NOTICE %s :%cVERSION DSOrganize IRC 3.1129 by DragonMinded%c%c%c" };

// homebrew database agent
char *res_uAgent = { "DSOrganize Homebrew Database" };

// sound streaming agent
char *res_sAgent = { "DSOrganize Streaming Client" };

// web browser agent
char *res_bAgent = { "DSOrganize Web Browser" };


void printfDebugger(sint8 *fmt, ...){
	
	uint8 VRAM_A_bits = VRAM_A_CR;
	uint8 VRAM_B_bits = VRAM_B_CR;
	uint8 VRAM_C_bits = VRAM_C_CR;
	uint8 VRAM_D_bits = VRAM_D_CR;
	uint8 VRAM_E_bits = VRAM_E_CR;
	uint8 VRAM_F_bits = VRAM_F_CR;
	uint8 VRAM_G_bits = VRAM_G_CR;
	uint8 VRAM_H_bits = VRAM_H_CR;
	uint8 VRAM_I_bits = VRAM_I_CR;
	
	//Init default console here
	bool project_specific_console = false;	//set default console or custom console: default console
	GUI_init(project_specific_console);
	clrscr();
	
	REG_IME = 0;
	REG_IF &= ~0x1;
	REG_IE = 0;
	
	char tempprintfbuf[64] = {0};
	va_list args;
	va_start (args, fmt);
	vsnprintf ((sint8*)tempprintfbuf, 64, fmt, args);
	va_end (args);
	
    printf("%s",tempprintfbuf);
	
	
	while(1==1){}
	VRAM_A_CR = VRAM_A_bits;
	VRAM_B_CR = VRAM_B_bits;
	VRAM_C_CR = VRAM_C_bits;
	VRAM_D_CR = VRAM_D_bits;
	VRAM_E_CR = VRAM_E_bits;
	VRAM_F_CR = VRAM_F_bits;
	VRAM_G_CR = VRAM_G_bits;
	VRAM_H_CR = VRAM_H_bits;
	VRAM_I_CR = VRAM_I_bits;
	
	
}


//customHandler 
void CustomDebugHandler(){
	REG_IME = 0;
	REG_IF &= ~0x1;
	REG_IE = 0;
	clrscr();
	
	//Init default console here
	bool project_specific_console = false;	//set default console or custom console: default console
	GUI_init(project_specific_console);
	GUI_clear();
	
	printf("custom Handler: Data Abort!");
	uint32 * debugVector = (uint32 *)&exceptionArmRegs[0];
	uint32 pc_abort = (uint32)exceptionArmRegs[0xf];
	
	if((debugVector[0xe] & 0x1f) == 0x17){
		pc_abort = pc_abort - 8;
	}
	
	printf("R0[%x] R1[%X] R2[%X] \n",debugVector[0],debugVector[1],debugVector[2]);
	printf("R3[%x] R4[%X] R5[%X] \n",debugVector[3],debugVector[4],debugVector[5]);
	printf("R6[%x] R7[%X] R8[%X] \n",debugVector[6],debugVector[7],debugVector[8]);
	printf("R9[%x] R10[%X] R11[%X] \n",debugVector[9],debugVector[0xa],debugVector[0xb]);
	printf("R12[%x] R13[%X] R14[%X]  \n",debugVector[0xc],debugVector[0xd],debugVector[0xe]);
	printf("R15[%x] SPSR[%x] CPSR[%X]  \n",pc_abort,debugVector[17],debugVector[16]);
	while(1==1){}
}