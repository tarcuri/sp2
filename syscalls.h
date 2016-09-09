/*
** SCCS ID:	@(#)syscalls.h	1.1	03/31/09
**
** File:	syscalls.h
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	system call handlers
*/

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "headers.h"

#include "shared_mem.h"

/*
** General (C and/or assembly) definitions
*/

#include <x86arch.h>

	// system call codes

#define	SYS_spawn		0
#define	SYS_spawnl		1
#define	SYS_sleep		2
#define	SYS_exit		3
#define	SYS_kill		4
#define	SYS_read		5
#define	SYS_write		6
#define	SYS_time		7
#define	SYS_getpid		8
#define	SYS_getppid		9
#define	SYS_getpidlist		10

//added shared memory syscalls
#define SYS_shmget              11
#define SYS_shmat               12
#define SYS_shmwrite            13
#define SYS_shmread             14
#define SYS_shmdt               15
#define SYS_shmrm               16

	// number of "real" system calls
	//changed number of real systems calls from 11 to 17

#define	N_SYSCALLS		17

	// dummy system call code to test the syscall ISR

#define	SYS_bogus		0xfeedface

	// system call interrupt vector

#define	INT_VEC_SYSCALL		0x80

	// default contents of user EFLAGS register

#define	DEFAULT_EFLAGS		(EFLAGS_MB1 | EFLAGS_IF)

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** Prototypes
*/

/*
** _isr_syscall()
**
** system call ISR
*/

void _isr_syscall( int vector, int code );

/*
** _sys_init()
**
** initializes all syscall-related data structures
*/

void _sys_init( void );

#endif

#endif
