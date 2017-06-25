#pragma once
#ifndef _drivers_motor_h_
#define _drivers_motor_h_

#include "common/mcu.h"

using namespace mcu;

namespace drivers {

	template < timer_num_t _timer_num, io::pin_num_t _pin_num >
	struct motor {
		
		using pin_t = io::pin < _pin_num >;
		using timer_t = timer < _timer_num >;

		static_assert (pin_t::traits.timer.tccra == timer_t::traits.tccra, "selected pin belongs to another timer");

		inline motor (typename timer_t::trait_types::clock_select_enum clock_select) {
			set_digital(0);

			_timer.set_clock_selection(clock_select);
			_timer.set_pwd(timer_t::trait_types::waveform_generation_enum::pwm_8bit_ff00);
		}

		inline void set_duty_cycle (uint8_t dt) {
			// min duty rate at 20%
			uint8_t const min_rate = 51;

			double r = dt / 255.0;
			dt = (r * (255.0 - min_rate)) + min_rate;

			if (dt <= min_rate)
				set_digital (0);
			else if (dt == 255)
				set_digital (1);
			else
				set_analog (dt);
		}
		
	private:

		inline void set_digital (uint8_t v) {
			if (!_locked) {
				_pin.set_compare_mode(hardware::compare_output_mode::normal);
				_locked = true;
			}

			_pin.set(v);
		}

		inline void set_analog (uint8_t v) {
			if (_locked) {
				_pin.set_compare_mode(hardware::compare_output_mode::clear);
				_locked = false;
			}

			_pin.set_ocr (v);
		}
	
		pin_t const		_pin = { io::pin_mode::output } ;
		timer_t const	_timer = {};

		bool			_locked;
	};
	
}

#endif
