#pragma once
#ifndef _arduino_io_pin_h_
#define _arduino_io_pin_h_

#include "arduino.details.h"

namespace arduino {

	using namespace arduino::details;

    namespace io {

		enum class pin_mode : uint8_t {
			input = 0,
			output = 1,
			pullup = 2
		};

		enum class pin_state : uint8_t {
			low = 0,
			high = 1
		};

	}

	namespace details {

		template < uint8_t _pin >
		struct pin_traits {
			static constexpr uint8_t const port_index       = board::pin_port_lookup			[_pin];
			static constexpr uint8_t const timer_index		= board::pin_timer_lookup 			[_pin];
			static constexpr uint8_t const native_pin_mask  = board::pin_bitmask_lookup			[_pin];

			static constexpr uint16_t const port_ref        = board::port_mode_lookup 			[port_index];
			static constexpr uint16_t const in_ref          = board::port_input_lookup 			[port_index];
			static constexpr uint16_t const out_ref         = board::port_output_lookup			[port_index];
			static constexpr uint16_t const timer_ref		= board::pin_timer_register_lookup	[timer_index];
		};

		template < uint8_t _pin, io::pin_mode _mode, bool _has_timer = false>
		struct pin_base {

			static_assert ( _pin <= details::board::max_pin_num, "invalid pin number");

			using traits = pin_traits < _pin >;

			inline pin_base () {

				volatile uint8_t * port_register = reinterpret_cast < volatile uint8_t * > (traits::port_ref);
				volatile uint8_t * out_register = reinterpret_cast < volatile uint8_t * > (traits::out_ref);

				uint8_t sreg_backup = SREG;
				cli ();

				switch (_mode) {
					case io::pin_mode::input:
						*port_register &= ~traits::native_pin_mask;
						*out_register &= ~traits::native_pin_mask;
						break;
					case io::pin_mode::pullup:
						*port_register &= ~traits::native_pin_mask;
						*out_register |= traits::native_pin_mask;
						break;
					case io::pin_mode::output:
						*port_register |= traits::native_pin_mask;
						break;
					default:
						break;
				}

				SREG = sreg_backup;

				if (_has_timer)
					enable_timer ();
				else
					disable_timer ();
			}

			inline static void disable_timer () {
				if (traits::timer_index == not_timer)
					return;

				auto const & timer_pair = board::pin_timer_pair [traits::timer_index];
				cbi (timer_pair.first, timer_pair.second);
			}

			inline static void enable_timer () {
				if (traits::timer_index == not_timer)
					return;

				auto const & timer_pair = board::pin_timer_pair [traits::timer_index];
				sbi (timer_pair.first, timer_pair.second);
			}

		};

	}

	namespace io {

		// pin input
		template < uint8_t _pin, pin_mode _mode = pin_mode::input >
		struct pin : public details::pin_base < _pin, _mode > {

			using traits = typename details::pin_base < _pin, _mode >::traits;

			inline pin_state get () const {
				volatile uint8_t * in_register = reinterpret_cast < volatile uint8_t * > (traits::in_ref);
				return static_cast < pin_state > (*in_register & traits::native_pin_mask);
			}

		};

		// pin output
		template < uint8_t _pin >
		struct pin < _pin, pin_mode::output > : public details::pin_base < _pin, pin_mode::output > {

			using traits = typename details::pin_base < _pin, pin_mode::output >::traits;

			inline void set_high () const {
				if (traits::port_ref == details::invalid_port)
					return;

				volatile uint8_t * out_register = reinterpret_cast < volatile uint8_t * > (traits::out_ref);
				uint8_t sreg_backup = SREG;
				cli();

				*out_register |= traits::native_pin_mask;

				SREG = sreg_backup;
			}

			inline void set_low () const {
				if (traits::port_ref == details::invalid_port)
					return;

                volatile uint8_t * out_register = reinterpret_cast < volatile uint8_t * > (traits::out_ref);
				uint8_t sreg_backup = SREG;
				cli();

				*out_register &= ~traits::native_pin_mask;

				SREG = sreg_backup;
			}

			inline void set (pin_state state) const {
				switch (state) {
					case pin_state::low:
						set_low ();
						break;
					default:
						set_high ();
						break;
				}
			}

		};

    }
}

#endif
