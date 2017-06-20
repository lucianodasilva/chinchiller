#include "common/mcu.h"
#include "drivers/lcd.h"
#include "drivers/led.h"

#include "tasks.h"

#include <math.h>

using namespace mcu::io;
using namespace drivers;

struct system_status {
	double	temp_c = 0.0;
	double	temp_ref = 0.0;

	double	temp_target = 22.0;
	double	fan_speed = 0.0;

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

led <17, 16, 15, true > danger_indicator;

double round_to_half (double v) {
	v /= .5;

	if (v > .0)
	v += .5;
	else
	v -= .5;

	return int (v) * 0.5;
}

void read_temperature () {

	static double	buffered_temperature [16] = {};
	static uint8_t	buffer_index = 0;
	
	uint16_t t_val = adc::read(analog_channel::acd0, adc_prescaler::p128);
	double temp_mv = (t_val / 1024.0) * 5000.0;

	buffered_temperature[buffer_index] = round_to_half(temp_mv / 10);

	double avr = 0.0;
	for (auto & t : buffered_temperature)
		avr += t;

	status.temp_c = avr / double(mcu::array_size(buffered_temperature));

	++buffer_index;
	if (buffer_index == mcu::array_size(buffered_temperature))
		buffer_index = 0;
}

void refresh_display () {
	display.clear();

	/*
	+0123456789ABCDEF
	0 T:xx.x  R:xx.x
	1 >xx.x<  V:xxx%
	*/

	display.set_precision (1);
	display << pos {0, 0} << "T:" << status.temp_c;
	display << pos {9, 0} << "R:" << status.temp_ref;

	display << pos {0, 1} << "<" << round_to_half(status.temp_target) << ">";
	display.set_precision(0);
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

	status.temp_target = mcu::clamp (10.0, 30.0, status.temp_target);
}

void update_fan() {

	static uint32_t prev_time = 0;
	static double	prev_error = 0;
	static double	integral = 0;

	static double const
		kp = 20,
		ki = 15,
		kd = 1;

	uint32_t time = mcu::millis();
	double dt = (time - prev_time) / 1000.0;

	double error = status.temp_c - status.temp_target;

	integral += error * dt;

	double 
		p = kp * error,
		i = ki * integral,
		d = kd * ((error - prev_error) / dt);

	status.fan_speed = p + i + d;

	status.fan_speed = mcu::clamp(0.0, 100.0, status.fan_speed);

	prev_error = error;
	prev_time = time;

	integral = mcu::clamp(-10.0, 10.0, integral);
}

void update_status_indicator () {
	//status_indicator.set(color{128, 128, 128});
	double max = 24.0;
	double safe = 16.0;


	double m = (status.temp_c - safe) / (max - safe);

	if (m < 0.0)
		m = 0.0;

	if (m > 1.0)
		m = 1.0;

	color c;

	if (m < 0.5) {
		c = color::interpolate(
			{ 0, 128, 0 },
			{ 255, 128, 0 },
			m * 2.0 * 255
		);
	} else {
		c = color::interpolate(
			{ 255, 128, 0 },
			{ 255, 0, 0 },
			(m - .5) * 2.0 * 255
		);
	}

	danger_indicator.set(c);
}

int main (int arg_c, char ** arg_v) {

    mcu::setup();

	display.init();

	// display << pos {0, 0} << "Temp: 16 V: 100%" << pos { 0, 1 } << "Min: 20 Max:[40]";

	adc::set_reference(voltage_reference::avcc);
	adc::enable();

	// setup timers for rgb led
	auto timer1 = mcu::timer < 1 >();

	timer1.set_clock_selection(mcu::timer< 1 >::trait_types::clock_select_enum::clk_io_none);
	timer1.set_pwd(mcu::timer< 1 >::trait_types::waveform_generation_enum::pwm_8bit_ff00);

	auto timer2 = mcu::timer < 2 >();
	timer2.set_clock_selection(mcu::timer< 2 >::trait_types::clock_select_enum::clk_io_none);
	timer2.set_pwd(mcu::timer< 2 >::trait_types::waveform_generation_enum::pwm_ff03);

	auto exec = make_task_executor(
		make_timed_task(200, 
			// read temperature
			make_task (&read_temperature),
			// display task
			make_task(&refresh_display),
			// update danger indicator
			make_task(&update_status_indicator),
			// check fan pid
			make_task(&update_fan)
		),
		make_timed_task(75,
			make_task(&update_target)
		),
		make_button_task < 19 > (&dec_press, &dec_release),
		make_button_task < 18 > (&inc_press, &inc_release)
	);

    while(1) {
		exec->run ();
		mcu::delay(10);
    }
		
    return 0;
}