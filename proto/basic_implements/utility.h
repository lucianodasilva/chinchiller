#pragma once
#ifndef _utility_h_
#define _utility_h_

#include "type_traits.h"

namespace common {

    template <typename T>
    typename remove_reference<T>::type&& move(T&& arg)
    {
        return static_cast<typename remove_reference<T>::type&&>(arg);
    }

}

#endif