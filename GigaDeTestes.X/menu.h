#ifndef INC_MENU_H
#define	INC_MENU_H

#include <stdint.h>

#include "display.h"

#define MENU_ITENS_CONT 1

typedef void (*OnEnter)();
typedef void (*OnRender)();
typedef void (*OnExit)();

typedef struct {
    OnEnter Enter; // callback que sera executado quando abrir a tela
    OnRender Render; // cb executado quando atualizar a tela (a cada 20ms)
    OnExit Exit; // cb executado quando fechar a tela
} MenuItem;

struct 
{
    lcd_t *lcd;
	uint8_t NeedsRedraw;
	uint8_t Index;
	MenuItem *Current;
	MenuItem *Items[MENU_ITENS_CONT];
} Menu;

void menuInit(lcd_t *lcd);
void menuGoto(uint8_t pos);
void menuSetPosition(uint8_t pos, MenuItem *item);
void menuNext();
void menuPrev();
void menuReset();
void menuRuntime();
// Required execution rate = 50hz;
void menuDraw();

#endif	/* INC_MENU_H */

