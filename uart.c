#include "LPC11xx.h"                    // Device header
#include "stdio.h"

/********************
PIO1_6/RXD/CT32B0_MAT0     UART0_RXD
PIO1_7/TXD/CT32B0_MAT1     UART0_TXD
手册 10章节  UART
********************/

void uart_init(void)
{
	/****************10.2 基本配置************************/
	//在 SYSAHBCLKCTRL 寄存器，设置 第 12 位, 使能uart时钟
	LPC_SYSCON->SYSAHBCLKCTRL = LPC_SYSCON->SYSAHBCLKCTRL | (0x1<<12);
	//允许 I/O 配置模块的时钟
	LPC_SYSCON->SYSAHBCLKCTRL = LPC_SYSCON->SYSAHBCLKCTRL | (0x1<<16);
	// UART 时钟分频器寄存器 (UARTCLKDIV  1: 分频值为 1.
	LPC_SYSCON->UARTCLKDIV = 1;
	
	//在 IOCONFIG 寄存器模块 (Section 7.4) 中配置 UART 引脚 
	//修改gpio功能为 RXD
	LPC_IOCON->PIO1_6 = LPC_IOCON->PIO1_6 | (0x1<<0);
	//修改gpio功能为 TXD
	LPC_IOCON->PIO1_7 = LPC_IOCON->PIO1_7 | (0x1<<0);
	
	
	/****************10.5 uart配置************************/
	//U0LCR 决定发送和接收数据字符的格式
	LPC_UART->LCR = 0X83;
	
	//计算波特率 115200
	LPC_UART->LCR = LPC_UART->LCR | (0x1<<7); //允许除数锁存
	LPC_UART->DLM = 0;
	LPC_UART->DLL = 17;
	LPC_UART->FDR = 0xf8;  //计算波特率 115200
	//LCR 第7位 设置为0 禁止访问除数锁存
	LPC_UART->LCR = LPC_UART->LCR & ~(0x1<<7);//禁止除数锁存
	
	//UART 发送允许寄存器 (U0TER)  第7位  1
	LPC_UART->TER = LPC_UART->TER | (0x1<<7);
	
	
	/****************uart 中断配置********************/
	//10.5.4 UART 中断允许寄存器 (U0IER   第0位  运行接收中断
	LPC_UART->IER = LPC_UART->IER | (0x1<<0);
	NVIC_EnableIRQ(UART_IRQn); //使能串口中断
}


void uart_send_byte(char data)
{
	LPC_UART->THR = data;  //发送数据
	
	//U0LSR 为只读寄存器，提供 UART Tx 和 Rx 模块的状态信息。
	// 第5位  -  1 U0THR 为空
	while((LPC_UART->LSR & (0x1<<5)) == 0);  //等待发送完成
}

void uart_send_str(char str[64])
{
	int i = 0;
	while(str[i] != '\0')
	{
		uart_send_byte(str[i]);
		i++;
	}
}

char uart_recv_byte(void)
{
	char data = 0;
	
	//U0LSR 为只读寄存器，提供 UART Tx 和 Rx 模块的状态信息。
	// 0位  -  1 U0RBR 包含有效数据
	while((LPC_UART->LSR & 0x1) == 0);  //等待有数据来
	
	data = LPC_UART->RBR;  //接收数据
	
	return data;
}

//printf() -> fputc() -> uart_send_byte()
//printf 头文件 stdio.h   魔法棒图标(options) 勾选 "Use MicroLIB"
//重写 fputc 函数
int fputc(int data, FILE * stream)
{
	uart_send_byte(data);
	return data;
}

//专门的串口中断函数  函数名固定, 不得改变
void UART_IRQHandler(void)
{
	char recv_data = 0;	
	recv_data = LPC_UART->RBR;  //接收数据
	uart_send_byte(recv_data);  //回显
	
//	if(recv_data == 'a')
//		led1_on();
}




