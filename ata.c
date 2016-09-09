/*
** File    : ata.c
**
** Author  : Thomas J Arcuri <tja1618>
**
** Created : Tue Apr 21 17:55:15 EDT 2009
**
** Description: Simple ATA disk driver.
**
** NOTES: -- implements simple programmed IO (READ_SECTOR(S), WRITE_SECTOR(S))
**        -- at the moment, disk requests larger than a single block are executed
**           as a series of single block transfers due to a lack of debugging time
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "ata.h"
#include "ide.h"
#include "device.h"

#include "ulib.h"
#include "startup.h"
#include "processes.h"
#include "scheduler.h"
#include "x86arch.h"

#define ATA_DEV_NOT_READY		0x01
#define ATA_DEV_READY			0x02
#define ATA_DEV_READ_PENDING		0x03
#define ATA_DEV_WRITE_PENDING		0x04

#define ATA_DEV_INT_PENDING		0x01
#define ATA_DEV_INT_ACKNOWLEDGED	0x02

disk_request_t         *_current_pending_request;
ata_pending_transfer_t *_current_pending_transfer;

ata_pending_transfer_t  _ata_transfer;

uint8_t  _ata_device 	   = DEV_IDE_PRI;
uint16_t _ata_cmd_blk_reg  = ATA_PRI_CMD_BLOCK_REG;
uint16_t _ata_ctrl_blk_reg = ATA_PRI_CTRL_BLOCK_REG;

void _ata_test_proc() {
	int len = 29;

	uint8_t recv_buf[ DISK_BLOCK_SIZE * 2 ];

	// wait for the system to 'come up'
	//c_puts("ATA TEST PROCESS: entering test process...\n");
	_ata_check_alt_status();
	sleep( 2 );

	_memclr( (void *) &recv_buf[0], DISK_BLOCK_SIZE * 2 );

	device_t *hdd = _dev_open( DEV_IDE_PRI );

	char input[DISK_BLOCK_SIZE];


	c_puts("\nEnter 'MBR' to print the master boot record of drive 1\n");
	c_gets( (char *) &input[0], DISK_BLOCK_SIZE );
	c_puts("\n");

	if (input[0] == 'M' && input[1] == 'B' && input[2] == 'R') {
	        _ata_test_read_mbr( hdd );
	}

	c_puts("\n\nEnter a 512 byte input string to write to LBA 1 (! to read sector)\n");
	c_gets( (char *) &input[0], DISK_BLOCK_SIZE );
	c_puts("\n");
	
	if (input[0] == '!') {
	        _ata_read_data( hdd, 1, 1, (void *) &recv_buf[0] );
	} else {
	        c_puts("Writing input string to hdd\n");

	        _ata_write_data( hdd, 1, 1, (void *) &input[0] );

		//_ata_test_read_mbr( hdd );

	        _ata_read_data( hdd, 1, 1, (void *) &recv_buf[0] );
	}

	c_puts("Reading back input string from hdd\n");

	// print the results
	c_printf("\n==> %s\n", (char *) &recv_buf[0]);

/* 	uint8_t sectors[ DISK_BLOCK_SIZE * 3 ]; */
/* 	_ata_read_data( hdd, 0, 3, (void *) &sectors[0] ); */
	
/* 	c_puts("\n\nPrinting first 3 sectors of the hdd...\n"); */
/* 	sleep( 1 ); */

/* 	_ata_print_sectors( (char *) &sectors[0], DISK_BLOCK_SIZE * 2 ); */

	_dev_close( hdd );

	c_puts("ATA TEST PROCESS: exiting ATA test process\n");

	exit();
}

