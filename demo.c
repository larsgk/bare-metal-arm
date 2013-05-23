//
// demo.c -- Simple demonstration program
//
//  Copyright (c) 2012-2013 Andrew Payne <andy@payne.org>
//

#include <stdio.h>
#include "freedom.h"
#include "common.h"

extern char *_sbrk(int len);

#define BUF_SIZE 256

// Main program
int main(void)
{
    char i;
    char ch;
    char *heap_end;
    char inBuffer[BUF_SIZE];
    int readLen,bufPos;
    int n;
    int nargs[10];
    int hasAccelData = 0;
    int hasTouchData = 0;

    // Initialize all modules
    RGB_LED(100,0,0);
    uart_init(115200);
    accel_init();
    touch_init((1 << 9) | (1 << 10));       // Channels 9 and 10
    // usb_init();
    setvbuf(stdin, NULL, _IONBF, 0);        // No buffering

    // Run tests
    tests();
    delay(500);

    // Welcome banner
    iprintf("\r\n\r\n====== Freescale Freedom FRDM-LK25Z\r\n");
    iprintf("Built: %s %s\r\n\r\n", __DATE__, __TIME__);
    heap_end = _sbrk(0);
    iprintf("Heap:  %p to %p (%d bytes used)\r\n", __heap_start, heap_end, 
                heap_end - (char *)__heap_start);
    iprintf("Stack: %p to %p (%d bytes used)\r\n", &i, __StackTop, 
                (char *)__StackTop - &i);
    iprintf("%d bytes free\r\n", &i - heap_end);
    
    inBuffer[0] = 0;  // reset buffer
    bufPos = 0;

    RGB_LED(0,100,0);                       // Green

    for(;;) {
        readLen = uart_read_nonblock(inBuffer+bufPos,BUF_SIZE-(bufPos+2));
        if (readLen>0) {
            bufPos+=readLen;
            // quick trim
            while(bufPos>0 && isspace(inBuffer[bufPos-1]))
                  bufPos--;
            inBuffer[bufPos] = 0;
            if (inBuffer[bufPos-1] == ';') {
                iprintf("(in buf = '%s', len=%d)\n",inBuffer,bufPos);
                switch(inBuffer[0]) {
                case 'C':
                    n = sscanf(inBuffer+1,"%i,%i,%i",&nargs[0],&nargs[1],&nargs[2]);
                    if(n==3) RGB_LED(nargs[0],nargs[1],nargs[2]);
                    break;
                case 'R':
                    RGB_LED(100,0,0);
                    break;
                case 'G':
                    RGB_LED(0,100,0);
                    break;
                case 'B':
                    RGB_LED(0,0,100);
                    break;
                case 'A':
                    hasAccelData = 1;
                    break;
                case 'a':
                    hasAccelData = 0;
                    break;
                case 'T':
                    hasTouchData = 1;
                    break;
                case 't':
                    hasTouchData = 0;
                    break;
                case 'I':
                    iprintf("[efb,0.1] empiriKit Freescale Board, v.0.1\n");
                    break;
                }
                bufPos = 0;  // "clear" the buffer
            }
        }
        if (hasAccelData)
            iprintf("a%d,%d,%d;\n", accel_x(), accel_y(), accel_z());
        if (hasTouchData)
            iprintf("t%d,%d;\n", touch_data(9), touch_data(10));
    }
}
