/*
** sccs id:	%W%	%G%
**
** File:	pci.S
**
** Author:	4003-506 class of 20083
**
** Contributor:
**
** Description:	?
*/

#define	__ASM__20083__

.section .text

	.type	_get_pci_config_space, @function
	.globl	_get_pci_config_space
_get_pci_config_space:
	pushl	%ebp
	movl	%esp, %ebp
	/* save reserved C program registers */
	pushl	%ebx
	pushl	%edi

	movl	$BIOS_SD_PCI_SIG, %eax
	xorl	%ebx, %ebx
	call	8(%ebp)

	// EBX should have the base address
	movl	%ebx, %eax

	popl	%edi
	popl	%ebx
	movl	%ebp, %esp
	popl	%ebp
	ret
