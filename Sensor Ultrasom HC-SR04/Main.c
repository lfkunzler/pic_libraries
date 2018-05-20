
#include <xc.h>
#include <plib/xlcd.h>
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

char txt[4];          // Usado para convers?o de dados em string
char Tempo_H, Tempo_L, distancia_cm;
unsigned int periodo;  // Periodo do pulso de ECHO do sensor de ultrassom

// Prototipos das funcoes
void inic_XLCD(void); 		// Inicializa o LCD
void limpa(void);		// Limpa a tela do display
void Mydelay(int tempo);
void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

void distance(void);
void trigger();

void interrupt capture()
{
     if(PIR1bits.CCP1IF && (CCP1CONbits.CCP1M0 == 0x05))  // Captura por CCP1 e borda de subida
     {
       PIR1bits.CCP1IF  = 0x00;     // Limpa a flag para nova captura
       PIE1bits.CCP1IE  = 0x00;     // Desabilita a interrupcao por perifericos (CCP1)
       PIE1bits.CCP1IE  = 0x04;     // Configura a borda de captura para descida
       PIE1bits.CCP1IE  = 0x01;     // Habilita a interrupcao por perifericos (CCP1)
       TMR1H       = 0x00;     // Zeram os registradores do TIMER1 para contagem de tempo
       TMR1L       = 0x00;
       T1CONbits.TMR1ON  = 0x01;     // Habilita contagem de tempo
     }
     else if(PIR1bits.CCP1IF && PIR1bits.CCP1IF == 0x04) // captura por CCP1 e borda de descida
     {
       PIR1bits.CCP1IF  = 0x00;     // Limpa a flag para nova captura
       T1CONbits.TMR1ON  = 0x00;     // Desabilita a contagem de tempo
       PIE1bits.CCP1IE = 0x00;     // Desabilita a interrupcao por perifericos
       CCP1CON     = 0x05;     // Configura a borda de captura para subida
       PIE1bits.CCP1IE  = 0x01;     // Habilita a interrupcao por perifericos
       Tempo_H     = CCPR1H;   // Carrega valores de tempo capturado (us)
       Tempo_L     = CCPR1L;
     }
}     // end interrupt


void main(void) {
   TRISB = 0; // TUDO COMO SAIDA
   TRISC = 0x04; // TUDO COMO SAIDA EXCETO PINO RC2 POIS É A ENTRADA DO CCP
 
   
     CMCON  = 0x07;               // desabilita os comparadores
     PORTC  = 0x00;               // inicia todo PORTC nivel low
     
     INTCON = 0b11000000;         // Liga interruptores GIE e PEIE
     PIE1bits.TMR1IE = 0x00;           // desabilita interrupcao de TMR1
     PIE1bits.CCP1IE = 0x01;           // habilita interrupcoes por captura
     CCP1CON    = 0x05;           // COnfigura modulo CCP1 para borda de subida
     
     T1CONbits.T1CKPS1 = 1;             // Prescaller TMR1 1:4
     T1CONbits.T1CKPS0 = 0;             //
    T1CONbits.TMR1CS  = 0;             // clock selecionado OSC/4
     T1CONbits.TMR1ON  = 0;             // desabilita contagem do timer1  
   
    inic_XLCD();
   
  
   
while(1){
   distance(); 
}
}
void distance(void)
{
  trigger();                  // dispara o "gatilho" do sensor
  Mydelay(100);              // aguarda 100 milisegundos
  
  periodo = (Tempo_H<<8)  + Tempo_L;  // periodo relativo ao ECHO em us
  
  distancia_cm = periodo/58;          // converte a distancia informada para cm
   
   SetDDRamAddr(0x00);
    while(BusyXLCD());
   if(periodo == 0){
    periodo = periodo + 48;   
    putcXLCD(periodo);
      
      
   }
  
  
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