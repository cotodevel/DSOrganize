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
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "fatwrapper.h"
#include "language.h"
#include "specific_shared.h"
#include "devoptab_devices.h"
#include "posixHandleTGDS.h"
#include "utilsTGDS.h"
#include <unistd.h>
#include <sys/dir.h>
#include <fcntl.h>
#include "fsfatlayerTGDSNew.h"
#include "fileHandleTGDS.h"
#include "xenofunzip.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "dldi.h"
#include "resources.h"
#include "general.h"



bool DRAGON_InitFiles()
{
	bool dldiCardSt = false;
	int ret=FS_init();
	if (ret == 0)
	{
		DRAGON_chdir("/");	//set default dir at "/" or root 
		dldiCardSt = true;	//FS Init OK
	}
	else if(ret == -1)
	{
		//FS Init error
	}
	return dldiCardSt;
}

bool DRAGON_FreeFiles()
{
	bool dldiCardSt = false;
	int ret=FS_deinit();
	if (ret == 0)
	{
		dldiCardSt = true;	//FS De-init OK
	}
	else if(ret == -1)
	{
		//FS Init error
	}
	return dldiCardSt;
}

char *DRAGON_tryingInterface()
{
	return dldi_tryingInterface();
}

int DRAGON_chdir(const char* path)	//use DRAGON_chdir("/DSOrganize"); for chdir
{
	return chdir(path);
}

//coto: assume full permision always
//fix full directory outside
int DRAGON_mkdir(const char* path)
{
	//std::string PathFix = (getDefaultDSOrganizeFolder() + string("/") + string(path));	//              /dir/path1 == mkdir OK
	int ret = mkdir(path,777);
	//printfDebugger("mkdir:%s:%d",PathFix.c_str(),ret);	//while(1==1){} inside
	return ret;
}

//FT_NONE = 0
//FT_FILE = 1
//FT_DIR = 2

int DRAGON_FileExists(const char* filename)
{
	return FAT_FileExists((char*)filename);
}

DRAGON_FILE* DRAGON_fopen(const char* path, const char* mode)
{
	DRAGON_FILE *df = (DRAGON_FILE *)safeMalloc(sizeof(DRAGON_FILE));
	
	if(!df){
		return NULL;
	}
	memset(df, 0, sizeof(DRAGON_FILE));
	
	FILE *fp = fopen(path, mode);
	
	sint32 fd = -1;
	struct fd * fdinst = NULL;
	if(fp){
		fd = fileno(fp);	//fdopen(fd,mode) => FILE *
		fdinst = fd_struct_get(fd);
	}
	else{
		clrscr();
		printf("DRAGON_fopen:the FILE could not be opened.");
		printf("%s",path);
		while(1==1){}
	}
	
	df->firstCluster = getStructFDFirstCluster(fdinst);
	df->chanceOfUnicode = 0;
	df->fp = fp;	
	df->cacheEnabled = false;
	
	return df;
}

int DRAGON_fseek(DRAGON_FILE* df, s32 offset, int origin)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// figure out how many bytes we actually have to play with
		u32 totalBytes = 0;
		
		if(df->leadCursor > df->lagCursor)
		{
			// normal subtraction to find length
			totalBytes = df->leadCursor - df->lagCursor;
		}
		else
		{
			// we've looped around
			totalBytes = CACHE_SIZE - df->lagCursor;
			totalBytes += df->leadCursor;
		}
		
		// now find out the relative seek position of the data
		int relPos = 0;
		
		switch(origin)
		{
			case SEEK_SET:
				// how many more bytes forward is it from the current position?
				relPos = offset - df->filePos;
				break;
			case SEEK_CUR:
				// this one's easy
				relPos = offset;
				break;
			case SEEK_END:
				// treat this as a SEEK_SET after figuring out how many bytes in it is
				relPos = (df->fileLen + offset) - df->filePos;
				break;
		}
		
		if(relPos == 0)
		{
			return 0;
		}
		
		if(origin == SEEK_CUR)
		{
			// we must move to where the requesting process thinks the file is first			
			fseek((FILE *)df->fp, df->filePos, SEEK_SET);
		}
		
		if(relPos < 0 || relPos > (totalBytes - SEEK_BUFFER))
		{
			// we don't have (enough) data cached here, do a manual seek, and update the data
			fseek((FILE *)df->fp, offset, origin);
			
			// make sure it's flagged to be updated
			df->needsUpdate = true;
			DRAGON_cacheLoop();
		}
		else
		{
			// easy cheese, just update the pointers to point to the new location
			df->lagCursor += relPos;
			df->filePos += relPos;
		}
		
		return 0;
	}
	else
	{
		return fseek((FILE *)df->fp, offset, origin);
	}
}

