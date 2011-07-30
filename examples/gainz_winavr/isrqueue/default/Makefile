###############################################################################
# Makefile for the project isrqueue
###############################################################################

## General Flags
PROJECT = isrqueue
MCU = atmega128
TARGET = isrqueue.elf
CC = AVR-GCC.EXE

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=isrqueue.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = isrqueue.o hal_assert.o hal_cpu.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_led.o hal_timer.o hpl_atmega128.o rtl_lightqueue.o hal_targetboard.o rtl_debugio.o rtl_foundation.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) isrqueue.hex isrqueue.eep isrqueue.lss## Compile
isrqueue.o: ../isrqueue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_assert.o: ../../../common/openwsn/hal/gainz/hal_assert.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cpu.o: ../../../common/openwsn/hal/gainz/hal_cpu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_debugio.o: ../../../common/openwsn/hal/gainz/hal_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_foundation.o: ../../../common/openwsn/hal/gainz/hal_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_interrupt.o: ../../../common/openwsn/hal/gainz/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../../common/openwsn/hal/gainz/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_timer.o: ../../../common/openwsn/hal/gainz/hal_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/openwsn/hal/gainz/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_lightqueue.o: ../../../common/openwsn/rtl/rtl_lightqueue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_targetboard.o: ../../../common/openwsn/hal/gainz/hal_targetboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_debugio.o: ../../../common/openwsn/rtl/rtl_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_foundation.o: ../../../common/openwsn/rtl/rtl_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) isrqueue.elf dep/* isrqueue.hex isrqueue.eep isrqueue.lss isrqueue.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