void _ata_test_read_mbr( device_t *hdd ) {
        disk_request_t *dr;

	uint8_t mbr[ DISK_BLOCK_SIZE ];

	_memclr( (void *) &mbr[0], DISK_BLOCK_SIZE );

	// READ the MBR
	dr = _disk_alloc_request();
	
	dr->cmd = DISK_CMD_READ;
	dr->lba = 0;
	dr->nblocks = 1;
	dr->buffer = (void *) &mbr[0];
	
	// ugly, this will soon be replaced
	((int8_t (*)( int32_t, void * )) hdd->_ctrl)( DISK_CMD_REQUEST, (void *) dr );

	uint8_t *buf = &mbr[0];

	int i, j;
	for (i = 0; i < DISK_BLOCK_SIZE / 32; ++i) {
	        for (j = 0; j < 32; ++j) {
		  //c_putchar( *buf++ );
		        c_printf("%02x", *buf++);
		}
		
		c_putchar('\n');
	}

	// cleanup the request
	_disk_dealloc_request( dr );
}

void _ata_write_data( device_t *hdd, uint32_t lba, uint32_t blocks, void *buffer ) {
        disk_request_t *dr = _disk_alloc_request();

	int i;
	for (i = 0; i < blocks; ++i) {
	        dr->cmd     = DISK_CMD_WRITE;
		dr->lba     = lba + i;
		dr->nblocks = 1;
		dr->buffer  = buffer + (DISK_BLOCK_SIZE * i);

		((int8_t (*)( int32_t, void * )) hdd->_ctrl)( DISK_CMD_REQUEST, (void *) dr );
	}

	_disk_dealloc_request( dr );
}

void _ata_read_data( device_t *hdd, uint32_t lba, uint32_t blocks, void *buffer ) {
        disk_request_t *dr = _disk_alloc_request();

	int i;
	for (i = 0; i < blocks; ++i) {
	        dr->cmd     = DISK_CMD_READ;
		dr->lba     = lba + i;
		dr->nblocks = 1;
		dr->buffer  = buffer + (DISK_BLOCK_SIZE * i);

		((int8_t (*)( int32_t, void * )) hdd->_ctrl)( DISK_CMD_REQUEST, (void *) dr );
	}

	_disk_dealloc_request( dr );
}

void _ata_test_write_data( device_t *hdd, uint32_t lba, void *buffer, uint32_t len ) {
	uint32_t blocks = (len / DISK_BLOCK_SIZE) + (len % DISK_BLOCK_SIZE);
        disk_request_t *dr;

	dr = _disk_alloc_request();

	dr->cmd = DISK_CMD_WRITE;
	dr->lba = lba;
	dr->nblocks = blocks;
	dr->buffer = buffer;

	// ugly, this will soon be replaced
	((int8_t (*)( int32_t, void * )) hdd->_ctrl)( DISK_CMD_REQUEST, (void *) dr );

	_disk_dealloc_request( dr );
}

void _ata_test_read_data( device_t *hdd, uint32_t lba, void *buffer, uint32_t len ) {
	uint32_t blocks = (len / DISK_BLOCK_SIZE) + (len % DISK_BLOCK_SIZE);
	disk_request_t *dr;

        _memclr( buffer, len );

	dr = _disk_alloc_request();
	
	dr->cmd = DISK_CMD_READ;
	dr->lba = lba;
	dr->nblocks = blocks;
	dr->buffer = buffer;
	

	// ugly, this will soon be replaced
	((int8_t (*)( int32_t, void * )) hdd->_ctrl)( DISK_CMD_REQUEST, (void *) dr );

	_disk_dealloc_request( dr );
}

void _ata_print_sectors( void *buffer, uint32_t len ) {
	uint32_t blocks = (len / DISK_BLOCK_SIZE) + (len % DISK_BLOCK_SIZE);

	char *ch = (char *) buffer;

	int i, j;
	for (i = 0; i < ( (DISK_BLOCK_SIZE * blocks) / 32 ); ++i) {
	        for (j = 0; j < 32; ++j) {
		  //c_putchar( *ch++ );
		        c_printf("%02x", *ch++);
		}
		
		c_putchar('\n');
	}	
}

