#pragma once
#ifndef _chinchiller_common_mcu_h_
#define _chinchiller_common_mcu_h_

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

namespace common {

    namespace mcu {

        union reg_ptr {
            uint16_t    address;
            uint8_t *   u8_ptr;
            uint16_t *  u16_ptr;
        };

        inline constexpr bool operator == (const reg_ptr & v1, const reg_ptr & v2) {
            return v1.address == v2.address;
        }

        inline constexpr bool operator != (const reg_ptr & v1, const reg_ptr & v2) {
            return !(operator == (v1, v2));
        }

        namespace io {

            struct _port_traits {
                reg_ptr ddr;
                reg_ptr port;
                reg_ptr port_pin;
            };

            inline constexpr bool operator == (const _port_traits & v1, const _port_traits & v2) {
                return
                        v1.ddr == v2.ddr &&
                        v1.port == v2.port &&
                        v1.port_pin == v2.port_pin;
            }

            inline constexpr bool operator != (const _port_traits & v1, const _port_traits & v2) {
                return !(operator == (v1, v2));
            }

            constexpr _port_traits const port_null = {};

            struct _pin_traits {
                _port_traits    port;
                uint8_t         bit;
            };

            inline constexpr bool operator == (const _pin_traits & v1, const _pin_traits & v2) {
                return
                        v1.port == v2.port &&
                        v1.bit == v2.bit;
            }

            inline constexpr bool operator != (const _pin_traits & v1, const _pin_traits & v2) {
                return !(operator == (v1, v2));
            }

            constexpr _pin_traits const pin_null = {};

        }
    }

    template < class _sbt, class _bt >
    inline void cbi (_sbt sfr, _bt bit) {
        _SFR_BYTE(sfr) &= ~_BV(bit);
    }

    template < class _sbt, class _bt >
    inline void sbi (_sbt sfr, _bt bit) {
        _SFR_BYTE(sfr) |= _BV(bit);
    }

    struct interrupt_guard {
        uint8_t sreg_backup;

        inline interrupt_guard () : sreg_backup(SREG){ cli(); }
        ~interrupt_guard() { SREG = sreg_backup; }
    };

    template < class _f_t, class _s_t >
    struct pair {
        _f_t first;
        _s_t second;
    };

    template < class _t , size_t _s >
    constexpr inline size_t array_size (_t (&)[_s]) {
        return _s;
    };

}

// include mcu specific details
//#if defined (__ATmega328P__)
#   include "mcu_atmega328p.h"
//#endif

#endif //_chinchiller_common_mcu_h_
