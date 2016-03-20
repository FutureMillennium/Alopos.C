   

Alopos
=======

operating system for x86 (32-bit)

Current features
-----------------

- Runs a charset, VGA terminal colours and variable sizes test
- Displays multiboot and memory map info

Build dependencies
-------------------

- MinGW
- i686-elf GCC
- QEMU

How to build
-------------

Copy `/_local_templates/vars.cmd` to `/vars.cmd` and edit the dependency directories to match where you've installed them.

Run `build-run.bat` to build and run in QEMU  
Run `build.bat` to just build
Run `run.bat` to just run in QEMU
