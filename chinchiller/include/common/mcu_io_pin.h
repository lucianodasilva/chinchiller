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

        using pin_num_t = size_t;

        template < pin_num_t pin_num >
        struct _basic_pin {

            // pin info
            constexpr static auto const &   traits      = mcu::hardware::io::pin_traits_lookup [pin_num - 1];

            // compile-time validation
            static_assert (pin_num > 0 && pin_num <= mcu::hardware::io::available_pin_count, "invalid pin number");
            static_assert (traits !=  mcu::hardware::io::pin_null, "unsupported pin");

        };

        constexpr bool _pin_has_timer (pin_num_t pin_num) {
            return (pin_num > 0 && pin_num <= mcu::hardware::io::available_pin_count) &&
                    mcu::hardware::io::pin_traits_lookup [pin_num - 1]
                           .timer_ocr != reg_ptr_null;
        }

        template < pin_num_t pin_num >
        struct _timer_pin : public _basic_pin < pin_num > {

            inline void set_compare_mode (hardware::compare_output_mode  com_mode) const {
                interrupt_guard iguard;
                write_n (
                        this->traits.timer.tccra,
                        (uint8_t)com_mode,
                        3, // TODO: replace by "non-magic number"
                        this->traits.timer_com_bit
                );
            }

            inline void set_ocr (uint8_t value) {
                interrupt_guard iguard;

                *from_reg(this->traits.timer_ocr) = value;
            }

        };

        template < pin_num_t pin_num >
        struct pin :
                public type_or <
                        _pin_has_timer(pin_num),
                        _timer_pin < pin_num >,
                        _basic_pin < pin_num >
                >::type
        {

            pin (pin_mode mode) {
                set_mode (mode);
            }

            inline void set_mode (pin_mode mode) const {
                auto ddr  = this->traits.port.ddr;
                auto port = this->traits.port.port;

				auto bit = this->traits.port_bit;

                interrupt_guard iguard;

                switch (mode) {
                    case pin_mode::input:
                        cbi(ddr, bit);
                        cbi(port, bit);
                        break;
                    case pin_mode::pullup:
                        cbi(ddr, bit);
                        sbi(port, bit);
                        break;
                    case pin_mode::output:
						sbi(ddr, bit);
                        cbi(port, bit);
                        break;
                    default:
                        break;
                }
            }

            inline void set_high () const {
				auto port = this->traits.port.port;
                interrupt_guard iguard;
				sbi (port, this->traits.port_bit);
			}

			inline void set_low () const {
                auto port = this->traits.port.port;
				interrupt_guard iguard;

				cbi(port, this->traits.port_bit);
			}

			inline void set (uint8_t v) const {
				auto out_reg = from_reg (this->traits.port.port);
				interrupt_guard iguard;
				set_bit (*out_reg, v, this->traits.port_bit);
			}

			inline uint8_t get () const {
				auto in_reg = from_reg (this->traits.port.pin);
				interrupt_guard iguard;
				return get_bit(*in_reg, this->traits.port_bit);
			}

        };
    } // namespace io
} // namespace mcu

#endif // _chinchiller_common_mcu_io_pin_h_
