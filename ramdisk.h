/*
** File    : ramdisk.h
**
** Author  : Thomas Arcuri <tja1618>
**
*/

#include "headers.h"

#include "disk.h"
#include "device.h"

/*
 * The 'ramdisk' - a static byte array
 */
//Extern uint8_t _ramdisk[ RD_MAX_LBA * DISK_BLOCK_SIZE ];

/*
** Prototypes
*/
int8_t 		_rd_open( device_t * );
void 		_rd_close( device_t * );
int32_t         _rd_ctrl( int32_t, void * );
int8_t 		_rd_request( disk_request_t * );
int32_t 	_rd_read_placeholder( void *buf, uint32_t size );
uint32_t 	_rd_read( uint32_t, uint32_t, void * );
int32_t 	_rd_write_placeholder( void *buf, uint32_t size );
uint32_t 	_rd_write( uint32_t, uint32_t, void * );
