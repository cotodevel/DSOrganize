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

#define CONFIRMDELETE 5
#define LOADBIN 4
#define MAXBROWSERCYCLES 4

#define MAX_CUSTOM_LIST 8192
#define COPY_SIZE 8192

void loadFile(int which);
void alternateLoadFile(int which);
void drawFileInfoScreen();
void drawFileBrowserScreen();
void drawEditFileName();
void drawEditFileControls();
void editFileNameAction(char c);
void renameFile();
void browserForward();
void browserBack();
void toggleHidden(int which);
void browserSetUnPopulated();
void browserChangeDir(char *dir);
bool isViewable(char *fName);
void loadFileWrapper(int which);
bool setBrowserCursor(char *file);
void browserResetPos();
void deleteFAT(char *entry, bool deleteFinal);
void resetCurDir();
int getBrowserEntries();
int getMaxCursorBrowser();
void resetCopyFlags();
bool confirmDelete();
void browserSafeFileName();

// for menu
void startPullUp();
void trackPullUp(int px, int py);
bool testPullUp(int px, int py);
void destroyPullUp();
bool isPullUp();

#ifdef __cplusplus
}
#endif

#endif
