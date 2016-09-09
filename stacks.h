/*
** SCCS ID:	@(#)stacks.h	1.1	03/31/09
**
** File:	stacks.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	stack-related types and functions
*/

#ifndef _STACKS_H
#define _STACKS_H

#include "headers.h"

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** Types
*/

	// stack data structure

typedef uint32_t stack_t[ STACK_SIZE ];

/*
** Globals
*/

extern uint32_t *_system_esp;	// pointer to top of system stack

/*
** Prototypes
*/

/*
** _stack_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

stack_t *_stack_alloc( void );

/*
** _stack_dealloc(stack)
**
** deallocate a stack structure
**
** returns the status from the _q_insert() call
*/

int _stack_dealloc( stack_t *stack );

/*
** _stack_init()
**
** initializes all stack-related data structures
*/

void _stack_init( void );

#endif

#endif
