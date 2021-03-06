/*

	disc_io.c

	uniformed io-interface to work with Chishm's FAT library

	Written by MightyMax
  
	Modified by Chishm:
	2005-11-06
		* Added WAIT_CR modifications for NDS

	Modified by www.neoflash.com:
	2006-02-03
		* Added SUPPORT_* defines, comment out any of the SUPPORT_* defines in disc_io.h to remove support
		  for the given interface and stop code being linked to the binary

	    * Added support for MK2 MMC interface

		* Added disc_Cache* functions

	Modified by Chishm:
	2006-02-05
		* Added Supercard SD support

	Modified by CyteX:
	2006-02-26
		* Added EFA2 support
*/

#include "disc_io.h"
#include "gba_nds_fat.h"

#ifdef NDS
	#include <nds.h>
#endif


// Include known io-interfaces:
#include "io_mpcf.h"
#include "io_nmmc.h"
#include "io_dldi.h"
#include "io_sccf.h"
#include "io_m3cf.h"
#include "io_scsd.h"

char strInterface[30];

// Keep a pointer to the active interface
LPIO_INTERFACE active_interface = 0;

/*

	Disc Cache functions
	2006-02-03:
		Added by www.neoflash.com 

*/

#ifdef DISC_CACHE

#include <string.h>

#define CACHE_FREE 0xFFFFFFFF
	
static u8 cacheBuffer[ DISC_CACHE_COUNT * 512 ];

static struct {
	u32 sector;
	u32 dirty;
	u32 count;
} cache[ DISC_CACHE_COUNT ];

extern bool disableWriting;

static u32 disc_CacheFind(u32 sector) {
	u32 i;
	
	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		if( cache[ i ].sector == sector )
			return i;
	}
	
	return CACHE_FREE;
}

static u32 disc_CacheFindFree(void) {
	
	u32 i = 0, j;
	u32 count = -1;
	
	for( j = 0; j < DISC_CACHE_COUNT; j++ )	{

		if( cache[ j ].sector == CACHE_FREE ) {
			i = j;
			break;
		}

		if( cache[ j ].count < count ) {
			count = cache[ j ].count;
			i = j;
		}
	}
	
	if( cache[ i ].sector != CACHE_FREE && cache[i].dirty != 0 ) {
		if(!disableWriting)
		{
			active_interface->fn_WriteSectors( cache[ i ].sector, 1, &cacheBuffer[ i * 512 ] );
			/* todo: handle write error here */
		}

		cache[ i ].sector = CACHE_FREE;
		cache[ i ].dirty = 0;
		cache[ i ].count = 0;
	}

	return i;
}

void disc_CacheInit(void)	{

	u32 i;

	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		cache[ i ].sector = CACHE_FREE;
		cache[ i ].dirty = 0;
		cache[ i ].count = 0;
	}

}

bool disc_CacheFlush(void)	{

	u32 i;

	if( !active_interface )	return false;

	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		if( cache[ i ].sector != CACHE_FREE && cache[ i ].dirty != 0 )	{
			if(!disableWriting)
			{
				if( active_interface->fn_WriteSectors( cache[ i ].sector, 1, &cacheBuffer[ i * 512 ] ) == false )
					return false;
			}

			cache[ i ].dirty = 0;
		}
	}
	return true;
}

bool disc_CacheReadSector( void *buffer, u32 sector) {
	u32 i = disc_CacheFind( sector );
	if( i == CACHE_FREE ) {
		i = disc_CacheFindFree();
		cache[ i ].sector = sector;
		if( active_interface->fn_ReadSectors( sector, 1, &cacheBuffer[ i * 512 ] ) == false )
			return false;
	}
#ifdef DISK_CACHE_DMA
		DMA3_SRC = (u32)&cacheBuffer[ i * 512 ]
		DMA3_DEST = (u32)buffer;
		DMA3_CR = 128 | DMA_COPY_WORDS;
#else
	memcpy( buffer, &cacheBuffer[ i * 512 ], 512 );
#endif
	cache[ i ].count++;
	return true;
}

bool disc_CacheWriteSector( void *buffer, u32 sector ) {
	u32 i = disc_CacheFind( sector );
	if( i == CACHE_FREE ) {
		i = disc_CacheFindFree();
		cache [ i ].sector = sector;
	}
#ifdef DISK_CACHE_DMA
		DMA3_SRC = (u32)buffer;
		DMA3_DEST = (u32)&cacheBuffer[ i * 512 ];
		DMA3_CR = 128 | DMA_COPY_WORDS;
#else
	memcpy( &cacheBuffer[ i * 512 ], buffer, 512 );
#endif
	cache[ i ].dirty=1;
	cache[ i ].count++;
	return true;
}

