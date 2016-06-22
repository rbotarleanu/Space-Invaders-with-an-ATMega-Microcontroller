INCLUDEDIR=-Isrc/QTouch/ -I.


OBJS +=  \
		 main.o \
		 lcd.o \
		 src/QTouch/BitBangSPI_Master.o \
		 src/QTouch/eeprom_access.o \
		 src/QTouch/init_mcu_atmega324.o \
		 src/QTouch/QDebug.o \
		 src/QTouch/QDebugTransport.o \
		 src/QTouch/qt_asm_tiny_mega.o \
		 src/QTouch/SPI_Master.o \
		 src/QTouch/TWI_Master.o \
		 touch.o \
		 gfx.o \
		 glcdfont.o \
		 space_invaders.o \
		 list.o


all: main.hex

%.o: %.c	
	avr-gcc -c -mmcu=atmega324a -Os -DF_CPU=16000000UL -Wall $(INCLUDEDIR)  -o  $@  $<


src/QTouch/%.o: src/QTouch/%.c	
	avr-gcc -c -mmcu=atmega324a -Os -Wall $(INCLUDEDIR)  -o  $@  $<


src/QTouch/qt_asm_tiny_mega.o: src/QTouch/qt_asm_tiny_mega.s
	avr-gcc -x assembler-with-cpp -c -mmcu=atmega324a -Os -Wall $(INCLUDEDIR)  -o  $@  $<

src/QTouch/%.o: ../src/QTouch/%.s
	avr-gcc -x assembler-with-cpp -c -mmcu=atmega324a -Os -Wall $(INCLUDEDIR)  -o  $@  $<

main.elf: $(OBJS)
	avr-gcc -mmcu=atmega324a -o $@ $^ -lavr5g1-4qt-k-1rs -Lsrc/QTouch

main.hex: main.elf
	avr-objcopy  -j .text -j .data -O ihex  main.elf main.hex
	avr-size main.elf

clean:
	rm -rf main.elf main.hex $(OBJS)
