
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "spifwTGDS.h"
#include "spiTGDS.h"
#include "busTGDS.h"

#include "InterruptsARMCores_h.h"
#include "loader.h"
#include "specific_shared.h"

#define IRQ_ALL (uint32)(~0)
#define REG_EXMEMCNT (*(vu16*)0x04000204)
#define ARM7_OWNS_ROM (1<<7)
#define ARM7_OWNS_CARD (1<<11)

//=================================================
#ifdef ARM7
//=================================================


__attribute__((noinline)) static void readFirmware2(uint32 address, uint32 size, uint8 * buffer) {
  uint32 index;

  // Read command
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  REG_SPI_CR = BIT_SPICNT_ENABLE | BIT_SPICNT_CSHOLDENABLE | BIT_SPICNT_NVRAMCNT;
  REG_SPI_DATA = FWCMD_READ;
  while (REG_SPI_CR & BIT_SPICNT_BUSY);

  // Set the address
  REG_SPI_DATA =  (address>>16) & 0xFF;
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  REG_SPI_DATA =  (address>>8) & 0xFF;
  while (REG_SPI_CR & BIT_SPICNT_BUSY);
  REG_SPI_DATA =  (address) & 0xFF;
  while (REG_SPI_CR & BIT_SPICNT_BUSY);

  for (index = 0; index < size; index++) {
    REG_SPI_DATA = 0;
    while (REG_SPI_CR & BIT_SPICNT_BUSY);
    buffer[index] = REG_SPI_DATA & 0xFF;
  }
  REG_SPI_CR = 0;
}

/*-------------------------------------------------------------------------
resetMemory_ARM7
Clears all of the NDS's RAM that is visible to the ARM7
Written by Darkain.
Modified by Chishm:
 * Added STMIA clear mem loop
--------------------------------------------------------------------------*/
__attribute__((noinline)) static void resetMemory_ARM7 (void)
{
	int i;
	u8 settings1, settings2;
	
	REG_IME = 0;

/*
	for (i=0; i<16; i++) {
		SCHANNEL_CR(i) = 0;
		SCHANNEL_TIMER(i) = 0;
		SCHANNEL_SOURCE(i) = 0;
		SCHANNEL_LENGTH(i) = 0;
	}
*/
	for (i=0x04000400; i<0x04000500; i+=4) {
	  *((u32*)i)=0;
	}
	SOUND_CR = 0;

	//clear out ARM7 DMA channels and timers
/*
	for (i=0; i<4; i++) {
		DMA_CR(i) = 0;
		DMA_SRC(i) = 0;
		DMA_DEST(i) = 0;
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}
*/

  for(i=0x040000B0;i<(0x040000B0+0x30);i+=4){
    *((vu32*)i)=0;
  }
  for(i=0x04000100;i<0x04000110;i+=2){
    *((u16*)i)=0;
  }

	//switch to user mode
  __asm volatile(
	"mov r6, #0x1F                \n"
	"msr cpsr, r6                 \n"
	:
	:
	: "r6"
	);

#if 0
  __asm volatile (
	// clear exclusive IWRAM
	// 0380:0000 to 0380:FFFF, total 64KiB
	"mov r0, #0 				\n"	
	"mov r1, #0 				\n"
	"mov r2, #0 				\n"
	"mov r3, #0 				\n"
	"mov r4, #0 				\n"
	"mov r5, #0 				\n"
	"mov r6, #0 				\n"
	"mov r7, #0 				\n"
	"mov r8, #0x03800000		\n"	// Start address 
	"mov r9, #0x03800000		\n" // End address part 1
	"orr r9, r9, #0x10000		\n" // End address part 2
	"clear_EIWRAM_loop:			\n"
	"stmia r8!, {r0, r1, r2, r3, r4, r5, r6, r7} \n"
	"cmp r8, r9					\n"
	"blt clear_EIWRAM_loop		\n"
	:
	:
	: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
	);
#endif

  __asm volatile (
	// clear most of EWRAM - except after 0x023FF800, which has DS settings
	"mov r8, #0x02000000		\n"	// Start address part 1 
	"orr r8, r8, #0x8000		\n" // Start address part 2
	"mov r9, #0x02300000		\n" // End address part 1
	"orr r9, r9, #0xff000		\n" // End address part 2
	"orr r9, r9, #0x00800		\n" // End address part 3
	"clear_EXRAM_loop:			\n"
	"stmia r8!, {r0, r1, r2, r3, r4, r5, r6, r7} \n"
	"cmp r8, r9					\n"
	"blt clear_EXRAM_loop		\n"
	:
	:
	: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
	);
  
	REG_IE = 0;
	REG_IF = ~0;
	(*(vu32*)(0x04000000-4)) = 0;  //IRQ_HANDLER ARM7 version
	(*(vu32*)(0x04000000-8)) = ~0; //VBLANK_INTR_WAIT_FLAGS, ARM7 version
	REG_POWERCNT = 1;  //turn off power to stuffs
	
	// Reload DS Firmware settings
	readFirmware2((u32)0x03FE70, 0x1, &settings1);
	readFirmware2((u32)0x03FF70, 0x1, &settings2);
	
	if (settings1 > settings2) {
		readFirmware2((u32)0x03FE00, 0x70, (u8*)0x027FFC80);
	} else {
		readFirmware2((u32)0x03FF00, 0x70, (u8*)0x027FFC80);
	}
}

