#include "common/mcu.h"
#include "tasks.h"

int main (int arg_c, char ** arg_v) {

    mcu::setup();

	auto exec = make_task_executor();

    while(1){
        exec.run();
    }

    return 0;
}