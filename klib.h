/*
** SCCS ID:	@(#)klib.h	1.1	03/31/09
**
** File:	klib.h
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	Prototypes for kernel-level library routines
**
** Note that some of these are defined in the klibs.S source file,
** and others in the klibc.c source file.
*/

#ifndef _KLIB_H
#define _KLIB_H

#ifndef __ASM__20083__

#include "types.h"

/*
** Start of C-only definitions
*/

/*
** Definitions
*/

	/* limit EBP chain following to addressed under this value */

#define	EBP_ADDR_LIMIT		0x00010000

	/* how many EPB entries to follow in _kpanic */

#define	EBP_CHAIN_LIMIT		5

/*
** Prototypes
*/

/*
** _get_ebp - return current contents of EBP at the time of the call
**
** Primarily used only by _kpanic
*/

uint32_t _get_ebp( void );

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length );

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length );

/*
** _kpanic - kernel-level panic routine
*/

void _kpanic( char *msg );

/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
*/

void _kprt_status( char *msg, int stat );

/*
** _strcpy(dest, src)
**
** copies one string into another
**
** returns dest string
*/

char* _strcpy(char* dest_string, char* src_string);

#endif

#endif