void NdsLoaderCheck(void) {
	
	if(*((vu32*)0x027FFE24) == (u32)0x027FFE04)
	{
		DisableIrq(IRQ_ALL);
		*((vu32*)0x027FFE34) = (u32)0x06000000;
		swiSoftReset();
	}
}

void bootndsGBAMPCheck(void) {
	if (*((vu32*)0x027FFE24) == (u32)0x027FFE04) {  // Check for ARM9 reset
		REG_IME = IME_DISABLE;   // Disable interrupts
        REG_IF = REG_IF;   // Acknowledge interrupt
        *((vu32*)0x027FFE34) = (u32)0x08000000;   // Bootloader start address
        swiSoftReset();   // Jump to boot loader
	}
}

void bootndsM3SCcheck(void) {
	if (*((vu32*)0x027FFE24) == (u32)0x027FFE08) {
		resetMemory_ARM7();
		
		REG_IME = IME_DISABLE;	// Disable interrupts
		REG_IF = REG_IF;	// Acknowledge interrupt
		*((vu32*)0x027FFE34) = (u32)0x08000000;	// Bootloader start address
		swiSoftReset();	// Jump to boot loader
	}
}

void bootndsCheck(void) {
	if(REG_IPC_FIFO_RX == 17) NdsLoaderCheck();
	if(REG_IPC_FIFO_RX == 18) bootndsGBAMPCheck();
	if(REG_IPC_FIFO_RX == 19) bootndsM3SCcheck();
}

#endif
  
//=================================================
#ifdef ARM9
//=================================================

#include "fatwrapper.h"
#include "videoTGDS.h"
#include "dldi.h"
#include "dswnifi_lib.h"

// --- Chishm's loader --- WORKING
#define LCDC_BANK_C (u16*)0x06840000
#define STORED_FILE_CLUSTER (*(((u32*)LCDC_BANK_C) + 1))
#define INIT_DISC (*(((u32*)LCDC_BANK_C) + 2))
#define WANT_TO_PATCH_DLDI (*(((u32*)LCDC_BANK_C) + 3))

#define STORED_FILE_CLUSTER_OFFSET 4 
#define INIT_DISC_OFFSET 8
#define WANT_TO_PATCH_DLDI_OFFSET 12




#define FIX_ALL	0x01
#define FIX_GLUE	0x02
#define FIX_GOT	0x04
#define FIX_BSS	0x08

