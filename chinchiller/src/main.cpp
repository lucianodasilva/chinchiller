#include "common/mcu.h"
#include "drivers/lcd.h"

#include "tasks.h"

int main (int arg_c, char ** arg_v) {

	using namespace drivers;

    mcu::setup();

	lcd <
		17, // rs
		19, // e
		11,	// data 0
		6,	// data 1
		5,	// data 2
		4 // data 3
	>
		display;

	auto exec = make_task_executor();

	display.init();

	display << pos {0, 0} << "Temp: 16 V: 100%" << pos { 0, 1 } << "Min: 20 Max:[40]";

    while(1){
        exec.run();
    }
		
    return 0;
}