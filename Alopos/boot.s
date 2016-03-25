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

	# To set up a stack, we simply set the esp register to point to the top of
	# our stack (as it grows downwards).
	movl $stack_top, %esp

	pushl %ebx  # should contain a pointer to the Multiboot information structure
	pushl %eax  # should contain the Multiboot bootloader magic number

	call Main_Kernel

	# In case the function returns
	cli  # clear interrupt
	# TODO kernel panic
	hlt  # halt
.Lhang:
	jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start

# Load GDT
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
