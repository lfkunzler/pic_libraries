#define _XTAL_FREQ 20000000     // Usar cristal de 20MHz 

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "I2C_Master_File.h"
#include "display.h"
#include "main.h"

void magneto_init() /* Magneto initialize function */
{
    I2C_Start(0x3C); /* Start and write SLA+W */
    I2C_Write(0x00); /* Write memory location address */
    I2C_Write(0x70); /* Configure register A as 8-average, 15 Hz default, normal measurement */
    I2C_Write(0xA0); /* Configure register B for gain */
    I2C_Write(0x00); /* Configure continuous measurement mode in mode register */
    I2C_Stop(); /* Stop I2C */
}

void get_cord(coordenadas_t *coord)
{
    I2C_Start_Wait(0x3C); /* Start and wait for acknowledgment */
    I2C_Write(0x03); /* Write memory location address */
    I2C_Repeated_Start(0x3D); /* Generate repeat start condition with SLA+R */
    /* Read 16 bit x,y,z value (2?s complement form) */
    coord->x = (((int) I2C_Read(0) << 8) | (int) I2C_Read(0));
    coord->z = (((int) I2C_Read(0) << 8) | (int) I2C_Read(0));
    coord->y = (((int) I2C_Read(0) << 8) | (int) I2C_Read(1));
    I2C_Stop(); /* Stop I2C */
}

int main(void)
{
    ADCON1 = 0x0F;
    char buffer[17];
    memset(buffer, 0x20, 17);

    TRISA = 0x00;

    RA0 = 1;

    lcd_t lcd;
    coordenadas_t cord;

    // Inicializa LCD
    lcd_init(&lcd);

    while (BusyXLCD());
    I2C_Init(); /* Initialize I2C */
    magneto_init(); /* Initialize magneto */
    lcd_write(&lcd, 1, 1, "  X    Y    Z  \0");
    
    while (1) {
        get_cord(&cord);
        
        sprintf(buffer, "%4d %4d %4d\0", cord.x, cord.y, cord.z);
        lcd_write(&lcd, 2, 1, buffer);
        lcd_runtime(&lcd);
    }
}

void delay_ms(uint16_t milis)
{
    for (uint16_t t = 0; t < milis; t++) {
        __delay_ms(1);
    }
}