void _ata_flush_cache() {
  asm("cli");

  uint8_t alt_status = _ata_check_alt_status();

  if (alt_status & ATA_STATUS_DRDY) {
          // write command byte
          __outb( _ata_cmd_blk_reg | ATA_CMD_COMMAND, ATA7_FLUSH_CACHE );
  }

  //c_puts("ATA: waiting for flush cache...");
  while ((alt_status = _ata_check_alt_status()) & ATA_STATUS_BSY) {
    // wait for command to complete
  }
  c_puts("\n");

  if ((alt_status = _ata_check_alt_status()) & ATA_STATUS_ERR) {
           c_printf("ERROR: ATA: flush cache error\n");
  }

  asm("sti");
}

int8_t _ata_open( device_t *dev ) {
	if (!dev) {
		c_puts("ERROR: ata: open called with null device pointer\n");
		return -1;
	}

	dev->type       = DEV_TYPE_BLOCK;
	dev->identifier = DEV_IDE_PRI;

	// assign pointers to device-specific functions
	dev->_read  = _ata_read_placeholder; 		// read, and write and dummys
	dev->_write = _ata_write_placeholder;
	dev->_ctrl  = _ata_ctrl;			// ctrl should be used with this device

	return 0;
}

void _ata_close( device_t *dev ) {
	_memclr( (void *) dev, sizeof(device_t) );

	_dev_dealloc_handle( dev );
}

int32_t _ata_ctrl( int32_t cmd, void *arg ) {
 	switch (cmd) {
	case DISK_CMD_REQUEST:
		if (!arg) {
			c_puts("ERROR: ata: ctrl called with a null argument\n");
			return -1;
		}

		// the arg should be a pointer to a valid request
		return _ata_request( (disk_request_t *) arg );
	default:
		c_puts("ERROR: ata: invalid ctrl command\n");
	};

	return -1;
}


int32_t _ata_read_placeholder( void *buf, uint32_t size ) {
	c_puts("ERROR: ata: must use request interface for reads\n");
	return -1;
}

int32_t _ata_write_placeholder( void *buf, uint32_t size ) {
	c_puts("ERROR: ata: must use request interface for writes\n");
	return -1;
}

int8_t _ata_request( disk_request_t *request ) {
	disk_request_t *dr = request;
	
	if (!dr) {
		c_puts("ERROR: ata: null disk request pointer\n");
		return -1;
	}

	//c_printf("ATA: request [ 0x%08x ]\n", (void *) dr);

	dr->source = DISK_REQUEST_USER;
	
	/* check for a valid address */
	//c_puts("\nATA: processing request\n");

	switch (dr->cmd) {
	case DISK_CMD_READ:
		// add to the disk io queue
		dr->status = DISK_STATUS_READ_PENDING;

		_current->pending_request = dr;

		if ( _q_insert( &_ata_io_pending, (void *) dr ) == STAT_ALLOC_FAIL ) {
			dr->status = DISK_ERR_IO_QUEUE_INSERT;
			c_puts("ERROR: ata: failed insertion into disk io queue\n");
		}


		break;
	case DISK_CMD_WRITE:
		// TODO: this needs to go to a write buffer immediately since we return
		dr->status = DISK_STATUS_WRITE_PENDING;

		_current->pending_request = dr;

		if ( _q_insert( &_ata_io_pending, (void *) dr ) == STAT_ALLOC_FAIL ) {
			dr->status = DISK_ERR_IO_QUEUE_INSERT;
			c_puts("ERROR: ata: failed insertion into disk io queue\n");
		}
		
		break;
	default:
		dr->status = DISK_ERR_INVALID_CMD;

		c_puts("ERROR: ata: invalid command\n");
		return dr->status;
	};

	
	// clear nIEN, re-enable interrupts
	__outb( _ata_ctrl_blk_reg | ATA_CTRL_DEV_CONTROL, 0x00 );

	_ata_do_pio_data( dr );

	uint8_t alt_status = _ata_check_alt_status();

	if (dr->cmd == DISK_CMD_WRITE) {
	        // wait 400ns before checking status
	        sleep( 2 );

		while (alt_status & ATA_STATUS_BSY) { alt_status = _ata_check_alt_status(); }

		if (alt_status & ATA_STATUS_DRQ) {
		  /* c_printf("ATA: request: processing write transfer (%d blocks remaining)\n", */
/* 		  	 _current_pending_transfer->blocks_remaining); */

			_ata_write_block ( _current_pending_transfer->request->buffer );

			alt_status = _ata_check_alt_status();

			if (!_current_pending_transfer) {
			        // schedule the blocked process	
				_current_pending_request->status = DISK_STATUS_IO_SUCCESS;

				_current_pending_request = 0;
			}
		}

		//c_puts("ATA: blocking write request\n");
	}

	

	while (dr->status != DISK_STATUS_IO_SUCCESS) {
	  // wait, unfortunately I was having issues with blocking processes
	}

	if (dr->cmd == DISK_CMD_WRITE) {
	        _ata_flush_cache();
	}

	return dr->status;
}


