#include "common/mcu_io_pin.h"
#include "common/mcu_timer.h"


int main (int arg_c, char ** arg_v) {

    mcu::setup();

    /*
    mcu::io::pin < 5 > pin (mcu::io::pin_mode::output);

    mcu::timer < 0 > timer;

    pin.set (mcu::io::pin_state::high);
    pin.set_compare_mode(mcu::hardware::compare_output_mode::toggle);

    timer.set_clock_selection(mcu::hardware::clock_select::clk_io_64);
*/
    while(1){
        mcu::millis();
    }

    return 0;
}