bool DRAGON_feof(DRAGON_FILE* df)
{
	if(!df)
		return true;
	if(!df->fp)
		return true;
	
	if(df->cacheEnabled)
	{
		// figure out if we ran over the buffer
		return ((df->filePos) >= (df->fileLen));
	}
	else
	{
		return feof((FILE *)df->fp);
	}
}

bool DRAGON_fclose(DRAGON_FILE* df)
{
	if(!df)
		return false;
	
	bool result = fclose((FILE *)df->fp);
	
	if(df->cacheEnabled)
	{
		free(df->cache);
		df->cache = NULL;
		
		df->cacheEnabled = false;
	}
	
	free(df);
	
	return result;
}

u32 DRAGON_fread(void* buffer, u32 size, u32 count, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled == true)
	{	
		// figure out the size of the copy
		u32 toCopy = size * count;
		
		if((df->filePos + toCopy) > df->fileLen)
		{
			// make sure we never read over the end of the file
			toCopy = df->fileLen - df->filePos;
		}
		
		if(toCopy == 0)
		{	
			// eof
			return 0;
		}
		
		// we assume we've got enough in the cache to read
		if((df->lagCursor + toCopy) > CACHE_SIZE)
		{
			// split into two parts
			u32 firstCopy = CACHE_SIZE - df->lagCursor;
			u32 secondCopy = toCopy - firstCopy;
			
			u8 *tBuffer = (u8 *)buffer;
			
			memcpy(tBuffer, df->cache + df->lagCursor, firstCopy);
			memcpy(tBuffer + firstCopy, df->cache, secondCopy);
			
			df->lagCursor = secondCopy;
			df->filePos += toCopy;
		}
		else
		{
			// one direct copy
			memcpy(buffer, df->cache + df->lagCursor, toCopy);
			
			df->lagCursor += toCopy;
			df->filePos += toCopy;
		}
		
		if(toCopy < (size * count))
		{
			// didn't copy enough, lets fill with 0 to be sure
			memset(buffer + toCopy, 0, (size * count) - toCopy);
		}
		
		// make sure it returns the actual number of elements
		return toCopy;
	}
	else
	{
		return fread(buffer, size, count, (FILE *)df->fp);
	}
}

u32 DRAGON_fwrite(const void* buffer, u32 size, u32 count, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return 0;
	}
	else
	{
		return fwrite(buffer, size, count, (FILE *)df->fp);
	}
}

u32 DRAGON_flength(DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// return length of file
		return df->fileLen;
	}
	else
	{
		u32 cPos = ftell((FILE *)df->fp);
		fseek((FILE *)df->fp, 0, SEEK_END);
		u32 aPos = ftell((FILE *)df->fp);
		fseek((FILE *)df->fp, cPos, SEEK_SET);
		
		return aPos;
	}
}

u32 DRAGON_ftell(DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return df->filePos;
	}
	else
	{
		return ftell((FILE *)df->fp);
	}
}


int DRAGON_FindFirstFile(char* lfn)
{
	return FAT_FindFirstFile(lfn);		//return FAT_FindFirstFileLFN(lfn);
}

int DRAGON_FindNextFile(char* lfn)
{
	return FAT_FindNextFile(lfn);		//return FAT_FindNextFileLFN(lfn);
}

void DRAGON_closeFind()
{
	
}

uint16 DRAGON_fgetc(DRAGON_FILE *df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// read in character
		int tChar = df->cache[df->lagCursor];
		df->lagCursor++;
		
		return tChar;
	}
	else
	{
		return fgetc((FILE *)df->fp);
	}
}

void DRAGON_fputc(uint16 c, DRAGON_FILE* df)
{
	if(!df)
		return;
	if(!df->fp)
		return;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return;
	}
	else
	{
		fputc(c, (FILE *)df->fp);
	}
}

