PROJECT=maglev
SOURCES=maglev.c pid.c
FCPU=8000000
MCU=attiny85
PROG=usbasp
PROGFLAGS=
CC=avr-gcc
CFLAGS=-Os -mmcu=$(MCU) -Wall -pedantic -std=c11

all: $(PROJECT).elf

$(PROJECT).elf: $(SOURCES)
	$(CC) $(CFLAGS) -o $(PROJECT).elf $(SOURCES) -DF_CPU=$(FCPU)

flash: $(PROJECT).elf
	sudo avrdude -p $(MCU) -c $(PROG) $(PROGFLAGS) -U flash:w:$(PROJECT).elf:e

size: $(PROJECT).elf
	avr-size $(PROJECT).elf

clean:
	rm $(PROJECT).elf
