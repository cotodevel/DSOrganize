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
 
#ifndef _SOUNDRECORDER_INCLUDED
#define _SOUNDRECORDER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define SR_RECORDING 0
#define SR_PLAYBACK 1

#define REC_FREQ 11025
#define REC_BLOCK_SIZE 1024

typedef struct
{
	u32 length; //
	u32 frequency; //
	u32 seconds; //
	u32 channels; //
	u32 significantBits; //
	
	bool validWav; //
} WAV_INFO;

void drawTopRecordScreen();
void drawBottomRecordScreen();
void recorderStartPressed();

void recorderForward();
void recorderBack();
void seekRecordSound(int xPos);
void switchRecordingMode();

#ifdef __cplusplus
}
#endif

#endif