char *DRAGON_fgets(char *tgtBuffer, int num, DRAGON_FILE* df)
{
	if(!df)
		return NULL;
	if(!df->fp)
		return NULL;
	
	if(df->chanceOfUnicode == 0)
	{
		if(df->cacheEnabled)
		{
			// nothing that's cached should require this function
			return 0;
		}
		else
		{
			return fgets(tgtBuffer, num, (FILE *)df->fp);
		}
	}
	else
	{
		if(df->cacheEnabled)
		{
			// nothing that's cached should require this function
			return 0;
		}
		else
		{
			u32 curPos = DRAGON_ftell(df);
			u32 readLength = 0;
			char *tStr = (char *)trackMalloc(num*2, "unicode fake load");
			
			readLength = DRAGON_fread(tStr,1,(num-1) * 2,df);
			memset(tgtBuffer, 0, num);
			tStr[num-1] = '\0';
			tStr[num] = '\0';
			
			if (readLength==0) 
			{ 
				// return error 
				tgtBuffer[0] = '\0';
				return NULL; 
			}
			
			u32 i = 0;
			
			while(i < (num-1))
			{
				if(tStr[(i << 1)+1] == '\n')
				{
					tgtBuffer[i] = 0;
					DRAGON_fseek(df, curPos + ((i+1) << 1), 0);
					break;
				}
				
				if(tStr[(i << 1)+1] == '\r')
				{
					tgtBuffer[i] = 0;
					
					if(tStr[(i << 1)+3] == '\n')
					{
						DRAGON_fseek(df, curPos + ((i+2) << 1), 0);
						break;				
					}
					
					DRAGON_fseek(df, curPos + ((i+1) << 1), 0);
					break;
				}
				
				tgtBuffer[i] = tStr[(i << 1)+1];
				i++;
			}
			
			trackFree(tStr);		
			return tgtBuffer;
		}
	}
}

void DRAGON_detectUnicode(DRAGON_FILE* df)
{
	char tStr[4];
	
	DRAGON_fseek(df, 0, SEEK_SET);
	DRAGON_fread(tStr, 4, 1, df);
	
	if(tStr[0] == 0 && tStr[2] == 0 && tStr[1] != 0 && tStr[3] != 0) // fairly good chance it's unicode
		df->chanceOfUnicode = 1;
	else
		df->chanceOfUnicode = 0;
	
	DRAGON_fseek(df, 0, SEEK_SET);
}

int DRAGON_fputs(const char *string, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return 0;
	}
	else
	{
		return fputs(string, (FILE *)df->fp);
	}
}

u8 DRAGON_GetFileAttributes()
{
	return FAT_GetFileAttributes();
}

u8 DRAGON_SetFileAttributes(const char* filename, u8 attributes, u8 mask)
{
	return FAT_SetFileAttributes (filename, attributes, mask);
}

void DRAGON_preserveVars()
{
	FAT_preserveVars();
}

void DRAGON_restoreVars()
{
	FAT_restoreVars();
}

bool DRAGON_GetAlias(char* alias)
{
	return FAT_GetAlias(alias);
}

int DRAGON_remove(const char* path)
{	
	return remove(path);
}

bool DRAGON_rename(const char *oldName, const char *newName)
{	
	return rename(oldName, newName);
}

u32 DRAGON_DiscType()
{
	return disc_HostType();
}

bool DRAGON_GetLongFilename(char* filename)
{
	return FAT_GetLongFilename(filename);
}

u32 DRAGON_GetFileSize()
{
	return FAT_GetFileSize();
}

u32 DRAGON_GetFileCluster()
{
	return FAT_GetFileCluster();
}

void DRAGON_enableCaching(DRAGON_FILE *df)
{	
	// fill in length
	df->fileLen = DRAGON_flength(df);

	// set up default memory space
	df->cacheEnabled = true;	
	df->cache = (u8 *)safeMalloc(CACHE_SIZE);
	
	df->needsUpdate = true;
	
	// load some actual data
	DRAGON_cacheLoop();
}

bool DRAGON_isCached(DRAGON_FILE *df)
{
	return df->cacheEnabled;
}

bool DRAGON_needsUpdate(DRAGON_FILE *df)
{
	return df->needsUpdate;
}

void DRAGON_cacheLoop()
{
	/*
	int i;
	for(i = 0;i < MAX_FILES; i++)
	{
		if(curFiles[i] != 0)
		{
			// found an open handle
			DRAGON_FILE *df = (DRAGON_FILE *)curFiles[i];
			
			// test to see if it's cached
			if(df->cacheEnabled)
			{
				if(df->needsUpdate)
				{
					df->needsUpdate = false;
					
					df->filePos = ftell((FILE *)df->fp);
					
					df->leadCursor = 0;
					df->lagCursor = 0;
				}
				
				// figure out how many bytes we actually have loaded
				u32 totalBytes = 0;
				
				if(df->leadCursor > df->lagCursor)
				{
					// normal subtraction to find length
					totalBytes = df->leadCursor - df->lagCursor;
				}
				else
				{
					// we've looped around
					totalBytes = CACHE_SIZE - df->lagCursor;
					totalBytes += df->leadCursor;
				}
				
				// make sure we haven't loaded past the end of the file
				if(df->filePos + totalBytes >= df->fileLen)
				{
					// we already have enough loaded
					continue;
				}
				
				// cached, ensure that we have enough data				
				u32 tLead = df->leadCursor;
				u32 tLag = df->lagCursor;
				u32 loadSize = 0;
				
				if(tLag > tLead)
				{
					// we have looped around the ring buffer
					u32 bufSize = (tLag - tLead); // should be CACHE_BUFFER if filled
					
					if(bufSize > CACHE_BUFFER)
					{
						// not entirely filled
						loadSize = bufSize - CACHE_BUFFER;
					}
				}
				else
				{
					// we are still in one consecutive chunk
					u32 bufSize = CACHE_SIZE - (tLead - tLag); // should be CACHE_BUFFER if filled
					
					if(bufSize > CACHE_BUFFER)
					{
						// not entirely filled
						loadSize = bufSize - CACHE_BUFFER;
					}
				}
				
				if(loadSize > 0)
				{	
					// we need to cache additional data, load it into the lead cursors position and update accordingly
					if(loadSize + tLead > CACHE_SIZE)
					{
						// special case, loops around
						u32 firstRead = CACHE_SIZE - tLead;
						u32 secondRead = loadSize - firstRead;
						
						fread(df->cache + tLead, 1, firstRead, (FILE *)df->fp);
						fread(df->cache, 1, secondRead, (FILE *)df->fp);
						
						df->leadCursor = secondRead;
					}
					else
					{
						// we can just read in
						fread(df->cache + tLead, 1, loadSize, (FILE *)df->fp);
						df->leadCursor += loadSize;
					}
				}
			}
		}
	}
	*/
}



