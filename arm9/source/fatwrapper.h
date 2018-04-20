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
 
#ifndef _FATWRAPPER_INCLUDED
#define _FATWRAPPER_INCLUDED

#ifdef __cplusplus
using namespace std;
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#endif

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "specific_shared.h"
#include "videoTGDS.h"
#include "consoleTGDS.h"
#include "fsfatlayerTGDSNew.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//FT_NONE = 0
//FT_FILE = 1
//FT_DIR = 2
#define FT_NONE (int)(0)
#define FT_FILE (int)(1)
#define FT_DIR (int)(2)

typedef struct {
	FILE * fp;	// int fd = fileno(fp); => fdinst = fd_struct_get(fd); == struct fd * fdinst (internal handle of FILE *)
	u32 firstCluster;
	u32 chanceOfUnicode;
	
	// for file caching	//coto: todo: revise file cache if segfaults or something
	bool cacheEnabled;
	bool needsUpdate;
	u8 *cache;
	u32 filePos;
	u32 fileLen;
	u32 lagCursor;
	u32 leadCursor;	
} DRAGON_FILE;

/*
enum {
	FE_NONE, 
	FE_FILE, 
	FE_DIR
};
*/

#ifndef EOF
#define EOF -1
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

#ifndef ATTRIB_ARCH
#define ATTRIB_ARCH	0x20			// Archive
#define ATTRIB_DIR	0x10			// Directory
#define ATTRIB_LFN	0x0F			// Long file name
#define ATTRIB_VOL	0x08			// Volume
#define ATTRIB_SYS	0x04			// System
#define ATTRIB_HID	0x02			// Hidden
#define ATTRIB_RO	0x01			// Read only
#endif

#define MAX_FILES 4 				// to match the max files in gba_nds_fat
#define CACHE_SIZE (64 * 1024) // 512 kb
#define CACHE_BUFFER 1024 // 1 kb buffer between the lead and lag
#define SEEK_BUFFER (16 * 1024) // 16 kb of data left after seeking


#ifdef __cplusplus
extern "C" {
#endif

extern bool			DRAGON_InitFiles();
extern bool 			DRAGON_FreeFiles();
extern u32 			DRAGON_DiscType();
extern char 			*DRAGON_tryingInterface();
extern void 			DRAGON_preserveVars();
extern void 			DRAGON_restoreVars();
extern bool 			DRAGON_GetLongFilename(char* filename);
extern u32 			DRAGON_GetFileSize();
extern u32 			DRAGON_GetFileCluster();
extern int				DRAGON_mkdir(const char* path);
extern bool			DRAGON_chdir(const char* path);
extern int 			DRAGON_remove(const char* path);
extern bool 			DRAGON_rename(const char *oldName, const char *newName);
extern int				DRAGON_FileExists(const char* filename);
extern u8 				DRAGON_GetFileAttributes();
extern u8 				DRAGON_SetFileAttributes(const char* filename, u8 attributes, u8 mask);
extern bool 			DRAGON_GetAlias(char* alias);
extern DRAGON_FILE* 	DRAGON_fopen(const char* path, const char* mode);
extern bool 			DRAGON_fclose (DRAGON_FILE* df);
extern bool 			DRAGON_feof(DRAGON_FILE* df);
extern u32 			DRAGON_fread(void* buffer, u32 size, u32 count, DRAGON_FILE* df);
extern u32 			DRAGON_fwrite(const void* buffer, u32 size, u32 count, DRAGON_FILE* df);
extern u32 			DRAGON_flength(DRAGON_FILE* df);
extern int 			DRAGON_FindFirstFile(char* lfn);
extern int 			DRAGON_FindNextFile(char* lfn);
extern void			DRAGON_closeFind();
extern uint16 			DRAGON_fgetc(DRAGON_FILE *df);
extern void 			DRAGON_fputc(uint16 c, DRAGON_FILE* df);
extern char 			*DRAGON_fgets(char *tgtBuffer, int num, DRAGON_FILE* df);
extern int 			DRAGON_fputs(const char *string, DRAGON_FILE* df);
extern int 			DRAGON_fseek(DRAGON_FILE* df, s32 offset, int origin);
extern u32 			DRAGON_ftell(DRAGON_FILE* df);
extern void 			DRAGON_detectUnicode(DRAGON_FILE* df);
extern void 			DRAGON_enableCaching(DRAGON_FILE *df);
extern void 			DRAGON_cacheLoop();
extern bool 			DRAGON_isCached(DRAGON_FILE *df);
extern bool 			DRAGON_needsUpdate(DRAGON_FILE *df);


extern int debug_FileExists(const char* filename, int indexSource);

#ifdef __cplusplus
extern std::string getPathFix();
#endif

#ifdef __cplusplus
}
#endif


#endif
