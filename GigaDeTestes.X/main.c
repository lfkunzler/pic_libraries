#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "xlcd_mod.h"
#include "display.h"
#include "menu.h"
#include "magnetometro.h"

#define LED_TESTE PORTCbits.RC0

char buffer[32];
lcd_t lcd;
coordenadas_t cord;

/* impleentacao dos callback do menu 1 */
void OnEnterMenu1()
{

}

void OnRenderMenu1()
{
    sprintf(buffer, "%4d %4d %4d\0", cord.x, cord.y, cord.z);
    lcdWrite(&lcd, 2, 1, buffer);
}

void OnExitMenu1()
{

}

MenuItem menu1 = {
    OnEnterMenu1,
    OnRenderMenu1,
    OnExitMenu1
};

void interrupt ISR()
{

}

void interrupt low_priority ISR_LP()
{
    if (TMR1IF) {
        TMR1IF = 0; // desliga interrupcao        
        TMR1H = 0x63; // configuracao para uma interrupcao a cada 20ms
        TMR1L = 0xC0;

        // TODO: rotinas que rodam a cada 20ms
        LED_TESTE = !LED_TESTE;
        menuDraw();
    }
}

void main(void)
{
    uint8_t opcao = MAGNETOMETRO;

    memset(buffer, 0, 32);

    ADCON1 = 0x0F;
    OSCCON = 0x72; // osc interno a 8MHz

    TRISC = 0xFE; // RC0 em output
    LED_TESTE = 1;

    /* CONFIGURACAO DAS INTERRUPCOES */
    IPEN = 1; // ativa diferenca de prioridade entre interrupcoes
    GIEH = 1; // ativa interrupcoes alta prioridade
    PEIE = 1; // ativa interrupcos perifericas de baixa prioridade

    /* CONFIGURACAO DO TIMER 1 */
    /* EQUACAO PARA CALCULAR O TEMPO DE INTERRUPCAO DO TMR1*/
    // t = Tosc * prescaler * (2^16 - (TMR1H:TMR1L))
    // 20ms = 0.02s = 4/8000000 * PRES * (2^16 - (TMR1H:TMR1L))
    // (0.02 * 8000000/4) / PRES = 2^16 - (TMR1H:TMR1L)
    //  400000/1 = 65536 - regs
    // regs = 65536 - 40000
    // regs = 25536 = 0x63C0
    T1CONbits.RD16 = 1; // usa 16 bits
    T1CONbits.T1RUN = 0; // nao usa o clock auxiliar
    T1CONbits.T1CKPS = 0; // pre em 1
    T1CONbits.T1OSCEN = 0; // desativa o oscilador do tmr1
    T1CONbits.TMR1CS = 0; // osc e baseado no ciclo de maquina (fosc/4)
    T1CONbits.TMR1ON = 1; // tmr1 ativo     
    TMR1H = 0x63; // configuracao para uma interrupcao a cada 20ms
    TMR1L = 0xC0;
    TMR1IF = 0; // desliga interrupcoes existentes
    TMR1IE = 1; // ativa as interrupcoes do timer 1
    TMR1IP = 0; // timer1 peripheral interrupt em baixa prioridade

    /* UTILIZACAO DO DISPLAY */
    lcdInit(&lcd);
    menuInit(&lcd);
    menuSetPosition(0, &menu1);
    menuGoto(0); // abre a tela 0

    while (1) {
        switch (opcao) {
        case MAGNETOMETRO:
            magnetoGetCord(&cord);

            break;
        }
        menuRuntime();
    }

    return;
}

void delay_ms(uint16_t milis)
{
    uint8_t t;
    for (t = 0; t < milis; t++) {
        ;
    }
}