//just used for debugging
int debug_FileExists(const char* filename, int indexSource){
	int ret =  DRAGON_FileExists(filename);

	if(ret == FT_NONE){
		if(
			(indexSource != 79)
			&&
			(indexSource != 11)
			&&
			(indexSource != 21)
			&&
			(indexSource != 81)	//config.ini creates here
			&&
			(indexSource != 13)
			&&
			(indexSource != 14)
			&&
			(indexSource != 80)	//Save Config (A button)
			&&
			(indexSource != 83)	//Save Config (A button)
			&&
			(indexSource != 43)	//browser tab (read filesystem)	/ browserChangeDir() -> populateDirList()	/ dldiFile: dldiName.dldi
			&&
			(indexSource != 44)	//browser tab (read filesystem)	/ current File properties
			&&
			(indexSource != 1)	//launchNDS(filename.ext);
			&&
			(indexSource != 49)	//launchNDS(filename.ext) -> launchNDSMethod2(char *file) -> loadFile itself
			&&
			(indexSource != 50)	//launchNDS(filename.ext) -> launchNDSMethod2(char *file) -> load.bin (bootstub)
			&&
			//(indexSource != 0)	//DLDI: launchNDS() -> launchNDSMethod2(char *file) -> <0:/dldiNameFromDldiDriver.dldi>
			//&&
			(indexSource != 1)	//DLDI patch: launchNDS() -> launchNDSMethod2(char *file) -> patchFile() -> <0:/filename.nds> (read )
			//&&
			//(indexSource != 2)	//DLDI patch: launchNDS() -> launchNDSMethod2(char *file) -> patchFile() -> <0:/filename.nds> (read + write file to update new dldi driver)
			
			/*
			&&
			(indexSource != 10)
			&&
			(indexSource != 56)
			&&
			(indexSource != 82)
			*/
		){
			printfDebugger("id:%d-missing:%s",indexSource,filename);
			//printfDebugger("%s",filename);
		}
	}
	
	return ret;
}


std::string getPathFix(){
	std::string PathLocal = std::string(getfatfsPath(""));
	PathLocal.erase(PathLocal.length()-1);
	return PathLocal;
}

std::string getDefaultDSOrganizeFolder(std::string str0){
	std::string PathFix = (string("/DSOrganize") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizePath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (PathFix + getDefaultDSOrganizeFolder(string("/")) + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeHelpFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/help") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeHelpPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("help") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeDayFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/day") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeDayPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("day") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeLangFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/lang") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeLangPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("lang") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeReminderFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/reminder") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeReminderPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("reminder") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeScribbleFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/scribble") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeScribblePath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("scribble") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeTodoFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/todo") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeTodoPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("todo") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeVcardFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/vcard") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeVcardPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("vcard") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeIconsFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/icons") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeIconsPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("icons") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeResourcesFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/resources") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeResourcesPath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("resources") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeCacheFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/cache") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeCachePath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("cache") + string("/") + str0);
	return FilePath;
}

std::string getDefaultDSOrganizeHomeFolder(std::string str0){
	std::string PathFix = (getDefaultDSOrganizeFolder("/home") + str0);
	return PathFix;
}

std::string getDefaultDSOrganizeHomePath(std::string str0){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("home") + string("/") + str0);
	return FilePath;
}

std::string getDefaultConfigPath(){
	std::string PathFix = getPathFix();
	std::string FilePath = (getDefaultDSOrganizePath(string("")) + string("config.ini"));
	return FilePath;
}