enum DldiOffsets {
	DO_magicString = 0x00,			// "\xED\xA5\x8D\xBF Chishm"
	DO_magicToken = 0x00,			// 0xBF8DA5ED
	DO_magicShortString = 0x04,		// " Chishm"
	DO_version = 0x0C,
	DO_driverSize = 0x0D,
	DO_fixSections = 0x0E,
	DO_allocatedSpace = 0x0F,

	DO_friendlyName = 0x10,

	DO_text_start = 0x40,			// Data start
	DO_data_end = 0x44,				// Data end
	DO_glue_start = 0x48,			// Interworking glue start	-- Needs address fixing
	DO_glue_end = 0x4C,				// Interworking glue end
	DO_got_start = 0x50,			// GOT start					-- Needs address fixing
	DO_got_end = 0x54,				// GOT end
	DO_bss_start = 0x58,			// bss start					-- Needs setting to zero
	DO_bss_end = 0x5C,				// bss end

	// IO_INTERFACE data
	DO_ioType = 0x60,
	DO_features = 0x64,
	DO_startup = 0x68,	
	DO_isInserted = 0x6C,	
	DO_readSectors = 0x70,	
	DO_writeSectors = 0x74,
	DO_clearStatus = 0x78,
	DO_shutdown = 0x7C,
	DO_code = 0x80
};

static addr_t readAddr (data_t *mem, addr_t offset) {
	return ((addr_t*)mem)[offset/sizeof(addr_t)];
}

static void writeAddr (data_t *mem, addr_t offset, addr_t value) {
	((addr_t*)mem)[offset/sizeof(addr_t)] = value;
}

static void vramcpy (void* dst, const void* src, int len)
{
	u16* dst16 = (u16*)dst;
	u16* src16 = (u16*)src;
	
	for ( ; len > 0; len -= 2) {
		*dst16++ = *src16++;
	}
}	

static addr_t quickFind (const data_t* data, const data_t* search, size_t dataLen, size_t searchLen) {
	const int* dataChunk = (const int*) data;
	int searchChunk = ((const int*)search)[0];
	addr_t i;
	addr_t dataChunkEnd = (addr_t)(dataLen / sizeof(int));

	for ( i = 0; i < dataChunkEnd; i++) {
		if (dataChunk[i] == searchChunk) {
			if ((i*sizeof(int) + searchLen) > dataLen) {
				return -1;
			}
			if (memcmp (&data[i*sizeof(int)], search, searchLen) == 0) {
				return i*sizeof(int);
			}
		}
	}

	return -1;
}

static const data_t dldiMagicString[] = "\xED\xA5\x8D\xBF Chishm";	// Normal DLDI file
static const data_t dldiMagicLoaderString[] = "\xEE\xA5\x8D\xBF Chishm";	// Different to a normal DLDI file
#define DEVICE_TYPE_DLDI 0x49444C44

//extern const u32 _io_dldi;

