#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/** sets the 7SEG display digit value */
#define SETDG(v) DGOUTPORT = ~(nums[v])

/** turns on the requested digit */
#define DGSEL(v) DGSELPORT = digits[v]

#define BITON(port,pin) port |= _BV(pin)
#define BITOFF(port,pin) port &= ~_BV(pin)
#define SETPORT(port,mask,value) port = ((port & ~(mask)) | value)
#define TOGGLEBIT(port,bit) port ^= _BV(bit)

//--CONSTANTS------------------------------
//--END CONSTANTS---------------------------

/**
 * @brief clock interupt service routine
 * 
 * toggles PC3
 *
 * void
 * @return void
 * 
 */
ISR(TIMER0_COMPA_vect)
{
	TOGGLEBIT(PORTC,PORTC3);
}

/**
 * @brief sets up the timer0 to CTC mode for 4.8ms 
 * 
 * @return void 
 * 
 */
void setupclk(void)
{
	/** disable the interrupt before config */
	BITOFF(TIMSK0,OCIE0A); 

	/** set the operation mode to CTC */
	SETPORT(TCCR0A,0xFF,_BV(WGM01));

	/** setup prescaler to 256*/
	SETPORT(TCCR0B,0x0F,_BV(CS02));

	/** set output compare register to 150 */
	SETPORT(OCR0A,0xFF,150);

	/** set counter */
	SETPORT(TCNT0,0xFF,0);

	/** enable interrupt */
	BITON(TIMSK0,OCIE0A);
}

int main(int argc, char const *argv[])
{
	//set PC3 to be output
	BITON(DDRC,DDC3);

	//enable global interrupts (must be done for interrupt to work, see
	//7.7 of datasheet for details)
	sei();

	//start running the timmer/interrupt
	setupclk();

	while(1);

	return 0;
}