void _ata_do_pio_data( disk_request_t *dr ) {
        /*
	  LBA partitioning:
	  1st 8 bits: ATA_CMD_BR_CYL_LOW
	  2nd 8 bits: ATA_CMD_BR_CYL_MID
	  3rd 8 bits: ATA_CMD_BR_CYL_HIGH
		  last 4 bits: ATA_CMD_BR_DRIVE_SELECT
	*/
        //c_puts("ATA: PIO data command\n");
  
	uint8_t status = _ata_check_alt_status();
	while ( (status & ATA_STATUS_BSY) || !(status & ATA_STATUS_DRDY) ) {
                status = _ata_check_alt_status();
	}

        //c_printf("ATA: request: initiating transfer command [ 0x%02x ]\n", _ata_check_alt_status());

	uint8_t lba_low  = (uint8_t) dr->lba;
	uint8_t lba_mid  = (uint8_t) dr->lba >> 8;
	uint8_t lba_high = (uint8_t) dr->lba >> 16;
	
	// set the device register, handle highest 4 bits of LBA
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_DRIVE_SELECT, ATA_DEV_REG_SELECT_1 |
		ATA_DEV_REG_SELECT_LBA | ((dr->lba >> 24) & 0x0f) );
	
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_CYL_LOW, lba_low );
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_CYL_MID, lba_mid );
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_CYL_HIGH, lba_high );
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_SEC_COUNT, 0x01 );


	// now write READ SECTOR(S) to the command register
	if ( dr->cmd == DISK_CMD_READ ) {
		 __outb( _ata_cmd_blk_reg | ATA_CMD_BR_COMMAND, ATA7_READ_SECTORS );
	} else {
	         __outb( _ata_cmd_blk_reg | ATA_CMD_BR_COMMAND, ATA7_WRITE_SECTORS );
	}
	
	// now we need to wait for an IRQ to proceed....
	// OR we can poll ;)
	
	//_ata_transfer = { dr, dr->lba, dr->n_blocks };
	_ata_transfer.request            = dr;
	_ata_transfer.current_block_addr = dr->lba;
	_ata_transfer.blocks_remaining   = dr->nblocks;

	_current_pending_request  = dr;
	_current_pending_transfer = &_ata_transfer;

	//c_printf("ATA: set pending request\n");

	_ata_check_alt_status();
}

/* read a 512 byte block */
void _ata_read_block( void *buffer ) {
	// read in 16-bit words at a time
	int i;
	uint16_t word;
	uint16_t *buf_p = (uint16_t *) buffer;

	for (i = 0; i < DISK_BLOCK_SIZE / 2; i++) {
		word = (uint16_t) __inw( _ata_cmd_blk_reg | ATA_CMD_DATA );
		*buf_p++ = word;
	}

	_current_pending_transfer->blocks_remaining--;

	if ( _current_pending_transfer->blocks_remaining ) {
		_current_pending_transfer->current_block_addr++;
	} else {
		_current_pending_transfer = 0;
	}
}

void _ata_write_block( void *buffer ) {
	// now we should be set up to actually transfer
	
	// write out 16-bit words at a time
	int i;
	uint16_t word;
	uint16_t *buf_p = (uint16_t *) buffer;

	for (i = 0; i < DISK_BLOCK_SIZE / 2; i++) {
		word = *buf_p++;
		__outw( _ata_cmd_blk_reg | ATA_CMD_BR_DATA, word );
	}

	_current_pending_transfer->blocks_remaining--;

	if ( _current_pending_transfer->blocks_remaining ) {
		_current_pending_transfer->current_block_addr++;
	} else {
		_current_pending_transfer = 0;
	}
}