bool dldiPatchLoader(data_t *binData, u32 binSize, bool clearBSS)
{
	addr_t memOffset;			// Offset of DLDI after the file is loaded into memory
	addr_t patchOffset;			// Position of patch destination in the file
	addr_t relocationOffset;	// Value added to all offsets within the patch to fix it properly
	addr_t ddmemOffset;			// Original offset used in the DLDI file
	addr_t ddmemStart;			// Start of range that offsets can be in the DLDI file
	addr_t ddmemEnd;			// End of range that offsets can be in the DLDI file
	addr_t ddmemSize;			// Size of range that offsets can be in the DLDI file

	addr_t addrIter;

	data_t *pDH;
	data_t *pAH;

	size_t dldiFileSize = 0;
	
	// Find the DLDI reserved space in the file
	patchOffset = quickFind (binData, dldiMagicLoaderString, binSize, sizeof(dldiMagicLoaderString));

	if (patchOffset < 0) {
		// does not have a DLDI section
		printfDebugger("%s no DLDI section.");
		return false;
	}

	pDH = (data_t*)(((u32*)(&io_dldi_data)) -24);	//pDH = (data_t*)(((u32*)(&_io_dldi)) -24);
	pAH = &(binData[patchOffset]);

	if (*((u32*)(pDH + DO_ioType)) == DEVICE_TYPE_DLDI) {
		// No DLDI patch
		printfDebugger("%s DLDI section corrupted");
		return false;
	}

	if (pDH[DO_driverSize] > pAH[DO_allocatedSpace]) {
		// Not enough space for patch
		printfDebugger("%s DLDI not enough space.");
		return false;
	}
	
	dldiFileSize = 1 << pDH[DO_driverSize];

	memOffset = readAddr (pAH, DO_text_start);
	if (memOffset == 0) {
			memOffset = readAddr (pAH, DO_startup) - DO_code;
	}
	ddmemOffset = readAddr (pDH, DO_text_start);
	relocationOffset = memOffset - ddmemOffset;

	ddmemStart = readAddr (pDH, DO_text_start);
	ddmemSize = (1 << pDH[DO_driverSize]);
	ddmemEnd = ddmemStart + ddmemSize;

	// Remember how much space is actually reserved
	pDH[DO_allocatedSpace] = pAH[DO_allocatedSpace];
	// Copy the DLDI patch into the application
	vramcpy (pAH, pDH, dldiFileSize);

	// Fix the section pointers in the header
	writeAddr (pAH, DO_text_start, readAddr (pAH, DO_text_start) + relocationOffset);
	writeAddr (pAH, DO_data_end, readAddr (pAH, DO_data_end) + relocationOffset);
	writeAddr (pAH, DO_glue_start, readAddr (pAH, DO_glue_start) + relocationOffset);
	writeAddr (pAH, DO_glue_end, readAddr (pAH, DO_glue_end) + relocationOffset);
	writeAddr (pAH, DO_got_start, readAddr (pAH, DO_got_start) + relocationOffset);
	writeAddr (pAH, DO_got_end, readAddr (pAH, DO_got_end) + relocationOffset);
	writeAddr (pAH, DO_bss_start, readAddr (pAH, DO_bss_start) + relocationOffset);
	writeAddr (pAH, DO_bss_end, readAddr (pAH, DO_bss_end) + relocationOffset);
	// Fix the function pointers in the header
	writeAddr (pAH, DO_startup, readAddr (pAH, DO_startup) + relocationOffset);
	writeAddr (pAH, DO_isInserted, readAddr (pAH, DO_isInserted) + relocationOffset);
	writeAddr (pAH, DO_readSectors, readAddr (pAH, DO_readSectors) + relocationOffset);
	writeAddr (pAH, DO_writeSectors, readAddr (pAH, DO_writeSectors) + relocationOffset);
	writeAddr (pAH, DO_clearStatus, readAddr (pAH, DO_clearStatus) + relocationOffset);
	writeAddr (pAH, DO_shutdown, readAddr (pAH, DO_shutdown) + relocationOffset);

	if (pDH[DO_fixSections] & FIX_ALL) { 
		// Search through and fix pointers within the data section of the file
		for (addrIter = (readAddr(pDH, DO_text_start) - ddmemStart); addrIter < (readAddr(pDH, DO_data_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GLUE) { 
		// Search through and fix pointers within the glue section of the file
		for (addrIter = (readAddr(pDH, DO_glue_start) - ddmemStart); addrIter < (readAddr(pDH, DO_glue_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GOT) { 
		// Search through and fix pointers within the Global Offset Table section of the file
		for (addrIter = (readAddr(pDH, DO_got_start) - ddmemStart); addrIter < (readAddr(pDH, DO_got_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (clearBSS && (pDH[DO_fixSections] & FIX_BSS)) { 
		// Initialise the BSS to 0, only if the disc is being re-inited
		memset (&pAH[readAddr(pDH, DO_bss_start) - ddmemStart] , 0, readAddr(pDH, DO_bss_end) - readAddr(pDH, DO_bss_start));
	}

	return true;
}

bool runNds(const void* loader, u32 loaderSize, u32 cluster, bool initDisc, bool dldiPatchNds)
{	
	//single player:
	switch_dswnifi_mode(dswifi_idlemode);
	
//   REG_IPC_FIFO_TX = 0x23232323;
	DisableIrq(IRQ_ALL);
		
	// Direct CPU access to VRAM bank C
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_LCDC_MODE;

	// Clear VRAM
	memset (LCDC_BANK_C, 0x00, 128 * 1024);
	
	// Load the loader/patcher into the correct address
	vramcpy (LCDC_BANK_C, loader, loaderSize);
	
	// Patch the loader with a DLDI for the card
	if (!dldiPatchLoader((data_t*)LCDC_BANK_C, loaderSize, initDisc)) {
		printfDebugger("runNds(): dldiPatchLoader() fail");
		while(true) { 
			IRQVBlankWait();
		}		
//		return false;
	}
	//printfDebugger("runNds(): dldiPatchLoader() OK");	//ok so far
	
	// Set the parameters for the loader
	// STORED_FILE_CLUSTER = cluster;
	writeAddr ((data_t*) LCDC_BANK_C, STORED_FILE_CLUSTER_OFFSET, cluster);
	// INIT_DISC = initDisc;
	writeAddr ((data_t*) LCDC_BANK_C, INIT_DISC_OFFSET, initDisc);
	// WANT_TO_PATCH_DLDI = dldiPatchNds;
	writeAddr ((data_t*) LCDC_BANK_C, WANT_TO_PATCH_DLDI_OFFSET, dldiPatchNds);

	// Give the VRAM to the ARM7
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_0x06000000_ARM7;	
	// Reset into a passme loop
	REG_EXMEMCNT |= ARM7_OWNS_ROM | ARM7_OWNS_CARD;
	*((vu32*)0x027FFFFC) = 0;
	*((vu32*)0x027FFE04) = (u32)0xE59FF018;
	*((vu32*)0x027FFE24) = (u32)0x027FFE04;

	REG_IPC_FIFO_TX = 17;
	//SendMultipleWordACK(ARM7COMMAND_LOAD_DLDI, 0, 0, NULL);
	
	swiSoftReset(); 
	return true;
}

bool bootndschishms(char* filename, bool dldipatch) 
{
	DRAGON_FILE *fFile = NULL;
	// get cluster
	uint32 fCluster = 0;
	
	if(debug_FileExists((const char*)filename,49) == FT_FILE){
		fFile = DRAGON_fopen(filename, "r");	//debug_FileExists index: 49
		fCluster = (uint32)fFile->firstCluster;
		DRAGON_fclose(fFile);
		//printfDebugger("ok:%s",filename);	//ok
	}
	else{
		//file not exists
		//printfDebugger("filename:%s",filename);
	}
	
	char * BootFname = getfatfsPath((char*)"DSOrganize/resources/load.bin");
	DRAGON_FILE *bootStub = NULL;
	uint32 bootStubLen = 0;
	uint8* bootStubPtr = NULL;
	if(debug_FileExists((const char*)BootFname,50) == FT_FILE){
		bootStub = DRAGON_fopen(BootFname,"r");	//debug_FileExists index: 50
		bootStubLen = (uint32)DRAGON_flength(bootStub);
		bootStubPtr = (char *)malloc(bootStubLen);
		DRAGON_fread(bootStubPtr,1,bootStubLen,bootStub);
		DRAGON_fclose(bootStub);
		//printfDebugger("ok:%s",BootFname);	//ok
	}
	else{
		//file not exists
		//printfDebugger("MissingFile:%s",BootFname);
		while(1==1){}
	}
	
	runNds(bootStubPtr, bootStubLen, fCluster, false, dldipatch);
	return true;
}

#endif

