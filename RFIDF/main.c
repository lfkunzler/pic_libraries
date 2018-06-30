
#include "RFIDlib.h"
#include "fuses.h"
#include <stdio.h>
#include <stdlib.h>
#include "mylcd.h"
#define _XTAL_FREQ   8000000 

void traduz(int numero, char *txt){   
    char a,b,c,d,e;
    a = numero & 0b00001111;
    if (a > 9){
	    a = a + 55;
    }
    else{
	    a = a + 48;
    }
    b = numero >> 4;
    if (b > 9){
	    b = b + 55;
    }
    else{
	    b = b + 48;
    }
    (txt)[0]=b;
    (txt)[1]=a;
    (txt)[2]='\0';
}

void Mydelay(int tempo)
{
   int i;
   for (i=0;i<tempo;i++)
   {
      __delay_ms(1);
   }
}

// Inicializa o LCD
void inic_XLCD(void)
{
   OpenXLCD(FOUR_BIT&LINES_5X7); // configura para 4-bit, 5x7, multiplas linhas
   while(BusyXLCD());            
   WriteCmdXLCD(0x06);           // move o cursor para direita depois de escrever 
   while(BusyXLCD()); 
   WriteCmdXLCD(0x0C);           // display ligado sem aparecer o cursor
   while(BusyXLCD()); 
}
// Limpa a tela do display
void limpa(void){		
   while(BusyXLCD());  
   WriteCmdXLCD(0x01); 	// comando para limpar a tela do display
   Mydelay(80);		// Eh necessario dar um tempo depois de limpar
} 

//////////////// Funcoes de tempo para a xlcd.h ///////////
void DelayFor18TCY(void)
{ __delay_us((8*18.0*4000000.0)/_XTAL_FREQ); 
   // Seria necessario 18, mas na pratica e necessario aumentar o valor
   // Quanto maior a frequencia, maior o valor
}
void DelayPORXLCD(void)
{ __delay_ms(60); }
void DelayXLCD(void)
{ __delay_ms(20); }



int main() {
    ADCON1 = 0x0F;
    OSCCON = 0b01110111; // oscilador interno
    TRISA = 0x00; // saida
    TRISD = 0b00000010; // saida
    TRISC = 0b11111111; // entrada
    TRISB = 0b00000000; // saida
    MFRC522_Init(); // inicializar o m�dulo leitor rfid
    inic_XLCD();
   char UID[10]; // vari?vel onde ser? salva a ID lida
    char buffer[3]; // vari?vel onde ser? salva a ID lida

    unsigned TagType; // armazenamento do caracter retornado pelo leitor quando verificado algum chaveiro
    char size;   
    SetDDRamAddr(0x03);
    while(BusyXLCD());    
    putsXLCD("ID DA TAG");
    while (1) {
            if (MFRC522_isCard(&TagType)) { // espera a presen�a de uma TAG
            	          
                if (MFRC522_ReadCardSerial(&UID)) { // faz a leitura da ID e salva em UID
                    RA0 = ! RA0;
                    Mydelay(500);                       
                    int var= 0x40;
                    SetDDRamAddr(var);
                    while(BusyXLCD());
                    for(int i = 0; i < 5; i++){
                        traduz(UID[i],buffer);
                        putsXLCD(buffer);
                        var = var +4;
                    }
                }
            }
        
    }
 }

