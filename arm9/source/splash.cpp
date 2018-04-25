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
 
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include <libfb/libcommon.h>
#include "general.h"
#include "splash.h"

uint16 topsplash[] = { 0xFFFF, 143, 43, 31, 22197, 21140, 20083, 26425, 29596, 30653, 31710, 28539, 23254, 19026, 3171, 0, 1057, 4228, 9513, 17969, 24311, 25368, 16912, 8456, 2114, 5285, 10570, 12684, 14798, 13741, 11627, 27482, 7399, 15855, 6342, // width, height, # of pallate entries, pallate entries
0x4200, 0x8410, 0x0821, 0x1042, 0x2184, 0x0408, 0x9001, 0x29A5, 0x528A, 0xA594, 0x4C29, 0x9452, 0x29A5, 0x814E, 0x4900, 0x444A, 0x0910, 0x2907, 0x624A, 0xA698, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD652, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0x35C6, 0x81CF, 0x52CC, 0xA594, 0x4A29, 0x9462, 0x94B1, 0xC5E8, 0x6BAD, 0xD65A, 0xADB5, 0x7194, 0x524A, 0xA694, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5B6B, 0x7264, 0xA594, 0x4A29, 0x8E62, 0x5ACC, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x616B, 0x520A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0xBE6C, 0x8C31, 0x3363, 0x166B, 0xADB5, 0x5A6B, 0xA1D6, 0xA514, 0x4A29, 0xAD54, 0x5A6B, 0xB5D6, 0xE259, 0x9F08, 0xD1A6, 0x616B, 0x520A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0xD12E, 0x2DA6, 0x5A6B, 0xB5D6, 0x85E4, 0x4B29, 0xAD05, 0x5A6B, 0x93E8, 0xA594, 0x4A29, 0x1C52, 0x7112, 0x520A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0xE951, 0x5A6B, 0xB5D6, 0xC4AD, 0x4A29, 0xADE5, 0x5A6B, 0x52D2, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9552, 0x5AEC, 0xB5D6, 0x78AD, 0x4829, 0xADA5, 0x5A6B, 0x52D0, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x5AA9, 0xB5D6, 0x6BAD, 0x4881, 0xADA5, 0x5A6B, 0x42EE, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0xB2A5, 0xB5D6, 0x6BAD, 0x4BE1, 0xADA5, 0x5A6B, 0xE8D6, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x8AA5, 0xB5D6, 0x6BAD, 0xCB58, 0x2DB6, 0x5A6B, 0xB6D6, 0x0769, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x23A5, 0xB556, 0x6BAD, 0x4A5A, 0x3150, 0x5A6B, 0xB5D6, 0x75AD, 0x08EC, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x2BA5, 0xB596, 0x6BAD, 0x4A5E, 0xEE52, 0x5ACB, 0xB5D6, 0x6BAD, 0x3C5B, 0x949D, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x2DA5, 0xB5D6, 0x6BAD, 0x8A5E, 0x9452, 0x73A0, 0xB516, 0x6BAD, 0xD65A, 0x69B6, 0x2905, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x2DA5, 0xB5D6, 0x6BAD, 0x8A5D, 0x9452, 0x24A5, 0x452F, 0x6BAD, 0xD65A, 0xADB5, 0x197A, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x2BA5, 0xB596, 0x6BAD, 0x4A5E, 0x9452, 0x29A5, 0x374A, 0x8BCD, 0xD65A, 0xADB5, 0x626B, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x25A5, 0xB556, 0x6BAD, 0x4A5A, 0x9452, 0x29A5, 0x524A, 0x3892, 0xD652, 0xADB5, 0x5A6B, 0x52D2, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x8AA5, 0xB5D6, 0x6BAD, 0x4A5C, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD83B, 0xADB5, 0x5A6B, 0xB2D9, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0xB2A5, 0xB5D6, 0x6BAD, 0x4AE1, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4629, 0xADD5, 0x5A6B, 0x82D7, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9452, 0x5AA9, 0xB5D6, 0x6BAD, 0x4A41, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0xAD05, 0x5A6B, 0xE2D7, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0x9552, 0x5A2B, 0xB5D6, 0x77AD, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4B29, 0xAD15, 0x5A6B, 0x32D7, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x3974, 0x524A, 0xA594, 0x4A29, 0xE958, 0x5A6B, 0xB5D6, 0xC7AD, 0x4829, 0x9472, 0x29A5, 0x524A, 0xA594, 0x4929, 0xAD75, 0x5A6B, 0x02D6, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0xD974, 0x524A, 0x8494, 0xC520, 0x2D76, 0x5A6B, 0xB5D6, 0xE5E8, 0x7729, 0x86EC, 0x2104, 0x520A, 0xA594, 0xEE20, 0xADB5, 0x5A6B, 0x42DC, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x9D6C, 0x6BAD, 0xDC59, 0x166B, 0xADB5, 0x5A6B, 0xA4D6, 0xA590, 0x7629, 0xADC5, 0xAF8A, 0x9E35, 0xB7CB, 0x16E5, 0xADB5, 0x5B6B, 0x52B6, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD6AA, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB2A5, 0xA594, 0x7629, 0xADC5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x796D, 0x52CA, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xD652, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0x2BB6, 0x0998, 0x52CA, 0xA594, 0x7729, 0x31EA, 0x5A8B, 0xB5D6, 0x6BAD, 0xD95A, 0xDC46, 0x2907, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xDE13, 0xBDF7, 0x7BEF, 0xF7DE, 0xEFBD, 0x8048, 0x9011, 0x2985, 0x524A, 0xA594, 0x4A29, 0x0C42, 0x0A22, 0x295F, 0x4F4A, 0x8010, 0x1472, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x8951, 0x7AEF, 0x520E, 0xA594, 0xDE83, 0xBCF7, 0x2951, 0x524A, 0xA594, 0x9E80, 0xA0F4, 0x2985, 0x524A, 0xA594, 0x4A4C, 0x9452, 0xDBA5, 0x52CE, 0xA594, 0x4329, 0xED02, 0x28E5, 0xF75E, 0xEFBD, 0xDE7B, 0x95FD, 0x7BEF, 0xF7DE, 0xE1BD, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4E29, 0xC8FC, 0x1600, 0x0232, 0xA594, 0x4007, 0x0100, 0x79F9, 0x520A, 0x7297, 0xC0CB, 0xE304, 0x2911, 0x524A, 0xB194, 0x4ACA, 0x9452, 0x1EA5, 0x5252, 0xA594, 0x5E29, 0x8F02, 0x2C25, 0x0000, 0x0000, 0x3000, 0x97F2, 0x0022, 0x0000, 0x1000, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0xF12E, 0x9421, 0x29A5, 0x9101, 0xA514, 0xCA03, 0x9452, 0xC0BB, 0x528A, 0x12EF, 0x4A39, 0x9052, 0x1959, 0x524A, 0x8294, 0x4EC6, 0x9452, 0x1EA5, 0x3272, 0xA594, 0x5E29, 0x8F02, 0x2925, 0x524A, 0xA594, 0x332A, 0x97B2, 0x2923, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x424A, 0xA594, 0xA424, 0x9042, 0x29A5, 0x0C0B, 0xA580, 0xCA03, 0x9452, 0x46A5, 0x5248, 0x2561, 0x4829, 0x9452, 0x29A3, 0x5248, 0x9894, 0x5086, 0x9052, 0x1E85, 0xD253, 0xA590, 0x1E29, 0x8F02, 0x2925, 0x5248, 0xA594, 0x0026, 0x1752, 0x2923, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0x8594, 0x0E06, 0x9452, 0x29A5, 0x504A, 0x85E0, 0xCA03, 0x1452, 0x3EA5, 0x4162, 0x8574, 0x0A29, 0x9452, 0x29A4, 0x524A, 0x1996, 0xBA20, 0x9452, 0x1EA5, 0x804F, 0xA594, 0x6429, 0x0F02, 0x2925, 0x420A, 0xA594, 0x484F, 0x9752, 0x2923, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x424A, 0xA494, 0x48E8, 0x9452, 0x29A5, 0x524A, 0x0561, 0xC803, 0x9452, 0x86A5, 0x4C48, 0xA5A0, 0x4821, 0x9452, 0x2985, 0x424A, 0xE291, 0xF228, 0x9432, 0x1EA5, 0x0C4A, 0x8510, 0x1E29, 0xC712, 0x2925, 0x524A, 0xA394, 0xCAC8, 0x1752, 0x2923, 0x524A, 0xA590, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x21A5, 0x524A, 0xBB90, 0xCAC8, 0x9452, 0x29A5, 0x524A, 0x2489, 0xCA03, 0x9452, 0xC29B, 0x1C4B, 0xA590, 0x4A29, 0x9452, 0x21A4, 0x5248, 0x319F, 0x3A29, 0x1412, 0x1EA5, 0x4E4A, 0xA488, 0x5E29, 0x8F12, 0x2125, 0x424A, 0x9D94, 0x4A49, 0x9352, 0x2123, 0x424A, 0xA494, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8810, 0x08C9, 0x1052, 0x2984, 0x4208, 0xE401, 0x0006, 0x0500, 0x9139, 0x07C8, 0x8490, 0x0821, 0x1042, 0x2185, 0x4208, 0xA423, 0x0121, 0x1092, 0x8E84, 0x4848, 0x2466, 0x1E21, 0x4702, 0x2924, 0x4208, 0x1810, 0x0839, 0x1342, 0x0029, 0x0000, 0x2402, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8010, 0x0891, 0x1042, 0x2184, 0x5248, 0x2447, 0x730E, 0xBDD7, 0x2128, 0x1748, 0x8490, 0x0F21, 0xC618, 0x3931, 0x4208, 0x2474, 0x0F21, 0x9098, 0x8E84, 0x4248, 0x443E, 0x2421, 0x4702, 0x2924, 0x4208, 0x281F, 0x0821, 0x1352, 0x9332, 0xF9DE, 0x043D, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8810, 0x0AC1, 0x1042, 0x21A4, 0x420A, 0xA5A3, 0xA504, 0x9023, 0x2184, 0x0708, 0x8490, 0x0321, 0xE79C, 0x0139, 0x4D48, 0x39E7, 0x73CE, 0x9094, 0x8E84, 0x424A, 0x281F, 0x1E21, 0x4702, 0x2124, 0x4248, 0x443E, 0x0821, 0x1342, 0x2123, 0x4208, 0x8490, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x9110, 0x08C9, 0x9042, 0x2185, 0x5208, 0xE401, 0xCF03, 0x1C89, 0x21A4, 0x0E0A, 0x8490, 0x4821, 0x1452, 0x018F, 0x5408, 0x71C8, 0x628C, 0x9C0C, 0x8E84, 0x4248, 0x3890, 0x2421, 0x4702, 0x2924, 0x4408, 0x6464, 0x0A21, 0x1342, 0x2131, 0x5208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8710, 0x08CA, 0x1042, 0x2184, 0x4208, 0x2448, 0x8904, 0x24BC, 0x2184, 0xBC49, 0x859C, 0x0829, 0x1042, 0x098F, 0x4C08, 0x8420, 0x0821, 0xA07C, 0x8E84, 0x5248, 0xF810, 0x5E09, 0x4712, 0x2124, 0x7C48, 0xA408, 0x4821, 0x9342, 0x2131, 0x4208, 0x8510, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x884C, 0x1042, 0x2184, 0x4808, 0x64E4, 0xC80B, 0xE141, 0x2124, 0x4E08, 0x84A4, 0x0821, 0x1042, 0x098F, 0x1C09, 0x84EC, 0x0821, 0xC844, 0x1E84, 0x4248, 0x8810, 0xDEC8, 0x4712, 0x2124, 0x9E08, 0x849C, 0x0821, 0x1342, 0x2131, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x401E, 0x1042, 0x2184, 0x7E08, 0x84C8, 0x8804, 0x2342, 0x2120, 0x4408, 0x2466, 0x0821, 0x1042, 0x018F, 0xF408, 0x8490, 0x0821, 0xE448, 0x8EE4, 0x4248, 0x8410, 0xDE7B, 0x4712, 0x2124, 0x9823, 0x8410, 0x0821, 0x1342, 0x2131, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x7220, 0x1082, 0x2184, 0x2C0F, 0x84EC, 0xC80B, 0x1142, 0x4119, 0x4208, 0x0327, 0x0821, 0x1042, 0x09FD, 0x940F, 0x8410, 0x0821, 0x6042, 0x8E44, 0x4248, 0x8410, 0x728E, 0x4712, 0x2124, 0xF23A, 0x8410, 0x0821, 0x1342, 0x2123, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0521, 0xC094, 0xDA84, 0x8131, 0x8490, 0xC803, 0x1042, 0xC889, 0x42C8, 0x3911, 0xC80E, 0x8743, 0x3912, 0x8211, 0x8410, 0x0821, 0x0742, 0x8EA4, 0x4248, 0x8410, 0x7222, 0x0F02, 0x2024, 0x8832, 0x71C4, 0x638C, 0x13B2, 0x8C28, 0x1863, 0x23C6, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0921, 0x6719, 0xCE39, 0x7252, 0x8410, 0xC803, 0x1042, 0x4E84, 0x4246, 0x8810, 0x73C6, 0xE08C, 0x2127, 0x9224, 0x8410, 0x0821, 0x6F42, 0x1E30, 0x4248, 0x8410, 0xF221, 0x4712, 0xDE24, 0x9C73, 0x39E7, 0x72CE, 0x1312, 0xCE39, 0x9C73, 0x2FE7, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x6E42, 0x8931, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0xE320, 0x9018, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410 };

