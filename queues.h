/*
** SCCS ID:	@(#)queues.h	1.1	03/31/09
**
** File:	queues.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	queue manipulation types and routines
*/

#ifndef _QUEUES_H
#define _QUEUES_H

#include "headers.h"

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

	// pseudo-function:  is a queue empty?

#define	_q_isempty(q)	((q)->first == 0)

/*
** Types
*/

	// queue header

typedef struct queue {
	uint32_t (*hashfcn)( void * );
	void *first;
	void *last;
} queue_t;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _q_insert( queue, data )
**
** add 'data' to 'queue'
**
** returns status of the insertion attempt
*/

int _q_insert( queue_t *queue, void *data );

/*
** _q_remove( queue )
**
** remove the first element from 'queue'
**
** returns the removed element, or NULL
*/

void *_q_remove( queue_t *queue );

/*
** _q_remove_if_le( queue, key )
**
** if the first item in 'queue' has a key value that is 
** less than or equal to the supplied 'key', remove it
**
** returns the removed element, or NULL
*/

void *_q_remove_if_le( queue_t *queue, uint32_t key );

/*
** _q_reset( queue, fcn )
**
** reset 'queue' to its original empty state, using
** 'fcn' as the hash function for ordered insertion
*/

void _q_reset( queue_t *queue, uint32_t (*fcn)(void *) );

/*
** _q_init()
**
** initialize the queue module
*/

void _q_init( void );

/*
** _hash_sleep(node)
**
** calculate the hash value for 'node', used to
** order the contents of the sleep queue
**
** returns the hash value
*/

uint32_t _hash_sleep( void *node );

#endif

#endif
