#include "biblioteca.h"
#include <math.h>
#include <stdint.h>

void traduz(int numero, char *txt) {
    char a, b, c, d, e;
    a = (numero % 10) + 48;
    numero = numero / 10;
    b = (numero % 10) + 48;
    numero = numero / 10;
    c = (numero % 10) + 48;
    (txt)[0] = c;
    (txt)[1] = b;
    (txt)[2] = a;
    (txt)[3] = '\0';
}
void main(void) {
    uint16_t valor = 0;
    char txt[4]; // Usado para convers?o de dados em string 
    TRISA = 0b00000001; // Setando RA0 como entrada p/ ADC

    // Inicilzando adc
    OpenADC(ADC_FOSC_16 & //FAD = FOSC/16 = 20MHz/16 = 125kHz. Desta forma, TAD=1/FAD = 0,8us.
            ADC_RIGHT_JUST &
            ADC_4_TAD, //Tempo de convers�o de uma palavra de 10-bits, neste caso ser� igual a 4*TAD = 2*0,8us = 3,2us.
            ADC_CH0 &
            ADC_INT_OFF &
            ADC_REF_VDD_VSS, //Determina o VDD (+5V) como tens�o de refer�ncia positiva (VREF+) e o VSS (0V)como tens�o de
            //refer�ncia negativa (VREF-).
            ADC_1ANA);

    // Inicializa LCD
    inic_XLCD();
    while (BusyXLCD());

    while (1) {
        SetChanADC(ADC_CH0);
        ConvertADC();
        while (BusyADC());
        
        valor = ReadADC();
        valor = (int) (12343.85 * pow(valor, -1.15));

        traduz(valor, &txt);
        
        SetDDRamAddr(0x01);
        while (BusyXLCD());
        putsXLCD("DISTANCIA (cm)");

        if ((valor < 10) || (valor > 80)) {
            SetDDRamAddr(0x47);
            while (BusyXLCD());
            putsXLCD("---");
        } else {
            SetDDRamAddr(0x47);
            while (BusyXLCD());
            putsXLCD(txt);
        }
        Mydelay(500);
    }
}
