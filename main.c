#include "LPC11xx.h"                    // Device header
#include "led.h"
#include "rfid.h"
#include "ssp.h"
#include "uart.h"
#include "string.h"

void delay(int ms){
int i;
	while(ms--)
		for(i=0;i<4500;i++);
}
void PWMinit()
{
	LPC_SYSCON->SYSAHBCLKCTRL|=(0x01<<16);
	LPC_SYSCON->SYSAHBCLKCTRL|=(0x01<<10);
	LPC_IOCON->R_PIO1_1|=0x03;
 
	LPC_TMR32B1->PR=4800;
	LPC_TMR32B1->MR1=10;
	LPC_TMR32B1->MR0=5;
	LPC_TMR32B1->MCR|=(0x01<<4);
	LPC_TMR32B1->EMR|=(0x03<<4);
	LPC_TMR32B1->PWMC|=0x01;
}
 


//�򿪷�����
void beepstart()
{
	LPC_TMR32B1->TCR|=0x01;
}

/**
�رշ�����
**/
void beepstop()
{
	LPC_TMR32B1->TCR&=~0x01;
}

/**
//����
**/
void faninit()
{
	LPC_GPIO0->DIR|=(0x01<<2);//���
 
}
 
void fanon()
{
	LPC_GPIO0->DATA&=~(0x01<<2);//�͵�ƽ
}
 
void fanoff()
{
	LPC_GPIO0->DATA|=(0x01<<2);//�ߵ�ƽ
}
int main(){

	
	
	unsigned char buf[32];
	unsigned char wbuf[16]={0x01,0x02,0x03,0x04};
	uart_init();
  PWMinit();
	faninit();
	led_init();
	RFID_Init();
	
	while(1){
	
		//�жϿ�����
		if(RFID_State() ==1)
			{
					led1_on();
			    fanon();
			rfid_read(buf);
			if(strcmp(buf,wbuf)!=0)//������벻�� ����������
			{
				led1_off();
  		  fanoff();
				beepstart();
				delay(500);
				beepstop();	
			}
			led2_on();
   		printf("%s",buf);//�������
		}
			else{
			delay(500);
			led1_off();
			fanoff();
			led2_off();
			}
				
	}
	
}
