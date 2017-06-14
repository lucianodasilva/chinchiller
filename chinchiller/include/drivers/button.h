#pragma once7
#ifndef _common_button_h_
#define _common_button_h_

#include "common/mcu.h"

namespace drivers {

	template < mcu::io::pin_num_t pin_num >
	struct button {
	public:

		inline bool is_pressed () const {
			return pin.get () == pin_state::high;
		}

	private:
		io::pin < pin_num > const pin = { pin_mode::pullup } ;
	};

}

#endif