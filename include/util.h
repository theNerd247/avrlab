/**
 * @file util.h 
 *
 * utility macros for AVR code
 *
 * @author Noah Harvey (nharvey@spsu.edu)
 * @copyright GNU Public License 2
 */

#define BITON(port,pin) port |= _BV(pin)
#define BITOFF(port,pin) port &= ~_BV(pin)
#define SETPORT(port,mask,value) port = ((port & ~(mask)) | value)
#define TOGGLEBIT(port,bit) port ^= _BV(bit)
