TARGET := lab3
obj-m += ece4220lab3.o
obj-m += lab3.o
lab3_mod-objs := lab3.o ece4220lab3.o
#WARN := -W -Wall #-Wmissing-prototype -Wstrict-prototypes
#INCLUDE := -isystem /lib/modules/`uname -r`/build/include
INCLUDE := -I ../includes/realtime/include -I../includes/src/linux24/include
CFLAGS := -O0 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
CC	:= /usr/local/opt/crosstool/arm-linux/gcc-3.3.4-glibc-2.3.2/bin/arm-linux-gcc
${TARGET}.o: ${TARGET}.c

.PHONY: clean
clean:
	rm -rf ${TARGET}.o
