/*
** SCCS ID:	@(#)user.h	1.1	03/31/09
**
** File:	user.h
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	User routines.
*/

#ifndef _USER_H
#define _USER_H

/*
** General (C and/or assembly) definitions
*/

/*
** Delay loop counts
*/

#define	LONG_DELAY	100000000
#define	STD_DELAY	  2500000
#define	ALT_DELAY	  4500000

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** User process controls.
**
** The value field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
**
** NOTE:  users W-Z should never be spawned directly.
*/

/*#define	SPAWN_A		write, exit
#define	SPAWN_B		write, exit
#define	SPAWN_C		write, exit
#define	SPAWN_D		write, spawn, exit
#define	SPAWN_E		write, sleep, exit
#define	SPAWN_F		write, sleep, exit
#define	SPAWN_G		write, sleep, exit
#define	SPAWN_H		write
// no user i
#define	SPAWN_J		write, spawn, exit
#define	SPAWN_K		write, sleep, spawn, exit
#define	SPAWN_L		write, sleep, spawn, time, exit
#define	SPAWN_M		write, spawn, sleep, exit
#define	SPAWN_N		write, sleep, spawnl, time, getpid, exit
// no user O
#define	SPAWN_P		write, sleep, time
#define	SPAWN_Q		write, bogus, exit
#define	SPAWN_R		write, read, sleep, exit
#define	SPAWN_S		write, sleep, forever
#define	SPAWN_T		write, sleep, time, getpidlist, forever*/
// no user U
// no user V

// user W:  write, sleep, getpid, exit
// user X:  write, getpid, getppid, exit
// user Y:  write, sleep, exit
// user Z:  write, exit

/* added user producer, consumer, evil_one to demo shared memory functionality */
#define SPAWN_PRODUCER
#define SPAWN_CONSUMER
#define SPAWN_EVIL_ONE


/*
** Prototypes for externally-visible routines
*/

void user_idle( void );

void user_first( void );

#endif

#endif
