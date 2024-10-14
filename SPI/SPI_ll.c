#include "SPI_ll.h"


HAL_StatusTypeDef SPI__wait_flag(SPI_TypeDef *SPIx, uint8_t bit, uint8_t timeout) {
	//!< uwTick - миллисекунды, считываемые внутренним таймером МК
	uint32_t start_wait = uwTick;

	//!< Ждем пока значение бита не изменится на значение bit 
	while ((SPIx->SR & bit) != bit) {
		
		//!< Если не изменилось за timeout мс, возвращаем статус SPI
		if (HAL_GetTick() - start_wait > timeout) {

			if((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY) {
				return HAL_BUSY;
			}
			
			return HAL_ERROR;
		}
	}
	
	return HAL_OK;
}


HAL_StatusTypeDef LL_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *buffer, uint8_t bytes_count, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;

	for (int i = 0; i < bytes_count; i++) {
		
		//!< Ждем пока сдвиговый регистр освободится
		if ((status = SPI__wait_flag(SPIx, SPI_SR_TXE, timeout)) != HAL_OK) {
			return status;
		}
		
		//!< Помещаем в него данные буфера
		LL_SPI_TransmitData8(SPIx, buffer[i]);

		//!< Ждем пока передача закончится
		if ((status = SPI__wait_flag(SPIx, SPI_SR_BSY, timeout)) != HAL_OK) {
			return status;
		}
	}

	return status;
}


HAL_StatusTypeDef LL_SPI_Receive(SPI *SPIx, uint8_t **buffer, uint8_t bytes_count, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;

	//!< Проверка, занят ли SPI
	if ((status = SPI__wait_flag(SPIx, SPI_SR_BSY, timeout)) != HAL_OK) {
		return status;
	}
	
	for (int i = 0; i < bytes_count; i++) {
		
		//!<  Ждем пока данные поступят в сдвиговый регистр
		if ((status = SPI__wait_flag(SPIx, SPI_ST_RXNE, timeout)) != HAL_OK) {
			return status;
		}

		//!< Записываем данные
		(*buffer)[i] = LL_SPI_ReceiveData8(SPIx);
		
		//!< Ждем пока прием закончится
		if ((status = SPI__wait_flag(SPIx, SPI_SR_BSY, timeout)) != HAL_OK) {
			return status;
		}
	}

	return status;
}
