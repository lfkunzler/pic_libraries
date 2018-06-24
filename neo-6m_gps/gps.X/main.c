/*
 * File:   main.c
 * Author: Luis Felipe Kunzler
 *
 * Created on 24 de Junho de 2018, 17:24
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "display.h"
#include "uart.h"

#define GGA_Buffer_Size 80
#define GGA_Pointers_Size 20

char GGA_Buffer[GGA_Buffer_Size]; /* to store GGA string */
char GGA_CODE[3];

unsigned char N_S, E_W; /* for getting direction polarity */
unsigned char GGA_Pointers[GGA_Pointers_Size]; /* to store instances of ',' */
char CommaCounter;
char Data_Buffer[15];
volatile unsigned int GGA_Index;
volatile unsigned char IsItGGAString = 0;

void main(void)
{
    float Latitude, Longitude;
    char GPS_Buffer[15];

    memset(GPS_Buffer, 0, 15);
    memset(GPS_Buffer, 0, 15);

    ADCON1 = 0x0F;
    OSCCON = 0x72; /* osc interno com 8MHz */

    lcd_t lcd;

    lcd_init(&lcd);

    INTCONbits.GIE = 1; /* enable Global Interrupt */
    INTCONbits.PEIE = 1; /* enable Peripheral Interrupt */
    PIE1bits.RCIE = 1; /* enable Receive Interrupt */

    uart_init();

    while (1) {

        lcd_write(&lcd, 1, 1, "Lt: \0");
        Latitude = get_latitude(GGA_Pointers[0]); /* Extract Latitude */
        Latitude = convert_to_degrees(Latitude); /* convert raw latitude in degree decimal*/
        sprintf(GPS_Buffer, "%.04f\0", Latitude); /* convert float value to string */
        lcd_write(&lcd, 1, 5, GPS_Buffer); /* display latitude in degree */
        memset(GPS_Buffer, 0x20, 14);

        lcd_write(&lcd, 2, 1, "Ln: \0");
        Longitude = get_longitude(GGA_Pointers[2]); /* Extract Latitude */
        Longitude = convert_to_degrees(Longitude); /* convert raw longitude in degree decimal*/
        sprintf(GPS_Buffer, "%.04f\0", Longitude); /* convert float value to string */
        lcd_write(&lcd, 2, 5, GPS_Buffer); /* display latitude in degree */
        memset(GPS_Buffer, 0x20, 14);

        lcd_runtime(&lcd);
    }
}

float get_latitude(char lat_pointer)
{
    unsigned char lat_index = lat_pointer + 1; /* index pointing to the latitude */
    unsigned char index = 0;
    char Lat_Buffer[15];
    float _latitude;

    /* parse Latitude in GGA string stored in buffer */
    for (; GGA_Buffer[lat_index] != ','; lat_index++) {
        Lat_Buffer[index] = GGA_Buffer[lat_index];
        index++;
    }
    lat_index++;
    N_S = GGA_Buffer[lat_index];
    _latitude = atof(Lat_Buffer); /* convert string of latitude to float */
    return _latitude; /* return float raw value of Latitude */
}

float get_longitude(unsigned char long_pointer)
{
    unsigned char long_index;
    unsigned char index = long_pointer + 1; /* index pointing to the longitude */
    char Long_Buffer[15];
    float _longitude;
    long_index = 0;

    /* parse Longitude in GGA string stored in buffer */
    for (; GGA_Buffer[index] != ','; index++) {
        Long_Buffer[long_index] = GGA_Buffer[index];
        long_index++;
    }
    long_index++;
    E_W = GGA_Buffer[long_index];
    _longitude = atof(Long_Buffer); /* convert string of longitude to float */
    return _longitude; /* return float raw value of Longitude */
}

float convert_to_degrees(float NMEA_lat_long)
{
    float minutes, dec_deg, decimal;

    int degrees;

    float position;

    degrees = (int) (NMEA_lat_long / 100.00);
    minutes = NMEA_lat_long - degrees * 100.00;
    dec_deg = minutes / 60.00;
    decimal = degrees + dec_deg;
    if (N_S == 'S' || E_W == 'W') { // return negative
        decimal *= -1;
    }
    /* convert raw latitude/longitude into degree format */
    return decimal;
}

void interrupt Serial_ISR()
{

    if (RCIF) {
        GIE = 0; /* Disable global interrupt */
        unsigned char received_char = RCREG;
        if (RCSTAbits.OERR) { /* check if any overrun occur due to continuous reception */
            CREN = 0;
            NOP();
            CREN = 1;
        }

        if (received_char == '$') { /* check for '$' */
            GGA_Index = 0;
            IsItGGAString = 0;
            CommaCounter = 0;
        } else if (IsItGGAString == 1) { /* if true save GGA info. into buffer */
            if (received_char == ',') GGA_Pointers[CommaCounter++] = GGA_Index; /* store instances of ',' in buffer */
            GGA_Buffer[GGA_Index++] = received_char;
        } else if (GGA_CODE[0] == 'G' && GGA_CODE[1] == 'G' && GGA_CODE[2] == 'A') { /* check for GGA string */
            IsItGGAString = 1;
            GGA_CODE[0] = 0;
            GGA_CODE[1] = 0;
            GGA_CODE[2] = 0;
        } else {
            GGA_CODE[0] = GGA_CODE[1];
            GGA_CODE[1] = GGA_CODE[2];
            GGA_CODE[2] = received_char;
        }
    }
}

void delay_ms(uint16_t milis)
{
    for (uint16_t t = 0; t < milis; t++) {
        __delay_ms(1);
    }
}