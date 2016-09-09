/*
** File    : disk.c
**
** Author  : Thomas Arcuri <tja1618>
**
** How should request sizes be handled? Should the driver
** assume all buffers are a multiple of the block size?
*/

#define __KERNEL__20083__

#include "headers.h"

#include "disk.h"
#include "ata.h"
#include "startup.h"

queue_t _ramdisk_io_pending;
queue_t _ata_io_pending;

pci_dev_t _ide_controller;

void _disk_dealloc_request( disk_request_t *dr ) {
	if (dr) {
		dr->next = _free_disk_requests;
		_free_disk_requests = dr;
	}
}

disk_request_t *_disk_alloc_request( void ) {
	disk_request_t *dr = _free_disk_requests;

	if (dr) {
		_free_disk_requests = dr->next;
		dr->cmd = 0;
		dr->status = 0;
		dr->lba = 0;
		dr->nblocks = 0;
		dr->buffer = 0;
		// for the free list
		dr->next = 0;
	}

	return dr;
}



void _disk_init( pci_dev_t ide_ctrl) {
	int i;

	//_ide_controller = { ide_ctrl.bus, ide_ctrl.device, ide_ctrl.function };
	_ide_controller.bus      = ide_ctrl.bus;
	_ide_controller.device   = ide_ctrl.device;
	_ide_controller.function = ide_ctrl.function;

	// reset the disk io queue
	_q_reset( &_ramdisk_io_pending, 0 );
	_q_reset( &_ata_io_pending, 0 );

	_memclr( (void *) _pending_requests, sizeof(_pending_requests) );

	_free_disk_requests = 0;

	// now setup the free list
	for (i = 0; i < sizeof(_pending_requests)/sizeof(disk_request_t); ++i) {
		_disk_dealloc_request( (disk_request_t *) &_pending_requests[i] );
	}

	c_puts( " disk:" );

	// initialize system disk devices
	_ata_init( DEV_IDE_PRI );
	//_init_ata( DEV_IDE_SEC );
}
