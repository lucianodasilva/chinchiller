#include "common/mcu.h"

#include "drivers/lcd.h"
#include "drivers/motor.h"
#include "drivers/temperature.h"

#include "tasks.h"

using namespace mcu::io;
using namespace drivers;

struct system_status {
	double	temp_c		= 0.0;
	double	temp_ref	= 0.0;

	double	temp_target = 22.0;
	double	fan_speed	= 0.0;

	bool	button_dec	= false;
	bool	button_inc	= false;
};

system_status status = {};

lcd <
	13, // rs
	14, // e
	3,	// data 0
	4,	// data 1
	5,	// data 2
	6	// data 3
>					display_driver;

using motor_driver_t = motor < 1, 15 >;

motor_driver_t motor_driver = { 
	motor_driver_t::timer_t::trait_types::clock_select_enum::clk_io_256
};

temperature < 23 >	temperature_driver = { hardware::voltage_reference::avcc };

void display_task ();

// manage target temperature value
void inc_press ();
void inc_release ();

void dec_press ();
void dec_release ();

void update_target();

// calculate fan speed
void fan_task ();

// update temperature
void temperature_task ();

int main (int arg_c, char ** arg_v) {

    mcu::setup();

	display_driver.init();

	auto exec = make_task_executor(
		make_timed_task(200, 
			// read temperature
			make_task (&temperature_task),
			// display task
			make_task(&display_task),
			// update danger indicator
			//make_task(&update_status_indicator),
			// check fan pid
			make_task(&fan_task)
		),
		make_timed_task(75,
			make_task(&update_target)
		),
		make_button_task < 19 > (&dec_press, &dec_release),
		make_button_task < 18 > (&inc_press, &inc_release)
	);

    while(1) {
		exec->run ();
		mcu::delay(25);
    }
		
    return 0;
}

double round_to_half (double v) {
	v /= .5;

	if (v > .0)
	v += .5;
	else
	v -= .5;

	return int (v) * 0.5;
}

// update display
void display_task() {
	display_driver.clear();

	/*
	+0123456789ABCDEF
	0 T:xx.x  R:xx.x
	1 >xx.x<  V:xxx%
	*/

	display_driver.set_precision (1);
	display_driver << pos {0, 0} << "T:" << status.temp_c;
	display_driver << pos {9, 0} << "R:" << status.temp_ref;

	display_driver << pos {0, 1} << "<" << round_to_half(status.temp_target) << ">";
	display_driver.set_precision(0);
	display_driver << pos {9, 1} << "V:" << status.fan_speed << "%";
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

// refresh temperature value
void temperature_task () {
	status.temp_c = temperature_driver.read();
}

// calculate and set fan speed
void fan_task () {

	static uint32_t prev_time	= 0;
	static double	prev_error	= 0;
	static double	integral	= 0;

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

	// update duty cycle
	uint8_t cycle = (status.fan_speed / 100.0) * 255.0;

	motor_driver.set_duty_cycle(cycle);
}