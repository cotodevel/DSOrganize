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
 
#ifndef _ADDRESS_INCLUDED
#define _ADDRESS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

uint16 getMaxCursor();
void drawAddressList();
void drawCurrentAddress();
void drawEditAddress();
bool isNumbers(char *str);
void formatCell(int eF);
void editAddressAction(char c);

void deleteAddress();
void addressForward();
void addressBack();
void editAddressForward();
void editAddressBack();
void addressSetUnPopulated();
int getAddressEntries();

#ifdef __cplusplus
}
#endif

#endif
