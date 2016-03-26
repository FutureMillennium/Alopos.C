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
	cli  # clear interrupt
	# TODO kernel panic
	hlt  # halt
.Lhang:
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
.global ISRoutine0
.global ISRoutine1
.global ISRoutine2
.global ISRoutine3
.global ISRoutine4
.global ISRoutine5
.global ISRoutine6
.global ISRoutine7
.global ISRoutine8
.global ISRoutine9
.global ISRoutine10
.global ISRoutine11
.global ISRoutine12
.global ISRoutine13
.global ISRoutine14
.global ISRoutine15
.global ISRoutine16
.global ISRoutine17
.global ISRoutine18
.global ISRoutine19
.global ISRoutine20
.global ISRoutine21
.global ISRoutine22
.global ISRoutine23
.global ISRoutine24
.global ISRoutine25
.global ISRoutine26
.global ISRoutine27
.global ISRoutine28
.global ISRoutine29
.global ISRoutine30
.global ISRoutine31

#  0: Divide By Zero Exception
ISRoutine0:
    cli
    push 0
    push 0
    jmp ISRCommonHandler

#  1: Debug Exception
ISRoutine1:
    cli
    push 0
    push 1
    jmp ISRCommonHandler

#  2: Non Maskable Interrupt Exception
ISRoutine2:
    cli
    push 0
    push 2
    jmp ISRCommonHandler

#  3: Int 3 Exception
ISRoutine3:
    cli
    push 0
    push 3
    jmp ISRCommonHandler

#  4: INTO Exception
ISRoutine4:
    cli
    push 0
    push 4
    jmp ISRCommonHandler

#  5: Out of Bounds Exception
ISRoutine5:
    cli
    push 0
    push 5
    jmp ISRCommonHandler

#  6: Invalid Opcode Exception
ISRoutine6:
    cli
    push 0
    push 6
    jmp ISRCommonHandler

#  7: Coprocessor Not Available Exception
ISRoutine7:
    cli
    push 0
    push 7
    jmp ISRCommonHandler

#  8: Double Fault Exception (With Error Code!)
ISRoutine8:
    cli
    push 8
    jmp ISRCommonHandler

#  9: Coprocessor Segment Overrun Exception
ISRoutine9:
    cli
    push 0
    push 9
    jmp ISRCommonHandler

# 10: Bad TSS Exception (With Error Code!)
ISRoutine10:
    cli
    push 10
    jmp ISRCommonHandler

# 11: Segment Not Present Exception (With Error Code!)
ISRoutine11:
    cli
    push 11
    jmp ISRCommonHandler

# 12: Stack Fault Exception (With Error Code!)
ISRoutine12:
    cli
    push 12
    jmp ISRCommonHandler

# 13: General Protection Fault Exception (With Error Code!)
ISRoutine13:
    cli
    push 13
    jmp ISRCommonHandler

# 14: Page Fault Exception (With Error Code!)
ISRoutine14:
    cli
    push 14
    jmp ISRCommonHandler

# 15: Reserved Exception
ISRoutine15:
    cli
    push 0
    push 15
    jmp ISRCommonHandler

# 16: Floating Point Exception
ISRoutine16:
    cli
    push 0
    push 16
    jmp ISRCommonHandler

# 17: Alignment Check Exception
ISRoutine17:
    cli
    push 0
    push 17
    jmp ISRCommonHandler

# 18: Machine Check Exception
ISRoutine18:
    cli
    push 0
    push 18
    jmp ISRCommonHandler

# 19: Reserved
ISRoutine19:
    cli
    push 0
    push 19
    jmp ISRCommonHandler

# 20: Reserved
ISRoutine20:
    cli
    push 0
    push 20
    jmp ISRCommonHandler

# 21: Reserved
ISRoutine21:
    cli
    push 0
    push 21
    jmp ISRCommonHandler

# 22: Reserved
ISRoutine22:
    cli
    push 0
    push 22
    jmp ISRCommonHandler

