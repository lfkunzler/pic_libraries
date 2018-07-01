#include <xc.h>
#include "magnetometro.h"
#include "i2c.h"

void magnetoInit(void)
{
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    
    I2C_Start(0x3C); /* Start and write SLA+W */
    I2C_Write(0x00); /* Write memory location address */
    I2C_Write(0x70); /* Configure register A as 8-average, 15 Hz default, normal measurement */
    I2C_Write(0xA0); /* Configure register B for gain */
    I2C_Write(0x00); /* Configure continuous measurement mode in mode register */
    I2C_Stop(); /* Stop I2C */
}

void magnetoGetCord(coordenadas_t *coord)
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

