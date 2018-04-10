@call vars.cmd

@SET "PATH=%minGW%;%gcc%;%qemu%;%PATH%"

@cd Alopos
i686-elf-as boot.s -o ../build/boot.o
i686-elf-gcc -c kernel.c -o ../build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
@cd ..

@cd build
i686-elf-gcc -T linker.ld -o ../bin/alopos.bin -ffreestanding -O2 -nostdlib *.o -lgcc
@cd ..

