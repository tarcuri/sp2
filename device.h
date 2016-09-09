/*
** File    : device.h
**
** Author  : Thomas Arcuri <tja1618>
**
** Description: The device module allows processes to retrieve 'handles' which
**              give them access to device specific functions. This module will
**              also be responsible for initializing the system devices at boot.
*/

#ifndef _DEVICE_H
#define _DEVICE_H

#define DEV_MAX_DEVICES 256

#define DEV_RAMDISK	1
#define DEV_IDE_PRI	2
#define DEV_IDE_SEC	3

#define DEV_TYPE_CHAR  1
#define DEV_TYPE_BLOCK 2

typedef struct device_handle {
	uint8_t type;  
	uint8_t identifier;

	int32_t (*_read) ( void *, uint32_t );
	int32_t (*_write)( void *, uint32_t );
	int32_t (*_ctrl) ( int32_t, void *buf );
	
	struct device_handle *next;	// only used for the free list
} device_t;

/*
** Prototypes
*/
device_t* _dev_open( uint8_t );
void 	  _dev_close( device_t * );
void      _dev_dealloc_handle( device_t * );
device_t* _dev_alloc_handle( void );
void      _dev_init( void );

void      ramdisk_test( void );

#endif
