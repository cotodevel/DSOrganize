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

//todo

/* 
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include <stdlib.h>
#include "fatwrapper.h"
#include "resetmemory.h"
#include "supercard.h"
#include "general.h"
#include "sound.h"
#include "specific_shared.h"

#define SSC_Disabled (0)
#define SSC_SDRAM (1)
#define SSC_CF (2)

#define SC_REG_UNLOCK	*(vu16*)(0x09FFFFFE)
static void SetSC_UNLOCK(int SSC)
{
  switch(SSC){
    case SSC_Disabled:
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0x0001;
      SC_REG_UNLOCK = 0x0001;
      break;
    case SSC_SDRAM:
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0x0005;
      SC_REG_UNLOCK = 0x0005;
      break;
    case SSC_CF:
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0xA55A;
      SC_REG_UNLOCK = 0x0003;
      SC_REG_UNLOCK = 0x0003;
  }
}
#undef SC_REG_UNLOCK

#define _REG_WAIT_CR (*(vuint16*)0x04000204)

#include "BootStrap_SC_bin.h"

#define ReadBufCount (16*512)

void boot_SCCF(char *file)
{
  DRAGON_FILE *fp = DRAGON_fopen(file, "r");
  
  vu16 *pROM16=(vu16*)0x08000000;
  
  {
    u16 *prb=(u16*)safeMalloc(8);
    
    DRAGON_fseek(fp,0,SEEK_SET);
    DRAGON_fread(prb,1,8,fp);
    DRAGON_fseek(fp,0,SEEK_SET);
    
    if((prb[2]==0)&&(prb[3]==0)){
      
      u16 *pBootStrap=(u16*)BootStrap_SC_bin;
      u32 BootStrapSize=BootStrap_SC_bin_size;
      
	  u32 adr = 0;
      for(adr=0;adr<BootStrapSize/2;adr++){
        pROM16[adr]=pBootStrap[adr];
      }
      pROM16+=BootStrapSize/2;
    }
    
    free(prb); prb=NULL;
  }
  
  {
    
    u16 *pReadBuf=(u16*)safeMalloc(ReadBufCount);
    u32 ReadCount=0;
        
    DRAGON_fseek(fp,0,SEEK_SET);
    
    while(1){      
      SetSC_UNLOCK(SSC_CF);
      u32 rs=DRAGON_fread(pReadBuf,1,ReadBufCount,fp);
      ReadCount+=rs;
      
      SetSC_UNLOCK(SSC_SDRAM);
	  u32 adr = 0;
      for(adr=0;adr<(rs+1)/2;adr++){
        pROM16[adr]=pReadBuf[adr];
      }
      pROM16+=(rs+1)/2;
      
      if(rs!=ReadBufCount){
        SetSC_UNLOCK(SSC_CF);
        break;
      }
    }
    
    free(pReadBuf); pReadBuf=NULL;
  }
  
  DRAGON_fclose(fp);
  DRAGON_FreeFiles();
  
  SetSC_UNLOCK(SSC_Disabled);
  
  resetMemory1_ARM9();
  resetMemory2_ARM9();
  
  {    
    REG_IME = IME_DISABLE;	// Disable interrupts
    WAIT_CR |= (0x8080);  // ARM7 has access to GBA cart
    *((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address (SCCF)
	SendArm7Command(ARM7COMMAND_BOOT_SUPERCARD, 0);
    swiSoftReset();  // Reset
  }
  
  while(1);
}

*/