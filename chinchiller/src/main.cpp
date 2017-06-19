#include "common/mcu.h"
#include "drivers/lcd.h"

#include "tasks.h"

using namespace mcu::io;
using namespace drivers;

struct system_status {
	double temp_c = 0.0;
	double temp_ref = 0.0;

	double temp_target = 22.0;
	double fan_speed = 0.0;

	bool	button_dec = false;
	bool	button_inc = false;
};

system_status status = {};

lcd <
	13, // rs
	14, // e
	3,	// data 0
	4,	// data 1
	5,	// data 2
	6	// data 3
>
	display;

double round_to_half (double v) {
	v /= .5;

	if (v > .0)
	v += .5;
	else
	v -= .5;

	return int (v) * 0.5;
}

void read_temperature () {
	uint16_t t_val = adc::read(analog_channel::acd0, adc_prescaler::p128);
	double temp_mv = (t_val / 1024.0) * 5000.0;
	status.temp_c = round_to_half(temp_mv / 10.0);
}

void refresh_display () {
	display.clear();

	/*
	+0123456789ABCDEF
	0 T:xx.x  R:xx.x
	1 >xx.x<  V:xxx.x
	*/

	display.set_precision (1);
	display << pos {0, 0} << "T:" << status.temp_c;
	display << pos {9, 0} << "R:" << status.temp_ref;

	display << pos {0, 1} << "<" << round_to_half(status.temp_target) << ">";
	display << pos {9, 1} << "V:" << status.fan_speed << "%";
}

// manage target temperature value
void inc_press () {
	status.button_inc = true;
	status.temp_target += .5;
}

void inc_release () {
	status.button_inc = false;
	status.temp_target = round_to_half(status.temp_target);
}

void dec_press () {
	status.button_dec = true;
	status.temp_target -= .5;
}

void dec_release () {
	status.button_dec = false;
	status.temp_target = round_to_half(status.temp_target);
}

void update_target() {
	double shift = .0;
	double const shift_inc = 0.1; 

	if (status.button_inc)
		shift += shift_inc;

	if (status.button_dec)
		shift -= shift_inc;

	status.temp_target += shift;

	if (status.temp_target > 30.0)
		status.temp_target = 30.0;

	if (status.temp_target < 10.0)
		status.temp_target = 10.0;
}

int main (int arg_c, char ** arg_v) {

    mcu::setup();

	display.init();

	// display << pos {0, 0} << "Temp: 16 V: 100%" << pos { 0, 1 } << "Min: 20 Max:[40]";

	adc::set_reference(voltage_reference::avcc);
	adc::enable();

	auto exec = make_task_executor(
		make_timed_task(200, 
			// read temperature
			make_task (&read_temperature),
			// display task
			make_task(&refresh_display)
		),
		make_timed_task(75,
			make_task(&update_target)
		),
		make_button_task < 19 > (&dec_press, &dec_release),
		make_button_task < 18 > (&inc_press, &inc_release)
	);

	//uint32_t t200ms_frame = 0;

    while(1) {
		exec->run ();
		mcu::delay(50);
    }
		
    return 0;
}