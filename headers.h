/*
** SCCS ID:	@(#)headers.h	1.1	03/31/09
**
** File:	headers.h
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	standard includes, pulled in by everything else
*/

#ifndef _HEADERS_H
#define _HEADERS_H

#ifndef __ASM__20083__

	// only pull these in if we're not in assembly language

#include "defs.h"
#include "types.h"
#include "c_io.h"
#include "support.h"

	// OS and user code have different library headers

#ifdef __KERNEL__20083__
#include "klib.h"
#else
#include "ulib.h"
#endif

#endif

#endif
