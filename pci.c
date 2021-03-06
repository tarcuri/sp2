/*
** File    : pci.c
**
** Author  : Thomas J Arcuri <tja1618>
**
** Contributor:
**
** Description:	?
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "pci.h"
#include "ide.h"
#include "startup.h"

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
uint32_t _bios32_sd_base_address;

uint32_t _config_addr_reg_state;

/*
** PUBLIC GLOBAL VARIABLES
*/


/*
** PRIVATE FUNCTIONS
*/


/*
** search E00000h - FFFFFh for "_32_"
*/
bios32sd_t *_find_bios_sd( uint32_t signature ) {
	int check = 0;
	int i;
	uint32_t crc;
	uint32_t loc = 0xe0000;
	bios32sd_t *bios_sd = 0;

	while (check == 0 && loc <= 0xfffff) {
		bios_sd = (bios32sd_t *) loc;

		//c_printf("\n probing 0x%6x [0x%8x <-> 0x%08x] \n", loc, signature, bios_sd->signature);

		if (signature == bios_sd->signature) {
			c_printf("\nPCI: BIOS32 service directory found at 0x%06x\n", loc);
			
			for (i = 0, crc = 0; i < (bios_sd->length * 16); i++) {
				crc += *((uint32_t *) bios_sd + i);
			}

			if (crc != 0) {
			        c_puts("\nERROR: PCI: failed BIOS service directory CRC\n");
			}

			return bios_sd;	
		} else {
			// the service directories are on 16-byte paragraph boundries
			loc += 0x10;
		}
	}

	c_printf("\nERROR: PCI: BIOS32 service [ 0x%8x ] directory NOT found\n", signature);
	return 0;
}

uint32_t _pci_set_config_addr_state( uint16_t bus, uint16_t device, uint16_t function, uint16_t reg_offset ) {
	uint32_t config_addr;

	// enable bit for config address register
	config_addr = 0x80000000;

	// specify the bus, device, and function for the IDE controller
	config_addr |= ( (bus << 16) | (device << 11) | (function << 8) );

	// mask off the first two bits for a type 0 configuration
	config_addr |= (reg_offset & 0xfc);

	_config_addr_reg_state = config_addr;

	// write state to the config address register
	__outl( PCI_CONFIG_ADDR_REGISTER, config_addr );

	return config_addr;
}

pci_dev_t _pci_find_device( uint16_t vendor, uint16_t device ) {
        uint16_t bus;
	uint16_t slot;
	uint8_t func;

	pci_dev_t dev = { 0xffffffff, 0xffffffff, 0xffffffff };
	
	for ( bus = 0; bus < 0xffff; ++bus ) {
	        for ( slot = 0; slot < 0xffff; ++slot ) {
		        for ( func = 0; func < 0xff; ++func ) {
		                uint16_t dev_id  = _pci_read_config_data( bus, slot, func, 0x00, PCI_READ_CONFIG_WORD );
			        uint16_t vend_id = _pci_read_config_data( bus, slot, func, 0x02, PCI_READ_CONFIG_WORD );

				if ((vendor == vend_id) && (device == dev_id)) {
				  /* 				        c_printf("PCI: found PCI device with device_id [ 0x%04x ] and vendor_id [ 0x%04x ]\n", */
				  /* 						 device, vendor ); */

					dev.bus      = bus;
					dev.device   = slot;
					dev.function = func;

					return dev;
				}
			}
		}
	}

	return dev;
}

/*
** Public FUNCTIONS
*/
uint32_t _pci_read_config_data( uint16_t bus, uint16_t device, uint16_t function, uint16_t reg_offset, uint8_t read_mode ) {
	uint32_t config_addr = _pci_set_config_addr_state( bus, device, function, reg_offset );

	uint32_t config_data = __inl( PCI_CONFIG_DATA_REGISTER );

	uint8_t  config_data_byte;
	uint16_t config_data_word;
	uint32_t config_data_dword;

	switch (read_mode) {
	case PCI_READ_CONFIG_BYTE:
		switch (reg_offset % 4) {
		case 0:
			config_data_byte = (config_data >> 24) & 0xff;
			break;
		case 1:
			config_data_byte = (config_data >> 16) & 0xff;
			break;
		case 2:
			config_data_byte = (config_data >> 8) & 0xff;
			break;
		case 4:
			config_data_byte = config_data & 0xff;
			break;
		}

		//c_printf("PCI: configuration data register read [ 0x%04x : 0x%02x ]\n", reg_offset, config_data_byte );

		return (uint32_t) config_data_byte;
	case PCI_READ_CONFIG_WORD:
		switch (reg_offset % 4) {
		case 0:
			config_data_word = (config_data >> 16) & 0xffff;
			break;
		case 1:
			config_data_word = (config_data >> 8) & 0xffff;
			break;
		case 2:
			config_data_word = config_data & 0xffff;
			break;
		case 4:
			config_data_word = 0xffff;
			c_puts("ERROR: PCI: invalid configuration read\n");
			break;
		}

		//c_printf("PCI: configuration data register read [ 0x%04x : 0x%04x ]\n", reg_offset, config_data_word );

		return (uint32_t) config_data_word;
	case PCI_READ_CONFIG_DWORD:
		if ( (reg_offset % 4) ) {
			config_data_dword = 0xffffffff;
			c_printf("ERROR: PCI: invalid configuration read offset [ 0x%02x ]\n", reg_offset);
			break;
		} else {
			config_data_dword = config_data;
		}

		//c_printf("PCI: configuration data register read [ 0x%04x : 0x%08x ]\n", reg_offset, config_data_dword );

		return (uint32_t) config_data_dword;
	default:
		c_printf("ERROR: PCI: invalid configuration read mode [ 0x%02x ]\n", read_mode);
		return 0;
	};

	return 0;
}

void _pci_write_config_data( uint16_t bus, uint16_t device, uint16_t function,
			     uint16_t reg_offset, uint8_t write_mode, uint32_t data ) {


	uint32_t config_addr = _pci_set_config_addr_state( bus, device, function, reg_offset );

	switch (write_mode) {
	case PCI_WRITE_CONFIG_BYTE:
	  //c_printf("PCI: configuration data register write [ 0x%04x : 0x%02x ]\n", reg_offset, (uint8_t)   data );

		__outb( PCI_CONFIG_DATA_REGISTER, data & 0xff );

		break;
	case PCI_WRITE_CONFIG_WORD:
	  //c_printf("PCI: configuration data register write [ 0x%04x : 0x%04x ]\n", reg_offset, (uint16_t)  data );

		__outw( PCI_CONFIG_DATA_REGISTER, data & 0xffff );

		break;
	case PCI_WRITE_CONFIG_DWORD:
	  //c_printf("PCI: configuration data register write [ 0x%04x : 0x%08x ]\n", reg_offset, (uint32_t)  data );

		__outl( PCI_CONFIG_DATA_REGISTER, data );

		break;
	default:
		c_printf("ERROR: PCI: invalid configuration write mode [ 0x%02x ]\n", write_mode);
	}
}


void _pci_init() {
	// first search for the IDE controller
	_ide_init();

	c_puts(" pci");
}


