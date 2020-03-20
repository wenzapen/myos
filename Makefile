

LD = i686-elf-ld
CC = i686-elf-gcc
C_SOURCES = $(wildcard src/kernel/*.c src/boot/*.c)
S_SOURCES = $(wildcard src/kernel/*.s src/boot/*.s)
C_HEADERS = $(wildcard src/kernel/*.h src/boot/*.h)
OBJ = ${C_SOURCES:.c=.o S_SOURCES:.s=.o}




all: kernel.iso


kernel.iso: kernel.elf
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.elf' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=kernel.iso iso
	rm -rf iso

kernel.elf: src/boot/loader.o
	${LD} -T linker.ld -melf_i386 $^ -o $@

%.o:%.s
	nasm -f elf32 $< -o $@

src/boot/%.o:src/boot/%.s
	nasm -f elf32 $< -o $@

bochs: kernel.iso
	bochs -f bochsrc.txt -q

clean:
	rm -rf *.elf *.o iso
	rm -rf src/kernel/*.o src/boot/*.o	
