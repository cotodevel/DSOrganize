@ ***************************************************************************
@ *                                                                         *
@ *  This file is part of DSOrganize.                                       *
@ *                                                                         *
@ *  DSOrganize is free software: you can redistribute it and/or modify     *
@ *  it under the terms of the GNU General Public License as published by   *
@ *  the Free Software Foundation, either version 3 of the License, or      *
@ *  (at your option) any later version.                                    *
@ *                                                                         *
@ *  DSOrganize is distributed in the hope that it will be useful,          *
@ *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
@ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
@ *  GNU General Public License for more details.                           *
@ *                                                                         *
@ *  You should have received a copy of the GNU General Public License      *
@ *  along with DSOrganize.  If not, see <http://www.gnu.org/licenses/>.    *
@ *                                                                         *
@ ***************************************************************************

#ifdef ARM7
.text
#endif

#ifdef ARM9
.section .itcm,"ax",%progbits
#endif

.arm
.code 32

#include "dsregs_asm.h"

.global 	swiSoftReset
.type   	swiSoftReset STT_FUNC
swiSoftReset:
REG_IME = 0;
#ifdef ARM7
	ldr	r0,=0x2FFFE34
#endif

#ifdef ARM9
	.arch	armv5te
	.cpu	arm946e-s
	ldr	r1, =0x00002078			@ disable TCM and protection unit
	mcr	p15, 0, r1, c1, c0
	@ Disable cache
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0		@ Instruction cache
	mcr	p15, 0, r0, c7, c6, 0		@ Data cache

	@ Wait for write buffer to empty 
	mcr	p15, 0, r0, c7, c10, 4

	ldr	r0,=0x2FFFE24
#endif

	ldr	r0,[r0]
	bx	r0


.pool
.end