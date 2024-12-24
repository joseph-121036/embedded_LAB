#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

int main(void){
	int fd;
	void *lw_virtual;
	volatile int *ledr;
	volatile int *key;
	int pressed;

	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
	ledr = (volatile int *) (lw_virtual + LEDR_BASE);
	key = (volatile int *) (lw_virtual + KEY_BASE);

	*ledr = 0;
	while(1){
	pressed = 0;
	while((*key)&0x1)
		pressed = 1;
	*ledr = *ledr + pressed;
	usleep(5000);
	}

	munmap(lw_virtual, LW_BRIDGE_BASE);
	close(fd);

	return 0;	

}
