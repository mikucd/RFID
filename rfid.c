/*******************************************************************************
* File:    rfid.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   RFID program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "LPC11xx.h"
#include "string.h"
#include "ssp.h"
#include "rfid.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t RFID_READ_MOD_TYPE_01[2] = {0x02, 0x01};
const uint8_t RFID_READ_TYPE_19[2] = {0x02, 0x19};
const uint8_t RFID_READ_CARD_20[2] = {0x02, 0x20};
//uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
//uint8_t RFID_WRITE_DATA_BLOCK_22_0[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//                                          0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
//										    0x38, 0x39, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00}; 
//uint8_t RFID_WRITE_DATA_BLOCK_22_1[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//                                          0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
//										    0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50}; 

uint8_t  RFID_READ_DATA_BLOCK[10]  = {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t RFID_WRITE_DATA_BLOCK[10] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 


/*******************************************************************************
* Function Name  : delay_ms.
* Description    : ms delay.
* Input          : - ms: number of ms
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(uint32_t ms)
{
#if 1
	int i = 0;
	while(ms--)
		for(i=0; i<4500; i++);
	
#else 
	  uint32_t	tim_now, tim_max, tim_cnt, reload_numb, tim_remain;
	  uint32_t	i;

	  tim_now = SysTick->VAL;
	  tim_max = SysTick->LOAD;  
	  tim_cnt = 48000 * ms;
	  reload_numb = tim_cnt / tim_max;
	  tim_remain = tim_cnt % tim_max;

	  
	  if(reload_numb > 0)
	  {
		for(i=0; i<reload_numb; i++)
		{
		  while(SysTick->VAL < tim_now);
		  while(SysTick->VAL > tim_now);
		}
	  }
	  
	  if(tim_remain)
	  {
		if(tim_remain < tim_now)
		{
		  while(SysTick->VAL > (tim_now-tim_remain));
		}
		else if(tim_remain == tim_now)
		{
		  while(SysTick->VAL < tim_now);
		}
		else
		{
		  while(SysTick->VAL < tim_now);
		  while(SysTick->VAL > (tim_max-(tim_remain-tim_now)));
		}
	  }   
#endif
} 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : RFID_CheckSum
* Description    : Init RFID control.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t RFID_CheckSum(uint8_t *databuf)
{ 
	unsigned char	numb, chksum=0;;

 	numb = databuf[0]-1;

	for(numb=0;numb<databuf[0];numb++)
	{
    	chksum += databuf[numb]; 
	} 
  	return chksum;
}
/*****************************************************************************
** Function name:		SSP_Send
**
** Descriptions:		Send a block of data to the SSP port, the 
**						first parameter is the buffer pointer, the 2nd 
**						parameter is the block length.
**
** parameters:			port #, buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/

static unsigned short SPI0_Send( unsigned char  portNum, unsigned char  buf )
{
	if ( portNum == 0 )
	{
	  GPIOSetValue( PORT2, 7, 0 ); 
	  while (( !(LPC_SSP0->SR & SSPSR_TNF)||(LPC_SSP0->SR & SSPSR_BSY)) != 0 );
	  LPC_SSP0->DR = buf;
	  while ( LPC_SSP0->SR & SSPSR_BSY );
	      /* Wait until the Busy bit is cleared */
      while((LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE);
	  GPIOSetValue( PORT2, 7, 1);
    }
    return	  LPC_SSP0->DR;
}