void _ata_isr( int vector, int code ) {
	uint8_t 	ide_status_reg;
	disk_request_t 	*dr;

	//c_printf("\nATA: received IDE interrupt -- vector: 0x%02x code: 0x%02x\n", vector, code );

	if ( !_current_pending_request ) {
	        /*
	        ** HPIOI0: INTRQ_Wait state
		*/
	  
		// there is no pending transfer, so check for a new request to initiate
		//c_puts("\nATA: preparing to initiate transfer...\n");

		// now we can read the status register
		ide_status_reg = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );
	
		if ( ide_status_reg & ATA_CMD_BR_R_STS_ERR ) {
			// an error occured during execution of the previous command
			c_printf("ERROR: ATA: IDE status [ 0x%08x ]\n", ide_status_reg );

			uint8_t error_reg = __inb( _ata_cmd_blk_reg | ATA_CMD_BR_ERROR );
	
			c_printf("ERROR: ATA: IDE error  [ 0x%02x ]\n", error_reg );
		}

		// need to clear nIEN
		//c_puts("ATA: identify device: clearing nIEN\n");
		__outb( ATA_PRI_CTRL_BLOCK_REG | IDE_CTRL_BLK_REG_DEVICE_CONTROL, 0x00 );		

		// looping forever here... || ide_status_reg & ATA_CMD_BR_R_STS_DRQ
		while ( ide_status_reg & ATA_CMD_BR_R_STS_BUSY ) {
			ide_status_reg = __inb( _ata_cmd_blk_reg | ATA_CMD_BR_STATUS );
		}

		if ( ide_status_reg & ATA_CMD_BR_R_STS_DRDY ) {
			// device can accept commands
	
			// Now we handle pending requests
			//dr = (disk_request_t *) _q_remove(&_ata_io_pending);

		        dr = (disk_request_t *) _q_remove(&_ata_io_pending);
			if (!dr) {
			        // no requests are waitng
			        //c_puts("ATA: no pending requests\n");

				// set nIEN, disable interrupts
				__outb( _ata_ctrl_blk_reg | ATA_CTRL_DEV_CONTROL, ATA_CTRL_DEV_CONTROL_NIEN );
				
				//				break;
				
			} else {
			  //c_printf("ATA: unhandled pending request [ 0x%08x ]\n", (void *) dr);

			  // should we simply set this as the next pending request?
			  if (dr->cmd == DISK_CMD_WRITE) {
			    //c_puts("ATA: write command complete\n");
			  }
			}
		} else {
			c_puts("\nATA: command block status is not ready\n");
		}

	} else {
		// process the pending request

		// since the drive is already selected, just check the status first
		//ide_status_reg = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );
	        if ( (_current_pending_request->cmd == DISK_CMD_WRITE) && !_current_pending_transfer ) {
			  _current_pending_request->status = DISK_STATUS_IO_SUCCESS;

			  _current_pending_request = 0;
		} else {
	                ide_status_reg = _ata_check_alt_status();

			// we might have to wait (poll) for the DRQ

			if ( ide_status_reg & ATA_CMD_BR_R_STS_ERR ) {
			  // an error occured during execution of the previous command
			  c_printf("ERROR: ATA: IDE status [ 0x%08x ]\n", ide_status_reg );
			  
			  uint8_t error_reg = __inb( _ata_cmd_blk_reg | ATA_CMD_BR_ERROR );
			  
			  c_printf("ERROR: ATA: IDE error  [ 0x%02x ]\n", error_reg );
			}
			
			if (_current_pending_request && ide_status_reg & ATA_STATUS_DRQ) {
			switch (_current_pending_request->cmd) {
			case DISK_CMD_READ:
			         if ( ide_status_reg & ATA_STATUS_DRQ ) {
				   //c_printf("ATA: processing read transfer (%d blocks remaining)\n",
				   //	  _current_pending_transfer->blocks_remaining);

					 _ata_read_block( _current_pending_transfer->request->buffer );
					 
					 _ata_check_alt_status();
					 
					 if ( !_current_pending_transfer ) {
					   // schedule the blocked process	
					         _current_pending_request->status = DISK_STATUS_IO_SUCCESS;
					 }
				 }
				 
				 break;
			case DISK_CMD_WRITE:
		        // 1) wait for BSY=0 and DRQ=1 to transfer
			  
			        if (ide_status_reg & ATA_STATUS_DRQ) {
				  
				  /* c_printf("ATA: processing write transfer (%d blocks remaining)\n", */
/* 				  	 _current_pending_transfer->blocks_remaining); */

					// from what I can tell we keep transferring
					_ata_write_block ( _current_pending_transfer->request->buffer );
					
					_ata_check_alt_status();
					
					if (!_current_pending_transfer) {
					  // schedule the blocked process	
					        _current_pending_request->status = DISK_STATUS_IO_SUCCESS;
					}
				}
				
				break;
			default:
			  c_puts("ERROR: ATA: reached default case\n");
			};
			}
		}
		

		if ( !_current_pending_transfer ) {
			// complete the request
			_current_pending_request = 0;
		}
	}

	// tell the PIC that we're done
	if( vector >= 0x20 && vector < 0x30 ){
	        __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
		if( vector > 0x28 ){
		        __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
		}
	}
}

