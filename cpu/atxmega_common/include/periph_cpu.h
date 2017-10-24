/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         xmega_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Josua Arndt
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/* only to satisfy system wide includes */

#define HAVE_SPI_T
#define spi_t SPI_t

#define HAVE_SPI_MODE_T
// #define spi_mode_t SPI_MODE_t

/**
 * @brief   SPI mode select macro
 *
 * The polarity is determined by bit 3 in the configuration register, the phase
 * by bit 2.
 */
#define SPI_MODE_SEL(pol, pha)          ((pol << 3) | (pha << 2))
/**
 * @brief   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = SPI_MODE_SEL(0, 0),    /**< mode 0 */
    SPI_MODE_1 = SPI_MODE_SEL(0, 1),    /**< mode 1 */
    SPI_MODE_2 = SPI_MODE_SEL(1, 0),    /**< mode 2 */
    SPI_MODE_3 = SPI_MODE_SEL(1, 1)     /**< mode 3 */
} spi_mode_t;
/** @} */

/**
 * @brief   SPI speed selection macro
 *
 * We encode the speed in bits 7, 1, and 0, where bit0 and bit1 hold the
 * prescaler bits, while bit7 holds the CLK2X bit.
 */
#define SPI_CLK_SEL(CLK2X, pr1, pr0)    ((CLK2X << 7) | (pr1 << 1) | pr0)
/**
 * @brief   Override SPI speed values
 *
 * We assume a master clock speed of 32MHz here.
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_250KHZ = SPI_CLK_SEL(0, 0, 0),      /**< 32/128 -> 250KHz */
    SPI_CLK_500KHZ = SPI_CLK_SEL(0, 0, 1),      /**< 32/64  -> 500kHz */
    SPI_CLK_2MHZ   = SPI_CLK_SEL(0, 1, 0),      /**< 32/16  -> 2MHz */
    SPI_CLK_8MHZ   = SPI_CLK_SEL(0, 1, 1),      /**< 32/4   -> 8MHz */
    SPI_CLK_16MHZ  = SPI_CLK_SEL(1, 0, 0),      /**< 32/2   -> 16MHz */
    SPI_CLK_4MHZ  = SPI_CLK_SEL(1, 0, 0),      /**< 32/8   -> 4MHz */
    SPI_CLK_1MHZ  = SPI_CLK_SEL(1, 0, 0),      /**< 32/32   -> 1MHz */
    SPI_CLK_500MHZ_CLK2X  = SPI_CLK_SEL(1, 0, 0) /**< 32/64   -> 500kHz */
} spi_clk_t;
/** @} */

#define spi_intlvl_t    SPI_INTLVL_t


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
