#include <arduino.h>
#include <util/delay.h>

using namespace arduino;

//io::pin < 13, io::pin_mode::output > led;
io::pin < 13, io::pin_mode::output > led;

int main () {

	// while (true) {
		//led.set_high ();
		// _delay_ms (1000);
		// led.set_low ();
		// _delay_ms (1000);
	// }

	led.set_high ();

	return 0;
}
