/*
 *******************************************************************************
 * 	@file			SPI_ll.h
 *	@brief			Header file for SPI_ll.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		14.12.2023
 *******************************************************************************
 */
#ifndef INC_SPI_LL_C_
#define INC_SPI_LL_C_

#include "main.h"

/* Internal auxiliary function for catch errors  -----------------------------*/
HAL_StatusTypeDef SPI__wait_flag(SPI_TypeDef *SPIx, uint8_t bit);

/* Send bytes to device ------------------------------------------------------*/
HAL_StatusTypeDef LL_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t* data, uint8_t bytes_count);

/* Read bytes from device ----------------------------------------------------*/
HAL_StatusTypeDef LL_SPI_Receive(SPI *SPIx, uint8_t** buffer, uint8_t bytes_count);

#endif /* INC_SPI_LL_C_ */
