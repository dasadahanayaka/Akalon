##---------------------------------------------------------------------------##
## Akalon RTOS                                                               ##
## Copyright (c) 2011-2016, Dasa Dahanayaka                                  ##
## All rights reserved.                                                      ##
##                                                                           ##
## Usage of the works is permitted provided that this instrument is retained ##
## with the works, so that any entity that uses the works is notified of     ##
## this instrument.                                                          ##
##                                                                           ##	
## DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.                               ##
##                                                                           ##
##---------------------------------------------------------------------------##

##---------------------------------------------------------------------------##
##                                                                           ##
## File Name       : Makefile                                                ##
## Description     : Config file for Akalon's Olimex sam9-l9260 Port (arm).  ##
## Notes           :                                                         ##
##                                                                           ##
##---------------------------------------------------------------------------##

################################
### BSP Specific Information ###
################################

ARCH     = arm
CPU      = 926ejs
BSP      = sam9-l9260

BASE_DIR = ../..
BLD_DIR  = $(BASE_DIR)/boards/$(BSP)/bld

## One day, COMPILE_DIRS will come from a configuration tool
COMPILE_DIRS = kernel            \
               cpu/arm           \
               modules/cli       \
               modules/libs      \
               modules/stdio     \
               modules/tests     \
               modules/net       \
               devices           



##################################################
### Generic Build Rules ... Do NOT change !!!! ###
##################################################

# Implicit Rules...

$(BLD_DIR)/%.o: %.c $(IFILES)
	$(CC) $(CFLAGS) $(IPATH) -c $< -o $@

$(BLD_DIR)/%.o: %.s
	$(AS) $(AFLAGS) $(IPATH) -s -c $< -o $@

all: $(BLD_DIR)
	@$(foreach var,$(COMPILE_DIRS), make DIR=$(var) lib_bld;\
           if [ "$$?" -ne 0 ] ; then echo Compile ERROR !!! ; exit 1 ; fi ; )
	make DIR=boards/$(BSP) bsp_bld

clean:
	rm -rf $(BLD_DIR)/*

echo_dir:
	@echo ========================
	@echo "Compiling $(DIR)"
	@echo ========================

$(BLD_DIR):
	mkdir -p $(BLD_DIR)

ifneq ($(DIR),)
include $(BASE_DIR)/$(DIR)/src_list
VPATH = $(BASE_DIR)/inc:$(BASE_DIR)/$(DIR):$(BASE_DIR)/boards:
IPATH = -I$(BASE_DIR)/inc -I$(BASE_DIR)/$(DIR)
endif

lib_bld: echo_dir $(OBJS)
	$(AR) $(ARFLAGS) $(LIB_FILE) $(OBJS)
	rm -f $(OBJS)



##########################
### BSP Specific Rules ### 
##########################

## Tool Info
TOOL_DIR = /Set_Your_Tool_Base_Here/codesourcery-armgcc-2008q1/bin
CC       = $(TOOL_DIR)/arm-none-eabi-gcc
AR       = $(TOOL_DIR)/arm-none-eabi-ar
AS       = $(TOOL_DIR)/arm-none-eabi-gcc
LD       = $(TOOL_DIR)/arm-none-eabi-ld
BIN      = $(TOOL_DIR)/arm-none-eabi-objcopy

## Flags  

CFLAGS   = -mcpu=arm926ej-s -Os -ansi -Wall -fno-stack-protector -nostdinc -fno-builtin -nostartfiles -D$(ARCH) 
AFLAGS   = -x assembler-with-cpp
ARFLAGS	 = -rcsv

LIB_FILE = $(BLD_DIR)/libakalon-$(ARCH)-$(CPU).a
EXE_NAME = $(BLD_DIR)/akalon-$(BSP)-$(CPU)

## BSP Build Rule

bsp_bld: echo_dir $(OBJS)
	$(LD) -T sam9-l9260.ld -N $(OBJS) -nostdlib -L$(BLD_DIR) -L$(TOOL_DIR)/../lib/gcc/arm-none-eabi/4.2.3/ -o $(BLD_DIR)/tmp.o -lakalon-$(ARCH)-$(CPU) -lgcc
	$(BASE_DIR)/tools/gen_symbols $(BLD_DIR)/tmp.o $(BLD_DIR)/symbols.c
	$(CC) $(CFLAGS) $(IPATH) -c $(BLD_DIR)/symbols.c -o $(BLD_DIR)/symbols.o
	$(LD) -T sam9-l9260.ld -nostdlib -N -L$(BLD_DIR) -L$(TOOL_DIR)/../lib/gcc/arm-none-eabi/4.2.3/ $(OBJS) $(BLD_DIR)/symbols.o -o $(EXE_NAME).o -lakalon-$(ARCH)-$(CPU) -lgcc 
	$(BIN) -O binary $(EXE_NAME).o $(EXE_NAME).bin
	rm -rf $(OBJS) $(BLD_DIR)/symbols.* $(BLD_DIR)/tmp.o



