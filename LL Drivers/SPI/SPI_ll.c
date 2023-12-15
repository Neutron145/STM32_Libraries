/*
 *******************************************************************************
 * 	@file			SPI_ll.c
 *	@brief			This file provides transmitting and receiving data from SPI via LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		14.12.2023
 *******************************************************************************
 */

#include "SPI_ll.h"

/*
 * @brief Interntal function for error catching
 * @param SPIx The SPI instance to which the device is connected.
 * @param bit Bit whose value needs to check in SR register
 * @retval HAL_OK		- In 10ms SPI responded to the command
 * 		   HAL_ERROR 	- In 10ms SPI not responded to the command
 * 		   HAL_BUSY		- SPI is busy
 */
HAL_StatusTypeDef SPI__wait_flag(SPI_TypeDef *SPIx, uint8_t bit) {
	uint32_t start_wait = uwTick;
	while((SPIx->SR & bit) != bit) {
		if(HAL_GetTick() - start_wait > 10) {
			if((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY) return HAL_BUSY;
			return HAL_ERROR;
		}
	}
	return HAL_OK;
}

/*
 * @brief Transmit data to device via SPIx
 * @param SPIx The SPI instance to which the device is connected.
 * @param buffer Data to be transmitted to device
 * @param bytes_count Number of bytes of buffer array.
 * @retval HAL_OK		- Successful reading of data on SPI
 * 		   HAL_ERROR	- Error reading data on SPI
 * 		   HAL_BUSY		- SPI is busy.
 */
HAL_StatusTypeDef LL_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t* buffer, uint8_t bytes_count) {
	HAL_StatusTypeDef status = HAL_OK;
	for(int i = 0; i < bytes_count; i++) {
		if((status = SPI__wait_flag(SPIx, SPI_SR_TXE)) != HAL_OK) return status;
		LL_SPI_TransmitData8(SPIx, buffer[i]);
		if((status = SPI__wait_flag(SPIx, SPI_SR_BSY)) != HAL_OK) return status;
	}
	return status;
}

/*
 * @brief Read data from DR
 * @param SPIx The SPI instance to which the device is connected.
 * @param buffer Buffer into which data is read.
 * @param bytes_count Number of bytes to read.
 * @retval HAL_OK		- Successful reading of data on SPI
 * 		   HAL_ERROR	- Error reading data on SPI
 * 		   HAL_BUSY		- SPI is busy.
 */
HAL_StatusTypeDef LL_SPI_Receive(SPI *SPIx, uint8_t** buffer, uint8_t bytes_count) {
	HAL_StatusTypeDef status = HAL_OK;
	if((status = SPI__wait_flag(SPIx, SPI_SR_BSY)) != HAL_OK) return status;
	for(int i = 0; i < bytes_count; i++) {
		if((status = SPI__wait_flag(SPIx, SPI_ST_RXNE)) != HAL_OK) return status;
		(*buffer)[i] = LL_SPI_ReceiveData8(SPIx);
		if((status = SPI__wait_flag(SPIx, SPI_SR_BSY)) != HAL_OK) return status;
	}
	return status;
}
