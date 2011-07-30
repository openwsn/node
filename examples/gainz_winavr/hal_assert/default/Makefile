###############################################################################
# Makefile for the project hal_assert_test
###############################################################################

## General Flags
PROJECT = hal_assert_test
MCU = atmega128
TARGET = hal_assert_test.elf
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
LDFLAGS +=  -Wl,-Map=hal_assert_test.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = hal_assert_test.o hal_assert.o hal_led.o hal_target.o hpl_atmega128.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_uart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) hal_assert_test.hex hal_assert_test.eep hal_assert_test.lss## Compile
hal_assert_test.o: ../hal_assert_test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_assert.o: ../../../common/openwsn/hal/gainz/hal_assert.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../../common/openwsn/hal/gainz/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_target.o: ../../../common/openwsn/hal/gainz/hal_target.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/openwsn/hal/gainz/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_debugio.o: ../../../common/openwsn/hal/gainz/hal_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_foundation.o: ../../../common/openwsn/hal/gainz/hal_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_interrupt.o: ../../../common/openwsn/hal/gainz/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../../common/openwsn/hal/gainz/hal_uart.c
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
	-rm -rf $(OBJECTS) hal_assert_test.elf dep/* hal_assert_test.hex hal_assert_test.eep hal_assert_test.lss hal_assert_test.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

