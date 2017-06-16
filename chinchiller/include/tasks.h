#pragma once
#ifndef _common_tasks_h_
#define _common_tasks_h_

#include "common/mcu.h"

using task_ptr_t = void (*)();

class task {
public:
	virtual ~task() = default;

	virtual void run () = 0;
};

class _ptr_task : public task {
public:

	virtual void run () override { 
		if (_f)
			_f(); 
	}

	_ptr_task (task_ptr_t f) : _f{f} {}

private:
	task_ptr_t _f;
};

inline auto make_task (task_ptr_t callback) {
	return new _ptr_task (callback);
}

template < size_t _capacity >
class _timed_task : public task {
public:

	template < class ... _t >
	_timed_task (size_t freq, _t * ... t) :
		_freq(freq),
		_tasks {t...}
	{}

	virtual void run () override {
		size_t time_frame = mcu::millis() / _freq;

		if (time_frame != _task_frame) {
			_task_frame = time_frame;

			for (auto * t : _tasks)
				t->run ();
		}
	}

	virtual ~_timed_task() {
		for (auto * t : _tasks)
			delete t;
	}

private:
	size_t      _task_frame = 0;
	size_t      _freq;
	task *      _tasks [_capacity];
};

template < class ... _t >
inline auto make_timed_task (size_t freq, _t * ... tasks) {
	return new _timed_task<sizeof...(_t)> (freq, tasks...);
}

template < mcu::io::pin_num_t _pin_n >
class _button_task : public _ptr_task {
public:

	static uint32_t const debounce_constant = 50;

	mcu::io::pin < _pin_n > pin = { mcu::io::pin_mode::pullup };

	virtual void run () override {
		if (pin.get() == 0) {
			if (_pressed) {
				_ptr_task::run ();
			} else {
				if (_debounce) {
					if ((mcu::millis () - _time_stamp) > debounce_constant) {
						_pressed = true;
						if (_on_press)
							_on_press();
					}
				} else {
					_time_stamp = mcu::millis();
					_debounce = true;
				}
			}

		} else if (_pressed) {
			_pressed = false;
			_debounce = false;
			_time_stamp = 0;

			if (_on_release)
				_on_release();
		}
	}

	_button_task (task_ptr_t on_press, task_ptr_t on_release, task_ptr_t callback) : 
		_ptr_task (callback),
		_on_press(on_press),
		_on_release(on_release)
	{}

private:

	task_ptr_t _on_press;
	task_ptr_t _on_release;

	bool 		_pressed 	= false;
	bool 		_debounce 	= false;
	uint32_t 	_time_stamp = 0;

};

template < mcu::io::pin_num_t _pin_n >
inline auto make_button_task (task_ptr_t on_press, task_ptr_t on_release = nullptr, task_ptr_t callback = nullptr) {
	return new _button_task < _pin_n > (on_press, on_release, callback);
}

template < size_t _capacity >
struct __task_executor {
	task * tasks [_capacity];

	void run () {
		for (auto * t : tasks)
			t->run ();
	}

	~__task_executor(){
		for (auto * t : tasks)
			delete t;
	}
};

template < class ... _t >
inline auto make_task_executor(_t ... tasks) {
	return new __task_executor <sizeof...(_t)> {
		tasks...
	};
}

#endif