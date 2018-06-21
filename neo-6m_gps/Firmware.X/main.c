/*
 * File:   main.c
 * Author: Luis Felipe Kunzler
 *
 * Created on 15 de Maio de 2018, 02:04
 */

#include <xc.h>
#include "main.h"
#include "uart.h"
#include "display.h"

void main(void)
{
    ADCON1 = 0x0F; // all pins [an12:an0] digital
    OSCCON = 0b01110010; // internal clock at 8MHz

    lcd_t lcd;

    uart_init();
    lcd_init(&lcd);
    
    while (1) {
        Nop();
    } // Fica parado fazendo nada
}

void delay_ms(uint16_t milis)
{
    for (uint16_t t = 0; t < milis; t++) {
        __delay_ms(1);
    }
}