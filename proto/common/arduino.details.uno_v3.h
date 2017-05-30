#pragma once
#ifndef _arduino_details_uno_v3_hpp_
#define _arduino_details_uno_v3_hpp_

#include <avr/interrupt.h>
#include <stdint.h>

namespace arduino {
    namespace details {

        constexpr uint16_t const invalid_port = 0;
        constexpr uint8_t const not_timer = 0;

        template < class _sbt, class _bt >
        inline void cbi (_sbt sfr, _bt bit) {
            _SFR_BYTE(sfr) &= ~_BV(bit);
        }

        template < class _sbt, class _bt >
        inline void sbi (_sbt sfr, _bt bit) {
            _SFR_BYTE(sfr) |= _BV(bit);
        }

        template < class _f_t, class _s_t >
        struct pair {
            _f_t first;
            _s_t second;
        };

        namespace board {

            constexpr uint8_t const max_pin_num = 20;

            constexpr uint16_t const port_mode_lookup[] = {
                invalid_port,
                invalid_port,
                (uint16_t) &DDRB,
                (uint16_t) &DDRC,
                (uint16_t) &DDRD
            };

            constexpr uint16_t const port_output_lookup[] = {
                invalid_port,
                invalid_port,
                (uint16_t) &PORTB,
                (uint16_t) &PORTC,
                (uint16_t) &PORTD,
            };

            constexpr uint16_t const port_input_lookup[] = {
                invalid_port,
                invalid_port,
                (uint16_t) &PINB,
                (uint16_t) &PINC,
                (uint16_t) &PIND,
            };

            constexpr uint8_t const pin_port_lookup[] = {
                4, /* 0 */
                4,
                4,
                4,
                4,
                4,
                4,
                4,
                2, /* 8 */
                2,
                2,
                2,
                2,
                2,
                3, /* 14 */
                3,
                3,
                3,
                3,
                3,
            };

            constexpr uint8_t const pin_bitmask_lookup[] = {
                _BV(0), /* 0, port D */
                _BV(1),
                _BV(2),
                _BV(3),
                _BV(4),
                _BV(5),
                _BV(6),
                _BV(7),
                _BV(0), /* 8, port B */
                _BV(1),
                _BV(2),
                _BV(3),
                _BV(4),
                _BV(5),
                _BV(0), /* 14, port C */
                _BV(1),
                _BV(2),
                _BV(3),
                _BV(4),
                _BV(5),
            };

            constexpr uint8_t const pin_timer_lookup[] = {
                not_timer, /* 0 - port D */
                not_timer,
                not_timer,
                8,
                not_timer,
                2,
                1,
                not_timer,
                not_timer, /* 8 - port B */
                3,
                4,
                7,
                not_timer,
                not_timer,
                not_timer,
                not_timer, /* 14 - port C */
                not_timer,
                not_timer,
                not_timer,
                not_timer,
            };

            constexpr pair < uint16_t, uint8_t > const pin_timer_pair [] = {
                { 0, 0 },
                { (uint16_t) &TCCR0A, COM0A1 },
                { (uint16_t) &TCCR0A, COM0B1 },
                { (uint16_t) &TCCR1A, COM1A1 },
                { (uint16_t) &TCCR1A, COM1B1 },
                { 0, 0 },
                { 0, 0 },
                { (uint16_t) &TCCR2A, COM2A1 },
                { (uint16_t) &TCCR2A, COM2B1 }
            };

            constexpr uint16_t const pin_timer_register_lookup [] = {
                not_timer,
                (uint16_t) &OCR0A,
                (uint16_t) &OCR0B,
                (uint16_t) &OCR1A,
                (uint16_t) &OCR1B,
                not_timer,
                not_timer,
                (uint16_t) &OCR2A,
                (uint16_t) &OCR2B
            };

        }
    }
}

#endif
