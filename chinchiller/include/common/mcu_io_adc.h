#pragma once
#ifndef _chinchiller_common_mcu_io_adc_h_
#define _chinchiller_common_mcu_io_adc_h_

#include "mcu.h"
#include <avr/sleep.h>

namespace mcu {
	namespace io {

		enum struct voltage_reference : uint8_t {
			aref = 0b00,
			avcc = 0b01,
			internal = 0b11
		};
		
		enum struct analog_channel : uint8_t {
			acd0 = 0b0000,
			acd1 = 0b0001,
			acd2 = 0b0010,
			acd3 = 0b0011,
			acd4 = 0b0100,
			acd5 = 0b0101
		};

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

			inline void set_reference (voltage_reference ref) {
				mcu::write_n(hardware::io::adc_traits.mux, (uint8_t)ref, 2, REFS0);
			}

			inline uint16_t read (analog_channel channel, adc_prescaler prescaler) {
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