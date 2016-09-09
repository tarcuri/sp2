/*
**
** File    : ide.c
**
** Author  : Thomas J Arcuri <tja1618>
**
** Created : Tue Apr 21 17:55:15 EDT 2009
**
** Intel ICH5 IDE I/O controller initialization and configuration
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "ide.h"
#include "disk.h"
#include "ata.h"
#include "pci.h"

// device goes into upper 5 bits, function in lower 3
#define PCI_DEV_31_FUNC_1	0xf9

#define IDE_DEV_READY		0x01
#define IDE_DEV_BUSY		0x02

void _ide_init() {
        // first find the bus/dev/func for the ICH5 IDE controller
        pci_dev_t ide_controller = _pci_find_device( ICH5_IDE_CTRL_VENDOR_ID, ICH5_IDE_CTRL_DEVICE_ID );

/* 	c_printf("\nIDE: found IDE controller B(%02xh):D(%02xh):F(%02xh)\n", */
/* 		 ide_controller.bus, ide_controller.device, ide_controller.function); */

	// program the PCI command register
	uint32_t pci_cmd_state = 0;

	// enable IO space for the IDE controller
	pci_cmd_state |= PCI_CMD_REG_IO_ENABLE;

	//_pci_write_config_data( ide_controller.bus, ide_controller.device, ide_controller.function,
	//			ICH5_IDE_CTRL_PCI_COMMAND_REGISTER, PCI_WRITE_CONFIG_DWORD, pci_cmd_state );

	// now program the Programming Interface (PI) register to enable legacy mode
	//_pci_write_config_data( ide_controller.bus, ide_controller.device, ide_controller.function,
	//			ICH5_IDE_CTRL_PROG_INT_REGISTER, PCI_WRITE_CONFIG_BYTE, 0x00 );

	_disk_init( ide_controller );

	c_puts(" ide");
}


