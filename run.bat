SET "qemu=C:\Program Files\qemu"
SET "PATH=%qemu%;%PATH%"

qemu-system-i386 -kernel iso/alopos.bin
