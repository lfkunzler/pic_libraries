/*
 * Este projeto tem como objetivo ler e escrever um determinada posição de memoria
 * de cartoes RFID. As funções de leitura e escrita sao habilitadas pelas portas
 * RC1 e RC6, respectivamente. As string a serem escritas estao predefinidas e variam
 * de acordo com o valor lido no potenciometro ligado a porta 2 da pic. Como a ideia
 * base é testar as funções principais para o funcionamento, não foram definidos métodos 
 * de seleção do bloco de memória a ser escrito, nem opção de escolha do dado a ser gravado 
 * por parte do usuário. Esses adicionais ficam a dispocição a partir da necessidade
 * do projeto a ser desenvolvido com essas funções com real aplicação. 
 */



#include "RFIDlib.h"
#include "fuses.h"
#include <stdio.h>
#include <stdlib.h>
#include "mylcd.h"

void traduz(int numero, char *txt){   
      char a,b,c,d,e;
      a = (numero % 10) + 48;
      numero = numero / 10 ;
      b = (numero % 10) + 48;
      numero=numero / 10 ;
      c = (numero % 10) + 48;
      //numero=numero / 10 ;
      //d = (numero % 10) + 48 ;
      //e = numero / 10  + 48;
      (txt)[0]=c;
      (txt)[1]=b;
      (txt)[2]=a;
      (txt)[3]='\0';
      //txt[4]=a;
}

//#define LED LATBbits.LB4 // Dá nome aos pinos 

#define _XTAL_FREQ   8000000 
char key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // chave de acesso do setor da TAG
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
    
    //OSCCONbits.IDLEN = 0;//dispositivo entra no modo Sleep quando o comando for executado
    MFRC522_Init(); // inicializar o módulo leitor rfid
    inic_XLCD();
   char UID[10]; // vari?vel onde ser? salva a ID lida
     char buffer[5]; // vari?vel onde ser? salva a ID lida

    unsigned TagType; // armazenamento do caracter retornado pelo leitor quando verificado algum chaveiro
    char size;
    int t = 0, k = 1, i = 0, cont = 0, valor = 0, j = 0;

      
    // informação a ser gravada na TAG 
    
    SetDDRamAddr(0x03);
   while(BusyXLCD());    
   putsXLCD("ID DA TAG");
   
   
   char vetor[5] = {0x14,0xB5,0xCE, 0xFC};
                    
   
                     
   
   
    while (1) {
        
         
            if (MFRC522_isCard(&TagType)) { // espera a presença de uma TAG
               // WriteCmdXLCD(0x01); 
                //SetDDRamAddr(0x00);
	      
	          
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
                        
                       
                    /*for (i = 0; i < 5; i++) {
                        sprintf (buffer,"%d",UID);
                       // putcXLCD(UID[i]); // imprime caracter por caracter da ID
                       // Mydelay(500);
                      //  var++;
                    }
                         SetDDRamAddr(var);
                        while(BusyXLCD());    
                        putsXLCD(buffer);
                    //size = MFRC522_SelectTag(&UID); // esta função prepara para a autenticação*/
                }
            }
        
    }
              /*  if (MFRC522_Auth(PICC_AUTHENT1A, 4, &key, &UID) == 0) { // autenticação
                   // SetDDRamAddr(0x40);
                   // putrsXLCD("Dado: ");
                    MFRC522_Read(4, &readData); // leitura
                    for (i = 0; i < 10; i++) {
                       // putcXLCD(readData[i]); 
                    }
                } else if (MFRC522_Auth(PICC_AUTHENT1B, 4, &key, &UID) == 0) {
                    //SetDDRamAddr(0x40);
                    //putrsXLCD("Dado: ");
                    MFRC522_Read(4, &readData);
                    for (i = 0; i < 10; i++) {
                        //putcXLCD(readData[i]);
                    }
                } else {
                    //SetDDRamAddr(0x40);
                    //putrsXLCD("Erro");
                    continue;
                }
            }
        }
        /*if (t == 1) { // escrita
        //    while (BusyXLCD());
            if (MFRC522_isCard(&TagType)) {
                //WriteCmdXLCD(0x01);
                //SetDDRamAddr(0x00);
                if (MFRC522_ReadCardSerial(&UID)) {
                    size = MFRC522_SelectTag(&UID);
                    if (MFRC522_Auth(PICC_AUTHENT1A, 4, &key, &UID) == 0) {
                        //SetDDRamAddr(0x00);
                       // putrsXLCD("Dado Escrito");
                        switch (j) {
                            case 0:
                                MFRC522_Write(4, writeData1);
                               // SetDDRamAddr(0x40);
                               // putrsXLCD(writeData1);
                                break;
                            case 1:
                                MFRC522_Write(4, writeData2);
                               // SetDDRamAddr(0x40);
                               // putrsXLCD(writeData2);
                                break;
                            case 2:
                                MFRC522_Write(4, writeData3);
                               // SetDDRamAddr(0x40);
                               // putrsXLCD(writeData3);
                                break;
                        }
                    } else if (MFRC522_Auth(PICC_AUTHENT1B, 4, &key, &UID) == 0) {
                        //SetDDRamAddr(0x00);
                        //putrsXLCD("Dado Escrito");
                        switch (j) {
                            case 0:
                                MFRC522_Write(4, writeData1);
                                //SetDDRamAddr(0x40);
                                //putrsXLCD(writeData1);
                                break;
                            case 1:
                                MFRC522_Write(4, writeData2);
                                //SetDDRamAddr(0x40);
                                //putrsXLCD(writeData2);
                                break;
                            case 2:
                                MFRC522_Write(4, writeData3);
                                //SetDDRamAddr(0x40);
                                //putrsXLCD(writeData3);
                                break;
                        }
                    } else {
                        //SetDDRamAddr(0x00);
                        //putrsXLCD("Nada Escrito");
                        continue;
                    }
                }
            }
        }*/
        /*if ((PIR1 & 0b00000010) && (t == 1 || k == 1)) { //PIR1 & 0b00000010 REGISTRADOR DO TIMER 2
            cont++; // incrementa variavel cont para controle do timer
            if (cont >= 125) {
                cont = 0;
                k = 0;
                t = 0; // zera a variavel t, interrompendo as leituras
               // WriteCmdXLCD(0x01); // limpa LCD
                //SetDDRamAddr(0x00);
            }
            PIR1 = PIR1 & 0b11111101; // MESMA COISA QUE PIR1bits.TMR2IF = 0;
        }*/
 }

