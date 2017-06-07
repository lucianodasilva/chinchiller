#include <iostream>

#include <list>
#include <memory>
#include <utility>
#include <chrono>

namespace common {
    namespace system {

        inline size_t cpu_time () {
            return std::chrono::duration_cast < std::chrono::milliseconds > (
                    std::chrono::system_clock::now().time_since_epoch()
            )
                    .count();
        }

    }

    template < class _t, size_t _capacity >
    struct array {

    };
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
    return new __lambda_task < _ft > (std::move(f));
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
        size_t time_frame = common::system::cpu_time() / _freq;

        if (time_frame != _task_frame) {
            _task_frame = time_frame;

            for (auto * t : _tasks)
                t->run ();
        }
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

template < size_t _capacity >
struct __task_executor {
    task * tasks [_capacity];

    void run () {
        for (auto * v : tasks)
            v->run ();
    }
};

template < class ... _t >
inline auto make_task_executor(_t ... tasks) {
    return __task_executor <sizeof...(_t)> {
            tasks...
    };
}

int main() {

    auto exec = make_task_executor (
            make_timed_task (500, make_task([](){ std::cout << "500 ms" << std::endl; })),
            make_timed_task (1000, make_task([](){ std::cout << "1000 ms" << std::endl; }))
    );

    while (true)
        exec.run ();

    return 0;
}