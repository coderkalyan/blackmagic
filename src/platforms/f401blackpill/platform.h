/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file implements the platform specific functions for the STM32
 * implementation.
 */
#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "gpio.h"
#include "timing.h"
#include "timing_stm32.h"
#include "version.h"

#include <setjmp.h>

#define PLATFORM_HAS_TRACESWO
#define BOARD_IDENT "Black Magic Probe (F401 Blackpill), (Firmware " FIRMWARE_VERSION ")"
#define  BOARD_IDENT_DFU  "Black Magic Firmware Upgrade (F4Discovery)"
#define DFU_IDENT   "Black Magic Firmware Upgrade (F4Discovery)"

/* Important pin mappings for STM32 implementation:
 *
 * LED0 = 	PB12	(Green  LED : Running)
 * LED1 = 	PB13	(Orange LED : Idle)
 * LED2 = 	PB14	(Red LED    : Error)
 * LED3 = 	PB15	(Blue LED   : Bootloader active)
 *
 * nTRST = 	PA1
 * SRST_OUT =   PA8
 * TDI = 	PA2
 * TMS = 	PA4 (input for SWDP)
 * TCK = 	PA5/SWCLK
 * TDO = 	PA6 (input for TRACESWO
 * nSRST =
 *
 * Force DFU mode button: PA0
 */

/* Hardware definitions... */
#define JTAG_PORT 	GPIOA
#define TDI_PORT	JTAG_PORT
#define TMS_PORT	JTAG_PORT
#define TCK_PORT	JTAG_PORT
#define TDO_PORT	GPIOA
#define TDI_PIN		GPIO2
#define TMS_PIN		GPIO4
#define TCK_PIN		GPIO5
#define TDO_PIN		GPIO6

#define SWDIO_PORT 	JTAG_PORT
#define SWCLK_PORT 	JTAG_PORT
#define SWDIO_PIN	TMS_PIN
#define SWCLK_PIN	TCK_PIN

#define TRST_PORT	GPIOA
#define TRST_PIN	GPIO1
#define SRST_PORT	GPIOA
#define SRST_PIN	GPIO8

#define LED_PORT	GPIOB
#define LED_PORT_UART	GPIOB
#define LED_UART	GPIO12
#define LED_IDLE_RUN	GPIO13
#define LED_ERROR	GPIO14
#define LED_BOOTLOADER	GPIO15
#define BOOTMAGIC0 0xb007da7a
#define BOOTMAGIC1 0xbaadfeed

//#define NUM_TRACE_PACKETS (128)
//#define TRACESWO_PROTOCOL 2

#define TMS_SET_MODE() \
	gpio_mode_setup(TMS_PORT, GPIO_MODE_OUTPUT, \
	                GPIO_PUPD_NONE, TMS_PIN);
#define SWDIO_MODE_FLOAT() \
	gpio_mode_setup(SWDIO_PORT, GPIO_MODE_INPUT, \
	                GPIO_PUPD_NONE, SWDIO_PIN);

#define SWDIO_MODE_DRIVE() \
	gpio_mode_setup(SWDIO_PORT, GPIO_MODE_OUTPUT, \
	                GPIO_PUPD_NONE, SWDIO_PIN);


#define USB_DRIVER      stm32f107_usb_driver
#define USB_IRQ         NVIC_OTG_FS_IRQ
#define USB_ISR         otg_fs_isr
/* Interrupt priorities.  Low numbers are high priority.
 * For now USART1 preempts USB which may spin while buffer is drained.
 * TIM3 is used for traceswo capture and must be highest priority.
 */
#define IRQ_PRI_USB		(2 << 4)
#define IRQ_PRI_USBUSART	(1 << 4)
#define IRQ_PRI_USBUSART_TIM	(3 << 4)
#define IRQ_PRI_TRACE		(0 << 4)

