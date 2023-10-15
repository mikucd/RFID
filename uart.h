#ifndef  __UART_H
#define  __UART_H

#include "stdio.h"

void uart_init(void);
void uart_send_byte(char data);
void uart_send_str(char str[64]);
char uart_recv_byte(void);


#endif
