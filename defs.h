/*
** SCCS ID:	@(#)defs.h	1.1	03/31/09
**
** File:	defs.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	system-wide definitions
*/

#ifndef _DEFS_H
#define _DEFS_H

/*
** General (C and/or assembly) definitions
*/

	// maximum number of simultaneous processes in the system

#define	MAX_PROCS	24

	// generic null pointer value

#define	NULL		0

	// size of user stacks (in longwords)

#define	STACK_SIZE	1024

	// status return values

#define	STAT_SUCCESS		0
#define	STAT_ALLOC_FAIL		1
#define	STAT_BAD_LEVEL		2

#endif
