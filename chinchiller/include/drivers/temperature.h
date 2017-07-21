#pragma once
#ifndef _drivers_temperature_h_
#define _drivers_temperature_h_

#include "common/mcu.h"

using namespace mcu;

namespace drivers {

	template < io::pin_num_t _pin_num, uint16_t conversion_ratio = 10 >
	struct temperature {

		using pin_t = io::pin < _pin_num >;

		static_assert (pin_t::traits.adc_channel != hardware::analog_channel::none, "selected pin not valid analog channel");

		inline temperature (hardware::voltage_reference ref) {
			mcu::io::adc::set_reference(ref);
			mcu::io::adc::enable();
		}

		double read () {
				uint16_t t_val = mcu::io::adc::read(pin_t::traits.adc_channel, mcu::io::adc_prescaler::p128);
				double t = ((t_val / 1024.0) * 5000.0) / double(conversion_ratio);

				if (_value_buffer_index == -1) {
					for (auto & t_ref : _value_buffer)
						t_ref = t;

					_value_buffer_index = 0;
				} else {
					_value_buffer[_value_buffer_index] = t;

					double avr = 0.0;
					for (auto & t : _value_buffer)
						avr += t;

					t = avr / double(mcu::array_size(_value_buffer));
				}

				++_value_buffer_index;

				if (_value_buffer_index >= int8_t (mcu::array_size(_value_buffer)))
					_value_buffer_index = 0;

				return t;
		}
		
	private:

		pin_t	_pin = { io::pin_mode::input };

		double _value_buffer [8]	= {};
		int8_t _value_buffer_index	= -1;

	};

}

#endif