# MCU type for Teensy 3.2
MCU = MK20DX256
MCU_LD = core/mk20dx256.ld

# MCU type for Teensy 3.6
#MCU = MK66FX1M0
#MCU_LD = core/mk66fx1m0.ld

# The name of your project (used to name the compiled .hex file)
TARGET = main

# Paths to Teensyduino tools
TOOLSPATH = $(TEENSY_TOOLS_PATH)
COMPILERPATH = $(TOOLSPATH)/arm/bin

# configurable options
OPTIONS = -DF_CPU=72000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH  -DUSING_MAKEFILE

# options needed by many Arduino libraries to configure for Teensy 3.0
OPTIONS += -D__$(MCU)__
#OPTIONS += -DARDUINO=10613 -DTEENSYDUINO=132

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -mcpu=cortex-m4 -mthumb -MMD $(OPTIONS) -I.

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=cortex-m4 -mthumb -T$(MCU_LD)

# additional libraries to link
LIBS = -lm

# names for the compiler programs
CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

# automatically create lists of the sources and objects
C_FILES := $(wildcard *.c) $(wildcard core/*.c)
CPP_FILES := $(wildcard *.cpp) $(wildcard core/*.cpp)
OBJS := $(C_FILES:%.c=build/%.o) $(CPP_FILES:%.cpp=build/%.o)

all: directories build/$(TARGET).hex

build/$(TARGET).elf: $(OBJS) $(MCU_LD)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

build/%.hex: build/%.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
ifneq (,$(wildcard $(TOOLSPATH)))
	$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot
endif

build/%.o: %.c
ifndef TEENSY_TOOLS_PATH
	$(error TEENSY_TOOLS_PATH is not set)
endif
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

build/%.o: %.cpp
ifndef TEENSY_TOOLS_PATH
	$(error TEENSY_TOOLS_PATH is not set)
endif
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

directories:
	mkdir -p build
	mkdir -p build/core

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	rm -rf build/*

flash: build/$(TARGET).hex
ifneq (,$(wildcard $(TOOLSPATH)))
	$(TOOLSPATH)/teensy_post_compile -file=$(TARGET) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot
endif
