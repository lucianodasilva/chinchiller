#include "common/mcu.h"

volatile uint32_t _timer_ovf_millis = 0;
volatile uint32_t _timer_ovf_counter_f = 0;

constexpr uint32_t const _f_cpu = 16000000;
constexpr uint32_t const _cycles_to_microseconds =  _f_cpu / 1000000;
constexpr uint32_t const _microseconds_per_timer0_ovf = 16384 / _cycles_to_microseconds;
constexpr uint32_t const _cycles_to_milliseconds = _cycles_to_microseconds / 1000;

constexpr uint32_t const _frac_inc = (_cycles_to_milliseconds % 1000) >> 3;
constexpr uint32_t const _frac_max = (1000 >> 3);

// use timer0 as cpu time counter
ISR(TIMER0_OVF_vect) {
    ++_timer_ovf_millis;
    _timer_ovf_counter_f += 3;

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

}