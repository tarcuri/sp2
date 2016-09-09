/*
** SCCS ID:	%W%	%G%
**
** File:	?
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	?
*/

#ifndef _?_H
#define _?_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#define BIOS_SD_32_BIT_ID	0x5f32335f

#ifndef __ASM__20083__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/* "_32_" */
typedef struct bios_32_service_dir {
	uint32_t identifier;
	uint32_t phys_bsd_addr;
	uint8_t	revision;
	uint8_t length;
	uint8_t crc;
} bios32sd_t;

/*
** Globals
*/

/*
** Prototypes
*/

#endif

#endif
