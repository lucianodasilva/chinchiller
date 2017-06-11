#pragma once
#ifndef _chinchiller_common_mcu_h_
#define _chinchiller_common_mcu_h_

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

namespace mcu {

    namespace hardware {

        union reg_ptr {
            uint16_t address;
            uint8_t *u8_ptr;
            uint16_t *u16_ptr;
        };

        inline constexpr bool operator==(const reg_ptr &v1, const reg_ptr &v2) {
            return v1.address == v2.address;
        }

        inline constexpr bool operator!=(const reg_ptr &v1, const reg_ptr &v2) {
            return !(operator==(v1, v2));
        }

        namespace io {

            struct _port_traits {
                reg_ptr ddr;
                reg_ptr port;
                reg_ptr pin;
            };

            inline constexpr bool operator==(const _port_traits &v1, const _port_traits &v2) {
                return
                        v1.ddr == v2.ddr &&
                        v1.port == v2.port &&
                        v1.pin == v2.pin;
            }

            inline constexpr bool operator!=(const _port_traits &v1, const _port_traits &v2) {
                return !(operator==(v1, v2));
            }

            constexpr _port_traits const port_null = {};

            struct _pin_traits {
                _port_traits port;
                uint8_t bit;
            };

            inline constexpr bool operator==(const _pin_traits &v1, const _pin_traits &v2) {
                return
                        v1.port == v2.port &&
                        v1.bit == v2.bit;
            }

            inline constexpr bool operator!=(const _pin_traits &v1, const _pin_traits &v2) {
                return !(operator==(v1, v2));
            }

            constexpr _pin_traits const pin_null = {};

        }

        struct _timer_traits {
            reg_ptr tccra;
            reg_ptr tccrb;
            reg_ptr tcnt;
            reg_ptr timsk;
            reg_ptr tifr;
        };

        inline constexpr bool operator==(const _timer_traits &v1, const _timer_traits &v2) {
            return
                    v1.tccra == v2.tccra &&
                    v1.tccrb == v2.tccrb &&
                    v1.tcnt == v2.tcnt &&
                    v1.timsk == v2.timsk &&
                    v1.tifr == v2.tifr;
        }

        inline constexpr bool operator!=(const _timer_traits &v1, const _timer_traits &v2) {
            return !(operator==(v1, v2));
        }

        constexpr _timer_traits const timer_null = {};

    }

    template<class _sbt, class _bt>
    inline void cbi(_sbt sfr, _bt bit) {
        _SFR_BYTE(sfr) &= ~_BV(bit);
    }

    template<class _sbt, class _bt>
    inline void sbi(_sbt sfr, _bt bit) {
        _SFR_BYTE(sfr) |= _BV(bit);
    }

    struct interrupt_guard {
        uint8_t sreg_backup;

        inline interrupt_guard() : sreg_backup(SREG) { cli(); }

        ~interrupt_guard() { SREG = sreg_backup; }
    };

    template<class _f_t, class _s_t>
    struct pair {
        _f_t first;
        _s_t second;
    };

    template<class _t, size_t _s>
    constexpr inline size_t array_size(_t (&)[_s]) {
        return _s;
    };

}

// include mcu specific details
#if defined (__AVR_ATmega328P__)
#   include "mcu_atmega328p.h"
#endif

// include hardware manipulators
#include "mcu_io_pin.h"
#include "mcu_timer.h"

#endif //_chinchiller_common_mcu_h_
