
#include <xc.h>
#include <plib/xlcd.h>
#include <math.h>
#include <plib/timers.h>
#include <plib/capture.h>
#define _XTAL_FREQ 20000000     // Usar cristal de 20MHz 


// Configuracoes basicas
// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS //HSPLL_HS      // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 1         // Brown-out Reset Voltage bits (Setting 2 4.33V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON     // CCP2 MUX bit (CCP2 input/output is not multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#define  TRIG PORTCbits.RC0

char txt[5];          // Usado para convers?o de dados em string
unsigned long long int Tempo_H, Tempo_L, distancia_cm;
unsigned  long long int periodo;  // Periodo do pulso de ECHO do sensor de ultrassom
unsigned long quant=0; // contador de overflows no registrador pir1
unsigned char eventoInicio=0;

// Prototipos das funcoes
void inic_XLCD(void); 		// Inicializa o LCD
void limpa(void);		// Limpa a tela do display
void Mydelay(int tempo);
void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

void interrupt captura(void);
void trata_interrup_CCP1(void);
void distance(void);
void trigger(void);



void interrupt captura(void){
   if (PIR1 & 0x01) //se ocorreu overflow no registrador pir1
   {
     quant++;
     PIR1 = PIR1 & 0xFE;  //reseta o bit e trata do overflow
   }
   else //if( PIR1 & 0x04) // interrupt flag, occoreu uma interrupçao 
   {
      trata_interrup_CCP1();
      //PIR1 = PIR1 & 0x00;
   }
   
}

void trata_interrup_CCP1(void)
{
   unsigned int valorTimer1;
   float tempofloat;
   unsigned char tempo8bits;
   if(eventoInicio==0) // se é inicio de evento
   {
      quant = 0; // zera-se a variavel para saber quantos overflows ocorreram ate o fim  do evento
      eventoInicio = 1; // flag para inicio de evento
      WriteTimer1(0); // coloca timer em 0
      OpenCapture1(CAPTURE_INT_ON & CAP_EVERY_FALL_EDGE); // Interrupção e borda de descida do ccp 1
   }
   else // se é fim de evento
   {
      valorTimer1=ReadCapture1(); // le o valor da captura do timer 1
      periodo= (65536 * quant) + valorTimer1; //multiplica quantidade de overflows por 2^18 que é o que se passou para se obter 1 overflow
      //periodo=(float)periodo*(0.0000002); // (4/_XTAL_FREQ) para retornar em segundos
      periodo = periodo / 5;
      eventoInicio=0;
      OpenCapture1(CAPTURE_INT_ON & CAP_EVERY_RISE_EDGE); // Interrupção e borda de subida
   }
}



void main(void) {
   TRISB = 0; // TUDO COMO SAIDA
   TRISC = 0x04; // TUDO COMO SAIDA EXCETO PINO RC2 POIS É A ENTRADA DO CCP
   inic_XLCD();
   
   
   CMCON = 0x07; // DESABILITA COMPARADORES
   
   //configuracao do timer 1
   OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF & T1_PS_1_1);
   // HABILITA INTERRUPCAO, REGISTRADOR DE 16 BITS, FONTE DE CLOCK INTERNA, DESATIVA OSCILADOR, MODO ASSINCRONO, PRESCALER 1
   SetTmrCCPSrc (T1_SOURCE_CCP); // TIMER 1 COMO FONTE PARA MODULO CCP
   // configuracao do CCP1
    OpenCapture1(CAPTURE_INT_ON & CAP_EVERY_RISE_EDGE);
   // HABILITA INTERRUPCAO  E DEFINE BORDA DE SUBIDA
   
   IPR1bits.TMR1IP = 1; //prioridade do timer 1 como alta
   IPR1bits.CCP1IP = 1; // Prioridade do CCP1 como alta;
   RCONbits.IPEN = 0; // uma só prioridade
   
   INTCON = 0b11000000;
   //INTCONbits.GIEH = 1; // habilita todas as fontes de interrupção
   //INTCONbits.GIEL = 1; // habilita todas as fontes de interrupção de perifericos
   WriteTimer1(0);
   //ZERA O TIMER 1
   while(1){
      distance();
   }
}

void traduz(int numero){   
      char a,b,c,d,e;
      a = (numero % 10) + 48;
      numero=numero /10 ;
      b = (numero % 10) + 48;
      numero=numero /10 ;
      c = (numero % 10) + 48;
      numero=numero /10 ;
      d = (numero % 10) + 48 ;
      e = numero /10  + 48;
      txt[0]=e;
      txt[1]=d;
      txt[2]=c;
      txt[3]=b;
      txt[4]=a;
   
}
void distance(void)
{
   trigger();                  // dispara o "gatilho" do sensor
   //periodo = ceil (periodo);
  distancia_cm = periodo/58;          // converte a distancia informada para cm
  //distancia_cm = ceil(distancia_cm);
   //distancia_cm = 1000;
   SetDDRamAddr(0x00);
   traduz(distancia_cm);
   while(BusyXLCD());    
   putsXLCD(txt);
      
      
  // }
  
  
  /*if((distancia_cm < 3)||(distancia_cm > 300)){
    unsigned char nova[16] = "NOVA SENHA";
  distancia_cm -= 48  ; 
    while(BusyXLCD());
    SetDDRamAddr(0x03);
    while(BusyXLCD());
    putrsXLCD(distancia_cm);
    while(BusyXLCD());
  }
  else
  {
      unsigned char velha[16] = "veia SENHA";   
    while(BusyXLCD());
    SetDDRamAddr(0x03);
    while(BusyXLCD());
    putrsXLCD(distancia_cm + 48);
    while(BusyXLCD());
 
  
  }*/
  
  }

void trigger()
{
   TRIG = 1;
   __delay_us(10);
   TRIG = 0;
   __delay_ms(60);
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
void Mydelay(int tempo)
{
   int i;
   for (i=0;i<tempo;i++)
   {
      __delay_ms(1);
   }
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