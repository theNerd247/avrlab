##############################
# @file Makefile for mechlab
##############################
#
## PROJECT ##############################

SHELL=/bin/sh
PROJECT=mechlab
VERSION=0.1
MCUTYPE=atmega328p

## END PROJECT ############################
#
## DIRS ##############################
LAB=lab4
PREFIX=$(LAB)
BINDIR=$(PREFIX)/bin
IDIR=./include
SRCDIR=$(PREFIX)
LIBDIR=$(PREFIX)/lib
CLEAN=$(LIBDIR) 
REQUIRED_DIRS=$(BINDIR)

## END DIRS ############################
#
## FILES ##############################
DOXYGEN_CONFIG_FILE=$(PREFIX)/doxygen.conf
## END FILES ############################
#
## FLAGS ##############################

#external libraries
LLIBS=dl llist pthread ncurses menu m
LLIBS:=$(patsubst %, -l%, $(LLIBS))

#compiler
export CC=avr-gcc

#compile flags
export CFLAGS=-Wall -g -Os -mmcu=$(MCUTYPE) -I$(IDIR)

#link flags
export LFLAGS=-g -mmcu=$(MCUTYPE) 

SOFLAGS=-shared

## END FLAGS ############################
#
## OBJECTS ##############################

OBJS:=$(patsubst %.c, %.o, $(wildcard $(SRCDIR)/*.c))
LIBOBJS=
LIBOBJS:=$(patsubst %.o, $(SRCDIR)/%.o, $(LIBOBJS))
LIBOBJECTS=$(filter $(LIBOBJS), $(OBJS))
LIBNAME=$(PROJECT)
LIBTARGET=$(LIBDIR)/lib$(LIBNAME).so
OBJECTS:=$(filter-out $(LIBOBJECTS), $(OBJS))

## END OBJECTS ############################
#
## TARGETS ##############################
	
TARGET=$(BINDIR)/$(PROJECT)

.PHONY: all setup clean package doc

all: setup $(TARGET)

#for debuging this make file
debugmk: 
	@echo $(OBJECTS)
	@echo $(LIBOBJECTS)

#create directories that are needed before building
setup:
	@mkdir -p $(REQUIRED_DIRS)

$(LIBTARGET): $(LIBOBJECTS)
	$(CC) -o $(LIBTARGET) $(LIBOBJECTS) $(SOFLAGS) $(LFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $(TARGET).elf $(OBJECTS)
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

#perform a recursive build 
#build: $(SUBDIRS)

#perform a recursive clean
#clean: $(SUBDIRS)
clean: CLEAN += $(OBJECTS) 
clean: 
	rm -rf $(CLEAN)

doc:
	doxygen $(DOXYGEN_CONFIG_FILE)

flash: all
	sudo avrdude -c dragon_isp -p $(MCUTYPE) -U flash:w:$(TARGET).hex

sim: all
	simulavr -g -d atmega328 -F 8 &
	avr-gdb -f $(TARGET).elf

pdf: 
	cd $(PREFIX); pdflatex ./$(LAB).tex
## END TARGETS ############################
