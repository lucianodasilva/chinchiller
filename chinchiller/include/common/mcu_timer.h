#pragma once
#ifndef _chinchiller_common_mcu_timer_h_
#define _chinchiller_common_mcu_timer_h_

#include "mcu.h"

namespace mcu {

    using timer_num_t = size_t;

    template < timer_num_t timer_num >
    struct timer {

        // timer info
        constexpr static auto const &   traits      = mcu::hardware::timer_traits_lookup [timer_num];

        using trait_types                           = typename mcu::hardware::timer_type_traits < timer_num >;

        // compile-time validation
        static_assert (timer_num <= mcu::hardware::available_timer_count, "invalid timer number");
        static_assert (traits !=  mcu::hardware::timer_null, "unsupported timer");

    };

}

#endif // _chinchiller_common_mcu_timer_h_