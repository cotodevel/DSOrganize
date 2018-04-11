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
#include <stdio.h>

// Font name: 'Arial' size 7
// [ ] Bold
// [ ] Italics

// Width bias: -1, -2
// Loc bias: 0, -2
// Underline: 8

static uint16 fa7_A032[] = { 0xFFFF, 2, 10, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000 };

static uint16 fa7_A033[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0010, 0x0000 };

static uint16 fa7_A034[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF4B, 0xFFFF, 0x0080 };

static uint16 fa7_A035[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xBC52, 0x52AF, 0x0080, 0x0000 };

static uint16 fa7_A036[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6875, 0xD5E2, 0x00C4, 0x0000 };

static uint16 fa7_A037[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5824, 0x0828, 0x1A14, 0x0014, 0x0000, 0x0000 };

static uint16 fa7_A038[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2439, 0x969C, 0x4027, 0x0000, 0x0000 };

static uint16 fa7_A039[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF5F, 0x00FC };

static uint16 fa7_A040[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x492A, 0x2022, 0x0000 };

static uint16 fa7_A041[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x6D77, 0x7FB5, 0x0080 };

static uint16 fa7_A042[] = { 0xFFFF, 3, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x5F1B, 0xFFFF, 0xF0FF };

static uint16 fa7_A043[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x904F, 0x0080, 0x0000 };

static uint16 fa7_A044[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0012, 0x0000 };

static uint16 fa7_A045[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0C00, 0x0000, 0x0000 };

static uint16 fa7_A046[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0010, 0x0000 };

static uint16 fa7_A047[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2525, 0x0020, 0x0000 };

static uint16 fa7_A048[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA564, 0x4B29, 0x0080, 0x0000 };

static uint16 fa7_A049[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0823, 0x1042, 0x0080, 0x0000 };

static uint16 fa7_A050[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8464, 0x2342, 0x00C0, 0x0000 };

static uint16 fa7_A051[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8464, 0x4941, 0x0080, 0x0000 };

static uint16 fa7_A052[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9511, 0x882F, 0x0040, 0x0000 };

static uint16 fa7_A053[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3972, 0x4921, 0x0080, 0x0000 };

