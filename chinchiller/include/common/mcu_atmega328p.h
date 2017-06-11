#pragma once
#ifndef _chinchiller_common_mcu_atmega328p_h_
#define _chinchiller_common_mcu_atmega328p_h_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu.h"

namespace mcu {

    namespace hardware {

        namespace io {

            constexpr _port_traits const port_b = {
                    (uint16_t) &DDRB,
                    (uint16_t) &PORTB,
                    (uint16_t) &PINB
            };

            constexpr _port_traits const port_c = {
                    (uint16_t) &DDRC,
                    (uint16_t) &PORTC,
                    (uint16_t) &PINC
            };

            constexpr _port_traits const port_d = {
                    (uint16_t) &DDRD,
                    (uint16_t) &PORTD,
                    (uint16_t) &PIND
            };

            constexpr _pin_traits const pin_traits_lookup[] = {
                    { // pin 01
                            port_d,
                            6
                    },
                    { // pin 02
                            port_d,
                            0
                    },
                    { // pin 03
                            port_d,
                            1
                    },
                    { // pin 04
                            port_d,
                            2
                    },
                    { // pin 05
                            port_d,
                            3
                    },
                    { // pin 06
                            port_d,
                            4
                    },
                    // pin 07
                    pin_null,
                    // pin 08
                    pin_null,
                    { // pin 09
                            port_b,
                            6
                    },
                    { // pin 10
                            port_b,
                            7
                    },
                    { // pin 11
                            port_d,
                            5
                    },
                    { // pin 12
                            port_d,
                            6
                    },
                    { // pin 13
                            port_d,
                            7
                    },
                    { // pin 14
                            port_b,
                            0
                    },
                    { // pin 15
                            port_b,
                            1
                    },
                    { // pin 16
                            port_b,
                            2
                    },
                    { // pin 17
                            port_b,
                            3
                    },
                    { // pin 18
                            port_b,
                            4
                    },
                    { // pin 19
                            port_b,
                            5
                    },
                    // pin 20
                    pin_null,
                    // pin 21
                    pin_null,
                    // pin 22
                    pin_null,
                    { // pin 23
                            port_c,
                            0
                    },
                    { // pin 24
                            port_c,
                            1
                    },
                    { // pin 25
                            port_c,
                            2
                    },
                    { // pin 26
                            port_c,
                            3
                    },
                    { // pin 27
                            port_c,
                            4
                    },
                    { // pin 28
                            port_c,
                            5
                    }
            };

            constexpr size_t const available_pin_count = array_size(pin_traits_lookup);

        }

        constexpr _timer_traits const timer_traits_lookup[] = {
                // timer0
                {
                        (uint16_t) &TCCR0A,
                        (uint16_t) &TCCR0B,
                        (uint16_t) &TCNT0,
                        (uint16_t) &TIMSK0,
                        (uint16_t) &TIFR0
                },
                // timer1
                {
                        (uint16_t) &TCCR1A,
                        (uint16_t) &TCCR1B,
                        (uint16_t) &TCNT1,
                        (uint16_t) &TIMSK1,
                        (uint16_t) &TIFR1
                },
                // timer2
                {
                        (uint16_t) &TCCR2A,
                        (uint16_t) &TCCR2B,
                        (uint16_t) &TCNT2,
                        (uint16_t) &TIMSK2,
                        (uint16_t) &TIFR2
                },
        };

        constexpr size_t const available_timer_count = array_size(timer_traits_lookup);

        enum struct compare_output_mode : uint8_t {
            normal  = 0b00,
            toggle  = 0b01,
            clear   = 0b10,
            set     = 0b11
        };

        enum struct waveform_generation_mode_8bit : uint8_t {
            normal  = 0b000,
            pwm_ff  = 0b001,
            ctc     = 0b010,
            pwm_ocr = 0b101
        };

        enum struct waveform_generation_mode_16bit : uint16_t {
            normal          = 0b0000,
            pwm_8bit_00ff   = 0b0001,
            pwm_9bit_01ff   = 0b0010,
            pwm_10bit_03ff  = 0b0011,
            ctc             = 0b0100,
            pwm_freq_icr1   = 0b1000,
            pwd_freq_ocr    = 0b1001,
            pwd_icr1        = 0b1010,
            pwd_ocr         = 0b1011,
            ctc_icr1        = 0b1100
        };

        enum struct clock_select : uint8_t {
            none            = 0b000,
            clk_io_none     = 0b001,
            clk_io_8        = 0b010,
            clk_io_64       = 0b011,
            clk_io_256      = 0b100,
            clk_io_1024     = 0b101,
            clk_io_T0_fall  = 0b110,
            clk_io_T0_raise = 0b111
        };

        enum struct clock_select_high_res : uint8_t {
            none            = 0b000,
            clk_io_none     = 0b001,
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

    }
}

#endif //_chinchiller_common_mcu_atmega328p_h_
