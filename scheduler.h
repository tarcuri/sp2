/*
** SCCS ID:	@(#)scheduler.h	1.1	03/31/09
**
** File:	scheduler.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	scheduler and dispatcher
*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "headers.h"

#ifndef __ASM__20083__

#include "processes.h"

/*
** Prototypes
*/

/*
** _schedule(pcb)
**
** put PCB into the ready queue
**
** returns:
**      STAT_BAD_LEVEL if the level in the PCB is invalid
**      else, _q_insert() status
*/

int _schedule( pcb_t *pcb );

/*
** _dispatch(void)
**
** dispatch the next process
*/

void _dispatch( void );

#endif

#endif
