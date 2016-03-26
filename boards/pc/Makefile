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
## Description     : Makefile for Akalon's PC Port (i386).                   ##
## Notes           :                                                         ##
##                                                                           ##
##---------------------------------------------------------------------------##

################################
### BSP Specific Information ###
################################

ARCH     = ia32
CPU      = i386
BSP      = pc

BASE_DIR = ../..
BLD_DIR  = $(BASE_DIR)/boards/pc/bld

## One day, COMPILE_DIRS will come from a configuration tool
COMPILE_DIRS = kernel            \
               cpu/ia32          \
               modules/cli       \
               modules/libs      \
               modules/stdio     \
               modules/tests     \
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
TOOL_DIR = 
CC       = $(TOOL_DIR)gcc
AR       = $(TOOL_DIR)ar
AS       = $(TOOL_DIR)gcc
LD       = $(TOOL_DIR)ld
BIN      = $(TOOL_DIR)objcopy

## Flags  

CFLAGS   = -m32 -march=$(CPU) -Os -ansi -Wall -fno-stack-protector -nostdinc -fno-builtin -D$(ARCH) -DRW_MULTI_THREAD -D_REENTRANT 
AFLAGS   = -m32 -march=$(CPU) -x assembler-with-cpp
ARFLAGS	 = -rcsv

LIB_FILE = $(BLD_DIR)/libakalon-$(ARCH)-$(CPU).a
EXE_NAME = $(BLD_DIR)/akalon-$(BSP)-$(CPU)


## BSP Build Rule

ifeq ($(SIM),yes)
BSP_OBJS = $(SIM_OBJS)
CFLAGS  += -DSIM_BUILD
else
BSP_OBJS = $(OBJS)
endif


bsp_bld: echo_dir $(BSP_OBJS)
	$(LD) -nostdlib -Ttext=0x00100000 -N -L$(BLD_DIR) $(BSP_OBJS) -o $(BLD_DIR)/tmp.o -lakalon-$(ARCH)-$(CPU)
	$(BASE_DIR)/tools/gen_symbols $(BLD_DIR)/tmp.o $(BLD_DIR)/symbols.c
	$(CC) $(CFLAGS) $(IPATH) -c $(BLD_DIR)/symbols.c -o $(BLD_DIR)/symbols.o
	$(LD) -nostdlib -Ttext=0x00100000 -N -L$(BLD_DIR) $(BSP_OBJS) $(BLD_DIR)/symbols.o -o $(EXE_NAME).o -lakalon-$(ARCH)-$(CPU) 
	$(BIN) -O binary $(EXE_NAME).o $(EXE_NAME).bin
ifeq ($(SIM),yes)
	dd if=/dev/zero of=$(EXE_NAME)-sim.bin bs=1M count=1
	dd if=../../tools/pcboot/pcboot.bin of=$(EXE_NAME)-sim.bin conv=notrunc
	dd if=$(EXE_NAME).bin of=$(EXE_NAME)-sim.bin bs=512 seek=8 conv=notrunc  
	@echo "*** Made Simulator Build ***"
endif
	rm -rf $(OBJS) $(BLD_DIR)/symbols.* $(BLD_DIR)/tmp.o