//#define USBUSART USART3
//#define USBUSART_CR1 USART3_CR1
//#define USBUSART_IRQ NVIC_USART3_IRQ
//#define USBUSART_CLK RCC_USART3
//#define USBUSART_TX_PORT GPIOD
//#define USBUSART_TX_PIN  GPIO8
//#define USBUSART_RX_PORT GPIOD
//#define USBUSART_RX_PIN  GPIO9
//#define USBUSART_ISR usart3_isr
//#define USBUSART_TIM TIM4
//#define USBUSART_TIM_CLK_EN() rcc_periph_clock_enable(RCC_TIM4)
//#define USBUSART_TIM_IRQ NVIC_TIM4_IRQ
//#define USBUSART_TIM_ISR tim4_isr

#define USBUSART USART2
#define USBUSART_CR1 USART2_CR1
#define USBUSART_IRQ NVIC_USART2_IRQ
#define USBUSART_CLK RCC_USART2
#define USBUSART_TX_PORT GPIOA
#define USBUSART_TX_PIN  GPIO2
#define USBUSART_RX_PORT GPIOA
#define USBUSART_RX_PIN  GPIO3
#define USBUSART_ISR usart12isr
#define USBUSART_TIM TIM4
#define USBUSART_TIM_CLK_EN() rcc_periph_clock_enable(RCC_TIM4)
#define USBUSART_TIM_IRQ NVIC_TIM4_IRQ
#define USBUSART_TIM_ISR tim4_isr

#define UART_PIN_SETUP() do { \
	gpio_mode_setup(USBUSART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, \
	                USBUSART_TX_PIN); \
	gpio_mode_setup(USBUSART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, \
	                USBUSART_RX_PIN); \
	gpio_set_af(USBUSART_TX_PORT, GPIO_AF7, USBUSART_TX_PIN); \
	gpio_set_af(USBUSART_RX_PORT, GPIO_AF7, USBUSART_RX_PIN); \
    } while(0)

#define SWO_UART USART1
#define SWO_UART_DR USART1_DR
#define SWO_UART_CLK RCC_USART1
#define SWO_UART_PORT GPIOA
#define SWO_UART_RX_PIN GPIO10

#define TRACE_TIM TIM3
#define TRACE_TIM_CLK_EN() rcc_periph_clock_enable(RCC_TIM3)
#define TRACE_IRQ   NVIC_TIM3_IRQ
#define TRACE_ISR   tim3_isr

//#define SWO_DMA_BUS DMA1
//#define SWO_DMA_CLK RCC_DMA1
//#define SWO_DMA_CHAN DMA_CHANNEL4
//#define SWO_DMA_CHAN DMA_STREAM4
//#define SWO_DMA_IRQ NVIC_DMA1_CHANNEL4_IRQ
//#define SWO_DMA_IRQ NVIC_DMA1_STREAM4_IRQ
//#define SWO_DMA_ISR(x) dma1_channel4_isr(x)

#define gpio_set_val(port, pin, val) do {	\
	if(val)					\
		gpio_set((port), (pin));	\
	else					\
		gpio_clear((port), (pin));	\
} while(0)

#define SET_RUN_STATE(state)	{running_status = (state);}
#define SET_IDLE_STATE(state)	{gpio_set_val(LED_PORT, LED_IDLE_RUN, state);}
#define SET_ERROR_STATE(state)	{gpio_set_val(LED_PORT, LED_ERROR, state);}

static inline int platform_hwversion(void)
{
	return 0;
}

/*
 * Use newlib provided integer only stdio functions
 */

/* sscanf */
#ifdef sscanf
#undef sscanf
#define sscanf siscanf
#else
#define sscanf siscanf
#endif
/* sprintf */
#ifdef sprintf
#undef sprintf
#define sprintf siprintf
#else
#define sprintf siprintf
#endif
/* vasprintf */
#ifdef vasprintf
#undef vasprintf
#define vasprintf vasiprintf
#else
#define vasprintf vasiprintf
#endif
/* snprintf */
#ifdef snprintf
#undef snprintf
#define snprintf sniprintf
#else
#define snprintf sniprintf
#endif

#endif