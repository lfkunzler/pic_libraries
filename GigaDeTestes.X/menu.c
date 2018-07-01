#include "menu.h"

void _menuEnterPosition(uint8_t pos);

void menuInit(lcd_t *lcd)
{
    Menu.lcd = lcd;
}

void menuGoto(uint8_t pos)
{
    _menuEnterPosition(pos);
}

void menuSetPosition(uint8_t pos, MenuItem *item)
{
    Menu.Items[pos] = item;
}

void _menuEnterPosition(uint8_t pos)
{
    Menu.Current = Menu.Items[pos];
    Menu.Current->Enter();
    menuDraw();
}

void menuNext()
{
    Menu.Index++;

    Menu.Current->Exit();
    if (Menu.Index >= MENU_ITENS_CONT)
        Menu.Index = 0;

    _menuEnterPosition(Menu.Index);
}

void menuPrev()
{
    Menu.Current->Exit();
    if (Menu.Index > 0)
        Menu.Index--;

    _menuEnterPosition(Menu.Index);
}

void menuDraw()
{
    Menu.NeedsRedraw = 1;
}

void menuReset()
{
    Menu.Current->Exit();
    Menu.Index = 0;
    _menuEnterPosition(Menu.Index);
}

void menuRuntime()
{
    if (Menu.NeedsRedraw) {
        Menu.NeedsRedraw = 0;
        if (Menu.Current) {
            Menu.Current->Render();
        }
        lcdRuntime(Menu.lcd);
    }
}