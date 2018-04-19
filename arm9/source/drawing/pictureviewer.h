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
 
#ifndef _PICVIEW_INCLUDED
#define _PICVIEW_INCLUDED

#include "libpicture.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void resetScreen();
extern void displayPicture(int offsetX, int offsetY, void *tmpData);
extern void drawPictureScreen();
extern void movePicture(int x, int y);
extern void drawZoomScreen();
extern void freePicture();
extern void clearBinName();
extern void setBinName(char *bN, int x, int y);
extern void getPrevPicture();
extern void getNextPicture();
extern void exitPictureScreen();
extern int progressCallBack(int type, u32 set);

extern int loc_X;
extern int loc_Y;
extern PICTURE_DATA curPicture;
extern bool picLoaded;

#ifdef __cplusplus
}
#endif

#endif