uint16 middlesplash[] = { 0xFFFF, 255, 29, 20, 28539, 29596, 25368, 21140, 15855, 17969, 19026, 23254, 26425, 13706, 10534, 7328, 9443, 11592, 14765, 13740, 7329, 12649, 7296, 6272, // width, height, # of pallate entries, pallate entries
0x4200, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x4380, 0x4621, 0x0420, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x3204, 0x8490, 0x0821, 0x1442, 0x0062, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8010, 0x2943, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x744C, 0x2104, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x0E0A, 0x9062, 0x2184, 0x4208, 0x2A11, 0x1A5B, 0x1097, 0x2184, 0x4208, 0xC310, 0x0210, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x6608, 0x0821, 0x1042, 0x2184, 0xD612, 0x6B2D, 0xD65A, 0xB1B5, 0x7149, 0x4208, 0x8410, 0x0A21, 0x8863, 0x0801, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x1120, 0x52CC, 0x8410, 0x0821, 0x3942, 0x622A, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xD7D4, 0x8490, 0x0821, 0x1042, 0x3085, 0x00C4, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4008, 0x9823, 0x21A4, 0x4208, 0x8410, 0xDA23, 0x2DA6, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x8AAD, 0x884B, 0x1042, 0x2184, 0x4208, 0x6298, 0x4240, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x43A0, 0x4831, 0x1042, 0x2184, 0x4708, 0x4C25, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xBDA6, 0x21C4, 0x4208, 0x8410, 0x0A21, 0x8931, 0x0801, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x1028, 0x52C6, 0x8410, 0x0821, 0x1042, 0x4B84, 0xC554, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x526C, 0xE292, 0x8410, 0x0821, 0x1042, 0x2884, 0x24CE, 0x2104, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x040A, 0x1873, 0x2184, 0x4208, 0x8410, 0x1C21, 0xB196, 0x5A8B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB6D6, 0xA931, 0x0871, 0x1042, 0x2184, 0x4208, 0xA610, 0xC419, 0x8480, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x2350, 0xC58C, 0x0821, 0x1042, 0x2184, 0x4208, 0x2D39, 0x1663, 0xADB5, 0x5A6B, 0xB8D6, 0x6B2D, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD85A, 0xA5A6, 0x21E4, 0x4208, 0x8410, 0x0821, 0x1442, 0x39C3, 0x00D0, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4008, 0x1C81, 0x2966, 0x4208, 0x8410, 0x0821, 0x1042, 0x4B8E, 0xC554, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0x05D7, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD75A, 0xAD05, 0x626B, 0x979A, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0xC314, 0xC419, 0x0480, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x2310, 0x668C, 0x0829, 0x1042, 0x2184, 0x4208, 0x8410, 0xD223, 0x31D5, 0x5A6B, 0xB8D6, 0x6B2D, 0xE05A, 0xADB5, 0x846B, 0xB516, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x70AD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x8CAD, 0x5254, 0x10F2, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x18C6, 0x71CE, 0x0121, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2884, 0x8E40, 0x8C71, 0x21C6, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x961C, 0x8CA9, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD682, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xAEB5, 0x5C10, 0x0817, 0x6B2D, 0xD65A, 0xB1B5, 0x6C8A, 0xE25C, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x8C21, 0x8C31, 0x3AE7, 0x8010, 0x2104, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x1008, 0x0884, 0x18E7, 0x63CC, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x1C21, 0xB5F4, 0x5A4C, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x826B, 0xB5D6, 0x6BAD, 0x1684, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x5A10, 0xB8D6, 0x8A31, 0x5254, 0x10E2, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8510, 0x8C31, 0x8C31, 0x3963, 0x73CE, 0x08A1, 0x1042, 0x2084, 0x0801, 0x1042, 0x2184, 0x4208, 0x8410, 0x4220, 0x8410, 0x0821, 0xCE41, 0x9C73, 0x1863, 0x63C6, 0xA418, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x981E, 0x4CA9, 0x2064, 0xADB5, 0x8470, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x5C10, 0xB516, 0x70AD, 0xD65A, 0xC1B5, 0x528C, 0x149B, 0xC4B9, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8E10, 0x6272, 0x29D5, 0x5A8C, 0xB8E0, 0x102E, 0xE082, 0xADB5, 0x846B, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0x165C, 0xADB5, 0x846B, 0xB816, 0x0B2E, 0x1883, 0x2AA5, 0x8B31, 0xE29E, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x1E21, 0xC6E7, 0x5231, 0xC694, 0x0B42, 0xD65A, 0x2EB8, 0x5A10, 0xB5D6, 0x70AD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB8D6, 0x0B2E, 0xD65A, 0xC1B5, 0x5A6B, 0xB5D6, 0x6BAD, 0x1684, 0xC2B5, 0x5A10, 0xB5D6, 0x6CAD, 0x1863, 0x29A5, 0x8C4A, 0x1863, 0xCFBD, 0x9E7B, 0xBCE7, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x7384, 0xF7DC, 0xCF3D, 0x637C, 0xC518, 0x524A, 0xC694, 0x9031, 0xD65A, 0xADB5, 0x5A70, 0x05D7, 0x0BAE, 0x165C, 0x2DB8, 0x5A6B, 0x05D7, 0x6BAD, 0xD65A, 0xADB5, 0x5A70, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0x965C, 0xAEB5, 0x5A0B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5C6B, 0xB516, 0x6BAD, 0x215C, 0x2D08, 0x846B, 0xB516, 0x6BAD, 0xD682, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD682, 0x41B8, 0x846B, 0x0821, 0x7241, 0x2184, 0xAD05, 0x5A6B, 0xB5D6, 0x6BAD, 0xE082, 0xADB5, 0x5C6B, 0xB516, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x846B, 0xB516, 0x6BAD, 0xE15A, 0xAE05, 0x844B, 0xB520, 0x6BAD, 0xD65A, 0xAEB5, 0x8410, 0xB596, 0x6BAD, 0xD65A, 0x2DB8, 0x5A6B, 0xB5D6, 0x10AE, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD75A, 0xAD05, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0x2DB8, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x10AE, 0xA184, 0xAE05, 0x8410, 0x0521, 0x6BC9, 0xA184, 0xC905, 0x846B, 0x0825, 0x1042, 0x2184, 0x4208, 0x5A0B, 0xB9E4, 0x102E, 0xA184, 0x4208, 0x5A50, 0xB5D6, 0x6BAD, 0xD65A, 0x2DB9, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0x42B8, 0x5A0B, 0x25D7, 0x70AD, 0xD65A, 0xADB5, 0x846B, 0x0821, 0x1042, 0x2184, 0x4208, 0x5A10, 0xB5D6, 0x10C2, 0xD75A, 0x4208, 0x5A4B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x5C10, 0x0821, 0x4B42, 0xD692, 0xAEB5, 0x5A10, 0xB5D6, 0x72AD, 0xD75A, 0x4209, 0x5A52, 0xB5D6, 0x6BAD, 0xD65A, 0xC2B5, 0x8450, 0x0925, 0x6B41, 0xE45A, 0xADB5, 0x846B, 0x0821, 0x1042, 0xA184, 0x4228, 0x8410, 0x0821, 0x1042, 0x2194, 0x4128, 0x8472, 0x0825, 0x6B41, 0xA184, 0x2D09, 0x846B, 0xB5A0, 0x6BC9, 0xD692, 0xADB5, 0x946B, 0x0525, 0x6BAD, 0x965C, 0x2DB9, 0x846B, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0x2184, 0x4208, 0x8410, 0x0821, 0x1042, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xD65A, 0xADB5, 0x5A6B, 0xB5D6, 0x6BAD, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xA594, 0x4A29, 0x9452, 0x29A5, 0x524A, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE, 0xE79C, 0xCE39, 0x9C73, 0x39E7, 0x73CE };

uint16 bottomsplash[] = { 0xFFFF, 0, 88, 13, 6272, 5248, 5216, 4192, 4160, 3168, 3136, 2112, 2080, 1056, 1024, 0, 2114, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1101, 0x1111, 0x2222, 0x2222, 0x3323, 0x3333, 0x4544, 0x6666, 0x6666, 0x6766, 0x7877, 0x8888, 0x8888, 0x9999, 0x9999, 0x9999, 0xAAAA, 0xBBBB, 0xBBBB, 0xBBBB, 0x00C0 };

void drawStartSplash()
{
	#ifndef DEBUG_MODE
	// first, display the dso graphic
	fb_dispSprite(32, 22, topsplash, 29596);
	
	// now, display the second graphic
	fb_dispSprite(0, 73, middlesplash, 29596);
	
	// now display and tile the third graphic
	fb_dispSprite(0, 103, bottomsplash, 31775);
	
	uint16 *fb = fb_backBuffer();
	
	for(int y = 103; y < 192; y++)
	{
		for(int x = 1; x < 256; x++)
		{
			fb[(y * 256) + x] = fb[y * 256];
		}
	}
	#endif
}
