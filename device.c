/*
** File    : device.c
**
** Author  : Thomas Arcuri <tja1618>
**
**
*/

#define __KERNEL__20083__

#include "headers.h"

#include "device.h"
#include "disk.h"
#include "ramdisk.h"
#include "pci.h"
#include "ata.h"

#include "ulib.h"

#define MAX_OPEN_DEVICES 256

/*
** Private data
*/
static device_t _device_handles[ DEV_MAX_DEVICES ];
static device_t *_free_device_handles;

/*
** Public functions
*/
device_t* _dev_open( uint8_t device_num ) {
	device_t *dev;

	dev = _dev_alloc_handle();
    	if (!dev) {
      		c_puts("ERROR: device: unable to allocate a device handle\n");
      		return dev;
    	}

	/*
	** Eventually pointers to device specific open/close functions should
	** be placed into a table.
	*/
	switch (device_num) {
	case DEV_RAMDISK:
		_rd_open( dev );
		break;
	case DEV_IDE_PRI:
		_ata_open( dev );
		break;
	case DEV_IDE_SEC:
		c_puts("ERROR: DEVICE: secondary IDE channel unavailable\n");
		break;
	default:
		c_printf("ERROR: device: invalid device number %d\n", device_num);
  	};

  	return dev;
}

void _dev_close( device_t *dev ) {
	if (dev) {
		// first call the device specific close function
		switch (dev->identifier) {
		case DEV_RAMDISK:
			_rd_close( dev );
			break;
		case DEV_IDE_PRI:
		        _ata_close( dev );
		case DEV_IDE_SEC:
			break;
		default:
			c_printf("ERROR: device: unable to close invalid device %d\n", dev->identifier);
		};

		_dev_dealloc_handle( (device_t *) dev );
	}
}

void _dev_dealloc_handle( device_t *dev ) {
	if (dev) {
    		dev->next = _free_device_handles;
    		_free_device_handles = dev;
  	}
}

device_t *_dev_alloc_handle( void ) {
	device_t *dev = _free_device_handles;

  	if (dev) {
    		_free_device_handles = dev->next;
		dev->type = 0;
    		dev->identifier = 0;
    		dev->next = 0;
  	}

  	return dev;
}


void ramdisk_test( void ) {
	disk_request_t *dr;
	char buffer[ 512 ];
	char test_string[ 512 ] = { "this is a 512 byte test string that will be separated by a macro for testing purposes in the systems programming 2 team 5 operating system project -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------============================================================================================+++++++++++++++++++++++++++++++++++++++" };

	c_puts("RAMDISK test process\n");

	// first open the device
	device_t *disk = _dev_open( DEV_RAMDISK );

	// needs error checking

	//dh = (disk_handle_t *) disk->device;	// this is the 'iffy' part; you need to cast this pointer
	dr = _disk_alloc_request();
	
	dr->cmd = DISK_CMD_WRITE;
	dr->lba = 0;
	dr->nblocks = 1;
	dr->buffer = (void *) &test_string[0];
	
	// ugly, this will soon be replaced
	((int8_t (*)( int32_t, void * )) disk->_ctrl)( DISK_CMD_REQUEST, (void *) dr );

	// wait for the last op to complete
	c_puts("Waiting for ramdisk write to complete\n");
	while (DISK_STATUS_IO_SUCCESS != dr->status) {
	  c_puts(".");
	  sleep(1);
	}
	c_puts("\n");
	
	// cleanup the request
	_disk_dealloc_request( dr );
	
	// now read them back in reverse order
	dr = _disk_alloc_request();

	dr->cmd = DISK_CMD_READ;
	dr->lba = 0;
	dr->nblocks = 1;
	dr->buffer = (void *) &buffer[0];
	
	((int8_t (*)( int32_t, void * )) disk->_ctrl)( DISK_CMD_REQUEST, (void *) dr );
	
	c_puts("Waiting for ramdisk read to complete");
	while (DISK_STATUS_IO_SUCCESS != dr->status) {
	  c_puts(".");
	  sleep(1);
	}
	
	c_printf("\n%s\n", buffer);
	
	// cleanup the request
	_disk_dealloc_request( dr );

	_dev_close( disk );

	exit();
}

void _dev_init( void ) {
	int i;
  
	// clear all the device handle memory
  	_memclr( (void *) _device_handles, sizeof( _device_handles ) );

  	_free_device_handles = 0;

  	// now set up the free list
  	for (i = 0; i < sizeof(_device_handles)/sizeof(device_t); ++i ) {
    		_dev_dealloc_handle( (device_t *) &_device_handles[i] );
  	}

  	c_puts( " device" );

	// initialize system devices
	_pci_init();
}
