#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0x00;
	
	while(1) {
		PORTB |= (1<<0);
		_delay_ms(500);
		
		PORTB &= ~(1<<0);
		_delay_ms(500);
	}
}
