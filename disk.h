/*
** File    : disk.h
**
** Author  : Thomas Arcuri <tja1618>
**
** Description: This 'disk' module is just setup to allocate a limited
** 	        number of disk requests, which can then be deallocated
** 		and re-used. Disk devices (hdd, ramdisk, floppy...) will
**              use this to pass disk i/o requests to the kernel.
*/

#ifndef _DISK_H
#define _DISK_H

#include "queues.h"
#include "pci.h"

#define DISK_BLOCK_SIZE   512
#define DISK_MAX_REQUESTS 256

#define DISK_CMD_READ    1
#define DISK_CMD_WRITE   2
#define DISK_CMD_REQUEST 3

/* RAM-disk stuff */

// ~500MB ram disk
#define RD_MAX_LBA 20

#define DISK_STATUS_READ_PENDING      	0x01
#define DISK_STATUS_WRITE_PENDING     	0x02
#define DISK_STATUS_IO_SUCCESS        	0x03
#define DISK_STATUS_IO_FINISHED_ERROR 	0x04

#define DISK_REQUEST_KERNEL		0x01
#define DISK_REQUEST_USER		0x02

#define DISK_ERR_INVALID_CMD      	0xa0
#define DISK_ERR_INVALID_REQ_SIZE 	0xb0
#define DISK_ERR_IO_QUEUE_INSERT  	0xc0

// TODO: need to add a device identifier field!!!
typedef struct disk_request {
	uint8_t source;		// KERNEL or USER

	uint8_t cmd;
	uint8_t status;

	uint32_t lba;
	uint32_t nblocks;
 
	void *buffer;

	// since we don't have dynamic memory, this is for the free list
	struct disk_request *next;
} disk_request_t;

/*
 * Global variables
 */
extern queue_t 		_ramdisk_io_pending;	// queue of requests blocked on disk io
extern queue_t 		_ata_io_pending;

extern pci_dev_t	_ide_controller;

disk_request_t _pending_requests[ DISK_MAX_REQUESTS ];		// pre-allocated request structures
disk_request_t *_free_disk_requests;				// pointer used for free request list

/*
** Prototypes
*/
void            _disk_dealloc_request( disk_request_t * );
disk_request_t *_disk_alloc_request( void );
void            _disk_init( pci_dev_t );

#endif
