#include "common/mcu_io_pin.h"

int main (int arg_c, char ** arg_v) {

    mcu::io::pin < 12 > pin_12 (mcu::io::pin_mode::output);

    pin_12.set (mcu::io::pin_state::high);

    return 0;
}