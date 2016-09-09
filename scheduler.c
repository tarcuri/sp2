/*
** SCCS ID:	@(#)scheduler.c	1.1	03/31/09
**
** File:	scheduler.c
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	scheduler and dispatcher
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "scheduler.h"

#include "processes.h"
#include "queues.h"
#include "system.h"

/*
** PUBLIC FUNCTIONS
*/

/*
** _schedule(pcb)
**
** put PCB into the ready queue
**
** returns:
**      STAT_BAD_LEVEL if the level in the PCB is invalid
**      _q_insert() status otherwise
*/

int _schedule( pcb_t *pcb ) {

	// verify the process level

	if( pcb->level >= MLQ_LEVELS ) {
		return( STAT_BAD_LEVEL );
	}

	// change the status

	pcb->status = PROC_READY;

	// add to the correct level in the ready queue

	return( _q_insert( &_ready[pcb->level], (void *) pcb ) );

}

/*
** _dispatch(void)
**
** dispatch the next process
*/

void _dispatch( void ) {
	pcb_t *pcb;
	int level;

	// this is a bit complicated, given that we have more than
	// one "ready queue"
	//
	// we start at the highest level in the ready queue, looking
	// for a process to dispatch; we stop when we find one
	//
	// if we don't find one, something has gone seriously wrong
	// with our system, as there should *always* be the idle
	// process sitting at the bottom level in the queue

	pcb = 0;
	level = 0;
	while( level < MLQ_LEVELS ) {

		if( _q_isempty( &_ready[level] ) ) {

			// nothing at this level - move to the next one
			++level;

		} else {

			// something here - remove it from the queue

			pcb = _q_remove( &_ready[level] );

/* 			c_printf("DISPATCH: removed pcb [ (%d) 0x%08x <0x%08x> ]\n", pcb->pid, pcb, pcb->context); */

			// if it's ready to go, we're done here

			if( pcb->status == PROC_READY ) {
				break;
			}

			// this is where we catch the undead (i.e.,
			// processes which have been killed, but
			// not cleaned up yet)

			if( pcb->status == PROC_ZOMBIE ) {

				_cleanup( pcb );
				pcb = 0;

			} else {

				// should never happen!

				_kpanic( "non-ready non-zombie in dispatch" );

			}

		}

	}

	// make sure we found a process

	if( !pcb ) {
		_kpanic( "null pcb in dispatch" );
	}

	// give it the cpu

	pcb->status = PROC_RUNNING;
	pcb->quantum = QUANTUM_DEFAULT;

	// make it the current process

	_current = pcb;

/* 	c_printf("DISPATCH: dispatched pcb [ (%d) 0x%08x <0x%08x> ]\n", _current->pid, _current, _current->context); */
}
