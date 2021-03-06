/*
** File    : ramdisk.c
**
** Author  : Thomas Arcuri <tja1618>
**
*/

#define __KERNEL__20083__

#include "headers.h"

#include "ramdisk.h"
#include "disk.h"
#include "device.h"
#include "scheduler.h"

// CAREFUL: I accidently allocated more memory than was available - result: hang
static uint8_t _ramdisk[ RD_MAX_LBA * DISK_BLOCK_SIZE ];

static uint8_t _write_buffer_block_allocation[ 10 ];
static uint8_t _write_buffer[ DISK_BLOCK_SIZE * 10 ];

// get KERNEL definition
int8_t _rd_open( device_t *dev ) {
	// TODO: handle null pointers
	if (!dev) {
		c_puts("ERROR: ramdisk: open called with null device pointer\n");
		return -1;
	}

	dev->type       = DEV_TYPE_BLOCK;
	dev->identifier = DEV_RAMDISK;

	// assign pointers to device-specific functions
	dev->_read  = _rd_read_placeholder; 		// read, and write and dummys
	dev->_write = _rd_write_placeholder;
	dev->_ctrl  = _rd_ctrl;			// ctrl should be used with this device

	return 0;
}

void _rd_close( device_t *dev ) {
	dev->type = 0;
	dev->identifier = 0;
	dev->_read = 0;
	dev->_write = 0;
	dev->_ctrl = 0;

	// add this handle back to the free list
	_dev_dealloc_handle( dev );
}

int32_t _rd_ctrl( int32_t cmd, void *arg ) {
 	switch (cmd) {
	case DISK_CMD_REQUEST:
		if (!arg) {
			c_puts("ERROR: ramdisk: ctrl called with a null argument\n");
			return -1;
		}

		// the arg should be a pointer to a valid request
		return _rd_request( (disk_request_t *) arg );
	default:
		c_puts("ERROR: ramdisk: invalid ctrl command\n");
	};

	return -1;
}



int8_t _rd_request( disk_request_t *request ) {
	disk_request_t *dr = request;
	
	if (!dr) {
		c_puts("ERROR: ramdisk: null disk request pointer\n");
		return -1;
	}

	/* check for a valid address */
	if ((dr->nblocks * DISK_BLOCK_SIZE) >
	    (RD_MAX_LBA - (dr->lba - (uint32_t) &_ramdisk[0]))) {

		dr->status = DISK_ERR_INVALID_REQ_SIZE;
		c_puts("ERROR: dev_ramdisk: invalid request size\n");
		return dr->status;
	}


	switch (dr->cmd) {
	case DISK_CMD_READ:
		// add to the disk io queue
		dr->status = DISK_STATUS_READ_PENDING;

		_current->pending_request = dr;

		if ( _q_insert( &_ramdisk_io_pending, (void *) dr ) == STAT_ALLOC_FAIL ) {
			dr->status = DISK_ERR_IO_QUEUE_INSERT;
			c_puts("ERROR: dev_ramdisk: failed insertion into disk io queue\n");
		}

		// now place the current process into the _blocked_request queue
		if ( _q_insert( &_blocked_requests, (void *) _current ) == STAT_ALLOC_FAIL ) {
		  c_puts("ERROR: ramdisk: failed insertion into blocked requests queue\n");
		}

		// and swap in a new process
		// c_puts("RAMDISK: dispatching new process\n");
		// _dispatch();

		break;
	case DISK_CMD_WRITE:
		// TODO: this needs to go to a write buffer immediately since we return
		dr->status = DISK_STATUS_WRITE_PENDING;

		_current->pending_request = dr;

		if ( _q_insert( &_ramdisk_io_pending, (void *) dr ) == STAT_ALLOC_FAIL ) {
			dr->status = DISK_ERR_IO_QUEUE_INSERT;
			c_puts("ERROR: dev_ramdisk: failed insertion into disk io queue\n");
		}
		
		break;
	default:
		dr->status = DISK_ERR_INVALID_CMD;

		c_puts("ERROR: dev_ramdisk: invalid command\n");
	};

	return dr->status;
}

int32_t _rd_read_placeholder( void *buf, uint32_t size ) {
	c_puts("ERROR: ramdisk: must use request interface for reads\n");
	return -1;
}

uint32_t _rd_read( uint32_t lba, uint32_t nblocks, void *buffer ) {
	uint32_t block = lba;

	while ( block < (lba + (nblocks * DISK_BLOCK_SIZE)) ) {
		// copy a block at a time
		_memcpy( (void *) buffer + (block - lba),
			 (void *) &_ramdisk[ block ],
			 DISK_BLOCK_SIZE );

		block += DISK_BLOCK_SIZE;
	}

	return (block - lba) * DISK_BLOCK_SIZE;
}

int32_t _rd_write_placeholder( void *buf, uint32_t size ) {
	c_puts("ERROR: ramdisk: must use request interface for writes\n");
	return -1;
}

uint32_t _rd_write( uint32_t lba, uint32_t nblocks, void *buffer ) {
	uint32_t block = lba;

	while ( block < (lba + (nblocks * DISK_BLOCK_SIZE)) ) {
		_memcpy( (void *) &_ramdisk[ block ],
			 (void *) buffer + (block - lba),
			 DISK_BLOCK_SIZE );

		block += DISK_BLOCK_SIZE;
	}

	return (block - lba) * DISK_BLOCK_SIZE;
}