// TODO: fix this
void _ata_identify_device() {
	// we are going to poll for this command...
	asm( "cli" );

	// set nIEN
	__outb( _ata_ctrl_blk_reg | ATA_CTRL_DEV_CONTROL, ATA_CTRL_DEV_CONTROL_NIEN );

	// we need to wait for DRDY
	uint8_t status = _ata_check_alt_status();
	
	c_puts("ATA: identify device: waiting for DRDY...\n");
	while ( !(status & ATA_STATUS_DRDY) ) {
		status = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );
	}

	// write the IDENTIFY DEVICE command
	c_puts("ATA: identify device: sending command byte\n");
	__outb( _ata_cmd_blk_reg | ATA_CMD_BR_COMMAND, ATA7_IDENTIFY_DEVICE );

	
	// now transfer 256 words of data
	int word_i;
	uint16_t ata_id_device_data[ 256 ];

	/*
	** i)  wait for DRQ=1 and BSY=0 (cycle status register)
	** ii) transfer a word, if more remain, repeat
	*/	
	// now transfer a word
	c_puts("ATA: identify device: reading coniguration data");
	for (word_i = 0; word_i < 256; ++word_i) {
		// wait for DRQ
		//c_puts("ATA: identify device: waiting for DRQ...\n");
		while ( (status & ATA_STATUS_BSY) || !(status & ATA_STATUS_DRQ) ) {
			status = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );

			
		}

		//c_puts("ATA: identify device: reading data...\n");
		
		// read from Data register
		ata_id_device_data[ word_i ] = __inw( _ata_cmd_blk_reg | ATA_CMD_DATA );

		c_puts(".");
	}
	c_puts("\n");
	
	// read status
	status = _ata_check_alt_status();

	_ata_print_device_info( (uint16_t *) &ata_id_device_data[0] );

	// clear nIEN so we get interrupts again
	__outb( _ata_ctrl_blk_reg | ATA_CTRL_DEV_CONTROL, 0x00 );

	// Subaru Technica International (STi)
	asm( "sti" );
}

