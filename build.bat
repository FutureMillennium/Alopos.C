SET "minGW=C:\MinGW\bin"
SET "gcc=D:\Software\ghost-i686-elf-tools-win32\bin"
SET "qemu=C:\Program Files\qemu"
SET "PATH=%minGW%;%gcc%;%qemu%;%PATH%"
i686-elf-as Alopos/boot.s -o build/boot.o
i686-elf-gcc -c Alopos/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

cd build
i686-elf-gcc -T linker.ld -o ../iso/alopos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
cd ..

REM grub-mkrescue -o bin/alopos.iso iso
REM qemu-system-i386 -cdrom bin/alopos.iso

qemu-system-i386 -kernel iso/alopos.bin
