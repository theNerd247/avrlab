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

//--GLOBALS------------------------------
//7SEG IOPIN CONFIG (pins 0-6):   0   1    2    3    4    5    6     7   8    9
const unsigned char nums[10] = {0x77,0x24,0x5d,0x6d,0x2e,0x6b,0x7b,0x25,0x7f,0x6f};

//pins to set the digit to use   DG2   DG1
const unsigned char digits[2] = {_BV(0),_BV(1)};

/** the digit to digits[] index mapping */
#define DG1 1
#define DG2 0

/** display config */
typedef struct
{
	/** the current digit to display */
	uint8_t dsel:1;
	/** @brief digit values to display
	 *
	 * stored as indexes to nums[] for their respective values (0-9)
	 */
	uint8_t dval[2];
} digit_cfg_t;
digit_cfg_t digit_cfg;
//--END GLOBALS---------------------------

//--GLOBALS------------------------------
uint16_t adc; /** the ADC output value */
//--END GLOBALS---------------------------

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

uint8_t ind = 0;
static const uint16_t tempData[][2] =
{
	{241,0},
	{343,10},
	{445,20},
	{567,30},
	{668,40},
	{753,50},
	{871,60},
	{910,70},
	{938,80}
};

#define U tempData[ind]
#define L tempData[ind-1]
/**
 * @brief gets the input temperature from the adc value
 * 
 * raw is linearly interpollated via the below table: 
 * **NOTE:** This function is an inline function and is meant to be used in an
 * ISR
 *
 *  ADC | Temp (degrees C)
 * -----|------------------
 *  241 | 10
 *  343 | 20
 *  445 | 30
 *  567 | 40
 *  668 | 50
 *  753 | 60
 *  871 | 70
 *  910 | 80
 *  938 | 90
 *
 * @param raw - the raw ADC value (should be read directly from the ADCH/L
 * registers)
 *
 * @return void
 * 
 */
inline void setTemp(uint16_t raw)
{
	ind = 0;
	//get the upper bound of the temperature
	while(ind < 7 && raw > tempData[ind][0])
		ind++;

	//do a nearest linear interpolation
	if(ind == 0)
		/*setNum(89);*/
		setNum(tempData[ind][1]/tempData[ind][0]*raw);
	else
		setNum((U[1]-L[1])*(raw-L[0])/(U[0]-L[0])+L[1]);
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
	/** get ADC value*/
	adc = ADCL;
	adc |= (ADCH << 8);

	setTemp(adc);
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

void setupadc(void)
{
	// Vref external
	BITON(ADMUX,REFS0);
	
	// Read from ADC5
	BITON(ADMUX,MUX0);
	BITON(ADMUX,MUX1);

	BITOFF(ADMUX,ADLAR);

	BITOFF(PRR,PRADC);

	// Enable ADC interrupt
	BITON(ADSCRA,_BV(3));
	
	// Enable auto trigger
	BITON(ADSCRA,_BV(5));

	// Enable ADC
	BITON(ADSCRA,_BV(7));
	
	// Start free-running conversions
	BITON(ADSCRA,_BV(6));

}

int main(int argc, char const *argv[])
{
	//set io port directions
	SETPORT(DDRD,0xff,0xff);
	SETPORT(DDRC,0x2,0x3);

	digit_cfg.dsel = DG1;
	setNum(0);

	sei();
	setupadc();
	setupclk();

	while(1);

	return 0;
}