// expects pointer to 256 words of data
void _ata_print_device_info( uint16_t *dev_data ) {
	// check for NULL pointers...

	c_puts("ATA IDENTIFY DEVICE:\n\n");

	// see Table 16 of ATA/ATAPI-7 Volume 1

	// words 27-46: model number (40 ASCII characters)
	char model_num[ 41 ];
	_memcpy( (void *) &model_num[0], (void *) &dev_data[ 27 ], 40 );

	model_num[ 40 ] = '\0';

	c_printf("MODEL: (%c%c) %s\n", model_num[0], model_num[1],(char *) &model_num[0]);

	// words 60-61 are the total number of user addressable sectors
	uint32_t user_addressable_sectors = *( (uint32_t *) &dev_data[ 60 ]);

	c_printf("Total number of user addressable sectors (maximum LBA + 1): 0x%08x\n", user_addressable_sectors);

	// check for multi-word DMA support (word 63)
	uint16_t dma_modes = dev_data[ 63 ];
	
	c_printf("DMA modes 0x%04x\n", dma_modes );

	if ( dma_modes & 0x0004 ) {
		c_puts("Multiword DMA mode 2 and below are supported\n");
	} else if ( dma_modes & 0x0002 ) {
		c_puts("Multiword DMA mode 1 and below are supported\n");
	} else if ( dma_modes & 0x0001 ) {
		c_puts("Multiword DMA mode 0 is supported\n");
	}

	// word 64: supported PIO modes (1st byte only)

	// word 88: Ultra DMA modes
	uint16_t ultra_dma_modes = dev_data[ 88 ];

	c_printf("Ultra DMA modes 0x%04x\n", ultra_dma_modes );

	if ( ultra_dma_modes & 0x0030 ) {
		c_puts("Ultra DMA mode 6 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0020 ) {
		c_puts("Ultra DMA mode 5 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0010 ) {
		c_puts("Ultra DMA mode 4 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0008 ) {
		c_puts("Ultra DMA mode 3 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0004 ) {
		c_puts("Ultra DMA mode 2 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0002 ) {
		c_puts("Ultra DMA mode 1 and below are supported\n");
	} else if ( ultra_dma_modes & 0x0001 ) {
		c_puts("Ultra DMA mode 0 is supported\n");
	}
	
}

uint8_t _ata_check_alt_status() {
	int i;

	uint8_t status = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );

	for (i = 0; i < 5; ++i) {
		status = __inb( _ata_ctrl_blk_reg | ATA_CTRL_ALT_STATUS );
	}

	//c_printf("ATA: alternate status [ 0x%02x ]\n", status );

/* 	if ( status & ATA_STATUS_BSY ) { */
/* 	        c_printf("ATA: BSY set\n"); */
/* 	} */
	
/* 	if ( status & ATA_STATUS_DRDY ) { */
/* 	        c_printf("ATA: DRDY set\n"); */
/* 	} */
	
/* 	if ( status & ATA_STATUS_DRQ ) { */
/* 	        c_printf("ATA: DRQ set\n"); */
/* 	} */

/* 	if ( status & ATA_STATUS_ERR ) { */
/* 	        c_printf("ATA: ERR set\n"); */
/* 	} */
	
/* 	c_puts("Press any key to continue\n"); */

/* 	c_getchar(); */

	return status;
}



void _ata_init( uint8_t ATA_DEVICE ) {
	switch (ATA_DEVICE) {
	case DEV_IDE_PRI:
		// select the PRIMARY IDE channel
		_ata_device 	  = DEV_IDE_PRI;
		_ata_cmd_blk_reg  = ATA_PRI_CMD_BLOCK_REG;
		_ata_ctrl_blk_reg = ATA_PRI_CTRL_BLOCK_REG;

		// select device 1
		__outb( _ata_cmd_blk_reg | ATA_CMD_BR_DRIVE_SELECT, ATA_DEV_REG_SELECT_LBA | ATA_DEV_REG_SELECT_1 );	

		// clear nIEN
		__outb( _ata_ctrl_blk_reg | IDE_CTRL_BLK_REG_DEVICE_CONTROL, 0x00 );

		// issue reset is DEVICE CONTROL register, clear nIEN
		//_ata_check_alt_status();

		//_ata_identify_device();

		break;
	case DEV_IDE_SEC:
		// read the command, control, and bus master base address registers
		
		break;
	default:
		c_printf("\nERROR: ATA: invalid ATA device [ 0x%02x ]\n", ATA_DEVICE );
	};

	c_puts(" ata");
}
