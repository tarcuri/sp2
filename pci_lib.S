/*
** sccs id:	%W%	%G%
**
** File:	?
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	?
*/


#define	__ASM__20083__

#include "pci.h"
	
.section .text

	.type	_call_bios32_service_directory, @function
	.globl	_call_bios32_service_directory
_call_bios32_service_directory:
	pushl	%ebp
	movl	%esp, %ebp

	// does this RETF or RET?
	movl	8(%ebp), %eax
	xorl	%ebx, %ebx
	call	_bios32_sd_base_address

	// AL:	 00h on success, 80h on service not present, 81h unknown function
	cmpb	$0, %al
	je	BIOS32_CALL_SUCCESS

	// else handle errors
	jmp	BIOS32_CALL_END
	
BIOS32_CALL_SUCCESS:
	// 12(%ebp) points to the bios_service_addr_t structure
	// %ebx -> bios_addr.base_address
	// %ecx -> bios_addr.length
	// %edx	-> bios_addr.entry_point
	movl	12(%ebp), %eax
	movl	%ebx, 0(%eax)
	movl	%ecx, 4(%eax)
	movl	%edx, 8(%eax)

	xorl	%eax, %eax	

BIOS32_CALL_END:
	// leave
	movl	%ebp, %esp
	popl	%ebp
	ret

	.type	_call_pci_bios_service_directory, @function
	.globl	_call_pci_bios_service_directory
_call_pci_bios_service_directory:
	pushl	%ebp
	movl	%esp, %ebp
	
	movb	$PCI_FUNCTION_ID, %ah
	movb	$PCI_FIND_DEVICE, %al
	movw	$0x24db, %cx		// device id
	movw	$0x8086, %dx		// vendor id
	movw	8(%ebp), %si		// device index
	call	_pci_bios_base_address

	cmpb	$0, %ah
	je	PCI_BIOS_CALL_SUCCESS

	// else error
	jmp	PCI_BIOS_CALL_END

PCI_BIOS_CALL_SUCCESS:
	movl	12(%ebp), %eax
	movl	%ebx, (%eax)		// save the return value in the argument pointer
	xorl	%eax, %eax

PCI_BIOS_CALL_END:
	// leave
	movl	%ebp, %esp
	popl	%ebp
	ret

