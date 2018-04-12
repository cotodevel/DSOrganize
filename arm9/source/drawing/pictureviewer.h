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

#ifdef __cplusplus
extern "C" {
#endif

void resetScreen();
void displayPicture(int offsetX, int offsetY, void *tmpData);
void drawPictureScreen();
void movePicture(int x, int y);
void drawZoomScreen();
void freePicture();
void clearBinName();
void setBinName(char *bN, int x, int y);
void getPrevPicture();
void getNextPicture();
void exitPictureScreen();

int progressCallBack(int type, u32 set);

#ifdef __cplusplus
}
#endif

#endif
