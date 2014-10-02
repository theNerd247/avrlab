#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD = 0xFF;
	PORTD = 0x00;
	
	while(1) {
		PORTD |= (1<<0);
		_delay_ms(500);
		
		PORTD &= ~(1<<0);
		_delay_ms(500);
	}
}
