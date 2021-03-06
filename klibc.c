/*
** SCCS ID:	@(#)klibc.c	1.1	03/31/09
**
** File:	klibc.c
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	C implementations of some kernel-level library routines
*/

#define	__KERNEL__20083__

#include "headers.h"

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length ) {
	register unsigned char *buf = (unsigned char *)buffer;
	register uint32_t num = length;

	while( num-- ) {
		*buf++ = 0;
	}

}

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length ) {
	register unsigned char *dst = (unsigned char *)destination;
	register unsigned char *src = (unsigned char *)source;
	register uint32_t num = length;

	while( num-- ) {
		*dst++ = *src++;
	}

}


/*
** _kpanic(msg)
**
** kernel-level panic routine
**
** usage:  _kpanic( msg )
**
** Prints an EBP chain (up to five values), then calls __panic.
*/

void _kpanic( char *msg ) {
	uint32_t ebp;
	int n;

	c_puts( "\n\n***** KERNEL PANIC *****\n\nChain of EBP values:\n\n" );

	//
	// Get current EBP so we can follow the chain
	//

	ebp = _get_ebp();

	c_printf( "EBP  %08x\n", ebp );

	//
	// Follow the chain until either EBP becomes 0, or
	// we have done five levels.
	//

	for( n = 0; ebp != 0 && n < 5; ++n ) {
		ebp = *(uint32_t *)ebp;
		c_printf( "-->  %08x\n", ebp );
	}

	//
	// Finally, call __panic to wrap things up
	//

	__panic( msg );

}


/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
*/

void _kprt_status( char *msg, int stat ) {

	if( msg == NULL ) {
		return;
	}

	switch( stat ) {
		case STAT_SUCCESS:
			c_printf( msg, "STAT_SUCCESS" );
			break;

		case STAT_ALLOC_FAIL:
			c_printf( msg, "STAT_ALLOC_FAIL" );
			break;

		case STAT_BAD_LEVEL:
			c_printf( msg, "STAT_BAD_LEVEL" );
			break;

		default:
			c_printf( msg, "STAT_????" );
			break;
	
	}

}

/*
** _strcpy(dest, src)
**
** copies one string into another
**
** returns dest string
*/

char* _strcpy( char* dest_string, char* src_string ) {

    while ((*dest_string++ = *src_string++));

    return dest_string;
}
