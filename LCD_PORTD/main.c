#include "biblioteca.h"
void main(void)
{
   unsigned int valor;
   unsigned char caracter;
   TRISB= 0;   // Setando toda a porta B como sa�da
   TRISD= 0;   // Setando toda a porta D como sa�da
   TRISA = 0b00000001; // Setando RA0 como entrada p/ ADC

// Inicilzando adc
   OpenADC (ADC_FOSC_16 & //FAD = FOSC/16 = 20MHz/16 = 125kHz. Desta forma, TAD=1/FAD = 0,8us.
      ADC_RIGHT_JUST &
      ADC_4_TAD,	//Tempo de convers�o de uma palavra de 10-bits, neste caso ser� igual a 4*TAD = 2*0,8us = 3,2us.

      ADC_CH0 & 
      ADC_INT_OFF & 
      ADC_REF_VDD_VSS, //Determina o VDD (+5V) como tens�o de refer�ncia positiva (VREF+) e o VSS (0V)como tens�o de
                       //refer�ncia negativa (VREF-).

      ADC_1ANA);  
   // Inicializa LCD
   inic_XLCD();
   while (BusyXLCD());
   while(1) 
   {
      SetChanADC (ADC_CH0); 
      ConvertADC();                    
      while(BusyADC());
      valor = ReadADC();  
      valor =(int)valor*0.49;// Adaptacao de ((valor*(5/1023))/0.01); 5V - 2elevado na 10 - 10mV por graus
      
      caracter=(unsigned char)valor/10;
      SetDDRamAddr(0);
      putcXLCD(caracter+48);
      while (BusyXLCD());
      caracter=(unsigned char)valor%10;
      SetDDRamAddr(1);
      putcXLCD(caracter+48);
      while (BusyXLCD());
      Mydelay(1000);
   } 
 }