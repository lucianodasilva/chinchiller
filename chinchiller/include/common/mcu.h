#pragma once
#ifndef _chinchiller_common_mcu_h_
#define _chinchiller_common_mcu_h_

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

template<class _ty>
struct _remove_reference
{   // remove reference
	typedef _ty _type;
};

template<class _ty>
struct _remove_reference<_ty&>
{   // remove reference
	typedef _ty _type;
};

template<class _ty>
struct _remove_reference<_ty&&>
{   // remove rvalue reference
	typedef _ty _type;
};

template<class _t> inline
constexpr typename _remove_reference<_t>::_type && move(_t && arg)
{   // forward _Arg as movable
	return ((typename _remove_reference<_t>::_type &&)arg);
}

void * operator new(size_t size);
void operator delete(void * ptr);
void operator delete(void * ptr, size_t size);

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

extern "C" void __cxa_pure_virtual(void);

namespace mcu {

    void setup ();

    uint32_t millis ();
	uint32_t micros ();

	void delay (uint32_t ms);
	void delay_us (uint32_t us);

    using reg_ptr_t = uint16_t;

#   define to_reg(r)(reg_ptr_t)&r
#   define from_reg(r)(volatile uint8_t *)r

    constexpr reg_ptr_t const reg_ptr_null = {};

    namespace hardware {

		enum class analog_channel : uint8_t;

        struct _timer_traits {
            reg_ptr_t tccra;
            reg_ptr_t tccrb;
            reg_ptr_t tcnt;
            reg_ptr_t timsk;
            reg_ptr_t tifr;
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

        namespace io {

            struct _port_traits {
                reg_ptr_t ddr;
                reg_ptr_t port;
                reg_ptr_t pin;
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
                _port_traits    port;
                uint8_t         port_bit;
                _timer_traits   timer;
                reg_ptr_t       timer_ocr;
                uint8_t         timer_com_bit;
				analog_channel	adc_channel;
            };

            inline constexpr bool operator==(const _pin_traits &v1, const _pin_traits &v2) {
                return
                        v1.port == v2.port &&
                        v1.port_bit == v2.port_bit;
            }

            inline constexpr bool operator!=(const _pin_traits &v1, const _pin_traits &v2) {
                return !(operator==(v1, v2));
            }

            constexpr _pin_traits const pin_null = {};

			struct _adc_traits {
				reg_ptr_t mux;
				reg_ptr_t csra;
				reg_ptr_t data_l;
				reg_ptr_t data_h;
			};

        } // namespace io

    } // namespace hardware

    template < class _t = uint8_t >
    inline void cbi(reg_ptr_t r, uint8_t bit) {
        auto nr = (volatile _t *)(r);
        (*nr)&= ~_BV(bit);
    }

    template < class _t = uint8_t >
    inline void sbi(reg_ptr_t r, uint8_t bit) {
        auto nr = (volatile _t *)(r);
        (*nr) |= _BV(bit);
    }

    template < class _t >
    constexpr inline _t make_mask (uint8_t n, uint8_t shift = 0) {
        return ~(~_t(0) << n) << shift;
    }

    template < class _t >
    constexpr _t inline get_bit (_t src, uint8_t n) {
        return (src & (_t(1) << n)) >> n;
    }

    template < class _t >
    constexpr inline void set_bit (_t & src, uint8_t v, uint8_t n) {
		auto bit_mask =  ((_t (1) & (v & 0x01)) << n);
        src = (src & ~(1 << n)) | bit_mask;
    }

    template < class _t >
    constexpr inline void write_n (reg_ptr_t r, _t v, uint8_t width, uint8_t shift) {
        auto nr = (volatile _t *)(r);
        auto mask = make_mask < _t > (width, shift);
        (*nr) = ((*nr) & ~mask) | (v << shift);
    }

    struct interrupt_guard {
        uint8_t sreg_backup{};

        inline interrupt_guard() : sreg_backup(SREG) { cli(); }

        ~interrupt_guard() { SREG = sreg_backup; }
    };

    template<class _f_t, class _s_t>
    struct pair {
        _f_t first;
        _s_t second;
    };

    template<class _t, size_t _s>
    constexpr inline size_t array_size(_t (& /*unused*/)[_s]) {
        return _s;
    };

    template < bool v, class _v1, class _v2 >
    struct type_or {
        using type = _v1;
    };

    template <class _v1, class _v2>
    struct type_or < false, _v1, _v2> {
        using type = _v2;
    };

	template < class _t >
	constexpr inline _t clamp (_t const & min, _t const & max, _t const & v) {
		return v < min ? min : ( v > max ? max : v );
	}

} // namespace mcu

// include mcu specific details
#if defined (__AVR_ATmega328P__)
#   include "mcu_atmega328p.h"
#endif

// include hardware manipulators
#include "mcu_io_pin.h"
#include "mcu_io_adc.h"
#include "mcu_timer.h"

#endif //_chinchiller_common_mcu_h_
