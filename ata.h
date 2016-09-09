#ifndef _ATA_H
#define _ATA_H

#include "headers.h"
#include "device.h"
#include "disk.h"
#include "processes.h"

#define ATA_INT_VEC_PRI_PATA		0x2e
#define ATA_INT_VEC_SEC_PATA		0x2f

// ATA/ATAPI-7 commands
#define ATA7_DEVICE_RESET		0x08
#define ATA7_READ_SECTORS		0x20
#define ATA7_WRITE_SECTORS		0x30
#define ATA7_DEVICE_CONFIG_IDENTIFY     0xb1
#define ATA7_DEVICE_CONFIG_ID_FEATURES  0xc2
#define ATA7_IDENTIFY_DEVICE		0xec
#define ATA7_FLUSH_CACHE                0xe7

// command block register addresses
#define ATA_PRI_CMD_BLOCK_REG		0x01f0
#define ATA_SEC_CMD_BLOCK_REG		0x0170

// device status
#define ATA_DEVICE_READY                0x01
#define ATA_DEVICE_BUSY                 0x02
#define ATA_DEVICE_UNAVAILABLE          0x03

// command block register offsets
#define ATA_CMD_BR_DATA			0x00	// data (r/w)
#define ATA_CMD_BR_ERROR		0x01	// error status (ro)
#define ATA_CMD_BR_FEATURES		0x01	// features (wo)
#define ATA_CMD_BR_SEC_COUNT		0x02	// sector count (r/w)
#define ATA_CMD_BR_CYL_LOW		0x03	// cylinder low (r/w)
#define ATA_CMD_BR_CYL_MID		0x04	// cylinder mid (r/w)
#define ATA_CMD_BR_CYL_HIGH		0x05	// cylinder high (r/w)
#define ATA_CMD_BR_DRIVE_SELECT		0x06	// drive select (r/w)
#define ATA_CMD_BR_STATUS		0x07	// status (ro)
#define ATA_CMD_BR_COMMAND		0x07	// command (wo)

#define ATA_CMD_DATA			0x00	// data (r/w)
#define ATA_CMD_ERROR			0x01	// error status (ro)
#define ATA_CMD_FEATURES		0x01	// features (wo)
#define ATA_CMD_SEC_COUNT		0x02	// sector count (r/w)
#define ATA_CMD_CYL_LOW			0x03	// cylinder low (r/w)
#define ATA_CMD_CYL_MID			0x04	// cylinder mid (r/w)
#define ATA_CMD_CYL_HIGH		0x05	// cylinder high (r/w)
#define ATA_CMD_DRIVE_SELECT		0x06	// drive select (r/w)
#define ATA_CMD_STATUS			0x07	// status (ro)
#define ATA_CMD_COMMAND			0x07	// command (wo)

// IDE status register bits
#define ATA_CMD_BR_R_STS_BUSY  		0x80 	// bit 7 - busy
#define ATA_CMD_BR_R_STS_DRDY		0x40	// bit 6 - device ready
#define ATA_CMD_BR_R_STS_DWF		0x20	// bit 5 -
#define ATA_CMD_BR_R_STS_DSC		0x10	// bit 4 -
#define ATA_CMD_BR_R_STS_DRQ		0x08	// bit 3 - device request
#define ATA_CMD_BR_R_STS_CORR		0x04	// bit 2 - 
#define ATA_CMD_BR_R_STS_IDX		0x02	// bit 1 - 
#define ATA_CMD_BR_R_STS_ERR		0x01	// bit 0 - error

// the 5th bit of the device register selects the IDE drive (set to select drive 1)
#define ATA_DEV_REG_SELECT_LBA		0x40
#define ATA_DEV_REG_SELECT_1		0x10


/*
** control block registers (4 bytes)
**
** primary  : 0x03f4 - 0x03f7
** secondary: 0x0374 - 0x0377
*/

// control block register addresses
#define ATA_PRI_CTRL_BLOCK_REG  		0x03f4
#define ATA_SEC_CTRL_BLOCK_REG 			0x0374

#define	ATA_CTRL_ALT_STATUS			0x06
#define ATA_CTRL_DEV_CONTROL			0x06

#define ATA_STATUS_BSY  			0x80 	// bit 7 - busy
#define ATA_STATUS_DRDY				0x40	// bit 6 - device ready
#define ATA_STATUS_DF				0x20	// bit 5 - device fault
#define ATA_STATUS_DSC				0x10	// bit 4 - reserved
#define ATA_STATUS_DRQ				0x08	// bit 3 - device request
#define ATA_STATUS_CORR				0x04	// bit 2 - obsolete
#define ATA_STATUS_IDX				0x02	// bit 1 - obsolete
#define ATA_STATUS_ERR				0x01	// bit 0 - error

