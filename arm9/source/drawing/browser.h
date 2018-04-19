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
 
#ifndef _BROWSER_INCLUDED
#define _BROWSER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "fsfatlayerTGDSLegacy.h"
#include "filerout.h"

#define CONFIRMDELETE 5
#define LOADBIN 4
#define MAXBROWSERCYCLES 4

#define MAX_CUSTOM_LIST 8192
#define COPY_SIZE 8192

extern void loadFile(int which);
extern void alternateLoadFile(int which);
extern void drawFileInfoScreen();
extern void drawFileBrowserScreen();
extern void drawEditFileName();
extern void drawEditFileControls();
extern void editFileNameAction(char c);
extern void renameFile();
extern void browserForward();
extern void browserBack();
extern void toggleHidden(int which);
extern void browserSetUnPopulated();
extern void browserChangeDir(char *dir);
extern bool isViewable(char *fName);
extern void loadFileWrapper(int which);
extern bool setBrowserCursor(char *file);
extern void browserResetPos();
extern void deleteFAT(char *entry, bool deleteFinal);
extern void resetCurDir();
extern int getBrowserEntries();
extern int getMaxCursorBrowser();
extern void resetCopyFlags();
extern bool confirmDelete();
extern void browserSafeFileName();

// for menu
extern void startPullUp();
extern void trackPullUp(int px, int py);
extern bool testPullUp(int px, int py);
extern void destroyPullUp();
extern bool isPullUp();

//Browser opens N APPs, so the "global" fileName loads here and it's shared
extern FILE_INFO curFile;
extern uint16 browserMode;
extern char curDir[MAX_TGDSFILENAME_LENGTH+1];
extern char copyFrom[MAX_TGDSFILENAME_LENGTH+1];
extern char fileName[MAX_TGDSFILENAME_LENGTH+1];

#ifdef __cplusplus
}
#endif

#endif
