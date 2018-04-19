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
 
#ifndef _SOUNDPLAYER_INCLUDED
#define _SOUNDPLAYER_INCLUDED

#include "pls.h"
#include "id3.h"

#define TYPE_NORMAL 0
#define TYPE_PLS 1

// sound modes
#define SOUND_ONESHOT 0
#define SOUND_NEXTFILE 1
#define SOUND_RANDFILE 2

typedef struct
{
	char fileName[MAX_TGDSFILENAME_LENGTH+1];	//at least 256 bytes
	int browserLoc;
} PLAYLIST_LIST;

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void drawTopSoundScreen();
extern void drawBottomSoundScreen();
extern void togglePause();
extern void destroySound();
extern void seekSound(int xPos);
extern void switchSoundMode();
extern void getPrevSound(int mode);
extern void getNextSound(int mode);
extern void toggleHold();
extern void volumeUp();
extern void volumeDown();
extern void loadSoundMode();
extern void saveSoundMode();
extern void checkEndSound();

// for random history
extern void initRandomList();
extern void destroyRandomList();
extern void addToRandomList(char *fName, int browserLoc);
extern void setFirstTime();

extern bool soundLoaded;
extern int sampleWidth;
extern int soundMode;
extern bool plsUpdate;
extern bool hold;
extern PLS_TYPE curPlaylist;
extern int sndMode;
extern int plsPos;
extern bool firstTime;
extern bool queued;
extern ID3V1_TYPE id3Data;
extern PLAYLIST_LIST *history;
extern int h_position;
extern int h_count;
extern int h_max;

#ifdef __cplusplus
}
#endif
