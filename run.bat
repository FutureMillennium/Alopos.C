call vars.cmd

SET "PATH=%qemu%;%PATH%"

qemu-system-i386 -kernel iso/alopos.bin

pause
