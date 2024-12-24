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

void add(){
    code = 0b01110111;
}

void sub(){
    code = 0b01101101;
}

void power(){
    code = 0b01110011;
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

int segment2(int number){
    void(*num[4])() = {num0,add,sub,power};
    num[number]();
    return code;
}

int main(void)
{
        int fd;
        void *lw_virtual;
        volatile int *HEX3_HEX0_ptr;
        volatile int *SW;
        volatile int *ledr;
        volatile int *key;
        int pressed=0;
        int pressed2=0;
        int operand=0;
        int new_num;

        fd = open("/dev/mem", (O_RDWR | O_SYNC));
        lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);

        HEX3_HEX0_ptr= (volatile int *) (lw_virtual + HEX3_HEX0_BASE);
        SW = (volatile int *) (lw_virtual + SW_BASE);
        ledr = (volatile int *) (lw_virtual + LEDR_BASE);
        key = (volatile int *) (lw_virtual + KEY_BASE);

        *HEX3_HEX0_ptr = 0b00000000;
        
	*ledr=0;
        while(1){
 	    
            while((*key)&0x1){          // select what is a -> sw0
                    while((*key)&0x1);
                    pressed+=1;
                }
            if (pressed>=10){
                     int ten = segment(pressed/10);
                     int one = segment(pressed%10);
                     int all = (ten<<8) | one;
                     *HEX3_HEX0_ptr = all;
                }
            else {
                    new_num= segment(pressed);
                    *HEX3_HEX0_ptr = new_num;
                 }

            while((*SW)&0b001){  // select what is operand -> sw0
               
  	
                while((*key)&0x1){
                    while((*key)&0x1);
                    operand+=1;
                }
                int op = segment2(operand);
                *HEX3_HEX0_ptr = op;
            }
          

            while((*SW)&0b010){   // select what is b -> sw0
              
		
                while((*key)&0x1){
                    while((*key)&0x1);
                    pressed2+=1;
                }
                if (pressed2>=10){
                        int ten = segment(pressed2/10);
                        int one = segment(pressed2%10);
                        int all = (ten<<8) | one;
                        *HEX3_HEX0_ptr = all;
                    }
                else {
                        new_num= segment(pressed2);
                        *HEX3_HEX0_ptr = new_num;
                    }
            }
            

            while((*SW)&0b100){
		
                int result;
                if (operand==1)
                    result = pressed+pressed2;
                else if (operand==2)
                    result = pressed-pressed2;

                else if (operand==3){
                    int j;
                    int p=1;
                    for (j=0; j<pressed2; j++){
                        p = p * pressed;
                    }
                    result = p;
                }

                if (result>=10 && result<100 ){
                        int ten = segment(result/10);
                        int one = segment(result%10);
                        int all = (ten<<8) | one;
                        *HEX3_HEX0_ptr = all;
                    }
                else if (result<10 && result>=0){
                        new_num= segment(result);
                        *HEX3_HEX0_ptr = new_num;
                    }
                else if (result>=100 && result<1000){
                        int hun = segment(result/100);
                        result = result - 100*(result/100);
                        int ten = segment(result/10);
                        int one = segment(result%10);
                        int all = (hun<<16) | (ten<<8) | one;
                        *HEX3_HEX0_ptr = all;
                    }
                else if {
                    *HEX3_HEX0_ptr = 0b00000000;
                    *ledr = 0b1111111111;
                }
            }
            *ledr = 0;
        }
        munmap(lw_virtual, LW_BRIDGE_BASE);
        close(fd);

        return 0;
}