# 23: Reserved
ISRoutine23:
    cli
    push 0
    push 23
    jmp ISRCommonHandler

# 24: Reserved
ISRoutine24:
    cli
    push 0
    push 24
    jmp ISRCommonHandler

# 25: Reserved
ISRoutine25:
    cli
    push 0
    push 25
    jmp ISRCommonHandler

# 26: Reserved
ISRoutine26:
    cli
    push 0
    push 26
    jmp ISRCommonHandler

# 27: Reserved
ISRoutine27:
    cli
    push 0
    push 27
    jmp ISRCommonHandler

# 28: Reserved
ISRoutine28:
    cli
    push 0
    push 28
    jmp ISRCommonHandler

# 29: Reserved
ISRoutine29:
    cli
    push 0
    push 29
    jmp ISRCommonHandler

# 30: Reserved
ISRoutine30:
    cli
    push 0
    push 30
    jmp ISRCommonHandler

# 31: Reserved
ISRoutine31:
    cli
    push 0
    push 31
    jmp ISRCommonHandler


# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
ISRCommonHandler:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov %ax, 0x10
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %eax, %esp
    push %eax
	mov $ExceptionHandler, %eax
    call *%eax
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add %esp, 8
    iret


# -----------------------------------------------------------------
# Interrupt request (IRQ) handlers
# -----------------------------------------------------------------
.global IRQHandler0
.global IRQHandler1
.global IRQHandler2
.global IRQHandler3
.global IRQHandler4
.global IRQHandler5
.global IRQHandler6
.global IRQHandler7
.global IRQHandler8
.global IRQHandler9
.global IRQHandler10
.global IRQHandler11
.global IRQHandler12
.global IRQHandler13
.global IRQHandler14
.global IRQHandler15

# 32: IRQ0
IRQHandler0:
    cli
    push 0
    push 32
    jmp IRQCommonHandler

# 33: IRQ1
IRQHandler1:
    cli
    push 0
    push 33
    jmp IRQCommonHandler

# 34: IRQ2
IRQHandler2:
    cli
    push 0
    push 34
    jmp IRQCommonHandler

# 35: IRQ3
IRQHandler3:
    cli
    push 0
    push 35
    jmp IRQCommonHandler

# 36: IRQ4
IRQHandler4:
    cli
    push 0
    push 36
    jmp IRQCommonHandler

# 37: IRQ5
IRQHandler5:
    cli
    push 0
    push 37
    jmp IRQCommonHandler

# 38: IRQ6
IRQHandler6:
    cli
    push 0
    push 38
    jmp IRQCommonHandler

# 39: IRQ7
IRQHandler7:
    cli
    push 0
    push 39
    jmp IRQCommonHandler

# 40: IRQ8
IRQHandler8:
    cli
    push 0
    push 40
    jmp IRQCommonHandler

# 41: IRQ9
IRQHandler9:
    cli
    push 0
    push 41
    jmp IRQCommonHandler

# 42: IRQ10
IRQHandler10:
    cli
    push 0
    push 42
    jmp IRQCommonHandler

# 43: IRQ11
IRQHandler11:
    cli
    push 0
    push 43
    jmp IRQCommonHandler

# 44: IRQ12
IRQHandler12:
    cli
    push 0
    push 44
    jmp IRQCommonHandler

# 45: IRQ13
IRQHandler13:
    cli
    push 0
    push 45
    jmp IRQCommonHandler

# 46: IRQ14
IRQHandler14:
    cli
    push 0
    push 46
    jmp IRQCommonHandler

# 47: IRQ15
IRQHandler15:
    cli
    push 0
    push 47
    jmp IRQCommonHandler


IRQCommonHandler:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    mov %ax, 0x10
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %eax, %esp

    push %eax
    mov $IRQHandler, %eax
    call *%eax
    pop %eax

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add %esp, 8
    iret