#endif

/*

	Hardware level disc funtions

*/

bool disc_setPatchedInterface (void)
{
	WAIT_CR &= ~ARM9_OWNS_ROM;
	WAIT_CR &= ~ARM9_OWNS_CARD; 

	// check if we have a patched card plugged in
	strcpy(strInterface, "Patched Driver");
	active_interface = DLDI_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set GBAMP as default IO
		return true ;
	} ;

	return false;
}

bool disc_setGbaSlotInterface (void)
{
	// If running on an NDS, make sure the correct CPU can access
	// the GBA cart. First implemented by SaTa.
#ifdef NDS
 #ifdef ARM9
	WAIT_CR &= ~(0x8080);
 #endif
 #ifdef ARM7
	WAIT_CR |= (0x8080);
 #endif
#endif
	
	// check if we have a GBA Movie Player plugged in
	strcpy(strInterface, "GBA Movie Player CF");
	active_interface = MPCF_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set GBAMP as default IO
		return true ;
	} ;	

	// check if we have a SuperCard CF plugged in
	strcpy(strInterface, "SuperCard CF");
	active_interface = SCCF_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set SC CF as default IO
		return true ;
	} ;
	
	// check if we have a M3 perfect CF plugged in
	strcpy(strInterface, "M3Perfect CF");
	active_interface = M3CF_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set M3 CF as default IO
		return true ;
	} ;
	
	// check if we have a SuperCard SD plugged in
	strcpy(strInterface, "SuperCard SD");
	active_interface = SCSD_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set SC SD as default IO
		return true ;
	} ;
	
	return false;
}

#ifdef NDS
// Check the DS card slot for a valid memory card interface
// If an interface is found, it is set as the default interace
// and it returns true. Otherwise the default interface is left
// untouched and it returns false.
bool disc_setDsSlotInterface (void)
{
#ifdef ARM9
	WAIT_CR &= ~(1<<11);
#endif
#ifdef ARM7
	WAIT_CR |= (1<<11);
#endif
	
	// check if we have a Neoflash MK2 / MK3 plugged in
	strcpy(strInterface, "MK2/MK3");
	active_interface = NMMC_GetInterface() ;
	if (active_interface->fn_StartUp())
	{
		// set Neoflash MK2 / MK3 as default IO
		return true ;
	} ;

	return false;
}
#endif


bool disc_Init(void) 
{
#ifdef DISC_CACHE
	disc_CacheInit();
#endif


	if (active_interface != 0) {
		return true;
	}
	
	if (disc_setPatchedInterface()) {
		return true;
	}

#ifdef NDS
	if (disc_setDsSlotInterface()) {
		return true;
	}
#endif

	if (disc_setGbaSlotInterface()) {
		return true;
	}

	// could not find a working IO Interface
	active_interface = 0 ;
	return false ;
} 

bool disc_IsInserted(void) 
{
	if (active_interface) return active_interface->fn_IsInserted() ;
	return false ;
} 

bool disc_ReadSectors(u32 sector, u8 numSecs, void* buffer) 
{
#ifdef DISC_CACHE
	u8 *p=(u8*)buffer;
	u32 i;
	u32 inumSecs=numSecs;
	if(numSecs==0)
		inumSecs=256;
	for( i = 0; i<inumSecs; i++)	{
		if( disc_CacheReadSector( &p[i*512], sector + i ) == false )
			return false;
	}
	return true;
#else
	if (active_interface) return active_interface->fn_ReadSectors(sector,numSecs,buffer) ;
	return false ;
#endif
} 

bool disc_WriteSectors(u32 sector, u8 numSecs, void* buffer) 
{
#ifdef DISC_CACHE
	u8 *p=(u8*)buffer;
	u32 i;
	u32 inumSecs=numSecs;
	if(numSecs==0)
		inumSecs=256;
	for( i = 0; i<inumSecs; i++)	{
		if( disc_CacheWriteSector( &p[i*512], sector + i ) == false )
			return false;
	}
	return true;
#else
	if (active_interface && !disableWriting) return active_interface->fn_WriteSectors(sector,numSecs,buffer) ;
	return false ;
#endif
} 

bool disc_ClearStatus(void) 
{
	if (active_interface) return active_interface->fn_ClearStatus() ;
	return false ;
} 

bool disc_Shutdown(void) 
{
#ifdef DISC_CACHE
	disc_CacheFlush();
#endif
	if (active_interface) active_interface->fn_Shutdown() ;
	active_interface = 0 ;
	return true ;
} 

u32	disc_HostType (void)
{
	if (active_interface) {
		return active_interface->ul_ioType;
	} else {
		return 0;
	}
}

