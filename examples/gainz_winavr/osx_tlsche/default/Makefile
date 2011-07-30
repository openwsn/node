###############################################################################
# Makefile for the project osxsche
###############################################################################

## General Flags
PROJECT = osxsche
MCU = atmega128
TARGET = osxsche.elf
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
LDFLAGS +=  -Wl,-Map=osxsche.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = osxsche.o hal_assert.o hal_cpu.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_led.o hal_rtc.o hal_targetboard.o hal_uart.o hpl_atmega128.o rtl_debugio.o rtl_dispatcher.o rtl_foundation.o osx_foundation.o osx_taskheap.o osx_taskpool.o osx_tlsche.o task_measure.o task_siocomm.o task_wlscomm.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) osxsche.hex osxsche.eep osxsche.lss## Compile
osxsche.o: ../osxsche.c
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

hal_rtc.o: ../../../common/openwsn/hal/gainz/hal_rtc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_targetboard.o: ../../../common/openwsn/hal/gainz/hal_targetboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../../common/openwsn/hal/gainz/hal_uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/openwsn/hal/gainz/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_debugio.o: ../../../common/openwsn/rtl/rtl_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_dispatcher.o: ../../../common/openwsn/rtl/rtl_dispatcher.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_foundation.o: ../../../common/openwsn/rtl/rtl_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_foundation.o: ../../../common/openwsn/osx/osx_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_taskheap.o: ../../../common/openwsn/osx/osx_taskheap.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_taskpool.o: ../../../common/openwsn/osx/osx_taskpool.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_tlsche.o: ../../../common/openwsn/osx/osx_tlsche.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

task_measure.o: ../task_measure.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

task_siocomm.o: ../task_siocomm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

task_wlscomm.o: ../task_wlscomm.c
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
	-rm -rf $(OBJECTS) osxsche.elf dep/* osxsche.hex osxsche.eep osxsche.lss osxsche.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

