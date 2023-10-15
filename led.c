#include "LPC11xx.h"                    // Device header



void led_init(void){
	LPC_GPIO3->DIR	= LPC_GPIO3->DIR | 0x3;//设置GPTO3_0用“或 ‘|’”修改末尾两位为1  32位机器，11位 位置0000000000000000000000000000011
	LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | ~0x3; //与上0x3取反 LED1和LED2都亮
}

void led1_on(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA & ~(0x1<<0); //清零 LED1亮   
}

void led1_off(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | (0x1<<0); //置1   LED1灭
}

void led2_on(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA & ~(0x1<<1); //清零 LED2亮
}

void led2_off(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | (0x1<<1); //置1   LED2灭
}
