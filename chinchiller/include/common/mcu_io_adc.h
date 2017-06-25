#pragma once
#ifndef _chinchiller_common_mcu_io_adc_h_
#define _chinchiller_common_mcu_io_adc_h_

#include "mcu.h"
#include <avr/sleep.h>

namespace mcu {
	namespace io {

		enum struct adc_prescaler : uint8_t {
			none = 0b000,
			p2 = 0b001,
			p4 = 0b010,
			p8 = 0b011,
			p16 = 0b100,
			p32 = 0b101,
			p64 = 0b110,
			p128 = 0b111
		};

		namespace adc {

			inline void enable() {
				sbi(hardware::io::adc_traits.csra, ADEN);
			}

			inline void disable () {
				cbi(hardware::io::adc_traits.csra, ADEN);
			}

			inline void set_reference (hardware::voltage_reference ref) {
				mcu::write_n(hardware::io::adc_traits.mux, (uint8_t)ref, 2, REFS0);
			}

			inline uint16_t read (hardware::analog_channel channel, adc_prescaler prescaler) {
				// select channel
				mcu::write_n(hardware::io::adc_traits.mux, (uint8_t)channel, 4, 0);
				// set prescaler
				mcu::write_n(hardware::io::adc_traits.csra, (uint8_t)prescaler, 3, 0);
				
				// start conversion (average four readings)
				uint16_t v = 0;

				for (uint8_t i = 0; i < 4; ++i) {
					sbi(hardware::io::adc_traits.csra, ADSC);

					while (!mcu::get_bit(*from_reg(hardware::io::adc_traits.csra), ADIF)){}

					sbi(hardware::io::adc_traits.csra, ADIF);

					v += ADC;
				}

				return v / 4;
			}

		};

	}
}

#endif