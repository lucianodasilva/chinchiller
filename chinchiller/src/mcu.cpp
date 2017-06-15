#include "common/mcu.h"

#define F_CPU 16000000L
#include <util/delay.h>

volatile uint32_t _timer_ovf_millis = 0;
volatile uint32_t _timer_ovf_counter_f = 0;

constexpr uint32_t const _cycles_to_microseconds =  F_CPU / 1000000;
constexpr uint32_t const _microseconds_per_timer0_ovf = 16384 / _cycles_to_microseconds;
constexpr uint32_t const _milliseconds_per_timer0_ovf = _cycles_to_microseconds / 1000;

constexpr uint32_t const _frac_inc = (_microseconds_per_timer0_ovf % 1000) >> 3;
constexpr uint32_t const _frac_max = (1000 >> 3);

// use timer0 as cpu time counter
ISR(TIMER0_OVF_vect) {
    _timer_ovf_millis += _milliseconds_per_timer0_ovf;
    _timer_ovf_counter_f += _frac_inc;

    if (_timer_ovf_counter_f >= _frac_max) {
        _timer_ovf_counter_f -= _frac_max;
        ++_timer_ovf_millis;
    }
}

namespace mcu {

    void setup () {
        // setup cpu time counter in timer 0
        using timer_t = timer < 0 >;
        using clock_selection = typename timer_t::trait_types::clock_select_enum;

        timer_t timer;

        timer.set_clock_selection(clock_selection::clk_io_64);
        // enable overflow interrupt
        sbi(timer_t::traits.timsk, TOIE0);

        sei();
    }

    uint32_t millis () {
        interrupt_guard iguard;
        return _timer_ovf_millis;
    }

	uint32_t micros () {
		interrupt_guard iguard;

		using timer_t = timer < 0 >;

		uint32_t m = _timer_ovf_counter_f;
		uint8_t t = *from_reg(timer_t::traits.tcnt);

		if ((*from_reg(timer_t::traits.tifr) & _BV(TOV0)) && (t < 255))
			++m;

		return ((m << 8) + t) * (64 / _cycles_to_microseconds);
	}

	void delay (uint32_t ms) {
		while (ms > 0) {
			_delay_ms(1);
			--ms;
		}
	}

	void delay_us (uint32_t us) {
		while (us > 0) {
			_delay_us(1);
			--us;
		}
	}

}