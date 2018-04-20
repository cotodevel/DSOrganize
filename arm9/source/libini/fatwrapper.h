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

//FT_NONE = 0
//FT_FILE = 1
//FT_DIR = 2
#define FT_NONE (int)(0)
#define FT_FILE (int)(1)
#define FT_DIR (int)(2)


typedef struct {
	void *fp;
	u32 firstCluster;
	u32 chanceOfUnicode;
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

#define MAX_FILES 10

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern bool			DRAGON_InitFiles();
bool 			DRAGON_FreeFiles();
u32 			DRAGON_DiscType();
char 			*DRAGON_tryingInterface();
void 			DRAGON_preserveVars();
void 			DRAGON_restoreVars();
bool 			DRAGON_GetLongFilename(char* filename);
u32 			DRAGON_GetFileSize();
u32 			DRAGON_GetFileCluster();
int				DRAGON_mkdir(const char* path);
bool			DRAGON_chdir(const char* path);
int 			DRAGON_remove(const char* path);
bool 			DRAGON_rename(const char *oldName, const char *newName);
int				DRAGON_FileExists(const char* filename);
u8 				DRAGON_GetFileAttributes();
u8 				DRAGON_SetFileAttributes(const char* filename, u8 attributes, u8 mask);
bool 			DRAGON_GetAlias(char* alias);
DRAGON_FILE* 	DRAGON_fopen(const char* path, const char* mode);
bool 			DRAGON_fclose (DRAGON_FILE* df);
bool 			DRAGON_feof(DRAGON_FILE* df);
u32 			DRAGON_fread(void* buffer, u32 size, u32 count, DRAGON_FILE* df);
u32 			DRAGON_fwrite(const void* buffer, u32 size, u32 count, DRAGON_FILE* df);
u32 			DRAGON_flength(DRAGON_FILE* df);
int 			DRAGON_FindFirstFile(char* lfn);
int 			DRAGON_FindNextFile(char* lfn);
extern void			DRAGON_closeFind();
uint16 			DRAGON_fgetc(DRAGON_FILE *df);
void 			DRAGON_fputc(uint16 c, DRAGON_FILE* df);
char 			*DRAGON_fgets(char *tgtBuffer, int num, DRAGON_FILE* df);
int 			DRAGON_fputs(const char *string, DRAGON_FILE* df);
int 			DRAGON_fseek(DRAGON_FILE* df, s32 offset, int origin);
u32 			DRAGON_ftell(DRAGON_FILE* df);
void 			DRAGON_detectUnicode(DRAGON_FILE* df);

extern int debug_FileExists(const char* filename, int indexSource);

#ifdef __cplusplus
extern std::string getPathFix();
extern std::string getDefaultDSOrganizeFolder(std::string str0);		// /d_base
extern std::string getDefaultDSOrganizePath(std::string str0);	// 0:/d_base
extern std::string getDefaultDSOrganizeHelpPath(std::string str0);
extern std::string getDefaultDSOrganizeDayPath(std::string str0);
extern std::string getDefaultDSOrganizeLangPath(std::string str0);
extern std::string getDefaultDSOrganizeLangFolder(std::string str0);
extern std::string getDefaultDSOrganizeReminderPath(std::string str0);
extern std::string getDefaultDSOrganizeScribbleFolder(std::string str0);
extern std::string getDefaultDSOrganizeScribblePath(std::string str0);
extern std::string getDefaultDSOrganizeTodoFolder(std::string str0);
extern std::string getDefaultDSOrganizeTodoPath(std::string str0);
extern std::string getDefaultDSOrganizeVcardFolder(std::string str0);
extern std::string getDefaultDSOrganizeVcardPath(std::string str0);
extern std::string getDefaultDSOrganizeIconsFolder(std::string str0);
extern std::string getDefaultDSOrganizeIconsPath(std::string str0);
extern std::string getDefaultDSOrganizeResourcesFolder(std::string str0);
extern std::string getDefaultDSOrganizeResourcesPath(std::string str0);
extern std::string getDefaultDSOrganizeCacheFolder(std::string str0);
extern std::string getDefaultDSOrganizeCachePath(std::string str0);
extern std::string getDefaultDSOrganizeHomeFolder(std::string str0);
extern std::string getDefaultDSOrganizeHomePath(std::string str0);

extern std::string getDefaultConfigPath();
#endif

#ifdef __cplusplus
}
#endif


