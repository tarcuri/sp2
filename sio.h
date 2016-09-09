/*
** SCCS ID:	@(#)sio.h	1.1	03/31/09
**
** File:	sio.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	SIO definitions
*/

#ifndef _SIO_H
#define _SIO_H

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** _isr_sio
**
** SIO interrupt service routine
*/

void _isr_sio( int vector, int code );

/*
** _sio_init
**
** Initialize the UART chip.
*/

void _sio_init( void );

/*
** _sio_enable - enable/disable SIO interrupts
**
** usage:       old = _sio_enable( xmit, rcv )
**
** parameters are booleans (0 for turn off, 1 for turn on)
**
** returns the prior settings
*/

uint8_t _sio_enable( int xmit, int rcv );

/*
** _sio_read - get the next input character
**
** usage:	ch = _sio_read()
**
** returns the next character, or -1 if no character is available
*/

int _sio_read( void );

/*
** _sio_write - write a character to the serial output
**
** usage:	_sio_write( ch )
*/

void _sio_write( int ch );

#endif

#endif