#define ATA_CTRL_DEV_CONTROL_SRST		0x04
#define ATA_CTRL_DEV_CONTROL_NIEN       	0x02

/*
** bus master regisers (8 bytes)
**
** primary  : 0xffa0 - 0xffa7
** secondary: 0xffa8 - 0xffaf
*/

#define ATA_PRI_BUS_MASTER_REG 			0xffa0
#define ATA_SEC_BUS_MASTER_REG 			0xffa8

/*
** ICH5 IDE controller registers
**
** The actual IO controller registers of the Intel 82801EB ICH5.
*/

#define ICH5_ATA_PCI_CONFIG_ADDR_PORT		0x0cf8	// PCI configuration 32-bit address port (0cf8-0cfbh)
#define ICH5_ATA_PCI_CONFIG_DATA_PORT		0x0cfc	// PCI configuration 32-bit data port    (0cfc-0cffh)

/*
** ICH5 IDE controller PCI command register
**
*/

#define ICH5_ATA_PCICMD_REGISTER		0x04	// 04-05 PCI command offset

// the 10th bit of the register disables interrupts
#define ICH5_ATA_PCICMD_REG_INT_DISABLE		0x0400
#define ICH5_ATA_PCICMD_REG_IO_ENABLE		0x0001

/*
** ICH5 IDE controller PCI status register
**
*/
#define ICH5_ATA_PCISTS_REGISTER		0x06	// 06-07h PCI status

#define ICH5_ATA_PCISTS_INT_STATUS_ASSERTED	0x0008	// 4th bit is on if INT is asserted
#define ICH5_ATA_PCISTS_SIG_SYS_ERROR		0x4000	// 15th bit is on for Signaled System Error
#define ICH5_ATA_PCISTS_SIG_PAR_ERROR		0x8000	// 16th bit is on for Detected Parity Error

/*
** ICH5 IDE controller Programming Interface Register
*/

#define ICH5_ATA_PROG_INTERFACE_REGISTER	0x09	// see 10.1.6

#define ICH5_ATA_PRI_OP_MODE_NATIVE		0x01	// primary mode select   :mode 1: native, mode 0: legacy (default)
#define ICH5_ATA_SEC_OP_MODE_NATIVE		0x04	// secondary mode select
#define ICH5_ATA_BUS_MASTER_OP_SUPPORTED	0x80	// ro bit is 1 to indicate bus master operation support


/*
** Private Global Variables
*/

#ifndef __ASM__20083__

extern pcb_t *_real_pcb, *_fake_pcb;

extern uint8_t _device_status;
extern uint8_t _device_int_status;
extern uint8_t _ata_device;
extern uint16_t _ata_cmd_blk_reg;


typedef struct ata_pending_transfer {
	disk_request_t  *request;
	uint32_t	current_block_addr;
        uint32_t        blocks_remaining;
} ata_pending_transfer_t;

/*
** Prototypes
*/
int8_t 		_ata_open( device_t * );
void 		_ata_close( device_t * );
int32_t         _ata_ctrl( int32_t, void * );
int8_t 		_ata_request( disk_request_t * );

/*
** init_ide()
**
** Initialize the ICH5 IDE controller (see the ICH5 programmers reference manual)
**
** 1) if native mode is desired, set the appropriate bit in the Programming Interface register
**
** 2) PCI "Expansion Base Memory Address" register (24-27h) is assigned 32-bit base memory address
**
** 3) base controller configuration:
**
**	a) "Bus Master Base IO Address" register (20-23h) is assigned 16-bit base IO address**
**	b) PCI command register bit 0 is 1, bit 2 is 1
**	c) IDE Decode Enable bit is 1 for each controller
**	d) which bit is this referring to?
**	e) see d
**
** 4) issue an IDENTIFY_DRIVE command to connected ATA devices to determine capabilities
**
*/
void _ata_test_proc( void );
void _ata_test_read_mbr( device_t * );
void _ata_write_data( device_t *, uint32_t, uint32_t, void *);
void _ata_read_data( device_t *, uint32_t, uint32_t, void *);

void _ata_identify_device( void );
void    _ata_print_device_info( uint16_t * );

void _ata_print_sectors( void *, uint32_t );
void _ata_flush_cache( void );
uint8_t _ata_check_alt_status( void );
void    _ata_do_pio_data( disk_request_t * );
void		_ata_read_block( void * );
int32_t         _ata_read_placeholder( void *, uint32_t );
void		_ata_write_block( void * );
int32_t         _ata_write_placeholder( void *, uint32_t );


void _ata_isr( int, int );
void _ata_init( uint8_t );

#endif

#endif
