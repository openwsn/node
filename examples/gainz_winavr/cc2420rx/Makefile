###############################################################################
# Makefile for the project cc2420rx
###############################################################################

## General Flags
PROJECT = cc2420rx
MCU = atmega128
TARGET = cc2420rx.elf
CC = avr-gcc

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
LDFLAGS +=  -Wl,-Map=cc2420rx.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = cc2420rx.o hal_assert.o hal_cc2420.o hal_cpu.o hal_foundation.o hal_interrupt.o hal_led.o hal_target.o hal_uart.o hpl_atmega128.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) cc2420rx.hex cc2420rx.eep cc2420rx.lss## Compile
cc2420rx.o: ../cc2420rx.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_assert.o: ../../common/hal/hal_assert.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cc2420.o: ../../common/hal/hal_cc2420.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cpu.o: ../../common/hal/hal_cpu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_foundation.o: ../../common/hal/hal_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_interrupt.o: ../../common/hal/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../common/hal/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_target.o: ../../common/hal/hal_target.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../common/hal/hal_uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../common/hal/hpl_atmega128.c
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
	-rm -rf $(OBJECTS) cc2420rx.elf dep/* cc2420rx.hex cc2420rx.eep cc2420rx.lss cc2420rx.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

