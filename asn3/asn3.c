#include <stdint.h>
#include <avr/io.h>

//incase we forget to add this to compiler options
#define F_CPU 1000000 //1 MHZ
#include <util/delay.h>

#define SETPIN(port) (1 << port)

int main(int argc, char const *argv[])
{
	uint8_t i = 0;
	DDRD = 0;

	/** setup io config */
  DDRD = 0x0F; //0-3 out; 4-7 in

	/** setup pullup resistors */
	PORTD &= SETPIN(PORTD6) | SETPIN(PORTD7);

	while(1)
	{
		if(!PINC6)
		{
			for(i = 0; i < 4; i++)
			{
				PORTD = (PORTD & 0xF0) | SETPIN((PORTD0+i));
				_delay_ms(50);
			}
		}
	}

	return 0;
}
