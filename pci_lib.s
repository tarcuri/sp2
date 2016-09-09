.section .text

        .type _call_bios32_service_directory, @function
        .globl _call_bios32_service_directory
_call_bios32_service_directory:
        pushl %ebp
        movl %esp, %ebp


        movl 8(%ebp), %eax
        xorl %ebx, %ebx
        call _bios32_sd_base_address


        cmpb $0, %al
        je BIOS32_CALL_SUCCESS


        jmp BIOS32_CALL_END

BIOS32_CALL_SUCCESS:




        movl 12(%ebp), %eax
        movl %ebx, 0(%eax)
        movl %ecx, 4(%eax)
        movl %edx, 8(%eax)

        xorl %eax, %eax

BIOS32_CALL_END:

        movl %ebp, %esp
        popl %ebp
        ret

        .type _call_pci_bios_service_directory, @function
        .globl _call_pci_bios_service_directory
_call_pci_bios_service_directory:
        pushl %ebp
        movl %esp, %ebp

        movb $0xb1, %ah
        movb $0x02, %al
        movw $0x24db, %cx
        movw $0x8086, %dx
        movw 8(%ebp), %si
        call _pci_bios_base_address

        cmpb $0, %ah
        je PCI_BIOS_CALL_SUCCESS


        jmp PCI_BIOS_CALL_END

PCI_BIOS_CALL_SUCCESS:
        movl 12(%ebp), %eax
        movl %ebx, (%eax)
        xorl %eax, %eax

PCI_BIOS_CALL_END:

        movl %ebp, %esp
        popl %ebp
        ret
