#pragma once
#ifndef _chinchiller_common_mcu_atmega328p_h_
#define _chinchiller_common_mcu_atmega328p_h_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu.h"

namespace mcu {

    namespace hardware {

        constexpr _timer_traits const timer_traits_lookup[] = {
                // timer0
                {
                        to_reg(TCCR0A),
                        to_reg(TCCR0B),
                        to_reg(TCNT0),
                        to_reg(TIMSK0),
                        to_reg(TIFR0)
                },
                // timer1
                {
                        to_reg(TCCR1A),
                        to_reg(TCCR1B),
                        to_reg(TCNT1),
                        to_reg(TIMSK1),
                        to_reg(TIFR1)
                },
                // timer2
                {
                        to_reg(TCCR2A),
                        to_reg(TCCR2B),
                        to_reg(TCNT2),
                        to_reg(TIMSK2),
                        to_reg(TIFR2)
                },
        };

        constexpr size_t const available_timer_count = array_size(timer_traits_lookup);

        enum struct compare_output_mode : uint8_t {
            normal			= 0b00,
            toggle			= 0b01,
            clear			= 0b10,
            set				= 0b11
        };

        enum struct waveform_generation_mode_8bit : uint8_t {
            normal			= 0b000,
            pwm_ff01		= 0b001,
            ctc				= 0b010,
			pwm_ff03		= 0b011,
            pwm_ocr			= 0b101
        };

        enum struct waveform_generation_mode_16bit : uint16_t {
            normal          = 0b0000,
            pwm_8bit_ff00   = 0b0001,
            pwm_9bit_ff01   = 0b0010,
            pwm_10bit_ff03  = 0b0011,
            ctc             = 0b0100,
            pwm_freq_icr1   = 0b1000,
            pwm_freq_ocr    = 0b1001,
            pwm_icr1        = 0b1010,
            pwm_ocr         = 0b1011,
            ctc_icr1        = 0b1100
        };

        enum struct clock_select : uint8_t {
            not_set         = 0b000,
            clk_io_none     = 0b001,
            clk_io_8        = 0b010,
            clk_io_64       = 0b011,
            clk_io_256      = 0b100,
            clk_io_1024     = 0b101,
            clk_io_T0_fall  = 0b110,
            clk_io_T0_raise = 0b111
        };

        enum struct clock_select_high_res : uint8_t {
			not_set			= 0b000,
            clk_io_none		= 0b001,
            clk_io_8        = 0b010,
            clk_io_32       = 0b011,
            clk_io_64       = 0b100,
            clk_io_128      = 0b101,
            clk_io_256      = 0b110,
            clk_io_1024     = 0b111
        };

        template < size_t timer_num = 0 >
        struct timer_type_traits {
            using tcnt_type                 = uint8_t;
            using waveform_generation_enum  = waveform_generation_mode_8bit;
            using clock_select_enum         = clock_select;
        };

        template <>
        struct timer_type_traits < 1 > {
            using tcnt_type                 = uint16_t;
            using waveform_generation_enum  = waveform_generation_mode_16bit;
            using clock_select_enum         = clock_select;
        };

        template <>
        struct timer_type_traits < 2 > {
            using tcnt_type                 = uint8_t;
            using waveform_generation_enum  = waveform_generation_mode_8bit;
            using clock_select_enum         = clock_select_high_res;
        };

        namespace io {

            constexpr _port_traits const port_b = {
                    to_reg(DDRB),
                    to_reg(PORTB),
                    to_reg(PINB)
            };

            constexpr _port_traits const port_c = {
                    to_reg(DDRC),
                    to_reg(PORTC),
                    to_reg(PINC)
            };

            constexpr _port_traits const port_d = {
                    to_reg(DDRD),
                    to_reg(PORTD),
                    to_reg(PIND)
            };

            constexpr _pin_traits const pin_traits_lookup[] = {
                    { // pin 01
                            port_d,
                            PORTD6,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 02
                            port_d,
                            PORTD0,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 03
                            port_d,
                            PORTD1,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 04
                            port_d,
                            PORTD2,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 05
                            port_d,
                            PORTD3,
                            timer_traits_lookup[2],
                            to_reg(OCR2B),
                            4
                    },
                    { // pin 06
                            port_d,
                            PORTD4,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    // pin 07
                    pin_null,
                    // pin 08
                    pin_null,
                    { // pin 09
                            port_b,
                            PORTB6,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 10
                            port_b,
                            PORTB7,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 11
                            port_d,
                            PORTD5,
                            timer_traits_lookup[0],
                            to_reg(OCR0B),
                            4
                    },
                    { // pin 12
                            port_d,
                            PORTD6,
                            timer_traits_lookup[0],
                            to_reg(OCR0A),
                            6
                    },
                    { // pin 13
                            port_d,
                            PORTD7,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 14
                            port_b,
                            PORTB0,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 15
                            port_b,
                            PORTB1,
                            timer_traits_lookup[1],
                            to_reg(OCR1A),
                            6
                    },
                    { // pin 16
                            port_b,
                            PORTB2,
                            timer_traits_lookup[1],
                            to_reg(OCR1B),
                            4
                    },
                    { // pin 17
                            port_b,
                            PORTB3,
                            timer_traits_lookup[2],
                            to_reg(OCR2A),
                            6
                    },
                    { // pin 18
                            port_b,
                            PORTB4,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 19
                            port_b,
                            PORTB5,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    // pin 20
                    pin_null,
                    // pin 21
                    pin_null,
                    // pin 22
                    pin_null,
                    { // pin 23
                            port_c,
                            PORTC0,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 24
                            port_c,
                            PORTC1,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 25
                            port_c,
                            PORTC2,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 26
                            port_c,
                            PORTC3,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 27
                            port_c,
                            PORTC4,
                            timer_null,
                            reg_ptr_null,
                            0
                    },
                    { // pin 28
                            port_c,
                            PORTC5,
                            timer_null,
                            reg_ptr_null,
                            0
                    }
            };

            constexpr size_t const available_pin_count = array_size(pin_traits_lookup);

			constexpr _adc_traits const adc_traits = {
				to_reg(ADMUX),
				to_reg(ADCSRA),
				to_reg(ADCL),
				to_reg(ADCH)
			};

        }

    }
}

#endif //_chinchiller_common_mcu_atmega328p_h_