static uint16 fa7_A054[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xB964, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A055[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x770F, 0xDEBB, 0xFFFF, 0x00FE };

static uint16 fa7_A056[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA464, 0x49C9, 0x0080, 0x0000 };

static uint16 fa7_A057[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA564, 0x4927, 0x0080, 0x0000 };

static uint16 fa7_A058[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0001, 0x0010, 0x0000 };

static uint16 fa7_A059[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0001, 0x0012, 0x0000 };

static uint16 fa7_A060[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8800, 0x0882, 0x0000, 0x0000 };

static uint16 fa7_A061[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C00, 0x000F, 0x0000, 0x0000 };

static uint16 fa7_A062[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0802, 0x2022, 0x0000, 0x0000 };

static uint16 fa7_A063[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4274, 0x0062, 0x0080, 0x0000 };

static uint16 fa7_A064[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x101E, 0xAA92, 0x4AD5, 0x78A9, 0x1E43, 0x0000, 0x0000 };

static uint16 fa7_A065[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5010, 0x27A2, 0xA0C8, 0x0080, 0x0000 };

static uint16 fa7_A066[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2471, 0x499E, 0x0027, 0x0000, 0x0000 };

static uint16 fa7_A067[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x0402, 0x8E08, 0x0000, 0x0000 };

static uint16 fa7_A068[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8978, 0x2412, 0x9E48, 0x0000, 0x0000 };

static uint16 fa7_A069[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x047D, 0x411F, 0xC007, 0x0000, 0x0000 };

static uint16 fa7_A070[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0479, 0x411C, 0x0004, 0x0000, 0x0000 };

static uint16 fa7_A071[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x6402, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A072[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8944, 0xE413, 0x9148, 0x0000, 0x0000 };

static uint16 fa7_A073[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0090, 0x0000 };

static uint16 fa7_A074[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8410, 0x4B21, 0x0080, 0x0000 };

static uint16 fa7_A075[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2545, 0x5118, 0x4024, 0x0000, 0x0000 };

static uint16 fa7_A076[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1042, 0x2184, 0x00E0, 0x0000 };

static uint16 fa7_A077[] = { 0xFFFF, 6, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x707C, 0xA5E2, 0x9B4A, 0xFF7F, 0xF8FF };

static uint16 fa7_A078[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xC944, 0xA492, 0x91C9, 0x0000, 0x0000 };

static uint16 fa7_A079[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A080[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2479, 0x419E, 0x0004, 0x0000, 0x0000 };

static uint16 fa7_A081[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x0F49, 0x0000, 0x0000 };

static uint16 fa7_A082[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8978, 0xC413, 0x9188, 0x0000, 0x0000 };

static uint16 fa7_A083[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2431, 0x090C, 0x0023, 0x0000, 0x0000 };

static uint16 fa7_A084[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xF706, 0xEFBD, 0xFF7F, 0x00FE };

static uint16 fa7_A085[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8944, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A086[] = { 0xFFFF, 6, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFA7C, 0xDDED, 0xFB7A, 0xFFFF, 0xF8FF };

static uint16 fa7_A087[] = { 0xFFFF, 8, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x3577, 0xB5AA, 0xAD5A, 0x7757, 0xFFFF, 0xFFFF, 0x00E0 };

static uint16 fa7_A088[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x6B75, 0xD5BA, 0xFFDF, 0x00FE };

static uint16 fa7_A089[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5044, 0x81A0, 0x0402, 0x0000, 0x0000 };

static uint16 fa7_A090[] = { 0xFFFF, 5, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDE03, 0xBDF7, 0x3FF0, 0xFFFF, 0x00C0 };

static uint16 fa7_A091[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2469, 0x6092, 0x0000 };

static uint16 fa7_A092[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDB6E, 0xFFB7, 0x0080 };

static uint16 fa7_A093[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDB36, 0x3F6D, 0x0080 };

static uint16 fa7_A094[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8056, 0x0000, 0x0000 };

static uint16 fa7_A095[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xF800, 0x0000 };

static uint16 fa7_A096[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF77, 0xFFFF, 0x0080 };

static uint16 fa7_A097[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C00, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A098[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xC67B, 0xB4D6, 0xFF7F, 0x00FE };

static uint16 fa7_A099[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1900, 0x4928, 0x0080, 0x0000 };

static uint16 fa7_A100[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9D10, 0x4929, 0x00C0, 0x0000 };

static uint16 fa7_A101[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1900, 0x412F, 0x00C0, 0x0000 };

static uint16 fa7_A102[] = { 0xFFFF, 3, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xE424, 0x4044, 0x0000 };

static uint16 fa7_A103[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D00, 0x4929, 0x60D2, 0x0000 };

static uint16 fa7_A104[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xC67B, 0xB5D6, 0xFFBF, 0x00FE };

static uint16 fa7_A105[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x5775, 0x00FC };

static uint16 fa7_A106[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x5575, 0x00FC };

static uint16 fa7_A107[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDA7B, 0xADB1, 0xFFBF, 0x00FE };

static uint16 fa7_A108[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x5755, 0x00FC };

static uint16 fa7_A109[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x92FC, 0x9292, 0x0092, 0x0000, 0x0000 };

static uint16 fa7_A110[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3900, 0x4A29, 0x0040, 0x0000 };

static uint16 fa7_A111[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1900, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A112[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3900, 0x4B29, 0x8090, 0x0000 };

static uint16 fa7_A113[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D00, 0x4929, 0x10C2, 0x0000 };

static uint16 fa7_A114[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xC903, 0x0020, 0x0000 };

static uint16 fa7_A115[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D00, 0x0B06, 0x0080, 0x0000 };

static uint16 fa7_A116[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4913, 0x0030, 0x0000 };

static uint16 fa7_A117[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2500, 0x4929, 0x00C0, 0x0000 };

static uint16 fa7_A118[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x5194, 0x0042, 0x0000, 0x0000 };

static uint16 fa7_A119[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0xAAAA, 0x00A5, 0x0000, 0x0000 };

static uint16 fa7_A120[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x32C6, 0x0040, 0x0000 };

static uint16 fa7_A121[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x5194, 0x0842, 0x0040, 0x0000 };

static uint16 fa7_A122[] = { 0xFFFF, 3, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xE200, 0xE048, 0x0000 };

static uint16 fa7_A123[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2869, 0x6092, 0x0000 };

static uint16 fa7_A124[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0092, 0x0000 };

static uint16 fa7_A125[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDD36, 0x3F6D, 0x0080 };

static uint16 fa7_A126[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x00AB, 0x0000, 0x0000 };

static uint16 fa7_A128[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3F7C, 0xC10F, 0x00E0, 0x0000 };

static uint16 fa7_A130[] = { 0xFFFF, 1, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x0000 };

static uint16 fa7_A131[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1C19, 0x2142, 0xC008, 0x0000 };

static uint16 fa7_A132[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x002D, 0x0000 };

static uint16 fa7_A133[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2401, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A134[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3E21, 0x1042, 0x2084, 0x0000 };

static uint16 fa7_A135[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3E21, 0x1342, 0x20E4, 0x0000 };

static uint16 fa7_A136[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0028, 0x0000, 0x0000 };

static uint16 fa7_A137[] = { 0xFFFF, 9, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2A48, 0x0005, 0x1440, 0xD28A, 0x0048, 0x0000, 0x0000 };

static uint16 fa7_A138[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2431, 0x090C, 0x0023, 0x0000, 0x0000 };

static uint16 fa7_A139[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2900, 0x0010, 0x0000 };

static uint16 fa7_A140[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA23F, 0x0811, 0x42F4, 0xFE20, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A142[] = { 0xFFFF, 5, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDE03, 0xBDF7, 0x3FF0, 0xFFFF, 0x00C0 };

static uint16 fa7_A145[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF5F, 0x00FC };

static uint16 fa7_A146[] = { 0xFFFF, 1, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF5F, 0x00FC };

static uint16 fa7_A147[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF4B, 0xFFFF, 0x0080 };

static uint16 fa7_A148[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF4B, 0xFFFF, 0x0080 };

static uint16 fa7_A149[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3600, 0x0000, 0x0000 };

static uint16 fa7_A150[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x800F, 0x0000, 0x0000 };

static uint16 fa7_A151[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xF80F, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A152[] = { 0xFFFF, 3, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x005A, 0x0000, 0x0000 };

static uint16 fa7_A153[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9678, 0x65CB, 0x0050, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A154[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D40, 0x0B06, 0x0080, 0x0000 };

static uint16 fa7_A155[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4400, 0x00A0, 0x0000 };

static uint16 fa7_A156[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8976, 0x888F, 0x0077, 0x0000, 0x0000 };

static uint16 fa7_A158[] = { 0xFFFF, 3, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xE220, 0xE048, 0x0000 };

static uint16 fa7_A159[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA244, 0x1084, 0x0041, 0x0000, 0x0000 };

static uint16 fa7_A160[] = { 0xFFFF, 2, 10, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000 };

static uint16 fa7_A161[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0401, 0x4092, 0x0000 };

static uint16 fa7_A162[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1921, 0x696C, 0x4088, 0x0000 };

static uint16 fa7_A163[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5132, 0x23C4, 0x00E0, 0x0000 };

static uint16 fa7_A164[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1E00, 0xBC94, 0x0000, 0x0000 };

static uint16 fa7_A165[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x6A75, 0x830D, 0xFF7F, 0x00FE };

static uint16 fa7_A166[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0049, 0x0092, 0x0000 };

static uint16 fa7_A167[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x6E1B, 0xD7B6, 0x8F6D, 0x00FE };

static uint16 fa7_A168[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xFF5F, 0xFFFF, 0x0080 };

static uint16 fa7_A169[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x067D, 0x196D, 0x5FB0, 0x0000, 0x0000 };

static uint16 fa7_A170[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7F10, 0xFFFF, 0x0080 };

static uint16 fa7_A171[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2855, 0x00A0, 0x0000 };

static uint16 fa7_A172[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C00, 0x0021, 0x0000, 0x0000 };

static uint16 fa7_A173[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0C00, 0x0000, 0x0000 };

static uint16 fa7_A174[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x067D, 0xDACD, 0x5FB0, 0x0000, 0x0000 };

static uint16 fa7_A175[] = { 0xFFFF, 4, 10, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A176[] = { 0xFFFF, 3, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x7075, 0x0000, 0x0000 };

static uint16 fa7_A177[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0901, 0x13F2, 0x00E0, 0x0000 };

static uint16 fa7_A178[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7F14, 0xFFFF, 0x0080 };

static uint16 fa7_A179[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7F14, 0xFFFF, 0x0080 };

static uint16 fa7_A180[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0028, 0x0000, 0x0000 };

static uint16 fa7_A181[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2500, 0x4B29, 0x80D0, 0x0000 };

static uint16 fa7_A182[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xB57E, 0x29A5, 0x004A, 0x0000 };

static uint16 fa7_A183[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0000, 0x0000 };

static uint16 fa7_A184[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x5802, 0x0000 };

static uint16 fa7_A185[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x802C, 0x0000, 0x0000 };

static uint16 fa7_A186[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7F08, 0xFFFF, 0x0080 };

static uint16 fa7_A187[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x2A45, 0x0080, 0x0000 };

static uint16 fa7_A188[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6422, 0x0828, 0x2612, 0x0042, 0x0000, 0x0000 };

static uint16 fa7_A189[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6422, 0x0828, 0x2217, 0x0047, 0x0000, 0x0000 };

static uint16 fa7_A190[] = { 0xFFFF, 7, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xBB1D, 0xF717, 0xD9ED, 0xFFDD, 0xFFFF, 0x00FF };

static uint16 fa7_A191[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x1000, 0x12C4, 0x0030, 0x0000 };

static uint16 fa7_A192[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA210, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A193[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA210, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A194[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA210, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A195[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA210, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A196[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA210, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A197[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA238, 0x7D91, 0x0018, 0x0000, 0x0000 };

static uint16 fa7_A198[] = { 0xFFFF, 8, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x941F, 0x090A, 0x84F7, 0x3E42, 0x0000, 0x0000, 0x0000 };

static uint16 fa7_A199[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1439, 0x4110, 0x8213, 0x6008, 0x0000 };

static uint16 fa7_A200[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x047D, 0x411F, 0xC007, 0x0000, 0x0000 };

static uint16 fa7_A201[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x047D, 0x411F, 0xC007, 0x0000, 0x0000 };

static uint16 fa7_A202[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x047D, 0x411F, 0xC007, 0x0000, 0x0000 };

static uint16 fa7_A203[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x047D, 0x411F, 0xC007, 0x0000, 0x0000 };

static uint16 fa7_A204[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0090, 0x0000 };

static uint16 fa7_A205[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0090, 0x0000 };

static uint16 fa7_A206[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0090, 0x0000 };

static uint16 fa7_A207[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2449, 0x0090, 0x0000 };

static uint16 fa7_A208[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1479, 0x457D, 0x8017, 0x0000, 0x0000 };

static uint16 fa7_A209[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9645, 0x4D55, 0x4034, 0x0000, 0x0000 };

static uint16 fa7_A210[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A211[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A212[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A213[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A214[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8938, 0x2412, 0x8E48, 0x0000, 0x0000 };

static uint16 fa7_A215[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5C04, 0x4447, 0x0000, 0x0000 };

static uint16 fa7_A216[] = { 0xFFFF, 6, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x913C, 0xA632, 0x9E44, 0x0000, 0x0000 };

static uint16 fa7_A217[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1445, 0x4551, 0x8013, 0x0000, 0x0000 };

static uint16 fa7_A218[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1445, 0x4551, 0x8013, 0x0000, 0x0000 };

static uint16 fa7_A219[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1445, 0x4551, 0x8013, 0x0000, 0x0000 };

static uint16 fa7_A220[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1445, 0x4551, 0x8013, 0x0000, 0x0000 };

static uint16 fa7_A221[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA244, 0x1084, 0x0041, 0x0000, 0x0000 };

static uint16 fa7_A222[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xE441, 0x4551, 0x00E4, 0x0000, 0x0000 };

static uint16 fa7_A223[] = { 0xFFFF, 5, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4521, 0x5918, 0x00A5, 0x0000, 0x0000 };

static uint16 fa7_A224[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C20, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A225[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C40, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A226[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C50, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A227[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xC35F, 0xB4D8, 0xFF3F, 0x00FE };

static uint16 fa7_A228[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C50, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A229[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3C70, 0x4B27, 0x00C0, 0x0000 };

static uint16 fa7_A230[] = { 0xFFFF, 7, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x12EC, 0x907E, 0x00EE, 0x0000, 0x0000 };

static uint16 fa7_A231[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1900, 0x4928, 0x2384, 0x0000 };

static uint16 fa7_A232[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1920, 0x412F, 0x00C0, 0x0000 };

static uint16 fa7_A233[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1940, 0x412F, 0x00C0, 0x0000 };

static uint16 fa7_A234[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1950, 0x412F, 0x00C0, 0x0000 };

static uint16 fa7_A235[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xE65F, 0xBED0, 0xFF3F, 0x00FE };

static uint16 fa7_A236[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4942, 0x0020, 0x0000 };

static uint16 fa7_A237[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4902, 0x0020, 0x0000 };

static uint16 fa7_A238[] = { 0xFFFF, 2, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xB65D, 0xFFDF, 0x0080 };

static uint16 fa7_A239[] = { 0xFFFF, 2, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4942, 0x0020, 0x0000 };

static uint16 fa7_A240[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D61, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A241[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xC65F, 0xB5D6, 0xFFBF, 0x00FE };

static uint16 fa7_A242[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1920, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A243[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1940, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A244[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1950, 0x4929, 0x0080, 0x0000 };

static uint16 fa7_A245[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xE65F, 0xB6D6, 0xFF7F, 0x00FE };

static uint16 fa7_A246[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xE65F, 0xB6D6, 0xFF7F, 0x00FE };

static uint16 fa7_A247[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x800F, 0x0080, 0x0000 };

static uint16 fa7_A248[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1D00, 0x6B6D, 0x0080, 0x0000 };

static uint16 fa7_A249[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2520, 0x4929, 0x00C0, 0x0000 };

static uint16 fa7_A250[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2540, 0x4929, 0x00C0, 0x0000 };

static uint16 fa7_A251[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDA5F, 0xB6D6, 0xFF3F, 0x00FE };

static uint16 fa7_A252[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2550, 0x4929, 0x00C0, 0x0000 };

static uint16 fa7_A253[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2220, 0x28A5, 0x4084, 0x0000 };

static uint16 fa7_A254[] = { 0xFFFF, 4, 10, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xC67B, 0xB4D6, 0x7F6F, 0x00FE };

static uint16 fa7_A255[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2250, 0x28A5, 0x4084, 0x0000 };

static uint16 fa7_TAB_STOP[] = { 25 };
static uint16 fa7_TRANSPARENT_VALUE[] = { 0 };
static uint16 fa7_UNDERLINE[] = { 8 };
static uint16 fa7_VERTBIAS[] = {  	0,		 0,		 -1,	 -1,	 0,		 -2,	 0,  	 0,		 0,		 0,		// 00
									0,		 0,		 0,		 0,		 0,		 -2,	 0,  	 0,		 0,		 0,		// 01
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
									0,		 0,		 0,		 0,		 0,		 0 };

//these next values are special characters to display buttons for the DS

uint16 fg6_AX28[] = { 0xFFFF, 8, 8, 3, 0, 16912, 32767, // width, height, # of pallate entries, pallate entries
0x5001, 0xA905, 0x6541, 0x5991, 0x5665, 0x55A5, 0x5196, 0x9065, 0x945A, 0x5001, 0x0000 };

uint16 fg6_AX29[] = { 0xFFFF, 8, 8, 3, 0, 16912, 31710, // width, height, # of pallate entries, pallate entries
0x5001, 0x5505, 0x9541, 0x5991, 0x5595, 0x5595, 0x5199, 0x9095, 0x5455, 0x5001, 0x0000 };

uint16 fg6_AX38[] = { 0xFFFF, 24, 6, 3, 0, 16912, 31710, // width, height, # of pallate entries, pallate entries
0x5515, 0x5555, 0x5555, 0xA515, 0x65A9, 0x6AA9, 0x9555, 0x6659, 0x9665, 0x5A55, 0x5A56, 0x959A, 0x5595, 0x9665, 0x6566, 0x5565, 0x65A5, 0x9699, 0x5159, 0x5555, 0x5555, 0x5055 };

uint16 fg6_AX39[] = { 0xFFFF, 11, 6, 3, 0, 16912, 32767, // width, height, # of pallate entries, pallate entries
0x1500, 0x0155, 0x5559, 0x5905, 0x1555, 0x5559, 0x5915, 0x5555, 0xA55A, 0x5555, 0x0055 };

uint16 fg6_AX40[] = { 0xFFFF, 11, 6, 3, 16912, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0A55, 0x1580, 0x2008, 0x0A05, 0x0180, 0x8008, 0x0801, 0x0020, 0x0000, 0x0000 };

static uint16 fa9_X005[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1500, 0x4155, 0x5555, 0x5945, 0x5555, 0x5595, 0x5559, 0x5955, 0x4555, 0x5559, 0x5515, 0x1554, 0x4055, 0x0000, 0x0000 };

static uint16 fa9_X008[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x5500, 0x0555, 0x5555, 0x5915, 0x5554, 0x5559, 0x5955, 0x5555, 0x5595, 0x5459, 0x5555, 0x5550, 0x0055, 0x0000, 0x0000 };

uint16* font_gautami_10[] = { 	0, 0, fg6_AX28, fg6_AX29, 0, fa9_X005, 0, 0, 0, 0,
								0, 0, 0, 0, 0, fa9_X008, 0, fg6_AX38, fg6_AX39, fg6_AX40,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								0, 0, fa7_A032, fa7_A033, fa7_A034, fa7_A035, fa7_A036, fa7_A037, fa7_A038, fa7_A039,
								fa7_A040, fa7_A041, fa7_A042, fa7_A043, fa7_A044, fa7_A045, fa7_A046, fa7_A047, fa7_A048, fa7_A049,
								fa7_A050, fa7_A051, fa7_A052, fa7_A053, fa7_A054, fa7_A055, fa7_A056, fa7_A057, fa7_A058, fa7_A059,
								fa7_A060, fa7_A061, fa7_A062, fa7_A063, fa7_A064, fa7_A065, fa7_A066, fa7_A067, fa7_A068, fa7_A069,
								fa7_A070, fa7_A071, fa7_A072, fa7_A073, fa7_A074, fa7_A075, fa7_A076, fa7_A077, fa7_A078, fa7_A079,
								fa7_A080, fa7_A081, fa7_A082, fa7_A083, fa7_A084, fa7_A085, fa7_A086, fa7_A087, fa7_A088, fa7_A089,
								fa7_A090, fa7_A091, fa7_A092, fa7_A093, fa7_A094, fa7_A095, fa7_A096, fa7_A097, fa7_A098, fa7_A099,
								fa7_A100, fa7_A101, fa7_A102, fa7_A103, fa7_A104, fa7_A105, fa7_A106, fa7_A107, fa7_A108, fa7_A109,
								fa7_A110, fa7_A111, fa7_A112, fa7_A113, fa7_A114, fa7_A115, fa7_A116, fa7_A117, fa7_A118, fa7_A119,
								fa7_A120, fa7_A121, fa7_A122, fa7_A123, fa7_A124, fa7_A125, fa7_A126, 0, fa7_A128, 0,
								fa7_A130, fa7_A131, fa7_A132, fa7_A133, fa7_A134, fa7_A135, fa7_A136, fa7_A137, fa7_A138, fa7_A139,
								fa7_A140, 0, fa7_A142, 0, 0, fa7_A145, fa7_A146, fa7_A147, fa7_A148, fa7_A149,
								fa7_A150, fa7_A151, fa7_A152, fa7_A153, fa7_A154, fa7_A155, fa7_A156, 0, fa7_A158, fa7_A159,
								fa7_A160, fa7_A161, fa7_A162, fa7_A163, fa7_A164, fa7_A165, fa7_A166, fa7_A167, fa7_A168, fa7_A169,
								fa7_A170, fa7_A171, fa7_A172, fa7_A173, fa7_A174, fa7_A175, fa7_A176, fa7_A177, fa7_A178, fa7_A179,
								fa7_A180, fa7_A181, fa7_A182, fa7_A183, fa7_A184, fa7_A185, fa7_A186, fa7_A187, fa7_A188, fa7_A189,
								fa7_A190, fa7_A191, fa7_A192, fa7_A193, fa7_A194, fa7_A195, fa7_A196, fa7_A197, fa7_A198, fa7_A199,
								fa7_A200, fa7_A201, fa7_A202, fa7_A203, fa7_A204, fa7_A205, fa7_A206, fa7_A207, fa7_A208, fa7_A209,
								fa7_A210, fa7_A211, fa7_A212, fa7_A213, fa7_A214, fa7_A215, fa7_A216, fa7_A217, fa7_A218, fa7_A219,
								fa7_A220, fa7_A221, fa7_A222, fa7_A223, fa7_A224, fa7_A225, fa7_A226, fa7_A227, fa7_A228, fa7_A229,
								fa7_A230, fa7_A231, fa7_A232, fa7_A233, fa7_A234, fa7_A235, fa7_A236, fa7_A237, fa7_A238, fa7_A239,
								fa7_A240, fa7_A241, fa7_A242, fa7_A243, fa7_A244, fa7_A245, fa7_A246, fa7_A247, fa7_A248, fa7_A249,
								fa7_A250, fa7_A251, fa7_A252, fa7_A253, fa7_A254, fa7_A255, fa7_TAB_STOP, fa7_TRANSPARENT_VALUE, fa7_UNDERLINE, (uint16*)fa7_VERTBIAS };
