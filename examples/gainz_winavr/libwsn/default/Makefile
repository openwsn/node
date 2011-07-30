###############################################################################
# Makefile for the project libwsn
###############################################################################

## General Flags
PROJECT = libwsn
MCU = atmega128
TARGET = libwsn.elf
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
LDFLAGS +=  -Wl,-Map=libwsn.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = libwsn.o hal_adc.o hal_assert.o hal_cc2420.o hal_cpu.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_led.o hal_luminance.o hal_spi.o hal_target.o hal_timer.o hal_uart.o hpl_atmega128.o rtl_iobuf.o rtl_openframe.o rtl_textcodec.o rtl_textspliter.o osx_dba.o osx_foundation.o svc_siocomm.o svc_timer.o rtl_cache.o rtl_random.o svc_aloha.o svc_flood.o svc_one2many.o rtl_foundation.o rtl_frame.o svc_debugio.o svc_simplealoha.o rtl_debugio.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) libwsn.hex libwsn.eep libwsn.lss## Compile
libwsn.o: ../libwsn.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_adc.o: ../../../common/openwsn/hal/gainz/hal_adc.c
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

hal_interrupt.o: ../../../common/openwsn/hal/gainz/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../../common/openwsn/hal/gainz/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_luminance.o: ../../../common/openwsn/hal/gainz/hal_luminance.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_spi.o: ../../../common/openwsn/hal/gainz/hal_spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_target.o: ../../../common/openwsn/hal/gainz/hal_target.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_timer.o: ../../../common/openwsn/hal/gainz/hal_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../../common/openwsn/hal/gainz/hal_uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/openwsn/hal/gainz/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_iobuf.o: ../../../common/openwsn/rtl/rtl_iobuf.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_openframe.o: ../../../common/openwsn/rtl/rtl_openframe.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_textcodec.o: ../../../common/openwsn/rtl/rtl_textcodec.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_textspliter.o: ../../../common/openwsn/rtl/rtl_textspliter.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_dba.o: ../../../common/openwsn/osx/osx_dba.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

osx_foundation.o: ../../../common/openwsn/osx/osx_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_siocomm.o: ../../../common/openwsn/svc/svc_siocomm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_timer.o: ../../../common/openwsn/svc/svc_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_cache.o: ../../../common/openwsn/rtl/rtl_cache.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_random.o: ../../../common/openwsn/rtl/rtl_random.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_aloha.o: ../../../common/openwsn/svc/svc_aloha.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_flood.o: ../../../common/openwsn/svc/svc_flood.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_one2many.o: ../../../common/openwsn/svc/svc_one2many.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_foundation.o: ../../../common/openwsn/rtl/rtl_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_frame.o: ../../../common/openwsn/rtl/rtl_frame.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_debugio.o: ../../../common/openwsn/svc/svc_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_simplealoha.o: ../../../common/openwsn/svc/svc_simplealoha.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_debugio.o: ../../../common/openwsn/rtl/rtl_debugio.c
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
	-rm -rf $(OBJECTS) libwsn.elf dep/* libwsn.hex libwsn.eep libwsn.lss libwsn.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

