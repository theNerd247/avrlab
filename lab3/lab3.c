#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

/** the digit value port to use */
#define DGOUTPORT PORTD

/** the digit selection port to use */
#define DGSELPORT PORTC

/** sets the 7SEG display digit value */
#define SETDG(v) DGOUTPORT = ~(nums[v])

/** turns on the requested digit */
#define DGSEL(v) DGSELPORT = digits[v]

//--CONSTANTS------------------------------
//7SEG IOPIN CONFIG (pins 0-6):   0   1    2    3    4    5    6     7   8    9
const unsigned char nums[10] = {0x77,0x24,0x5d,0x6d,0x2e,0x6b,0x7b,0x25,0x7f,0x6f};

//pins to set the digit to use   DG2   DG1
const unsigned char digits[2] = {0x01,0x02};

/** the digit to index mapping */
#define DG1 1
#define DG2 0

/** the current number to display (0-99) */
typedef struct
{
	uint8_t dsel:1;
	uint8_t dval[2];
} digit_cfg_t;
digit_cfg_t digit_cfg;

//--END CONSTANTS---------------------------

/**
 * @brief sets the iopins for the 7SEG display
 * 
 * @param num - the number to output (between 0 and 99)
 *
 * @return void
 * 
 */
void setNum(uint8_t num)
{
	if(num > 99)
		return;

	digit_cfg.dval[DG1] = num%10;
	digit_cfg.dval[DG2] = num/10;
}

/**
 * @brief clock interupt service routine
 * 
 * the digits to multiplex are 
 *
 * void
 * @return void
 * 
 */
/*ISR(TIMER2_COMPA_vect)*/
void clockISR(void)
{
	//quickly turn off the digits before switching 
	DGSELPORT = 0x00;

	/** switch the digit to use and set the 7SEG value */
	digit_cfg.dsel ^= 1;
	SETDG(digit_cfg.dval[digit_cfg.dsel]);

	/** turn on the 7SEG digit */
	DGSEL((digit_cfg.dsel));

	//allow more interupts
	/*TIFR2 = 0x02;*/
}

/**
 * @brief sets up the timer
 * 
 * void
 * @return void 
 * 
 */
void setupclk(void)
{
	TCCR2A = 0x02; /** sets the output mode to normal */
	TCNT2 = 0;
	TCCR2B = 0x01; /** set to toggle output */
	OCR2A |= 0x01; /** every 1 cycle (10ms) call ISR */
	TIMSK2 |= 0x02;
}

int main(int argc, char const *argv[])
{
	DDRD = 0xff;
	DDRC = 0x3;

	uint8_t cnt = 0;
	uint8_t cnt2 = 0;
	
	digit_cfg.dsel = DG1;
	digit_cfg.dval[DG1] = 0;
	digit_cfg.dval[DG2] = 0;

	DGSEL(digit_cfg.dsel);
	SETDG(digit_cfg.dval[digit_cfg.dsel]);

	/*setupclk();*/

	while(1)
	{
		clockISR();
		_delay_ms(10);
		if(cnt2 == 50)
		{
			if(cnt == 100)
				cnt = 0;
			setNum(cnt++);
			cnt2 = 0;
		}
		cnt2++;
	}

	return 0;
}
