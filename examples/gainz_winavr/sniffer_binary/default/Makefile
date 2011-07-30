###############################################################################
# Makefile for the project sniffer
###############################################################################

## General Flags
PROJECT = sniffer
MCU = atmega128
TARGET = sniffer.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99     -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=sniffer.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Library Directories
LIBDIRS = -L"D:\dev\openwsn\source\node\gainz-winavr\sniffer_binary\..\lib" 

## Objects that must be built in order to link
OBJECTS = sniffer.o hal_assert.o hal_cc2420.o hal_cpu.o hal_debugio.o hal_foundation.o hal_led.o hal_spi.o hal_targetboard.o hal_timer.o hal_uart.o hpl_atmega128.o rtl_debugio.o rtl_foundation.o rtl_frame.o hal_interrupt.o apl_ieee802frame154_dump.o rtl_ieee802frame154.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) sniffer.hex sniffer.eep sniffer.lss size

## Compile
sniffer.o: ../sniffer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_assert.o: ../../../common/openwsn/hal/gainz/hal_assert.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cc2420.o: ../../../common/openwsn/hal/gainz/hal_cc2420.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cpu.o: ../../../common/openwsn/hal/gainz/hal_cpu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_debugio.o: ../../../common/openwsn/hal/gainz/hal_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_foundation.o: ../../../common/openwsn/hal/gainz/hal_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../../common/openwsn/hal/gainz/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_spi.o: ../../../common/openwsn/hal/gainz/hal_spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_targetboard.o: ../../../common/openwsn/hal/gainz/hal_targetboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_timer.o: ../../../common/openwsn/hal/gainz/hal_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../../common/openwsn/hal/gainz/hal_uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/openwsn/hal/gainz/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_debugio.o: ../../../common/openwsn/rtl/rtl_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_foundation.o: ../../../common/openwsn/rtl/rtl_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_frame.o: ../../../common/openwsn/rtl/rtl_frame.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_interrupt.o: ../../../common/openwsn/hal/gainz/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

apl_ieee802frame154_dump.o: ../apl_ieee802frame154_dump.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_ieee802frame154.o: ../../../common/openwsn/rtl/rtl_ieee802frame154.c
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

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) sniffer.elf dep/* sniffer.hex sniffer.eep sniffer.lss sniffer.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

