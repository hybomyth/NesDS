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
export SOURCE_MAKEFILE9 = default

# Project Specific
export EXECUTABLE_FNAME = NesDS.nds
export EXECUTABLE_VERSION_HEADER =	1.3c
export EXECUTABLE_VERSION =	"$(EXECUTABLE_VERSION_HEADER)"
export GAME_TITLE	:=	nesDS
export GAME_SUBTITLE1	:=	$(EXECUTABLE_VERSION_HEADER)
export GAME_SUBTITLE2	:=	Enjoy yourself!

# TGDS Generic start
#The ndstool I use requires to have the elf section removed, so these rules create elf headerless- binaries.
export BINSTRIP_RULE_7 =	arm7.bin
export BINSTRIP_RULE_9 =	arm9.bin
export DIR_ARM7 = arm7
export BUILD_ARM7	=	build
export DIR_ARM9 = arm9
export BUILD_ARM9	=	build
export ELF_ARM7 = arm7.elf
export ELF_ARM9 = arm9.elf

# Shared
export GCC_BUILD_ENV	= $(DEFAULT_GCC_PATH)
export GCC_BIN_PATH	= bin/
export ASGCC	=	arm-none-eabi-as.exe
export GCC_C++	=	arm-none-eabi-gcc.exe
export GCC_G++	=	arm-none-eabi-gcc.exe
export GCC_OBJCOPY	=	arm-none-eabi-objcopy.exe
export CC = $(GCC_BUILD_ENV)$(GCC_BIN_PATH)$(GCC_C++)
export AS	= $(GCC_BUILD_ENV)$(GCC_BIN_PATH)$(ASGCC)
export G++	= $(GCC_BUILD_ENV)$(GCC_BIN_PATH)$(GCC_G++)
export OBJCOPY	=	$(GCC_BUILD_ENV)$(GCC_BIN_PATH)$(GCC_OBJCOPY)
export NDSTOOL	=	$(GCC_BUILD_ENV)$(GCC_BIN_PATH)ndstool.exe
export AR	=	$(GCC_BUILD_ENV)$(GCC_BIN_PATH)arm-none-eabi-ar.exe
export LD	=	$(GCC_BUILD_ENV)$(GCC_BIN_PATH)arm-none-eabi-ld.exe
export BIN2C	=	$(GCC_BUILD_ENV)$(GCC_BIN_PATH)bin2c.exe

#GCC Defs
export TARGET_LIBRARY_NAME_7 = toolchaingen7
export TARGET_LIBRARY_NAME_9 = toolchaingen9
export TOOLCHAIN_LIBRARY_SRCPATH = $(GCC_BUILD_ENV)arm-none-eabi
export TOOLCHAIN_PATH	= 	$(TOOLCHAIN_LIBRARY_SRCPATH)/lib/
export LIBPATH	=	$(TOOLCHAIN_PATH)newlib-nano-2.1-nds
export DIRLIBS_PROHIBITED =	"$(TOOLCHAIN_LIBRARY_SRCPATH)/include"
export DIRLIBS_ALLOWED =	-I "$(TOOLCHAIN_PATH)newlib-nano-2.1-nds/include/"	-I "$(TOOLCHAIN_PATH)newlib-nano-2.1-nds/newlib-nds-headers/" -I "$(TOOLCHAIN_PATH)newlib-nano-2.1-nds/newlib-nds-headers/bits/" -I "$(TOOLCHAIN_PATH)newlib-nano-2.1-nds/newlib-nds-headers/machine/" -I "$(TOOLCHAIN_PATH)newlib-nano-2.1-nds/newlib-nds-headers/sys/"
export TARGET_LIBRARY_PATH =	$(TOOLCHAIN_LIBRARY_SRCPATH)/lib/newlib-nano-2.1-nds
export LIBPATHNEWLIB	= 	$(TOOLCHAIN_LIBRARY_SRCPATH)/lib/newlib-nano-2.1-nds/
export TARGET_LIBRARY_LINKER_SRC = /linkers
export TARGET_LIBRARY_LINKER_DEST = $(TARGET_LIBRARY_PATH)
export TARGET_LIBRARY_MAKEFILES_SRC = /TGDSMakefiles
export TARGET_LIBRARY_MAKEFILES_DEST = $(TARGET_LIBRARY_PATH)
export TARGET_LIBRARY_CRT0_FILE_7 = nds_arm_ld_crt0
export TARGET_LIBRARY_CRT0_FILE_9 = nds_arm_ld_crt0
export TARGET_LIBRARY_LINKER_FILE_7 = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/$(TARGET_LIBRARY_CRT0_FILE_7).S
export TARGET_LIBRARY_LINKER_FILE_9 = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/$(TARGET_LIBRARY_CRT0_FILE_9).S
export TARGET_LIBRARY_LINKER_PATH_LD = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/
export TARGET_LIBRARY_HEADERS_7 = $(TARGET_LIBRARY_PATH)/headers7
export TARGET_LIBRARY_HEADERS_9 = $(TARGET_LIBRARY_PATH)/headers9
export TARGET_LIBRARY_HEADERS_7_INC = -I	"$(TARGET_LIBRARY_PATH)/headers7"
export TARGET_LIBRARY_HEADERS_9_INC = -I	"$(TARGET_LIBRARY_PATH)/headers9"

export TARGET_LIBRARY_FILE_7	=	lib$(TARGET_LIBRARY_NAME_7).a
export TARGET_LIBRARY_FILE_9	=	lib$(TARGET_LIBRARY_NAME_9).a
export TARGET_LIBRARY_LIBNAME_7	=	l$(TARGET_LIBRARY_NAME_7)
export TARGET_LIBRARY_LIBNAME_9	=	l$(TARGET_LIBRARY_NAME_9)

#default built-in rules removal
MAKEFLAGS += --no-builtin-rules

#cleanup default C/C++/ASM flags
export CFLAGS	=''
export CPPFLAGS	=''
export ASFLAGS	=''
export VPATH 	=''
# TGDS Generic end

#####################################################ARM7#####################################################

export DIRS_ARM7_SRC = source/	\
			source/interrupts/	\
			source/sound/	\
			../common/	\
			data/
			
export DIRS_ARM7_HEADER = ../include/	\
			source/	\
			source/interrupts/	\
			source/sound/	\
			include/	\
			../common/	\
			data/
#####################################################ARM9#####################################################

export DIRS_ARM9_SRC = source/mappers/	\
			source/	\
			source/interrupts/source/	\
			source/gui/	\
			source/wnifilib/	\
			source/cpu_misc/	\
			source/sound/	\
			source/fs_ext/	\
			../common/	\
			data/
			
export DIRS_ARM9_HEADER =	source/interrupts/include/	\
			../include/	\
			include/	\
			source/	\
			source/interrupts/	\
			source/gui/	\
			source/wnifilib/	\
			source/cpu_misc/	\
			source/sound/	\
			source/fs_ext/	\
			../common/	\
			data/

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
	$(NDSTOOL)	-v	-c $@	-7  $(CURDIR)/arm7/$(BINSTRIP_RULE_7)	-e7  0x03800000	-9 $(CURDIR)/arm9/$(BINSTRIP_RULE_9) -e9  0x02040000	-b $(CURDIR)/icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)"
	-@echo 'ndstool end: built: $@'
	
#---------------------------------------------------------------------------------
# Clean
each_obj = $(foreach dirres,$(dir_read_arm9_files),$(dirres).)
	
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