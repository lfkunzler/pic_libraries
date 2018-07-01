/*
 * File:   display.c
 * Authors: Luis Felipe Kunzler & Ruan Carlos Pinto
 *
 * Created on 18 de Maio de 2018, 01:36
 */

#include <xc.h>
#include <stdint.h>

#include "xlcd_mod.h"
#include "display.h"

// Inicializa o LCD

void lcdInit(lcd_t *lcd) {
    TRISD = 0;
    
    OpenXLCD(FOUR_BIT & LINES_5X7); // configura para 4-bit, 5x7, multiplas linhas
    while (BusyXLCD());
    WriteCmdXLCD(0x06); // move o cursor para direita depois de escrever 
    while (BusyXLCD());
    WriteCmdXLCD(0x0C); // display ligado sem aparecer o cursor
    while (BusyXLCD());

    lcdCleanAll(lcd);
}

// Limpa a tela do display
void lcdClean(void) {
    while (BusyXLCD());
    WriteCmdXLCD(0x01); // comando para limpar a tela do display

    delay_ms(80); // Eh necessario dar um tempo depois de limpar
}

// limpa a memoria, inserindo espacos
void lcdMemClean(lcd_t *lcd) {
    for (uint8_t i = 0; i < 16; i++) {
        lcd->Memory[0][i] = 0x20;
        lcd->Memory[1][i] = 0x20;
    }
    lcd->NeedsRedraw = 1;
}

void lcdCleanAll(lcd_t *lcd) {
    lcdClean();
    lcdMemClean(lcd);
}


int lcdWrite(lcd_t *lcd, uint8_t row, uint8_t col, char *string) {
    // se estiver fora dos limites de escrita
    if (row == 0 || row > 2 || col == 0 || col > 16) {
        return -1;
    }

    row--; // ajusta para posicao de vetor
    col--;

    if (row == 0) { // se for escrever na linha de cima
        while (*string && col <= 0x0F) {
            lcd->Memory[0][col++] = *string;
            string++;
        }
        col = 0; // volta para a primeira posicao
        row = 1; // da linha de baixo
    }

    // se ainda houver dados ou se a instrucao for apenas para a linha de baixo
    while (*string && col <= 0x0F) {
        lcd->Memory[1][col++] = *string;
        string++;
    }

    lcd->NeedsRedraw = 1;

    return 0;
}

void lcdRuntime(lcd_t *lcd) {
    while (BusyXLCD());

    if (lcd->NeedsRedraw == 1) {
        SetDDRamAddr(0x00);
        for (int i = 0; i < 16; i++) {
            while (BusyXLCD());
            putcXLCD((char) lcd->Memory[0][i]);
        }
        SetDDRamAddr(0x40);
        for (int i = 0; i < 16; i++) {
            while (BusyXLCD());
            putcXLCD((char) lcd->Memory[1][i]);
        }

        lcd->NeedsRedraw = 0;
    }
}

//////////////// Funcoes de tempo para a xlcd.h ///////////

void DelayFor18TCY(void) {
    for (int i = 0; i < 18; i++) {
        continue;
    }
}

void DelayPORXLCD(void) {
    delay_ms(60);
}

void DelayXLCD(void) {
    delay_ms(20);
}