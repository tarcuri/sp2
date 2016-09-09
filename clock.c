/*
** SCCS ID:	@(#)clock.c	1.1	03/31/09
**
** File:	clock.c
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	clock handler
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "clock.h"

#include "processes.h"
#include "queues.h"
#include "scheduler.h"

#include "ata.h"
#include "ramdisk.h"
#include "startup.h"

#include <uart.h>
#include <x86arch.h>

/*
** PRIVATE GLOBAL VARIABLES
*/

	// pinwheel control variables
static uint32_t _pinwheel;
static uint32_t _pindex;

static uint32_t ramdisk_service = 4;

/*
** PUBLIC GLOBAL VARIABLES
*/

	// system time (in ticks)

uint32_t _system_time;

/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code ) {
	pcb_t *pcb;
	disk_request_t *dr;

	// increment system time

	++_system_time;

	// spin the pinwheel

	++_pinwheel;
	if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
		_pinwheel = 0;
		++_pindex;
		c_putchar_at( 79, 0, "|/-\\"[ _pindex & 3 ] );
	}

	// wake up any sleepers whose time has come

	while( (pcb = (pcb_t *) _q_remove_if_le(&_sleeping,_system_time)) ) {
		_schedule( pcb );
	}

 	// perform a pending RAM-disk operation <tja1618>
 	// NOTE: must be disabled once the disk driver is implemented;
 	//            this is simply a slow way to test ram disk usage
 	if ( (_system_time % ramdisk_service == 0) && (dr = (disk_request_t *) _q_remove(&_ramdisk_io_pending)) ) {
 		// TODO: need to handle errors
 
 		switch (dr->cmd) {
 	  	case DISK_CMD_READ:
 	    		_rd_read( dr->lba, dr->nblocks, dr->buffer );
 	    
 			// check for a blocked process
 			pcb_t *proc;
 			while ( (proc = (pcb_t *) _q_remove( &_blocked_requests )) ) {
 				if (proc->pending_request == dr) {
 					// we just serviced this request, schedule this process
 					proc->pending_request = 0;
 					//if ( proc == _current ) {
 					//c_puts("CLOCK: scheduling current process with pending request\n");
 					  //_schedule( proc );
 					//}
 
 					c_puts("CLOCK: schueduling process after successful IO operation\n");
 					_schedule( proc );
 
 	    				// finish the request
 	    				dr->status = DISK_STATUS_IO_SUCCESS;
 
 				     	break;
 				} else {
 					// re-insert this into the queue, however these should stay in order
 					c_puts("clock: had to reinsert a processes blocked on a request\n");
 					_q_insert( &_blocked_requests, (void *) proc );
 				}
 			}
 
 			if (dr->status != DISK_STATUS_IO_SUCCESS) {
 				c_puts("ERROR: clock: couldn't find a pending disk request's process\n");
 				dr->status = DISK_STATUS_IO_FINISHED_ERROR;
 			}
 
 			break;
 	  	case DISK_CMD_WRITE:
 	    		_rd_write( dr->lba, dr->nblocks, dr->buffer );
 
 	    		dr->status = DISK_STATUS_IO_SUCCESS;
 			break;
 	  	default:
 	    		dr->status = DISK_ERR_INVALID_CMD;
 	    		c_puts("ERROR: clock: invalid disk request\n");
 	  	};
 	}


	// decrement quantum for current process
	// process switch if required
	if( _current->quantum == 0 ) {
		// time for a change!
/* 	        c_printf("CLOCK_: [ (%d) 0x%08x <0x%08x> ] :: [ (%d) 0x%08x <0x%08x> ]\n", _real_pcb->pid, _real_pcb, _real_pcb->context, */
/* 			                                                                   _current->pid, _current, _current->context); */

		_schedule( _current );
		_dispatch();
	} else {
		// decrement quantum
		_current->quantum -= 1;
	}

	// tell the PIC that we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void ) {

	// start the pinwheel

	_pinwheel = _pindex = 0;

	// reset the system time to the epoch

	_system_time = 0;

	// set the clock to tick at CLOCK_FREQUENCY Hz.

	__outb( TIMER_0, ( TIMER_FREQUENCY / CLOCK_FREQUENCY ) & 0xff );
	__outb( TIMER_1, ( TIMER_FREQUENCY / CLOCK_FREQUENCY ) >>   8 );


	// announce that we have initialized the clock module

	c_puts( " clock" );

}
