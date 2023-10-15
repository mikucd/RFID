#include "LPC11xx.h"                    // Device header
#include "stdio.h"

/********************
PIO1_6/RXD/CT32B0_MAT0     UART0_RXD
PIO1_7/TXD/CT32B0_MAT1     UART0_TXD
�ֲ� 10�½�  UART
********************/

void uart_init(void)
{
	/****************10.2 ��������************************/
	//�� SYSAHBCLKCTRL �Ĵ��������� �� 12 λ, ʹ��uartʱ��
	LPC_SYSCON->SYSAHBCLKCTRL = LPC_SYSCON->SYSAHBCLKCTRL | (0x1<<12);
	//���� I/O ����ģ���ʱ��
	LPC_SYSCON->SYSAHBCLKCTRL = LPC_SYSCON->SYSAHBCLKCTRL | (0x1<<16);
	// UART ʱ�ӷ�Ƶ���Ĵ��� (UARTCLKDIV  1: ��ƵֵΪ 1.
	LPC_SYSCON->UARTCLKDIV = 1;
	
	//�� IOCONFIG �Ĵ���ģ�� (Section 7.4) ������ UART ���� 
	//�޸�gpio����Ϊ RXD
	LPC_IOCON->PIO1_6 = LPC_IOCON->PIO1_6 | (0x1<<0);
	//�޸�gpio����Ϊ TXD
	LPC_IOCON->PIO1_7 = LPC_IOCON->PIO1_7 | (0x1<<0);
	
	
	/****************10.5 uart����************************/
	//U0LCR �������ͺͽ��������ַ��ĸ�ʽ
	LPC_UART->LCR = 0X83;
	
	//���㲨���� 115200
	LPC_UART->LCR = LPC_UART->LCR | (0x1<<7); //�����������
	LPC_UART->DLM = 0;
	LPC_UART->DLL = 17;
	LPC_UART->FDR = 0xf8;  //���㲨���� 115200
	//LCR ��7λ ����Ϊ0 ��ֹ���ʳ�������
	LPC_UART->LCR = LPC_UART->LCR & ~(0x1<<7);//��ֹ��������
	
	//UART ��������Ĵ��� (U0TER)  ��7λ  1
	LPC_UART->TER = LPC_UART->TER | (0x1<<7);
	
	
	/****************uart �ж�����********************/
	//10.5.4 UART �ж�����Ĵ��� (U0IER   ��0λ  ���н����ж�
	LPC_UART->IER = LPC_UART->IER | (0x1<<0);
	NVIC_EnableIRQ(UART_IRQn); //ʹ�ܴ����ж�
}


void uart_send_byte(char data)
{
	LPC_UART->THR = data;  //��������
	
	//U0LSR Ϊֻ���Ĵ������ṩ UART Tx �� Rx ģ���״̬��Ϣ��
	// ��5λ  -  1 U0THR Ϊ��
	while((LPC_UART->LSR & (0x1<<5)) == 0);  //�ȴ��������
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
	
	//U0LSR Ϊֻ���Ĵ������ṩ UART Tx �� Rx ģ���״̬��Ϣ��
	// 0λ  -  1 U0RBR ������Ч����
	while((LPC_UART->LSR & 0x1) == 0);  //�ȴ���������
	
	data = LPC_UART->RBR;  //��������
	
	return data;
}

//printf() -> fputc() -> uart_send_byte()
//printf ͷ�ļ� stdio.h   ħ����ͼ��(options) ��ѡ "Use MicroLIB"
//��д fputc ����
int fputc(int data, FILE * stream)
{
	uart_send_byte(data);
	return data;
}

//ר�ŵĴ����жϺ���  �������̶�, ���øı�
void UART_IRQHandler(void)
{
	char recv_data = 0;	
	recv_data = LPC_UART->RBR;  //��������
	uart_send_byte(recv_data);  //����
	
//	if(recv_data == 'a')
//		led1_on();
}




