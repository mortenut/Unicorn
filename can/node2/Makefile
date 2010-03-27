TARGET         = node2
CSRC           = main.c can_drv.c can_lib.c isr.c 
ASRC           = 
MCU_TARGET     = at90can128
OPTIMIZE       = -Os -mcall-prologues
DEFS           =
LIBS           =
DEBUG          = dwarf-2

CC             = avr-gcc
ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS)
CFLAGS         = -g$(DEBUG) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
LDFLAGS        = -Wl,-Map,$(TARGET).map
OBJ            = $(CSRC:.c=.o) $(ASRC:.S=.o)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump


AVRDUDE_PROGRAMMERID=avrispmkii
PROGRAMMER_MCU=c128
HEXROMTRG=$(TARGET).hex 

AVRDUDE=avrdude

all: $(TARGET).elf lst text

.PHONY: writeflash

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -rf *.o $(TARGET).elf *.eps *.bak *.a
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf $(TARGET).hex



lst:  $(TARGET).lst
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

writeflash: hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
	 -p $(PROGRAMMER_MCU) -P usb -B 1     \
	 -U flash:w:$(HEXROMTRG)

install: writeflash

installt: hex
	$(AVRDUDE) -c usbtiny   \
	 -p $(PROGRAMMER_MCU) -P usb -B 1     \
	 -U flash:w:$(HEXROMTRG)
    
text: hex
hex:  $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@




