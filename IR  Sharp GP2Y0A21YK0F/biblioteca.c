#include "biblioteca.h"

void Mydelay(int tempo)
{
   int i;
   for (i=0;i<tempo;i++)
   {
      __delay_ms(1);
   }
}
// Inicializa o LCD

void inic_XLCD(void) {
    OpenXLCD(FOUR_BIT & LINES_5X7); // configura para 4-bit, 5x7, multiplas linhas
    while (BusyXLCD());
    WriteCmdXLCD(0x06); // move o cursor para direita depois de escrever 
    while (BusyXLCD());
    WriteCmdXLCD(0x0C); // display ligado com cursor piscado
    while (BusyXLCD());
}
// Limpa a tela do display

void limpa(void) {
    while (BusyXLCD());
    WriteCmdXLCD(0x01); // comando para limpar a tela do display
    Mydelay(80); // Eh necessario dar um tempo depois de limpar
}

//////////////// Funcoes de tempo para a xlcd.h ///////////

void DelayFor18TCY(void) {
    __delay_us((8 * 18.0 * 4000000.0) / _XTAL_FREQ);
    // Seria necessario 18, mas na pratica e necessario aumentar o valor
    // Quanto maior a frequencia, maior o valor
}

void DelayPORXLCD(void) {
    __delay_ms(15);
}

void DelayXLCD(void) {
    __delay_ms(5);
}

