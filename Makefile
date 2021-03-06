
GDB = gdb
LD = i686-elf-ld
CC = i686-elf-gcc
CSOURCES=$(wildcard src/kernel/*.c \
		src/boot/*.c \
		src/drivers/*c \
		src/common/*.c)
SSOURCES=$(wildcard src/kernel/*.s \
		src/boot/*.s \
		src/drivers/*.s) 

CHEADERS=$(wildcard src/kernel/*.h \
		src/boot/*.h \
		src/drivers/*.h \
		src/common/*.h)
OBJ=${CSOURCES:.c=.o}
OBJS=${SSOURCES:.s=.o}
OBJ += ${OBJS}
CFLAGS=-g -ffreestanding



all: kernel.iso


#kernel.iso: kernel.elf src/modules/test_module
kernel.iso: kernel.elf 
	mkdir iso
	mkdir iso/modules
	mkdir iso/boot
	mkdir iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	cp initrd.img iso/modules/initrd.img
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.elf' >> iso/boot/grub/grub.cfg
	echo '  module /modules/initrd.img' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=kernel.iso iso
	rm -rf iso

kernel.elf: src/boot/loader.o ${OBJ} 
	${LD} -T linker.ld -melf_i386 -o $@ $^

src/modules/test_module: src/modules/test_program.s
	nasm -f bin -o $@ $^ 

%.o:%.s
	nasm -f elf32 $< -o $@

%.o:%.c ${CHEADERS}
	${CC} ${CFLAGS} -c $< -o $@

src/boot/%.o:src/boot/%.asm
	nasm -f elf32 $< -o $@

bochs: kernel.iso
	bochs -f bochsrc.txt -rc bochsconfig.txt -q
qemu: kernel.elf
	qemu-system-i386 -kernel kernel.elf -monitor stdio
qemu-iso: kernel.iso
	qemu-system-i386 -cdrom kernel.iso -monitor stdio 

qemu-debug: kernel.elf
	qemu-system-i386 -kernel kernel.elf -s -monitor stdio &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
qemu-iso-debug: kernel.iso kernel.elf
	qemu-system-i386 -cdrom kernel.iso -s -monitor stdio &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
clean:
	rm -rf *.elf *.o iso
	rm -rf src/kernel/*.o src/boot/*.o src/drivers/*.o src/common/*.o src/modules/test_module	
