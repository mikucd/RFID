#include "LPC11xx.h"                    // Device header



void led_init(void){
	LPC_GPIO3->DIR	= LPC_GPIO3->DIR | 0x3;//����GPTO3_0�á��� ��|�����޸�ĩβ��λΪ1  32λ�ƻ�����11λ λ��0000000000000000000000000000011
	LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | ~0x3; //����0x3ȡ�� LED1��LED2����
}

void led1_on(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA & ~(0x1<<0); //���� LED1��   
}

void led1_off(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | (0x1<<0); //��1   LED1��
}

void led2_on(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA & ~(0x1<<1); //���� LED2��
}

void led2_off(void){
		LPC_GPIO3->DATA	=	LPC_GPIO3->DATA | (0x1<<1); //��1   LED2��
}
