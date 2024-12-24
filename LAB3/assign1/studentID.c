#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

int code;


void num0() {
    code = 0b00111111;
}

void num1() {
    code = 0b00000110;
}

void num2() {
    code = 0b01011011;
}

void num3() {
    code = 0b01001111;
}

void num4() {
    code = 0b01100110;
}

void num5() {
    code = 0b01101101;
}

void num6() {
    code = 0b01111101;
}

void num7() {
    code = 0b00100111;
}

void num8() {
    code = 0b01111111;
}

void num9() {
    code = 0b01100111;
}


int segment(int number) {

    void (*num[10])() = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};

    if (number >= 0 && number < 10) {
        num[number]();
    } else {
        code = 0b00000000;
    }

    return code;
}



int main(void)
{
        int fd;
        void *lw_virtual;
        volatile int *HEX3_HEX0_ptr;
        volatile int *HEX5_HEX4_ptr;
        volatile int *SW0;

        int student_id[23] = {2,0,1,9,1,2,4,0,1,7,10,2,0,1,9,1,2,1,0,3,6,10};
	int student_id2[22] = {10,6,3,0,1,2,1,9,1,0,2,10,7,1,0,4,2,1,9,1,0,2};
        int new_num;

        fd = open("/dev/mem", (O_RDWR | O_SYNC));
        lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);

        HEX3_HEX0_ptr= (volatile int *) (lw_virtual + HEX3_HEX0_BASE);
        HEX5_HEX4_ptr= (volatile int *) (lw_virtual + HEX5_HEX4_BASE);
        SW0 = (volatile int *) (lw_virtual + SW_BASE);

	*HEX3_HEX0_ptr = 0b00000000;
	*HEX5_HEX4_ptr = 0b00000000;

        while(1){
        while(*SW0 == 0){
        int a,b=0;
        int i=0;
        for(i;i<22;i++){
                new_num= segment(student_id2[i]);
                a = ((*HEX3_HEX0_ptr) >>8);
                *HEX3_HEX0_ptr = ((*HEX5_HEX4_ptr<<24)|a);
                b = *(HEX5_HEX4_ptr) >> 8;
                *(HEX5_HEX4_ptr)=(b|new_num << 8);
                usleep(500000);
                if(*SW0 != 0){
                        break; //escape when SW0 is changed
                }

        }
        }

    //    *HEX3_HEX0_ptr = 0b00000000;  //reset when SW0 is changed
   //     *HEX5_HEX4_ptr = 0b00000000;  //reset when SW0 is changed

        while(*SW0 == 1){
        int a,b=0;
        int i=0;
        for(i;i<22;i++){
                new_num= segment(student_id[i]);
                a=((*HEX3_HEX0_ptr) >> 24);
                *HEX5_HEX4_ptr=((*HEX5_HEX4_ptr<<8)|a);
                b=((*HEX3_HEX0_ptr) << 8);
                *HEX3_HEX0_ptr=(b|new_num);
                usleep(500000);
                if(*SW0 != 1){
                        break;  //escape when SW0 is changed
                }
           }

        }
        }



        munmap(lw_virtual, LW_BRIDGE_BASE);
        close(fd);

        return 0;  

}


