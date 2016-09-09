/*
** SCCS ID:	@(#)clock.h	1.1	03/31/09
**
** File:	clock.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	clock handler
*/

#ifndef _CLOCK_H
#define _CLOCK_H

#include "headers.h"

#define	CLOCK_FREQUENCY		100

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** Globals
*/

extern uint32_t _system_time;

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code );

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void );

#endif

#endif