/*******************************************************************************
* Function Name  : RFID_Init
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void RFID_Init(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL = LPC_SYSCON->SYSAHBCLKCTRL  | (0x1 << 16);
	
	SPI_IOConfig(0);  //SPI 0 配置相关信息
	SPI_Init(0, 8, 2);
	
	delay_ms(10);  
	GPIOSetDir(PORT2, 8, 0);	  //拉低片选，使能芯片	LPC_GPIO2->DIR &= ~(1 << 8);
}

/*******************************************************************************
* Function Name  : RFID_State
* Description    : 寻卡
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
int RFID_State(void)
{
	static unsigned int pre = 1, cur = 1;
	
	cur = (LPC_GPIO2->DATA >> 8) & 1;
	if (cur != pre && pre == 1) {   //卡存在
		pre = cur;
		return 1;
	} else {
		pre = cur;
		return 0;
	}
}


/*******************************************************************************
* Function Name  : RFID_Operate
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t RFID_Operate(uint8_t *tbuf, uint8_t *rbuf)
{
  uint8_t	chksum;
  uint32_t 	i, j, rnumb;

  SPI_Init(0, 8, 128);

  chksum = RFID_CheckSum(tbuf);

  SPI0_Send(0, 0xaa);
  //delay_ms(1);

  i = 0;
  rbuf[i] = SPI0_Send(0, 0xbb);
  i ++;
  //delay_ms(1);

  for(j=0; j<tbuf[0]; j++)
  {
    rbuf[i] = SPI0_Send(0, tbuf[j]);
	i ++;
    delay_ms(1);
  }

  rbuf[i] = SPI0_Send(0, chksum);
  i ++;

   delay_ms(200);

  rbuf[0] = 1;
 
  if(SPI0_Send(0, 0) != 0xaa)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }

  delay_ms(1);
  
  if(SPI0_Send(0, 0) != 0xbb)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }
	
  delay_ms(1);
	 
  switch(tbuf[1])
  {
    case 0x01:
	  rnumb = 8 + 2 + 1;
	  break;

	case 0x20:
	  rnumb = 4 + 2 + 1;
	  break;

	case 0x21:
	  rnumb = 16 + 2 + 1;
	  break;

	case 0x22:
	  rnumb = 2 + 1;
	  break;

	default:
	  rnumb = 4 + 2 + 1;
	  break;
  }

  for(j=0, i=0; j<=rnumb; j++, i++)
  {
    rbuf[i] = SPI0_Send(0, 0);
	delay_ms(1);
  }		
  SPI_Init(0, 8, 2);

  return i;
}

void rfid_delay(unsigned int loop)
{
	int i = 0;
	while(loop--)
		for(i=0; i<1000; i++);
}

int rfid_read(unsigned char *rbuf)
{
	int i;
	unsigned char buf[32];
	uint8_t chksum = RFID_CheckSum(RFID_READ_DATA_BLOCK);
	
	SPI0_Send(0, 0xaa);
	rfid_delay(10);
	SPI0_Send(0, 0xbb);
	rfid_delay(10);
	for (i = 0; i < RFID_READ_DATA_BLOCK[0]; i++) 
	{
		SPI0_Send(0, RFID_READ_DATA_BLOCK[i]);
		rfid_delay(10);
	}
	SPI0_Send(0, chksum);
	rfid_delay(500);
	
	SPI0_Send(0, 0xaa);
	rfid_delay(10);
	SPI0_Send(0, 0xbb);
	rfid_delay(10);
	for (i = 0; i < 19; i++) 
	{
		buf[i] = SPI0_Send(0, 0);
		rfid_delay(10);
	}
	chksum = RFID_CheckSum(buf);
	if (chksum != buf[buf[0]])
		return -1;
	else {
		memcpy(rbuf, buf + 2, 16);
		return 0;
	}
}


int rfid_write(unsigned char *wbuf)
{
	int i;
	unsigned char chksum;
	unsigned char buf[32];

	memcpy(buf, RFID_WRITE_DATA_BLOCK, 10);
	memcpy(buf + 10, wbuf, 16);
	chksum = RFID_CheckSum(buf);
	SPI0_Send(0, 0xAA);//01100110
	rfid_delay(10);
	SPI0_Send(0, 0xBB);//01110111
	rfid_delay(10);
	for (i = 0; i < buf[0]; i++) {
		SPI0_Send(0, buf[i]);
		rfid_delay(10);
	}
	SPI0_Send(0, chksum);
	rfid_delay(500);
	SPI0_Send(0, 0xAA);
	rfid_delay(10);
	SPI0_Send(0, 0xBB);
	rfid_delay(10);
	
	for (i = 0; i < 3; i++) {
		buf[i] = SPI0_Send(0, 0);
		rfid_delay(10);
	}
	
	if (buf[0] != 3 || buf[1] != 0x22)
		return -1;
	else
		return 0;
}


/**** End of File ****/


