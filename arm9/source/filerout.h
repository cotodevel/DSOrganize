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
 
#ifndef _FILEROUT_INCLUDED
#define _FILEROUT_INCLUDED


#define DIRECTORY		0
#define NDSFILE			1
#define TXTFILE			2
#define VCFILE			3
#define DVFILE			4
#define REMFILE			5
#define PICFILE			6
#define BINFILE     	7
#define TODOFILE		8
#define SOUNDFILE		9
#define HBFILE			10
#define PKGFILE			11
#define PLAYLISTFILE	13
#define SHORTCUTFILE	15
#define UNKNOWN			16

#define NDS_ICON_SIZE 1024

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

typedef struct
{
	char longName[256];
	uint16 fileType;
	u32 fileSize;
	bool hidden;
	bool played;
} BROWSER_FILE;

typedef struct
{
	char internalName[256];
	char formattedName[3][256];
	uint16 iconData[1026];
	uint16 transColor;
	char longName[256];
} FILE_INFO;

typedef struct
{
	char urgency;
	char title[41];
	char message[1025];
	char fileName[256];
} TODO_FILE;

typedef struct
{
	char fileName[256];
	char shortName[64];
} SCRIBBLE_FILE;

#endif



#ifdef __cplusplus
extern "C" {
#endif

// General
extern void safeFileName(char *fName);

// vcard
extern uint16 populateList(char *dir);
extern void freeVCard();
extern int addVCard();
extern void sortList(int pos);

//browser
extern uint16 populateDirList(char *dir);
extern void freeDirList();
extern void getInfo(BROWSER_FILE *bf, char *path, FILE_INFO *file);
extern uint16 getFileType(char *fName);

//todo
extern uint16 populateTodoList(char *dir);
extern void freeTodo();
extern int addTodo();
extern void sortTodoList(int pos);
extern void clearTodo(TODO_FILE *vf);

//scribble
extern uint16 populateScribbleList(char *dir);
extern void freeScribble();
extern void sortScribbleList(int pos);

// random stuff
extern void separateDir(char *dir, char *file);
extern int nextFile(char *curFile, int numEntries);
extern int prevFile(char *curFile);
extern int randFile(char *curFile, int fileType, int numEntries);

// for pls files
extern char *getLastDir();

extern bool isScribbleFile(char *tmpFile);

#ifdef __cplusplus
}
#endif
