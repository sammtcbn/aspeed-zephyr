/*
 * Copyright (c) 2023 Aspeed Technology Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_RESET_AST27XX_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_RESET_AST27XX_H_

/* CPU Die */
#define ASPEED_RESET_SDRAM		(0)
#define ASPEED_RESET_DDRPHY		(1)
#define ASPEED_RESET_RSA		(2)
#define ASPEED_RESET_SHA3		(3)
#define ASPEED_RESET_HACE		(4)
#define ASPEED_RESET_SOC		(5)
#define ASPEED_RESET_VIDEO		(6)
#define ASPEED_RESET_2D			(7)
#define ASPEED_RESET_PCIS		(8)
#define ASPEED_RESET_RVAS0		(9)
#define ASPEED_RESET_RVAS1		(10)
#define ASPEED_RESET_SM3		(11)
#define ASPEED_RESET_SM4		(12)
#define ASPEED_RESET_CRT0		(13)
#define ASPEED_RESET_CRT1		(14)
#define ASPEED_RESET_DP_PCI		(15)
#define ASPEED_RESET_UFS		(16)
#define ASPEED_RESET_EMMC		(17)
#define ASPEED_RESET_RCRST		(18)
#define ASPEED_RESET_RCRSTOE		(19)
#define ASPEED_RESET_PERST		(20)
#define ASPEED_RESET_PERSTOE		(21)
#define ASPEED_RESET_JTAG0		(22)
#define ASPEED_RESET_MCTP0		(23)
#define ASPEED_RESET_MCTP1		(24)
#define ASPEED_RESET_XDMA0		(25)
#define ASPEED_RESET_XDMA1		(26)
#define ASPEED_RESET_H2X1		(27)
#define ASPEED_RESET_DP			(28)
#define ASPEED_RESET_DP_MCU		(29)
#define ASPEED_RESET_GP_MCU		(30)
#define ASPEED_RESET_H2X		(31)
#define ASPEED_RESET_P0_VHUB2		(32)
#define ASPEED_RESET_P0_VHUB3		(33)
#define ASPEED_RESET_P0_XHCI		(34)
#define ASPEED_RESET_P1_VHUB2		(35)
#define ASPEED_RESET_P1_VHUB3		(36)
#define ASPEED_RESET_P1_XHCI		(37)
#define ASPEED_RESET_P1_USB2		(38)
#define ASPEED_RESET_P0_EHCI		(39)
#define ASPEED_RESET_USB11		(40)

/* IO Die */
#define ASPEED_RESET_LPC0		(0)
#define ASPEED_RESET_LPC1		(1)
#define ASPEED_RESET_MII		(2)
#define ASPEED_RESET_PECI		(3)
#define ASPEED_RESET_PWM		(4)
#define ASPEED_RESET_MAC0		(5)
#define ASPEED_RESET_MAC1		(6)
#define ASPEED_RESET_MAC2		(7)
#define ASPEED_RESET_ADC		(8)
#define ASPEED_RESET_SD			(9)
#define ASPEED_RESET_ESPI0		(10)
#define ASPEED_RESET_ESPI1		(11)
#define ASPEED_RESET_JTAG1		(12)
#define ASPEED_RESET_SPI0		(13)
#define ASPEED_RESET_SPI1		(14)
#define ASPEED_RESET_SPI2		(15)
#define ASPEED_RESET_I3C0		(16)
#define ASPEED_RESET_I3C1		(17)
#define ASPEED_RESET_I3C2		(18)
#define ASPEED_RESET_I3C3		(19)
#define ASPEED_RESET_I3C4		(20)
#define ASPEED_RESET_I3C5		(21)
#define ASPEED_RESET_I3C6		(22)
#define ASPEED_RESET_I3C7		(23)
#define ASPEED_RESET_I3C8		(24)
#define ASPEED_RESET_I3C9		(25)
#define ASPEED_RESET_I3C10		(26)
#define ASPEED_RESET_I3C11		(27)
#define ASPEED_RESET_I3C12		(28)
#define ASPEED_RESET_I3C13		(29)
#define ASPEED_RESET_I3C14		(30)
#define ASPEED_RESET_I3C15		(31)
/* reserved 32 */
#define ASPEED_RESET_IOMCU		(33)
#define ASPEED_RESET_H2A_SPI1		(34)
#define ASPEED_RESET_H2A_SPI2		(35)
#define ASPEED_RESET_UART0		(36)
#define ASPEED_RESET_UART1		(37)
#define ASPEED_RESET_UART2		(38)
#define ASPEED_RESET_UART3		(39)
#define ASPEED_RESET_I2C_FILTER		(40)
#define ASPEED_RESET_CALIPTRA		(41)
/* reserved 42:43 */
#define ASPEED_RESET_FSI		(44)
#define ASPEED_RESET_CAN		(45)
#define ASPEED_RESET_MCTP		(46)
#define ASPEED_RESET_I2C		(47)
#define ASPEED_RESET_UART5		(48)
#define ASPEED_RESET_UART6		(49)
#define ASPEED_RESET_UART7		(50)
#define ASPEED_RESET_UART8		(51)
#define ASPEED_RESET_LTPI		(52)
#define ASPEED_RESET_VGAL		(53)
/* reserved 54:62 */
#define ASPEED_RESET_I3CDMA             (63)

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_RESET_AST27XX_H_ */
