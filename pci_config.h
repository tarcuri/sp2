/*
** File    : pci_config.h
**
** Author  : Thomas J Arcuri <tja1618>
**
*/

#ifndef _PCI_CONFIG_H_
#define _PCI_CONFIG_H_

#define __KERNEL__20083__

/* for PCI_FUNCTION_ID: 0xb1 -> AH */
#define PCI_FUNCTION_ID		0xb1

/* interrupt used for BIOS functions */
#define PCI_BIOS_FUNCTION_INT	0x1a

/*	READ_CONFIG_DWORD: 0x0a -> AL */
#define PCI_FIND_DEVICE		0x02
#define READ_CONFIG_BYTE	0x08
#define READ_CONFIG_WORD	0x09
#define READ_CONFIG_DWORD	0x0a

#define BAD_REGISTER_NUMBER	0x87


/* C-only definitions */
#ifndef __ASM__20083__

/*
** _get_pci_config_space()
**
** Given a pointer to the BIOS32 service directory, retrieves the PCI configuration
** space address.
*/
unsigned int  _get_pci_config_space( unsigned int );

/*
** _detect_pci_device( unsigned int config_addr, unsigned short vendor_id, unsigned int device_id )
*/
unsigned int _detect_pci_device( unsigned int, unsigned short, unsigned short );

/*
** _pci_bios_read_byte( unsigned char bus, unsigned char dev_function, unsigned char reg_number ) : unsigned char
**
*/
unsigned char _pci_bios_read_byte( unsigned char, unsigned char, unsigned char );

/*
** _pci_bios_read_word( unsigned char bus, unsigned char dev_function, unsigned char reg_number ) : unsigned short
**
*/
unsigned short _pci_bios_read_word( unsigned char, unsigned char, unsigned char );

/*
** _pci_bios_read_dword( unsigned char bus, unsigned char dev_function, unsigned char reg_number ) : unsigned int
**
*/
unsigned int _pci_bios_read_dword( unsigned char, unsigned char, unsigned char );

#endif

#endif
