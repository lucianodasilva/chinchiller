#pragma once
#ifndef _common_tasks_h_
#define _common_tasks_h_

#include "common/mcu.h"

template<class _ty>
struct _remove_reference
{   // remove reference
	typedef _ty _type;
};

template<class _ty>
struct _remove_reference<_ty&>
{   // remove reference
	typedef _ty _type;
};

template<class _ty>
struct _remove_reference<_ty&&>
{   // remove rvalue reference
	typedef _ty _type;
};

template<class _t> inline
typename _remove_reference<_t>::_type && move(_t && arg)
{   // forward _Arg as movable
	return ((typename _remove_reference<_t>::_type &&)arg);
}

class task {
public:
	virtual ~task() = default;

	virtual void run () = 0;
};

template < class _ft >
class __lambda_task : public task {
public:

	virtual void run () override { _f(); }

	explicit __lambda_task (_ft && f) : _f(f) {}

private:
	_ft _f;
};

template < class _ft >
inline auto make_task (_ft && f) {
	return new __lambda_task < _ft > (move(f));
}

template < size_t _capacity >
class __timed_task : public task {
public:

	template < class ... _t >
	__timed_task (size_t freq, _t * ... t) :
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

	virtual ~__timed_task() {
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
	return new __timed_task<sizeof...(_t)>(freq, tasks...);
}

template < mcu::io::pin_num_t _pin_n, class _ft >
class __button_task : public __lambda_task < _ft > {
public:

	static uint32_t const debounce_constant = 100;

	mcu::io::pin < _pin_n > pin = { mcu::io::pin_mode::pullup };

	virtual void run () override {
		if (pin.get() == 1) {

			if (_pressed) {
				__lambda_task< _ft >::run ();
			} else {
				if (_debounce) {
					if ((mcu::millis () - _time_stamp) > debounce_constant) {
						_pressed = true;
						__lambda_task< _ft >::run ();
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
		}
	}

	explicit __button_task (_ft && f) : __lambda_task < _ft > (f) {}

private:

	bool 		_pressed 	= false;
	bool 		_debounce 	= false;
	uint32_t 	_time_stamp = 0;

};

template < mcu::io::pin_num_t _pin_n, class _ft >
inline auto make_button_task (_ft && f) {
	return new __button_task < _pin_n, _ft > (move(f));
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
	return __task_executor <sizeof...(_t)> {
		tasks...
	};
}

#endif