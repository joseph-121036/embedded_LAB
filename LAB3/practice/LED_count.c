#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

int main(void)
{
	int fd;
	void *lw_virtual;
	volatile int *ledr;
	int cnt = 0;

	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
	ledr = (volatile int *) (lw_virtual + LEDR_BASE);

	*ledr = 0;
	while(cnt<8){
		*ledr = *ledr + 1;	
		cnt++;
		usleep(1000000);
	}

	munmap(lw_virtual, LW_BRIDGE_BASE);
	close(fd);
	
	return 0;	



}
