#
#			Copyright (C) 2017  Coto
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful, but
#WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
#USA
#

#TGDS1.3 compatible Makefile

#ToolchainGenericDS specific: Use Makefiles from either TGDS, or custom
export SOURCE_MAKEFILE7 = default
export SOURCE_MAKEFILE9 = custom

#Shared
include $(DEFAULT_GCC_PATH)Makefile.basenewlib

#Custom
# Project Specific
export EXECUTABLE_FNAME = dsorganize.nds
export EXECUTABLE_VERSION_HEADER =	3.1129
export EXECUTABLE_VERSION =	"$(EXECUTABLE_VERSION_HEADER)"

#The ndstool I use requires to have the elf section removed, so these rules create elf headerless- binaries.
export BINSTRIP_RULE_7 =	arm7.bin
export BINSTRIP_RULE_9 =	arm9.bin
export DIR_ARM7 = arm7
export BUILD_ARM7	=	build
export DIR_ARM9 = arm9
export BUILD_ARM9	=	build
export ELF_ARM7 = arm7.elf
export ELF_ARM9 = arm9.elf

export TARGET_LIBRARY_CRT0_FILE_7 = nds_arm_ld_crt0
export TARGET_LIBRARY_CRT0_FILE_9 = nds_arm_ld_crt0
export TARGET_LIBRARY_LINKER_FILE_7 = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/$(TARGET_LIBRARY_CRT0_FILE_7).S
export TARGET_LIBRARY_LINKER_FILE_9 = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/$(TARGET_LIBRARY_CRT0_FILE_9).S

export TARGET_LIBRARY_NAME_7 = toolchaingen7
export TARGET_LIBRARY_NAME_9 = toolchaingen9
export TARGET_LIBRARY_FILE_7	=	lib$(TARGET_LIBRARY_NAME_7).a
export TARGET_LIBRARY_FILE_9	=	lib$(TARGET_LIBRARY_NAME_9).a

#####################################################ARM7#####################################################
export DIRS_ARM7_SRC = source/	\
			source/interrupts/	\
			data/	\
			../common/
			
export DIRS_ARM7_HEADER = source/	\
			source/interrupts/	\
			data/	\
			include/	\
			../common/


#####################################################ARM9#####################################################
export DIRS_ARM9_SRC = source/openssl091c/apps/	\
			source/openssl091c/crypto/asn1/	\
			source/openssl091c/crypto/bf/	\
			source/openssl091c/crypto/bio/	\
			source/openssl091c/crypto/bn/	\
			source/openssl091c/crypto/bn/stuff/	\
			source/openssl091c/crypto/buffer/	\
			source/openssl091c/crypto/cast/	\
			source/openssl091c/crypto/comp/	\
			source/openssl091c/crypto/conf/	\
			source/openssl091c/crypto/des/	\
			source/openssl091c/crypto/dh/	\
			source/openssl091c/crypto/dsa/	\
			source/openssl091c/crypto/err/	\
			source/openssl091c/crypto/evp/	\
			source/openssl091c/crypto/hmac/	\
			source/openssl091c/crypto/idea/	\
			source/openssl091c/crypto/lhash/	\
			source/openssl091c/crypto/md/	\
			source/openssl091c/crypto/md2/	\
			source/openssl091c/crypto/md5/	\
			source/openssl091c/crypto/mdc2/	\
			source/openssl091c/crypto/objects/	\
			source/openssl091c/crypto/pem/	\
			source/openssl091c/crypto/pkcs7/	\
			source/openssl091c/crypto/rand/	\
			source/openssl091c/crypto/rc2/	\
			source/openssl091c/crypto/rc4/	\
			source/openssl091c/crypto/rc5/	\
			source/openssl091c/crypto/ripemd/	\
			source/openssl091c/crypto/rsa/	\
			source/openssl091c/crypto/sha/	\
			source/openssl091c/crypto/stack/	\
			source/openssl091c/crypto/threads/	\
			source/openssl091c/crypto/txt_db/	\
			source/openssl091c/crypto/x509/	\
			source/openssl091c/crypto/x509v3/	\
			source/openssl091c/crypto/	\
			source/openssl091c/rsaref/	\
			source/openssl091c/ssl/	\
			source/openssl091c/	\
			source/interrupts/	\
			source/gui/	\
			source/drawing/	\
			source/aac/	\
			source/aac/real/	\
			source/aac/real/asm/	\
			source/mad/	\
			source/tremor/	\
			source/mp4ff/	\
			source/mikmod/	\
			source/mikmod/drivers/	\
			source/mikmod/include/	\
			source/mikmod/loaders/	\
			source/mikmod/mmio/	\
			source/mikmod/playercode/	\
			source/libffmpegFLAC/	\
			source/emulated/	\
			source/emulated/libspc/	\
			source/emulated/api68/	\
			source/emulated/emu68/	\
			source/emulated/file68/	\
			source/emulated/io68/	\
			source/libdt/	\
			source/libfb/	\
			source/libini/	\
			source/libm_apm/	\
			source/libpicture/	\
			source/libpicture/gif/	\
			source/libpicture/jpeg/	\
			source/libpicture/png/	\
			source/libvcard/	\
			source/	\
			data/	\
			../common/
			
