#include <stdint.h>
#include <avr/io.h>

#define DG1 PORTD
#define DG2 PORTC
#define SETDG(port,digit) port = nums[digit]

//--CONSTANTS------------------------------
const uint32_t FREQ = 0;

//7SEG IOPIN CONFIG (pins 0-6):   0   1    2    3    4    5    6     7   8    9
const unsigned char nums[10] = {0x77,0x24,0x5d,0x6d,0x2e,0x6b,0x7b,0x25,0x7f,0x6f};

/** the current digit to display */
uint8_t curDigit; 

/** the current display of digit */
uint8_t curVal;

//--END CONSTANTS---------------------------

/**
 * @brief sets the iopins for the 7SEG display
 * 
 * @param num - the number to output (between 0 and 99)
 *
 * @return void
 * 
 */
void outNUM(uint8_t num)
{
	unint8_t d1,d2;
	if(num > 99)
		return;

	//set digit one
	d1 = num%10;

	//set digit two
	d2 = num/10;

	//set the digits
	SETDG(DG1,d1);
	SETDG(DG2,d2);
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
	
}

int main(int argc, char const *argv[])
{
  
	return 0;
}
