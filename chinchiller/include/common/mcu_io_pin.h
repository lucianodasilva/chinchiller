#pragma once
#ifndef _chinchiller_common_mcu_io_pin_h_
#define _chinchiller_common_mcu_io_pin_h_

#include "mcu.h"

namespace mcu {
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

        using pin_num_t = size_t;

        template < pin_num_t pin_num >
        struct pin {

            // pin info
            constexpr static auto const &   traits      = mcu::hardware::io::pin_traits_lookup [pin_num - 1];
            constexpr static uint8_t const  pin_mask    = _BV(traits.bit);

            // compile-time validation
            static_assert (pin_num > 0 && pin_num <= mcu::hardware::io::available_pin_count, "invalid pin number");
            static_assert (traits !=  mcu::hardware::io::pin_null, "unsupported pin");

            pin (pin_mode mode) {
                set_mode (mode);
            }

            inline void set_mode (pin_mode mode) const {
                volatile uint8_t * ddr  = traits.port.ddr.u8_ptr;
                volatile uint8_t * port = traits.port.port.u8_ptr;

                interrupt_guard iguard;

                switch (mode) {
                    case pin_mode::input:
                        cbi(ddr, pin_mask);
                        cbi(port, pin_mask);
                        break;
                    case pin_mode::pullup:
                        cbi(ddr, pin_mask);
                        sbi(port, pin_mask);
                        break;
                    case pin_mode::output:
                        sbi(port, pin_mask);
                        break;
                    default:
                        break;
                }
            }

            inline void set_high () const {
				volatile uint8_t * port = traits.port.port.u8_ptr;
                interrupt_guard iguard;

				sbi (port, pin_mask);
			}

			inline void set_low () const {
                volatile uint8_t * port = traits.port.port.u8_ptr;
				interrupt_guard iguard;

				cbi(port, pin_mask);
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

#endif // _chinchiller_common_mcu_io_pin_h_
