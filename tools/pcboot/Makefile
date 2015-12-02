##---------------------------------------------------------------------------##
## Akalon RTOS                                                               ##
## Copyright (c) 2011-2015, Dasa Dahanayaka                                  ##
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
## Description     : Makefile for PC bootloader                              ##
## Notes           :                                                         ##
##                                                                           ##
##---------------------------------------------------------------------------##
all:
	gcc -O3 -I. -c start.S -o start.o
	gcc -Wall -O3 -nostdinc -fno-builtin -fno-stack-protector -c printf.c
	gcc -Wall -O3 -nostdinc -fno-builtin -fno-stack-protector -c main.c
	ld -nostartfiles -nostdlib -Ttext=0x7c00 -X -N start.o printf.o \
        main.o -o pcboot
	objcopy --pad-to=0x8c00 -O binary pcboot pcboot.bin
	rm -rf *.o pcboot

clean:
	rm -rf *.o pcboot pcboot.bin
