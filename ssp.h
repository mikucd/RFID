/****************************************************************************
 *   $Id:: ssp.h 3635 2010-06-02 00:31:46Z usb00423                         $
 *   Project: NXP LPC11xx SSP example
 *
 *   Description:
 *     This file contains SSP code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __SSP_H__
#define __SSP_H__

#include "stdint.h"
#include "LPC11xx.h"

#define PORT0		0
#define PORT1		1
#define PORT2		2
#define PORT3		3
static LPC_GPIO_TypeDef (* const LPC_GPIO[4]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 };

/* There are there modes in SSP: loopback, master or slave. */
/* Here are the combination of all the tests. 
(1) LOOPBACK test:		LOOPBACK_MODE=1, TX_RX_ONLY=0, SSP_USE_CS=1;
(2) Serial EEPROM test:	LOOPBACK_MODE=0, TX_RX_ONLY=0, SSP_USE_CS=0; (default)
(3) TX(Master) Only:	LOOPBACK_MODE=0, SSP_SLAVE=0, TX_RX_ONLY=1, SSP_USE_CS=1;
(4) RX(Slave) Only:		LOOPBACK_MODE=0, SSP_SLAVE=1, TX_RX_ONLY=0, SSP_USE_CS=1 */

#define LOOPBACK_MODE   0		/* 1 is loopback, 0 is normal operation. */
#define SSP_SLAVE       0		/* 1 is SLAVE mode, 0 is master mode */
#define TX_RX_ONLY      0		/* 1 is TX or RX only depending on SSP_SLAVE
								flag, 0 is either loopback mode or communicate
								with a serial EEPROM. */

/* if SSP_USE_CS is zero, set SSEL as GPIO that you have total control of the sequence */
/* When test serial SEEPROM(LOOPBACK_MODE=0, TX_RX_ONLY=0), set SSP_USE_CS to 0. */
/* When LOOPBACK_MODE=1 or TX_RX_ONLY=1, set SSP_USE_CS to 1. */

#define SSP_USE_CS      0
#define SSP_DEBUG       0

/* SPI read and write buffer size */
#define SSP_BUFSIZE     16
#define SSP_FIFOSIZE    8

#define SSP_DELAY_COUNT 10
#define SSP_MAX_TIMEOUT 0xFF

/* Port0.2 is the SSP select pin */
#define SSP0_SEL        (0x1<<2)
	
/* SSP Status register */
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1) 
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3) 
#define SSPSR_BSY       (0x1<<4)

/* SSP CR0 register */
#define SSPCR0_DSS      (0x1<<0)
#define SSPCR0_FRF      (0x1<<4)
#define SSPCR0_SPO      (0x1<<6)
#define SSPCR0_SPH      (0x1<<7)
#define SSPCR0_SCR      (0x1<<8)

/* SSP CR1 register */
#define SSPCR1_LBM      (0x1<<0)
#define SSPCR1_SSE      (0x1<<1)
#define SSPCR1_MS       (0x1<<2)
#define SSPCR1_SOD      (0x1<<3)

/* SSP Interrupt Mask Set/Clear register */
#define SSPIMSC_RORIM   (0x1<<0)
#define SSPIMSC_RTIM    (0x1<<1)
#define SSPIMSC_RXIM    (0x1<<2)
#define SSPIMSC_TXIM    (0x1<<3)

/* SSP0 Interrupt Status register */
#define SSPRIS_RORRIS   (0x1<<0)
#define SSPRIS_RTRIS    (0x1<<1)
#define SSPRIS_RXRIS    (0x1<<2)
#define SSPRIS_TXRIS    (0x1<<3)

/* SSP0 Masked Interrupt register */
#define SSPMIS_RORMIS   (0x1<<0)
#define SSPMIS_RTMIS    (0x1<<1)
#define SSPMIS_RXMIS    (0x1<<2)
#define SSPMIS_TXMIS    (0x1<<3)

/* SSP0 Interrupt clear register */
#define SSPICR_RORIC    (0x1<<0)
#define SSPICR_RTIC     (0x1<<1)

/* ATMEL SEEPROM command set */
#define WREN            0x06		/* MSB A8 is set to 0, simplifying test */
#define WRDI            0x04
#define RDSR            0x05
#define WRSR            0x01
#define READ            0x03
#define WRITE           0x02

/* RDSR status bit definition */
#define RDSR_RDY        0x01
#define RDSR_WEN        0x02

void GPIOSetValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal );
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );

/* If RX_INTERRUPT is enabled, the SSP RX will be handled in the ISR
SSPReceive() will not be needed. */
void SSP0_IRQHandler (void);
void SSP1_IRQHandler (void);
void SSP_IOConfig(uint8_t portNum );
void SPI_IOConfig(uint8_t portNum);
void SSP_Init(uint8_t portNum );
void SPI_Init(uint8_t portNum, uint8_t Bit, uint8_t EvenDiv);
void SSP_Send(uint8_t portNum, uint8_t *Buf, uint32_t Length );
void SSP_Receive(uint8_t portNum, uint8_t *buf, uint32_t Length );
uint16_t SPI_PutGet(uint8_t portNum, uint16_t SendData);



#endif  /* __SSP_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

