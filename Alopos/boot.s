# Declare constants used for creating a multiboot header.
.set MULTIBOOT_ALIGN,    1<<0             # align loaded modules on page boundaries
.set MULTIBOOT_MEMORY_INFO,  1<<1             # provide memory map
.set MULTIBOOT_FLAGS,    MULTIBOOT_ALIGN | MULTIBOOT_MEMORY_INFO  # this is the Multiboot 'flag' field
.set MULTIBOOT_MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard
.section .multiboot
.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

# stack pointer register (esp)
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The linker script specifies _start as the entry point to the kernel and the
# bootloader will jump to this position once the kernel has been loaded. It
# doesn't make sense to return from this function as the bootloader is gone.
.section .text
.global _start
.type _start, @function
_start:

	cli
	# To set up a stack, we simply set the esp register to point to the top of
	# our stack (as it grows downwards).
	movl $stack_top, %esp

	pushl %ebx  # should contain a pointer to the Multiboot information structure
	pushl %eax  # should contain the Multiboot bootloader magic number

	call Main_Kernel

	# In case the function returns
.Lhang: # @TODO kernel panic
	cli  # clear interrupt
	hlt  # halt
	jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start

# Load and flush GDT
.global GDTFlush
.type GDTFlush, @function

GDTFlush:
    mov 4(%esp), %eax  # TODO: are 4 bytes staying in the stack?
    lgdt (%eax)

    mov $0x10, %ax  # 0x10 is entry[2]
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %ss

    ljmp $0x08, $.flush  # 0x08 is entry[1]
.flush:
    ret

# Load IDT
.global IDTLoad
.type IDTLoad, @function

IDTLoad:
    mov 4(%esp), %eax  # TODO: are 4 bytes staying in the stack?
    lidt (%eax)
    ret


# Interrupt Service Routines (ISRs)
.macro ISR_NOERR index
    .global ISRoutine\index
    ISRoutine\index:
        cli
        push $0
        push $\index
        jmp ISRCommonHandler
.endm

.macro ISR_ERR index
    .global ISRoutine\index
    ISRoutine\index:
        cli
        push $\index
        jmp ISRCommonHandler
.endm

# Standard x86 interrupt service routines
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31


# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
ISRCommonHandler:

    pusha
    push %ds
    push %es
    push %fs
    push %gs

	mov $0x10, %ax  # 0x10 is GDT entry[2]
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp

	mov $ExceptionHandler, %eax
    call *%eax  # preserves the 'eip' register

    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp  # Cleans up the pushed error code and pushed ISR number
    iret  # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


# -----------------------------------------------------------------
# Interrupt request (IRQ) handlers
# -----------------------------------------------------------------

.macro GENERATE_IRQ_HANDLER index
    .global IRQHandler\index
    IRQHandler\index:
        cli
        push $0
        push $(\index+32)
        jmp IRQCommonHandler
.endm

GENERATE_IRQ_HANDLER 0
GENERATE_IRQ_HANDLER 1
GENERATE_IRQ_HANDLER 2
GENERATE_IRQ_HANDLER 3
GENERATE_IRQ_HANDLER 4
GENERATE_IRQ_HANDLER 5
GENERATE_IRQ_HANDLER 6
GENERATE_IRQ_HANDLER 7
GENERATE_IRQ_HANDLER 8
GENERATE_IRQ_HANDLER 9
GENERATE_IRQ_HANDLER 10
GENERATE_IRQ_HANDLER 11
GENERATE_IRQ_HANDLER 12
GENERATE_IRQ_HANDLER 13
GENERATE_IRQ_HANDLER 14
GENERATE_IRQ_HANDLER 15


IRQCommonHandler:

    pusha
    push %ds
    push %es
    push %fs
    push %gs

    mov $0x10, %ax  # 0x10 is GDT entry[2]
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov %esp, %eax
    push %eax

	mov $IRQHandler, %eax
    call *%eax  # preserves the 'eip' register
    
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    iret
