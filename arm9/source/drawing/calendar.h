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
 
#ifndef _CALENDAR_INCLUDED
#define _CALENDAR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define REM_SIZE 1024
#define MAX_LINES_REMINDER 7

void drawCalendar();
int getDayFromTouch(int tx, int ty);
void drawReminders();
void drawReminder();
void drawEditReminder();
void saveReminder();
void editReminderAction(char c);
void resetCalLoaded();
bool isReminderEnd(int c);
void loadCurrentReminder(char **rStr);
void destroyReminder();

extern char *reminder;
extern char *dayView;
extern uint16 reminders[32];
extern uint16 dayViews[32];
extern uint16 curDay;
extern uint16 curMonth;
extern uint16 curYear;
extern uint16 curTime;
extern uint16 oldYear;
extern uint16 oldDay;
extern uint16 oldMonth;
extern bool calLoaded;

#ifdef __cplusplus
}
#endif

#endif
