#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

int to_segment(int num) {   //for 'open', 'closed'
    int code = 0;
    switch(num) {
        case 0: code = 0b00111111; break; // O
        case 1: code = 0b01110011; break; // P
        case 2: code = 0b01111001; break; // E
        case 3: code = 0b00110111; break; // N
        case 4: code = 0b00111001; break; // C
        case 5: code = 0b00111000; break; // L
        case 6: code = 0b01101101; break; // S
        case 7: code = 0b01011110; break; // d
        case 13: code = 0b00111110; break; // U
        case 14: code = 0b01111000; break; // t
        default: code = 0b00000000;
    }
    return code;
}

int to_segment1(int num){   //for 'current time'
    int code = 0;
    switch(num) {
        case 0: code = 0b00111111; break; // 0
        case 1: code = 0b00000110; break; // 1
        case 2: code = 0b01011011; break; // 2
        case 3: code = 0b01001111; break; // 3
        case 4: code = 0b01100110; break; // 4
        case 5: code = 0b01101101; break; // 5
        case 6: code = 0b01111101; break; // 6
        case 7: code = 0b00100111; break; // 7
        case 8: code = 0b01111111; break; // 8
        case 9: code = 0b01100111; break; // 9
        default : code = 0b00000000;
    }
    return code;
}


int main(void) {
    FILE *fp_hex3_0, *fp_hex5_4, *fp_key, *fp;
    int open[4] = {3, 2, 1, 0}; // OPEN
    int close[6] = {7, 2, 6, 0, 5, 4}; // CLOSEd
    int SO1[6] = {13,0,7,5,0,6}; // sold ou
    int SO2[6] = {14,13,0,7,5,0}; // old out
    int cnt = 0; // count
    int KEY = 0;
    int time = 0;
    int i;
    int sold_out = 0;
    unsigned int ch1;

    fp_hex3_0 = fopen("/dev/hex3_0", "w");
    fp_hex5_4 = fopen("/dev/hex5_4", "w");
    fp_key = fopen("/dev/key", "r");
    fp = fopen("/dev/ledr", "wb");


    if (!fp_hex3_0 || !fp_hex5_4 || !fp_key || !fp) {
        perror("Error opening file");
        return 1;
    }

    //initialize
    int ch = 0b0000000000;   //led initialize
    fwrite(&ch, sizeof(unsigned int), 1, fp);
    fflush(fp);

    for (i = 4; i < 6; i++) {   //Hex initialize
         fputc(to_segment(8), fp_hex5_4);
    }
    fflush(fp_hex5_4);
    for (i = 0; i < 4; i++) {
         fputc(to_segment(8), fp_hex3_0);
    }
    fflush(fp_hex3_0);


    while (1) {
        KEY = fgetc(fp_key);
        fflush(fp_key);
	
        if (KEY & 0b1) {    //display current time
            
            for (i = 4; i < 6; i++) {   //Hex initialize
                 fputc(to_segment(8), fp_hex5_4);
            }
            fflush(fp_hex5_4);
            for (i = 0; i < 4; i++) {
                 fputc(to_segment(8), fp_hex3_0);
            }
            fflush(fp_hex3_0); 
            cnt += 1;
            if (cnt<12){
                printf("It's %d:00AM\n",cnt);
            }
            else if(cnt>=12){
                printf("It's %d:00PM\n",cnt);
            }
            int d0 = cnt % 10;   //0th decimal of cnt
            int d1 = cnt / 10;   //1th decimal of cnt
            fputc(to_segment1(d0), fp_hex3_0);
            fputc(to_segment1(d1), fp_hex3_0);
            fflush(fp_hex3_0);
            usleep(500000); // debounce delay
        }

        if (KEY & 0b10){
           
           time = cnt;
           


        if (10 <= time && time < 23){
            printf("Welcome!\n");
            for (i = 0; i < 4; i++) {
                fputc(to_segment(open[i]), fp_hex3_0);
            }
            fflush(fp_hex3_0);
            for (i = 4; i < 6; i++) {
                fputc(to_segment(8), fp_hex5_4);
            }
            fflush(fp_hex5_4);
        }
         else{
            printf("Sorry, It's closed time\n");
            for (i = 4; i < 6; i++) {
                fputc(to_segment(close[i]), fp_hex5_4);
            }
            fflush(fp_hex5_4);

            for (i = 0; i < 4; i++) {
                fputc(to_segment(close[i]), fp_hex3_0);
            }
            fflush(fp_hex3_0);
        }
        }

        if(KEY & 0b100) {  // clear
          
            //printf("fsfsfseee\n");
            cnt = 0;
            time = 0;
            ch1 = 0b0000000000; // ledr clear
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);
            usleep(500000);
            for (i = 4; i < 6; i++) {
                fputc(to_segment(8), fp_hex5_4);
            }
            fflush(fp_hex5_4);
            for (i = 0; i < 4; i++) {
                fputc(to_segment(8), fp_hex3_0);
            }
            fflush(fp_hex3_0);
        }
        if (KEY & 0b1000){
        
           sold_out = 1;
        }
        if (sold_out ==1){
            for (i = 4; i < 6; i++) {
                fputc(to_segment(SO1[i]), fp_hex5_4);
            }
            fflush(fp_hex5_4);

            for (i = 0; i < 4; i++) {
                fputc(to_segment(SO1[i]), fp_hex3_0);
            }
            fflush(fp_hex3_0);
            ch1 = 0b0101010101; // half light on
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);

            usleep(1000000);

            for (i = 4; i < 6; i++) {
                fputc(to_segment(SO2[i]), fp_hex5_4);
            }
            fflush(fp_hex5_4);

            for (i = 0; i < 4; i++) {
                fputc(to_segment(SO2[i]), fp_hex3_0);
            }
            fflush(fp_hex3_0);
            ch1 = 0b1010101010; // half light on
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);
            usleep(1000000);
        }
        if (10 <= time && time < 19) {
            ch1 = 0b0101010101; // half light on
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);
            
        } else if (19 <= time && time < 23) {
            ch1 = 0b1111111111; // full light on
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);
        } else {
            ch1 = 0b00000000; // full light off
            fwrite(&ch1, sizeof(unsigned int), 1, fp);
            fflush(fp);
        }
    }

    fclose(fp_hex5_4);
    fclose(fp_hex3_0);
    fclose(fp_key);
    fclose(fp);
    return 0;
}


