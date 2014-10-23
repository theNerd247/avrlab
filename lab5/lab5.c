#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include "util.h"

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
/*void clockISR(void)*/
ISR(TIMER2_COMPA_vect)
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

ISR(ADC_vect)
{
	uint8_t temp;
	uint16_t adc;

	/** ADC value*/
	adc = ADCL;
	adc |= (ADCH << 8);

	/** convert ADC value to temperature */
	temp = -0.001688(R1*(1024/adc-1))+41.995;

	/** display the Number*/
	setNum(temp);
}

#ifndef MODE1
/**
 * @brief sets up the timer
 * 
 * void
 * @return void 
 * 
 */
void setupclk(void)
{
	/** disable the interrupt before config */
	BITOFF(TIMSK2,OCIE2A); 

	/** set to normal operation */
	BITOFF(TCCR2A,COM2A1);
	BITOFF(TCCR2A,COM2A0);

	/** set the operation mode to CTC */
	BITON(TCCR2A,WGM21); 
	BITOFF(TCCR2A,WGM20);

	/** make sure we use the internal clock */
	BITOFF(ASSR,AS2);

	/** setup prescaler to 8*/
	SETPORT(TCCR2B,0x0F, _BV(CS21));

	/** set output compare */
	SETPORT(OCR2A,0xFF,19);

	/** set counter */
	SETPORT(TCNT2,0xff,0);

	/** enable interrupt */
	BITON(TIMSK2,OCIE2A);
}
#endif

int main(int argc, char const *argv[])
{
	SETPORT(DDRD,0xff,0xff);
	SETPORT(DDRC,0x2,0x3);

	uint8_t cnt = 0;
	uint8_t cnt2 = 0;
	
	digit_cfg.dsel = DG1;
	digit_cfg.dval[DG1] = 0;
	digit_cfg.dval[DG2] = 0;

	DGSEL(digit_cfg.dsel);
	SETDG(digit_cfg.dval[digit_cfg.dsel]);

	#ifndef MODE1
	sei();
	setupclk();
	#endif

	while(1)
	{
	#ifdef MODE1
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
	#else
		if(cnt == 100)
			cnt = 0;
		setNum(cnt++);
		_delay_ms(250);
	#endif
	}

	return 0;
}
