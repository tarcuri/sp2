/*
** SCCS ID:	@(#)ulib.h	1.1	03/31/09
**
** File:	ulib.h
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	Prototypes for user-level library routines
**
** Note that some of these are defined in the ulibs.S source file,
** and others in the ulibc.c source file.
*/

#ifndef _ULIB_H
#define _ULIB_H

#ifndef __ASM__20083__

#include "types.h"

#include "processes.h"

#include "shared_mem.h"

/*
** Start of C-only definitions
*/

/*
** Prototypes
*/

/*
** spawn - create a new process
**
** usage:	pid = spawn( entry );
**
** returns:
**	-1 if the creation failed
**	else, the PID of the new child
*/

int spawn( void *entry );

/*
** spawnl - create a new process at a specified level
**
** usage:	pid = spawnl( entry, level );
**
** returns:
**	-1 if the creation failed
**	else, the PID of the new child
*/

int spawnl( void *entry, uint8_t level );

/*
** sleep - put the current process to sleep
**
** usage:	sleep( sec );
*/

void sleep( int sec );

/*
** exit - terminate the current process and clean up after it
**
** usage:	exit();
*/

void exit( void );

/*
** kill - terminate a process with extreme prejudice
**
** usage:	n = kill( pid );
**
** returns:
**	-1 if the PID isn't active, or is the idle process
**	else, the state of the process at the time it was killed
*/

int kill( uint16_t pid );

/*
** read - read one character from the SIO
**
** usage:	ch = read();
**
** returns:
**	the next input character
*/

int read( void );

/*
** write - write one character to the SIO
**
** usage:	write( ch );
**
** based on a simple interrupt-driven SIO model taken from SP1
*/

void write( int ch );

/*
** time - retrieve the current system time
**
** usage:	n = time();
**
** returns:
**	the current system time
*/

uint32_t time( void );

/*
** getpid - retrieve pid of the current process
**
** usage:	n = getpid();
**
** returns:
**	the current process' pid
*/

uint16_t getpid( void );

/*
** getppid - retrieve parent pid of the current process
**
** usage:	n = getppid();
**
** returns:
**	PID of the parent of the current process
*/

uint16_t getppid( void );

/*
** getpidlist - retrieve parent pid of the current process
**
** usage:	n = getpidlist( buf, size );
**
** returns:
**	the list of PIDs (in the buffer parameter)
**	the count of active processes in the system
**
** note that the returned count may be larger than the size of the
** supplied buffer; in that case, only the first 'size' PIDs will
** be put into the buffer
*/

int getpidlist( uint16_t buf[], int size );

/*
** bogus - a bogus system call
**
** should force the calling process to exit
*/

void bogus( void );

/* added prototypes for user-level shared memory library routines */

/*
** shmget(size, mode)
**
** allocate shared memory segment (always creates new and unique segment)
**
** returns the segment id or -1 if fails
*/
int shmget(int size, int mode); 

/*
** shmat(shmid)
**
** attach the shared memory segment
**
** returns the pointer to segment or null if fails
*/
void *shmat(int shmid); 

/*
** shmwrite(shmseg, message)
**
** write message to shared memory segment
**
** returns 0 on success, -1 on failure
*/
int shmwrite(void *shmseg, char *message); 

/*
** shmread(shmseg, message)
**
** read message from shared memory segment
**
** returns 0 on success, -1 on failure
*/
int shmread(void *shmseg, char* message); 

/*
** shmdt(shmseg)
**
** detaches shared memory segment
**
** returns 0 on success, -1 on failure
*/
int shmdt(void *shmseg);

/*
** shmrm(shmseg)
**
** detaches shared memory segment
*/
int shmrm(void* shmseg);

#endif

#endif
