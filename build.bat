call vars.cmd

SET "PATH=%minGW%;%gcc%;%qemu%;%PATH%"

i686-elf-as Alopos/boot.s -o build/boot.o
i686-elf-gcc -c Alopos/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

cd build
i686-elf-gcc -T linker.ld -o ../bin/alopos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
cd ..

REM grub-mkrescue -o iso/alopos.iso bin
REM qemu-system-i386 -cdrom iso/alopos.iso

qemu-system-i386 -kernel bin/alopos.bin
