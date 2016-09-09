/*
** SCCS ID:	%W%	%G%
**
** File    : pci.h
**
** Author  : Thomas J Arcuri <tja1618>
**
** Contributor:
**
** Description:	?
*/

#ifndef _PCI_H
#define _PCI_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// "_32_" ((uint32_t) ( ('_' << 24) | ('3' << 16) | ('2' << 8) | ('_') ))
#define BIOS_SD_32_BIT_SIG		0x5f32335f

// "$PCI" ((uint32_t) ( ('$' << 24) | ('P' << 16) | ('C' << 8) | ('I') ))
#define BIOS_SD_PCI_SIG			0x49435024

// PCI CONFIG_ADDRESS
#define PCI_CONFIG_ADDR_REGISTER	0x0cf8
// PCI CONFIG DATA
#define PCI_CONFIG_DATA_REGISTER	0x0cfc

#define PCI_FUNCTION_ID			0xb1
#define	PCI_FIND_DEVICE			0x02
#define PCI_READ_CONFIG_BYTE		0x08
#define PCI_READ_CONFIG_WORD		0x09
#define PCI_READ_CONFIG_DWORD		0x0a
#define PCI_WRITE_CONFIG_BYTE 		0x0b
#define PCI_WRITE_CONFIG_WORD		0x0c
#define PCI_WRITE_CONFIG_DWORD 		0x0d

#define PCI_SUCCESSFUL 			0x00
#define PCI_FUNC_NOT_SUPPORTED 		0x81
#define PCI_BAD_VENDOR_ID 		0x83
#define PCI_DEVICE_NOT_FOUND 		0x86
#define PCI_BAD_REGISTER_NUMBER		0x87
#define PCI_SET_FAILED 			0x88
#define PCI_BUFFER_TOO_SMALL 		0x89


// PCICMD register
#define PCI_CMD_REG_OFFSET		0x0004
#define PCI_CMD_REG_INT_DISABLE		0x0200
#define PCI_CMD_REG_IO_ENABLE		0x0001

// PCI bus master base address register
#define PCI_BUS_BASE_ADDR_REG_OFFSET	0x0020


#ifndef __ASM__20083__

extern uint32_t _bios32_sd_base_address;

/*
** Start of C-only definitions
*/

/*
** Types
*/
typedef struct bios_32_service_dir {
	uint32_t signature;		// directory signature ($PCI, _32_, etc..)
	uint32_t phys_bsd_addr;		// 32-bit entry point address
	uint8_t	revision;
	uint8_t length;			// length of data structure in 16-byte paragraphs
	uint8_t crc;
	uint8_t reserved[5];
} bios32sd_t;

typedef struct bios_service_address {
        uint32_t base_address;         // EBX
	uint32_t length;               // ECX
	uint32_t entry_point;          // EDX
} bios_service_addr_t;

typedef struct pci_device {
        uint32_t bus;
        uint32_t device;
        uint32_t function;
} pci_dev_t;
 
/*
** Globals
*/

/*
** Prototypes
*/
pci_dev_t       _pci_find_device( uint16_t, uint16_t );
uint32_t 	_pci_set_config_addr_state( uint16_t, uint16_t, uint16_t, uint16_t );
uint32_t 	_pci_read_config_data( uint16_t, uint16_t, uint16_t, uint16_t, uint8_t );
void		_pci_write_config_data( uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, uint32_t );
void		_pci_init( void );
bios32sd_t	*_find_pci_bios_sd( void );

// assembly routines
uint32_t       _call_bios32_service_directory( uint32_t service_id, bios_service_addr_t *bios_addr );
uint32_t       _call_pci_bios_service_directory( uint16_t, uint32_t * );	

#endif

#endif
