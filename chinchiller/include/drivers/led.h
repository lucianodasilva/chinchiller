#pragma once
#ifndef _common_drivers_led_h_
#define _common_drivers_led_h_

#include "common/mcu.h"

namespace drivers {

	struct color {
		uint8_t r, g, b;

		inline color invert () const {
			return {
				uint8_t(255 - r),
				uint8_t(255 - g),
				uint8_t(255 - b)
			};
		}

		static inline color interpolate (color a, color b, uint8_t alpha) {
			return {
				uint8_t ((a.r * (255-alpha) + b.r * alpha) / 255),
				uint8_t ((a.g * (255-alpha) + b.g * alpha) / 255),
				uint8_t ((a.b * (255-alpha) + b.b * alpha) / 255)
			};
		}
	};

	template < 
		mcu::io::pin_num_t _r_pin_n,
		mcu::io::pin_num_t _g_pin_n,
		mcu::io::pin_num_t _b_pin_n,
		bool _common_anode = false
	>
	struct led {

		mcu::io::pin < _r_pin_n > r_pin	= {mcu::io::pin_mode::output};
		mcu::io::pin < _g_pin_n > g_pin	= {mcu::io::pin_mode::output};
		mcu::io::pin < _b_pin_n > b_pin	= {mcu::io::pin_mode::output};

		static_assert (mcu::io::pin < _r_pin_n >::traits.timer_ocr != mcu::reg_ptr_null, "R pin must be a timer");
		static_assert (mcu::io::pin < _g_pin_n >::traits.timer_ocr != mcu::reg_ptr_null, "G pin must be a timer");
		static_assert (mcu::io::pin < _b_pin_n >::traits.timer_ocr != mcu::reg_ptr_null, "B pin must be a timer");

		led () {
			r_pin.set_compare_mode (mcu::hardware::compare_output_mode::clear);
			g_pin.set_compare_mode (mcu::hardware::compare_output_mode::clear);
			b_pin.set_compare_mode (mcu::hardware::compare_output_mode::clear);
		}

		void set (color c) {
			mcu::interrupt_guard iguard;
			if (_common_anode)
				c = c.invert ();

			set_pin(r_pin, c.r);
			set_pin(g_pin, c.g);
			set_pin(b_pin, c.b);
		}

	private:

		template < class _t >
		inline void set_pin (_t & pin, uint8_t v) {
			if (v == 0) {
				pin.set_compare_mode (mcu::hardware::compare_output_mode::normal);
				pin.set_low();
			} else if (v == 255) {
				pin.set_compare_mode (mcu::hardware::compare_output_mode::normal);
				pin.set_high();
			} else {
				pin.set_compare_mode (mcu::hardware::compare_output_mode::clear);
				pin.set_ocr (v);
			}
		}

	};

};

#endif