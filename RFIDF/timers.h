#define _XTAL_FREQ 8000000

void Mydelay(int tempo) {
    int i;
    for (i = 0; i < tempo; i++) {
        __delay_ms(1);
    }
}

void DelayFor18TCY(void) {
    __delay_us((8 * 18.0 * 4000000.0) / _XTAL_FREQ);
}

void DelayPORXLCD(void) {
    __delay_ms(15);
}

void DelayXLCD(void) {
    __delay_ms(5);
}