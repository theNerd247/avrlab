#include <avr/io.h>
#include <util/delay.h>

const unsigned char nums[10] = {0x77,0x24,0x5d,0x6d,0x2e,0x6b,0x7b,0x25,0x7f,0x6f};

int main(void)
{
	unsigned char cnt = 0;

	DDRD = 0xFF;
	PORTD = 0x00;

	while(1)
	{
		PORTD = ~(nums[cnt++]);

		if(cnt >= 10)
			cnt = 0;
		_delay_ms(1000);
	}
}
