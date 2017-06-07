#include "common/io.pin.h"

int main (int arg_c, char ** arg_v) {

    common::io::pin < 12 > pin_12 (common::io::pin_mode::output);

    return 0;
}