#include "common/mcu.h"
#include "drivers/lcd.h"

#include "tasks.h"

int main (int arg_c, char ** arg_v) {

	using namespace mcu::io;
	using namespace drivers;

    mcu::setup();

	lcd <
		13, // rs
		14, // e
		6,	// data 0
		5,	// data 1
		4,	// data 2
		3	// data 3
	>
		display;

	auto exec = make_task_executor();

	display.init();

	// display << pos {0, 0} << "Temp: 16 V: 100%" << pos { 0, 1 } << "Min: 20 Max:[40]";

	adc::set_reference(voltage_reference::avcc);
	adc::enable();

    while(1){
        exec.run();

		uint16_t t_val = adc::read(analog_channel::acd0, adc_prescaler::p128);
		display.clear();

		double mv = (t_val / 1024.0) * 5000;
		double t_celsius = mv / 10.0;

		display << pos { 0, 0 } << "mV: " << mv;
		display << pos { 0, 1 } << " c: " << t_celsius;

		mcu::delay(500);
    }
		
    return 0;
}