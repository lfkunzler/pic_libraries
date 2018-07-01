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

/* impleentacao dos callback da tela de magnetometro */
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

/* impleentacao dos callback da tela de ultrassom */
void OnEnterMenu2()
{

}

void OnRenderMenu2()
{
    sprintf(buffer, "%4d %4d %4d\0", cord.x, cord.y, cord.z);
    lcdWrite(&lcd, 2, 1, buffer);
}

void OnExitMenu2()
{

}

MenuItem menu1 = {
    OnEnterMenu1,
    OnRenderMenu1,
    OnExitMenu1
};

MenuItem menu2 = {
    OnEnterMenu2,
    OnRenderMenu2,
    OnExitMenu2
};

void interrupt ISR()
{

}

void interrupt low_priority ISR_LP()
{
    if (TMR0IF) {
        TMR0IF = 0; // desliga interrupcao        
        TMR0H = 0x63; // configuracao para uma interrupcao a cada 20ms
        TMR0L = 0xC0;

        // TODO: rotinas que rodam a cada 20ms
        LED_TESTE = !LED_TESTE;
        menuDraw();
    }
}

void main(void)
{
    uint8_t opcao;

    memset(buffer, 0, 32);

    ADCON1 = 0x0F;
    OSCCON = 0x72; // osc interno a 8MHz

    TRISC = 0xFE; // RC0 em output
    LED_TESTE = 1;

    /* CONFIGURACAO DAS INTERRUPCOES */
    IPEN = 1; // ativa diferenca de prioridade entre interrupcoes
    GIEH = 1; // ativa interrupcoes alta prioridade
    PEIE = 1; // ativa interrupcos perifericas de baixa prioridade    
        
    /* CONFIGURACAO DO TIMER0 */
    /* EQUACAO PARA CALCULAR O TEMPO DE INTERRUPCAO DO TMR1*/
    // t = Tosc * prescaler * (2^16 - (TMR1H:TMR1L))
    // 20ms = 0.02s = 4/8000000 * PRES * (2^16 - (TMR1H:TMR1L))
    // (0.02 * 8000000/4) / PRES = 2^16 - (TMR1H:TMR1L)
    //  400000/1 = 65536 - regs
    // regs = 65536 - 40000
    // regs = 25536 = 0x63C0

    T0CONbits.TMR0ON = 1; // timer0 ativo
    T0CONbits.T08BIT = 0; // 16bits
    T0CONbits.T0CS = 0; // incrementado pelo ciclo de maquina
    T0CONbits.PSA = 1; // prescaler nao associado ao timer0W    
    
    TMR0H = 0x63; // configuracao para uma interrupcao a cada 20ms
    TMR0L = 0xC0;
    TMR0IF = 0; // desliga interrupcoes existentes
    TMR0IE = 1; // ativa as interrupcoes do timer 1
    TMR0IP = 0; // timer1 peripheral interrupt em baixa prioridade

    /* UTILIZACAO DO DISPLAY */
    lcdInit(&lcd);
    menuInit(&lcd);
    menuSetPosition(MAGNETOMETRO, &menu1);
    menuSetPosition(ULTRASSOM, &menu2);
    
    opcao = 2;

    switch (opcao) {
    case MAGNETOMETRO:
        magnetoInit();
        menuGoto(MAGNETOMETRO); // abre a tela do magneto
        while (1) {
            magnetoGetCord(&cord);
            menuRuntime();
        }
        break;
    case ULTRASSOM:

        break;
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