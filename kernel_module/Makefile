obj-m += tecmp.o
tecmp-objs := ./src/tecmp.o ./src/tecmp_ring_buffer.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
