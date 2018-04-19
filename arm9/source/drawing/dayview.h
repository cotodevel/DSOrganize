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
 
#ifndef _DAYVIEW_INCLUDED
#define _DAYVIEW_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define DV_SIZE 1024
#define DV_COUNT 48

#define TYPE_DAILY 0
#define TYPE_WEEKLY 1
#define TYPE_MONTHLY 2
#define TYPE_ANNUALLY 3

typedef struct {
	char dayView[DV_COUNT][DV_SIZE];
	char dayPath[DV_COUNT][256];
	int  dayType[DV_COUNT];
} DV_DYNAMIC;

void saveDayView();
void drawCurDVTime();
void drawCurrentEvent();
void drawEditDayView();
void drawEditCurrentEvent();
void drawDayView();
void editDayViewAction(char c);
void resetDayView();
void dvBookMarkUp();
void dvBookMarkDown();
void loadRepeatMode();
void loadCurrentDayPlanner(char **rStr);
void loadDayPlanner(char **rStr, int tDay, int tMonth, int tYear);
bool dayViewCursorMove(int curT, int curC);
void increaseDuration();
void decreaseDuration();

extern DV_DYNAMIC *dvStruct;
extern uint16 colors[12];
extern bool dvLoaded;
extern int whatType;

#ifdef __cplusplus
}
#endif

#endif
