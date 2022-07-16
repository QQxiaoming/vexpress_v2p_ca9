/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc -nostartfiles -T./hello_word.lds hello_world.c -o hello_world
/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-objcopy -O binary -S hello_world hello_world.bin
/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-objcopy -O ihex -S hello_world hello_world.hex
/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-objdump --source --demangle --disassemble --reloc --wide hello_world > hello_world.lst
/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-readelf -a hello_world > hello_world.info