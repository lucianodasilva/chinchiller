#pragma once
#ifndef _chinchiller_common_mcu_atmega328p_h_
#define _chinchiller_common_mcu_atmega328p_h_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu.h"

namespace common {
    namespace mcu {
        namespace io {
                
                constexpr _port_traits const port_b = {
                        (uint16_t)&DDRB,
                        (uint16_t)&PORTB,
                        (uint16_t)&PINB
                };

                constexpr _port_traits const port_c = {
                        (uint16_t)&DDRC,
                        (uint16_t)&PORTC,
                        (uint16_t)&PINC
                };

                constexpr _port_traits const port_d = {
                        (uint16_t)&DDRD,
                        (uint16_t)&PORTD,
                        (uint16_t)&PIND
                };

                constexpr _pin_traits const pin_traits_lookup [] = {
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
                        },
                };

                constexpr size_t const available_pin_count = array_size(pin_traits_lookup);
        }
    }
}

#endif //_chinchiller_common_mcu_atmega328p_h_
