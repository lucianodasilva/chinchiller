#pragma once
#ifndef _common_drivers_lcd_h_
#define _common_drivers_lcd_h_

#include "common/mcu.h"

namespace drivers {

	struct pos {
		uint8_t x, y;
	};

	template < 
		mcu::io::pin_num_t _rs_pin_n,
		mcu::io::pin_num_t _e_pin_n,
		mcu::io::pin_num_t _data0_pin_n,
		mcu::io::pin_num_t _data1_pin_n,
		mcu::io::pin_num_t _data2_pin_n,
		mcu::io::pin_num_t _data3_pin_n
	>
	struct lcd {

		mcu::io::pin < _rs_pin_n >		rs_pin		= {mcu::io::pin_mode::output};
		mcu::io::pin < _e_pin_n >		e_pin		= {mcu::io::pin_mode::output};
		mcu::io::pin < _data0_pin_n >	data0_pin	= {mcu::io::pin_mode::output};
		mcu::io::pin < _data1_pin_n >	data1_pin	= {mcu::io::pin_mode::output};
		mcu::io::pin < _data2_pin_n >	data2_pin	= {mcu::io::pin_mode::output};
		mcu::io::pin < _data3_pin_n >	data3_pin	= {mcu::io::pin_mode::output};

		inline void clear () const {
			command (cmd_clear);
			mcu::delay(2);
		}

		inline void home () const {
			command (cmd_return_home);
			mcu::delay(2);
		}

		inline void set_display_on () {
			_disp_ctrl |= ctrl_display_on;
			command (cmd_display_control, _disp_ctrl);
		}

		inline void set_display_off () {
			_disp_ctrl &= ~ctrl_display_on;
			command (cmd_display_control, _disp_ctrl);
		}

		inline void set_cursor (uint8_t x, uint8_t y) const {
			uint8_t c_index = x + (y * 40);
			command (cmd_set_ddramaddr, c_index);
		}

		void init () {

			// function flags
			_disp_func 
				= 0x00 /* 4 bit */
				| 0x08 /* 2 line */
				| 0x00 /* 5x8 dot */;

			// wait for lcd init
			mcu::delay (50);

			// reset pins
			rs_pin	.set_low();
			e_pin	.set_low();
			
			// set command sequence for 4 bit mode
			write_4(0x03);
			mcu::delay_us(4500);

			write_4(0x03);
			mcu::delay_us(4500);

			write_4(0x03);
			mcu::delay_us(150);
			
			write_4(0x02);

			command(cmd_function_set, _disp_func);

			// set default display control mode
			_disp_ctrl = ctrl_display_on | ctrl_cursor_off | ctrl_blink_off;

			set_display_on();
			clear();

			// set default display mode
			_disp_mode = mode_entry_left | mode_entry_shift_dec;

			command (cmd_entry_mode_set, _disp_mode);
		}

		inline void set_precision (uint8_t v) {
			_precision = v;
		}

		inline lcd & operator << (const pos & p) {
			set_cursor(p.x, p.y);
			return *this;
		}

		lcd & operator << (const char * s) {
			while (*s)
				send (*s++, 1);

			return *this;
		}

		lcd & operator << (int32_t v) {
			char buffer [16];
			itoa(v, +buffer, 10);

			return operator << (+buffer);
		}

		lcd & operator << (uint32_t v) {
			char buffer [16];
			utoa(v, +buffer, 10);

			return operator << (+buffer);
		}

		lcd & operator << (double v) {
			char buffer [16];
			dtostrf(v, 0, _precision, +buffer);

			return operator << (+buffer);
		}

	private:

		inline void command (uint8_t c, uint8_t p = 0) const {
			send (c | p, 0);
		}

		inline void send (uint8_t value, uint8_t rs_mode) const {
			rs_pin.set(rs_mode);

			write(value);
		}

		inline void write (uint8_t v) const {
			write_4 (v >> 4);
			write_4 (v);
		}

		inline void write_4 (uint8_t v) const {
			write_to(v, data0_pin, data1_pin, data2_pin, data3_pin);
			pulse();
		}

		template < class _pin_t >
		inline void write_to (uint8_t & v, _pin_t & p) const {
			p.set(v);
			v = v >> 1;
		}

		template < class _pin0_t, class ... _pin_t >
		inline void write_to (uint8_t v, _pin0_t & pin0, _pin_t & ... pins) const {
			write_to < _pin0_t > (v, pin0);
			write_to (v, pins...);
		}
		
		inline void pulse () const {
			e_pin.set_low();
			mcu::delay_us(1);
			e_pin.set_high();
			mcu::delay_us(1);
			e_pin.set_low();
			mcu::delay_us(100);
		}

		uint8_t _disp_func;
		uint8_t _disp_ctrl;
		uint8_t _disp_mode;

		uint8_t _precision = 2;

		static constexpr uint8_t const
			// commands
			cmd_clear = 0x01,
			cmd_return_home = 0x02,
			cmd_entry_mode_set = 0x04,
			cmd_display_control = 0x08,
			cmd_cursor_shift = 0x10,
			cmd_function_set = 0x20,
			cmd_set_cgramaddr = 0x40,
			cmd_set_ddramaddr = 0x80,
			// display entry mode
			mode_entry_right = 0x00,
			mode_entry_left = 0x02,
			mode_entry_shift_inc = 0x01,
			mode_entry_shift_dec = 0x00,
			// display on / off
			ctrl_display_on = 0x04,
			ctrl_display_off = 0x00,
			ctrl_cursor_on = 0x02,
			ctrl_cursor_off = 0x00,
			ctrl_blink_on = 0x01,
			ctrl_blink_off = 0x00,
			// display/cursor shift
			cursor_display_move = 0x08,
			cursor_cursor_move = 0x00,
			cursor_move_right = 0x04,
			cursor_move_left = 0x00;

	};

}

#endif