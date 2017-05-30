#pragma once
#ifndef _arduino_io_analog_pin_h_
#define _arduino_io_analog_pin_h_

#include "arduino.io.pin.h"

namespace arduino {

    namespace details {
        
        template < uint8_t _pin, io::pin_mode _mode >
        using analog_pin_base = pin_base < _pin, _mode, true /* has timer */ >;

    }
    
    namespace io {

        template < uint8_t _pin, pin_mode _mode >
        struct analog_pin : public details::analog_pin_base < _pin, _mode > {

            using traits = typename details::analog_pin_base < _pin, _mode >::traits;

            inline uint16_t get () const {
                // if (traits::timer_index == details::not_timer)
                //     return;

                // uint8_t high, low;
                return uin16_t();
            }

        }; 

        // analog pin output
		template < uint8_t _pin >
		struct analog_pin < _pin, pin_mode::output > : public details::analog_pin_base < _pin, pin_mode::output > {

            using base_t = analog_pin_base < _pin, pin_mode::output >;
			using traits = typename details::analog_pin_base < _pin, pin_mode::output >::traits;

            inline void set( uint8_t pulse_width ) const {
				// if (traits::timer_index == details::not_timer) {
				// 	if (pulse_width < 128)
				// 		base_t::set_low ();
				// 	else
				// 		base_t::set_high ();
				// } else {

                //     if (pulse_width == 0)
				// 		base_t::set_low ();
				// 	else if (pulse_width == 255)
				// 		base_t::set_high ();
				// 	else {
				// 		volatile uint8_t * analog_register = reinterpret_cast < volatile uint8_t * > (traits::timer_ref);

				// 		auto const & timer_pair = board::pin_timer_pair [traits::timer_index];
				// 		sbi (timer_pair.first, timer_pair.second);
				// 		*analog_register = pulse_width;
				// 	}
				// }
            }

        };

    }
}

#endif