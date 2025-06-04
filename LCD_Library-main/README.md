# LCD_Library
Adaptation of Joerg Wunsch HD44780 Library https://github.com/vancegroup-mirrors/avr-libc/tree/master/avr-libc/doc/examples/stdiodemo


We used Joerg Wunsch's library for the HD44780 LCD and make changes and added functions to allow for easier use and the ability to use a 2x16 LCD rather than a 1x16 that the library was designed for.

Steps:
1. Add all these files to your project
2. Make edits in define.h
   
    A. Change to your hardware ports
4. include
   
    A. include "lcd.c"
   
    B. include "defines.h"
   
    C. include "lcd.h"
6. FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);\
8. Main
   
   A. stdout = &lcd_str
   
   B. lcd_init();
9. Use printf("") to put messages on the screen





/*
 ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Charlie & Tristan wrote and adapted this file.  
 * As long as you retain this notice you can do whatever you want with this stuff. 
 * If we meet some day, and you think this stuff is worth it, you can buy me a beer.
 *                                          Charles Wilmot and Tristan Pawlenty
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */
