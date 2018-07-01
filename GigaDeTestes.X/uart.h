#ifndef UART_H
#define	UART_H

void uart_init(void);
short uart_check_rx(void);
uint8_t uart_receive_byte(void);
void uart_send_byte(uint8_t dbyte);

#endif	/* UART_H */