export DIRS_ARM9_HEADER = include/	\
			source/openssl091c/apps/	\
			source/openssl091c/crypto/asn1/	\
			source/openssl091c/crypto/bf/	\
			source/openssl091c/crypto/bio/	\
			source/openssl091c/crypto/bn/	\
			source/openssl091c/crypto/bn/stuff/	\
			source/openssl091c/crypto/buffer/	\
			source/openssl091c/crypto/cast/	\
			source/openssl091c/crypto/comp/	\
			source/openssl091c/crypto/conf/	\
			source/openssl091c/crypto/des/	\
			source/openssl091c/crypto/dh/	\
			source/openssl091c/crypto/dsa/	\
			source/openssl091c/crypto/err/	\
			source/openssl091c/crypto/evp/	\
			source/openssl091c/crypto/hmac/	\
			source/openssl091c/crypto/idea/	\
			source/openssl091c/crypto/lhash/	\
			source/openssl091c/crypto/md/	\
			source/openssl091c/crypto/md2/	\
			source/openssl091c/crypto/md5/	\
			source/openssl091c/crypto/mdc2/	\
			source/openssl091c/crypto/objects/	\
			source/openssl091c/crypto/pem/	\
			source/openssl091c/crypto/pkcs7/	\
			source/openssl091c/crypto/rand/	\
			source/openssl091c/crypto/rc2/	\
			source/openssl091c/crypto/rc4/	\
			source/openssl091c/crypto/rc5/	\
			source/openssl091c/crypto/ripemd/	\
			source/openssl091c/crypto/rsa/	\
			source/openssl091c/crypto/sha/	\
			source/openssl091c/crypto/stack/	\
			source/openssl091c/crypto/threads/	\
			source/openssl091c/crypto/txt_db/	\
			source/openssl091c/crypto/x509/	\
			source/openssl091c/crypto/x509v3/	\
			source/openssl091c/crypto/	\
			source/openssl091c/rsaref/	\
			source/openssl091c/ssl/	\
			source/openssl091c/	\
			source/gui/	\
			source/aac/pub/	\
			source/emulated/	\
			source/interrupts/	\
			source/gui/	\
			source/drawing/	\
			source/aac/	\
			source/aac/real/	\
			source/aac/real/asm/	\
			source/mad/	\
			source/tremor/	\
			source/mp4ff/	\
			source/mikmod/	\
			source/mikmod/drivers/	\
			source/mikmod/include/	\
			source/mikmod/loaders/	\
			source/mikmod/mmio/	\
			source/mikmod/playercode/	\
			source/libffmpegFLAC/	\
			source/emulated/	\
			source/emulated/libspc/	\
			source/emulated/api68/	\
			source/emulated/emu68/	\
			source/emulated/file68/	\
			source/emulated/io68/	\
			source/libdt/	\
			source/libfb/	\
			source/libini/	\
			source/libm_apm/	\
			source/libpicture/	\
			source/libpicture/gif/	\
			source/libpicture/jpeg/	\
			source/libpicture/png/	\
			source/libvcard/	\
			data/	\
			../common/

# Build Target(s)	(both processors here)
all: $(EXECUTABLE_FNAME)
#all:	debug

#ignore building this
.PHONY: $(ELF_ARM7)	$(ELF_ARM9)

#Make
compile	:
ifeq ($(SOURCE_MAKEFILE7),default)
	cp	-r	$(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_MAKEFILES_SRC)/$(DIR_ARM7)/Makefile	$(CURDIR)/$(DIR_ARM7)
endif
	$(MAKE)	-R	-C	$(DIR_ARM7)/
ifeq ($(SOURCE_MAKEFILE9),default)
	cp	-r	$(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_MAKEFILES_SRC)/$(DIR_ARM9)/Makefile	$(CURDIR)/$(DIR_ARM9)
endif
	$(MAKE)	-R	-C	$(DIR_ARM9)/

$(EXECUTABLE_FNAME)	:	compile
	-@echo 'ndstool begin'
	$(NDSTOOL)	-v	-c $@	-7  $(CURDIR)/arm7/$(BINSTRIP_RULE_7)	-e7  0x03800000	-9 $(CURDIR)/arm9/$(BINSTRIP_RULE_9) -e9  0x02000000	-o banner.bmp -b logo.bmp "DSOrganize;Shaun Taylor;Version $(EXECUTABLE_VERSION_HEADER)" 
	-@echo 'ndstool end: built: $@'
	
#---------------------------------------------------------------------------------
# Clean
	
clean:
	$(MAKE)	clean	-C	$(DIR_ARM7)/
ifeq ($(SOURCE_MAKEFILE7),default)
	-@rm -rf $(CURDIR)/$(DIR_ARM7)/Makefile
endif
	$(MAKE)	clean	-C	$(DIR_ARM9)/
ifeq ($(SOURCE_MAKEFILE9),default)
	-@rm -rf $(CURDIR)/$(DIR_ARM9)/Makefile
endif
	-@rm -fr $(EXECUTABLE_FNAME)