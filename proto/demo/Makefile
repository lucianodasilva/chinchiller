
OUTNAME=demo
MCU=-mmcu=atmega328p
CPU_SPEED=-DF_CPU=16000000UL
BOARD=ARDUINO_UNO_V3

INCDIR=../common/
CC=avr-gcc
CXX=avr-g++

CFLAGS=$(MCU) $(CPU_SPEED) -I$(INCDIR) -D$(BOARD) -g -Os -Wl,--gc-sections -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -std=c++17

SOURCE=$(wildcard *.cpp)
OBJECTS=$(addprefix obj/,$(notdir $(SOURCE:.cpp=.o)))

default: build

clean:
	@echo -n Cleaning ...
	$(shell rm -rf obj 2> /dev/null)
	$(shell rm -rf out 2> /dev/null)
	@echo " done"

setup:
	$(shell mkdir obj 2> /dev/null)
	$(shell mkdir out 2> /dev/null)

compile: $(OBJECTS)
	$(CXX) $(CFLAGS) $^ -o out/$(OUTNAME)

export: out/$(OUTNAME)
	avr-objcopy -O ihex $< out/$(OUTNAME).hex
	avr-size --format=avr --target=ihex $<

build: setup compile export

obj/%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<
