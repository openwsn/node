###############################################################################
# Makefile for the project nio_csma_recv
###############################################################################

## General Flags
PROJECT = nio_csma_recv
MCU = atmega128
TARGET = nio_csma_recv.elf
CC = avr-gcc

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
LDFLAGS +=  -Wl,-Map=nio_csma_recv.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = nio_csma_recv.o nio_apl_output_frame.o hal_assert.o hal_cc2420.o hal_cpu.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_led.o hal_targetboard.o hal_timer.o hal_uart.o hpl_atmega128.o rtl_debugio.o rtl_dumpframe.o rtl_foundation.o rtl_frame.o rtl_ieee802frame154.o rtl_lightqueue.o rtl_random.o svc_nio_acceptor.o svc_nio_csma.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) nio_csma_recv.hex nio_csma_recv.eep nio_csma_recv.lss size

## Compile
nio_csma_recv.o: ../nio_csma_recv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nio_apl_output_frame.o: ../nio_apl_output_frame.c
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

rtl_dumpframe.o: ../../../common/openwsn/rtl/rtl_dumpframe.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_foundation.o: ../../../common/openwsn/rtl/rtl_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_frame.o: ../../../common/openwsn/rtl/rtl_frame.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_ieee802frame154.o: ../../../common/openwsn/rtl/rtl_ieee802frame154.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_lightqueue.o: ../../../common/openwsn/rtl/rtl_lightqueue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_random.o: ../../../common/openwsn/rtl/rtl_random.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_nio_acceptor.o: ../../../common/openwsn/svc/svc_nio_acceptor.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

svc_nio_csma.o: ../../../common/openwsn/svc/svc_nio_csma.c
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
	-rm -rf $(OBJECTS) nio_csma_recv.elf dep/* nio_csma_recv.hex nio_csma_recv.eep nio_csma_recv.lss nio_csma_recv.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

