/*
** SCCS ID:	@(#)processes.c	1.1	03/31/09
**
** File:	processes.c
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	process-related routines
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "processes.h"

#include "queues.h"

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _free_pcbs;		// pool of free PCBs

/*
** PUBLIC GLOBAL VARIABLES
*/

pcb_t _pcbs[ MAX_PROCS ];	// all PCBs in the system
pcb_t *_current;		// current process
uint16_t _next_pid;		// next available PID

queue_t _ready[ MLQ_LEVELS ];	// system ready queue
queue_t _sleeping;		// queue of sleeping processes
queue_t _reading;		// queue of processes blocked on input

queue_t _blocked_requests;      // queue of processes blocked on a disk request

/*
** PUBLIC FUNCTIONS
*/

/*
** _pcb_alloc()
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/

pcb_t *_pcb_alloc( void ) {
	pcb_t *pcb;

	// remove the first entry from the queue of free PCBs

	pcb = (pcb_t *) _q_remove( &_free_pcbs );

	// if we got something, mark it as non-FREE

	if( pcb ) {
		pcb->status = PROC_NEW;
	}

	return( pcb );

}


/*
** _pcb_dealloc(pcb)
**
** deallocate a PCB structure
**
** returns the status from the _q_insert() call
*/

int _pcb_dealloc( pcb_t *pcb ) {

	// mark it as available

	pcb->status = PROC_FREE;

	// put it into the free queue

	return( _q_insert( &_free_pcbs, (void *) pcb ) );

}


/*
** _pcb_init()
**
** initializes all process-related data structures
*/

void _pcb_init( void ) {
	int i, stat;

	// first, reset all the ready queue levels

	for( i = 0; i < MLQ_LEVELS; ++i ) {
		_q_reset( &_ready[i], 0 );
	}

	// next, the separate queues of processes

	_q_reset( &_sleeping, _hash_sleep );
	_q_reset( &_reading, 0 );
	_q_reset( &_free_pcbs, 0 );

	// empty out all the PCBs

	_memclr( (void *) _pcbs, sizeof(_pcbs) );

	// iterate through the pcb array, deallocating
	// each one to put it into the free queue
	//
	// NOTE:  THIS ASSUMES the qnodes have been reset as well

	for( i = 0; i < sizeof(_pcbs)/sizeof(pcb_t); ++i ) {
		stat = _pcb_dealloc( &_pcbs[i] );
	}

	// check the last insertion to be sure it worked

	if( stat != STAT_SUCCESS ) {
		_kprt_status( "*** pcb init, last insert status %s\n", stat);
		_kpanic( "pcb init: insert fail" );
	}

	// no current process

	_current = 0;

	// reset the PID counter

	_next_pid = 0;

	// report that we have finished

	c_puts( " process" );

}
