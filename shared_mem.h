/*
** SCCS ID:	%W%	%G%
**
** File:	shared_mem.h
**
** Author:	Ines Pavon (4003-506 class of 20083)
**
** Contributor: Group 5
**
** Description:	basic structures to be used with shared memory and shared memory
** function prototypes. NOTE: Each process can only have access to one shared
** memory segment and each segment can only be accessed by at most 2 processes.
*/

#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

#include "headers.h"

#ifndef __ASM__20083__

// max number of shared memory segments in the system
#define MAX_SHMS        4

/* max number of processes that can have access to a single shared 
memory segment */
#define MAX_MEMBERS     2 
			
/* determines if shared memory segment will be used to read, write, or
read and write */
#define SHM_RD        0
#define SHM_WR        1			
#define SHM_RW        2

/* default size of shared memory segment */
#define SHM_DSIZE       96

/* size of memory to be used by shared memory segments */
#define MAX_MEM_SIZE    512           

//shared mem seg state
#define	SHM_FREE	0
#define	SHM_NEW		1

/*
** Types
*/

//shared memory segment
typedef struct {
        char* address;
	uint32_t num_members;
	uint32_t size;
	uint32_t mode;
	uint16_t id;
	uint16_t current_pids[MAX_MEMBERS];
	uint8_t status;
} sharedmem_segment;

/*
** Globals
*/

extern sharedmem_segment _shms[];	//all shared mem segs in system
extern sharedmem_segment *_current_shms;	//current shared mem seg
extern uint16_t _next_shm;		//next available shm seg id

char _memory[MAX_MEM_SIZE];             //memory chunk of max size
char* _free_ptr;			//location of where remaining cleared
					//mem starts
					
/*
** Prototypes
*/

/*
** _mem_init()
**
** clears chunk of memory and sets pointer to beginning of chunk
*/
void _mem_init( void );

/*
** _shm_malloc(size)
**
** allocate chunk of memory of specified size and move pointer accordingly
**
** returns pointer to start of allocated mem chunk or null if size too big
*/
char* _shm_malloc(int size);

/*
** _shm_free(mem)
**
** Should free previously allocated chunk of mem but we are not actually
** allocating so we are not going to free either
*/
void _shm_free(void* mem);

/*
** _shm_alloc()
**
** allocate a shared mem segment
**
** returns a pointer to the shared memory segment, or NULL on failure
*/
sharedmem_segment *_shm_alloc( void );

/*
** _shm_dealloc(pcb)
**
** deallocate a shared mem segment
**
** returns the status from the _q_insert() call
*/

int _shm_dealloc( sharedmem_segment *shm_seg );

/*
** _shm_init()
**
** initializes all shared memory segments
*/

void _shm_init( void );

#endif

#endif
