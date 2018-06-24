#define _XTAL_FREQ 20000000     // Usar cristal de 20MHz 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>										/* Include math header file */
#include "biblioteca.h"
#include "I2C_Master_File.h"
#include "mylcd.h"



#define PI	3.14159265359								/* Define Pi value */
#define Declination	-0.301651						   /* declinio magnetico em caxias do sul, http://www.magnetic-declination.com */

void Magneto_init()										/* Magneto initialize function */
{
	I2C_Start(0x3C);									/* Start and write SLA+W */
	I2C_Write(0x00);									/* Write memory location address */
	I2C_Write(0x70);									/* Configure register A as 8-average, 15 Hz default, normal measurement */
	I2C_Write(0xA0);									/* Configure register B for gain */
	I2C_Write(0x00);									/* Configure continuous measurement mode in mode register */
	I2C_Stop();											/* Stop I2C */
}

int Magneto_GetHeading()
{
	int x, y, z;
	double Heading;
	I2C_Start_Wait(0x3C);								/* Start and wait for acknowledgment */
	I2C_Write(0x03);									/* Write memory location address */
	I2C_Repeated_Start(0x3D);							/* Generate repeat start condition with SLA+R */
	/* Read 16 bit x,y,z value (2?s complement form) */
	x = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
	z = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
	y = (((int)I2C_Read(0)<<8) | (int)I2C_Read(1));
	I2C_Stop();											/* Stop I2C */
	Heading = atan2((double)y,(double)x) + Declination;
	if (Heading>2*PI)									/* Due to declination check for >360 degree */
		Heading = Heading - 2*PI;
	if (Heading<0)										/* Check for sign */
		Heading = Heading + 2*PI;
	return ((int)(Heading* 180 / PI));					/* Convert into angle and return */
}


int main(void)
{
    ADCON1 = 0x0F;
	char buffer[25];
	//TRISB &= ~(1<<4);									/* Make pin5 of PORT B as input */	
    TRISA = 0x00; // porra toda como saida
    RA0 = 1;
  // Inicializa LCD
    inic_XLCD();
    while (BusyXLCD());
	I2C_Init();											/* Initialize I2C */
	Magneto_init();										/* Initialize magneto */
	SetDDRamAddr(0x01);
   while (BusyXLCD());
    putsXLCD("MAGNETOMETRO\0");
	while (1)
	{
        RA0 = ! RA0;
        Mydelay(500);        
        sprintf(buffer,"Heading = %d%c    ",Magneto_GetHeading(),0xDF);
         SetDDRamAddr(0x40);
            while (BusyXLCD());
            putsXLCD(buffer);
	}
}