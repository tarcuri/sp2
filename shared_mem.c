/*
** SCCS ID:	%W%	%G%
**
** File:	shared_mem.c
**
** Author:	Ines Pavon (4003-506 class of 20083)
**
** Contributor: Group 5
**
** Description:	shared memory related routines
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "shared_mem.h"

#include "queues.h"

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _free_shms;		// pool of free shared mem segments


/*
** PUBLIC GLOBAL VARIABLES
*/
sharedmem_segment _shms[ MAX_SHMS ];	//all shared mem segs in system
sharedmem_segment *_current_shms;	//current shared mem seg
uint16_t _next_shm;			//next available shm seg id


/*
** PUBLIC FUNCTIONS
*/

/*
** _mem_init()
**
** clears chunk of memory and sets pointer to beginning of chunk
*/
void _mem_init()
{
	/* fill memory chunk with zeros and set pointer to beginning */
	_memclr(_memory, sizeof(_memory));
	_free_ptr = _memory;
	
	c_puts( " clearmem " );
} 


/*
** _shm_malloc(size)
**
** allocate chunk of memory of specified size and move pointer accordingly
**
** returns pointer to start of allocated mem chunk
*/
char* _shm_malloc(int size)
{
    	char* new_mem = _free_ptr;

    	// If there isn't enough space, return null
    	if (_free_ptr + size > _memory + MAX_MEM_SIZE) {
        	return 0;
	}

    	// Move the free ptr past the currently allocated block
    	_free_ptr += size;

    	return (new_mem);
}


/*
** _shm_free(mem)
**
** Should free previously allocated chunk of mem but we are not actually
** allocating so we are not going to free either
*/
void _shm_free(void* mem)
{
    // Don't need to do anything
}


/*
** _shm_alloc()
**
** allocate a shared mem segment
**
** returns a pointer to the shared memory segment, or NULL on failure
*/

sharedmem_segment *_shm_alloc( void ) {
	sharedmem_segment *shm_seg;

	// remove the first entry from the queue of free PCBs

	shm_seg = (sharedmem_segment *) _q_remove( &_free_shms );

	// if we got something, mark it as non-FREE

	if( shm_seg ) {
		shm_seg->status = SHM_NEW;
	}

	return( shm_seg );

}

/*
** _shm_dealloc(pcb)
**
** deallocate a shared mem segment
**
** returns the status from the _q_insert() call
*/

int _shm_dealloc( sharedmem_segment *shm_seg ) {

	// mark it as available

	shm_seg->status = SHM_FREE;

	// put it into the free queue

	return( _q_insert( &_free_shms, (void *) shm_seg ) );

}

/*
** _shm_init()
**
** initializes all shared memory segments
*/

void _shm_init( void ) {
	int i, stat;
	
	// reset queue of free shared mem segments
	_q_reset( &_free_shms, 0 );

	// empty out all the shared mem segments

	_memclr( (void *) _shms, sizeof(_shms) );

	// iterate through the shared mem segs array, deallocating
	// each one to put it into the free queue

	for( i = 0; i < sizeof(_shms)/sizeof(sharedmem_segment); ++i ) {
		stat = _shm_dealloc( &_shms[i] );
	}

	// check the last insertion to be sure it worked

	if( stat != STAT_SUCCESS ) {
		_kprt_status( "*** shm init, last insert status %s\n", stat);
		_kpanic( "shm init: insert fail" );
	}

	// no current shared mem segment

	_current_shms = 0;

	// reset the shared mem counter

	_next_shm = 0;

	// report that we have finished

	c_puts( " sharedmem " );

}


