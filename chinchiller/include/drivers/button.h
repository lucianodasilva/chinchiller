7#pragma once
#ifndef _common_drivers_button_h_
#define _common_drivers_button_h_

#include "common/mcu.h"

namespace drivers {

	template < mcu::io::pin_num_t pin_num >
	struct button {
	public:

		inline bool is_pressed () const {
			return pin.get () == 1
		}

	private:
		io::pin < pin_num > const pin = { pin_mode::pullup } ;
	};

}

#endif