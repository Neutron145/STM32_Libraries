#include "I2C_ll.h"


HAL_StatusTypeDef I2C__wait_flag(I2C_TypeDef *I2Cx, uint8_t bit, uint8_t timeout) {
	//!< uwTick - миллисекунды, считаемые внутренним таймером МК
	uint32_t start_wait = uwTick;
	
	//!< Ждем пока значение бита не изменится на значение bit 
	while ((I2Cx->SR1 & bit) != bit) {
		
		//!< Если не изменилось за timeout мс, возвращаем статус I2C
		if(HAL_GetTick() - start_wait > timeout) {
			if ((I2Cx->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY) {
				return HAL_BUSY;
			}
			return HAL_ERROR;
		}
	}
	
	return HAL_OK;
}


HAL_StatusTypeDef LL_I2C_Master_Receive(I2C_TypeDef *I2Cx, uint8_t device_address, uint8_t *buffer, uint8_t buffer_size, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;
	
	LL_I2C_DisableBitPOS(I2Cx);
	
	//!< Разрешаем сигнал ACK
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	
	//!< Отправляем команду отправки сигнала Старт на шину и ждем, когда сигнал Старт поступит на шину
	LL_I2C_GenerateStartCondition(I2Cx);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_SB, timeout)) != HAL_OK) {
			return status;
	}
	
	(void) I2Cx->SR1;
	
	//!< Отправляем адрес устройства на шину и команду чтения данных. Ждем, когда указанное устройство вернет сигнал ADDR
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_READ_SIGNAL);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_ADDR, timeout)) != HAL_OK) {
		return status;
	}
	
	LL_I2C_ClearFlag_ADDR(I2Cx);
	

	//Сохраняем полученные байты от устройства в буфер
	uint16_t i;
	for (i = 0; i < buffer_size - 1; i++) {
	  if ((status = I2C__wait_flag(I2Cx, I2C_SR1_RXNE, timeout)) != HAL_OK) {
	  	return status;
	  }
	  
	  buffer[i] = LL_I2C_ReceiveData8(I2Cx);
	}

	//Отправляем команду на отправку сигнала NACK и сигнала Стоп
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2Cx);
    
    //Ждем последний байт с устройства и записываем его в буфер
    if ((status = I2C__wait_flag(I2Cx, I2C_SR1_RXNE, timeout)) != HAL_OK) {
    	return status;
    }
    buffer[++i] = LL_I2C_ReceiveData8(I2Cx);
    
    return status;
}


HAL_StatusTypeDef LL_I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint8_t device_address, uint8_t *buffer, uint8_t buffer_size, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;
	
	LL_I2C_DisableBitPOS(I2Cx);
	
	//!< Разрешаем сигнал ACK
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	
	//!< Отправляем команду отправки сигнала Старт на шину и ждем, когда сигнал Старт поступит на шину
	LL_I2C_GenerateStartCondition(I2Cx);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_SB, timeout)) != HAL_OK) {
		return status;
	}
	
	(void) I2Cx->SR1;
	
	//!< Отправляем адрес устройства на шину и команду записи данных. Ждем, когда указанное устройство вернет сигнал ADDR
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_WRITE_SIGNAL);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_ADDR)) != HAL_OK) {
		return status;
	}
	
	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	
	//Отправляем байты из буфера в сдвиговый регистр и ждем, когда сдвиговый регистр освободится (передаст байты на шину для отправки устройству)
	for (uint16_t i = 0; i < buffer_size; i++) {
		LL_I2C_TransmitData8(I2Cx, buffer[i]);
		
		if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
			return status;
		}
	}

	//Отправляем сигнал Стоп
	LL_I2C_GenerateStopCondition(I2Cx);
	
	return status;
}


HAL_StatusTypeDef LL_I2C_Mem_Read(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t register_address, uint8_t *buffer, uint8_t buffer_size, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;
	
	LL_I2C_DisableBitPOS(I2Cx);
	
	//!< Разрешаем сигнал ACK
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	
	//!< Отправляем команду отправки сигнала Старт на шину и ждем, когда сигнал Старт поступит на шину
	LL_I2C_GenerateStartCondition(I2Cx);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_SB, timeout)) != HAL_OK) {
		return status;
	}
	
	(void) I2Cx->SR1;
	
	//!< Отправляем адрес устройства на шину и команду записи данных. Ждем, когда указанное устройство вернет сигнал ADDR
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_WRITE_SIGNAL);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_ADDR, timeout)) != HAL_OK) {
		return status;
	}

	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	
	//!< Отправляем на устройство адрес регистра, в который будут записываться байты. Адрес может быть однобайтовым или двухбайтовым
	if (register_address == (register_address & 0b11111111)) {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) register_address);
	}
	else {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) (register_address >> 8));
		
		if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
			return status;
		}
		
		LL_I2C_TransmitData8(I2Cx, (uint8_t) register_address);
	}

	//!< Ждем, когда сдвиговый регистр освободится (передаст байты на шину для отправки устройству)
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
		return status;
	}
	
	//!< Отправляем команду на отправку сигнала Рестарт и ждем, когда сигнал Рестарт поступит на шину
	LL_I2C_GenerateStartCondition(I2Cx);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_SB, timeout)) != HAL_OK) { 
		return status;
	}

	(void) I2Cx->SR1;
	
	//!< Отправляем адрес устройства, с которого будем считывать данные и команду на чтение данных
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_READ_SIGNAL);
	
	//Ждем отправки сигнала ADDR от устройства 
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_ADDR, timeout)) != HAL_OK) {
		return status;
	}
	
	LL_I2C_ClearFlag_ADDR(I2Cx);
	//!< Ждем, когда в сдвиговом регистре появятся байты и сохраняем их в буфер 
	uint16_t i;
	for (i = 0; i < buffer_size - 1; i++) {
	  if ((status = I2C__wait_flag(I2Cx, I2C_SR1_RXNE, timeout)) != HAL_OK) {
	  	return status;
	  }
	  
	  buffer[i] = LL_I2C_ReceiveData8(I2Cx);
	}

	//!< Отправляем сигнал NACK и сигнал Стоп. Ждем поялвения сигнала Стоп на шине
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2Cx);
    if ((status = I2C__wait_flag(I2Cx, I2C_SR1_RXNE, timeout)) != HAL_OK) {
    	return status;
    }

    //!< Считываем последний байт в буфер
    buffer[++i] = LL_I2C_ReceiveData8(I2Cx);
    
    return status;
}


HAL_StatusTypeDef LL_I2C_Mem_Write(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t register_address, uint8_t *buffer, uint8_t buffer_size, uint8_t timeout) {
	HAL_StatusTypeDef status = HAL_OK;
	
	LL_I2C_DisableBitPOS(I2Cx);
	
	//!< Разрешаем сигнал ACK
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	
	//!< Отправляем команду отправки сигнала Старт на шину и ждем, когда сигнал Старт поступит на шину
	LL_I2C_GenerateStartCondition(I2Cx);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_SB, timeout)) != HAL_OK) {
		return status;
	}
	
	(void) I2Cx->SR1;
	
	//!< Отправляем адрес устройства на шину и команду записи данных. Ждем, когда указанное устройство вернет сигнал ADDR
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_WRITE_SIGNAL);
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_ADDR, timeout)) != HAL_OK) {
		return status;
	}
	
	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	
	//!< Отправляем на устройство адрес регистра, в который будут записываться байты. Адрес может быть однобайтовым или двухбайтовым
	if (register_address == (register_address & 0b11111111)) {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) register_address);
	}
	else {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) (register_address >> 8));
		
		if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
			return status;
		}
		
		LL_I2C_TransmitData8(I2Cx, (uint8_t) register_address);
	}

	//!< Ждем, когда сдвиговый регистр освободится (передаст байты на шину для отправки устройству)
	if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
		return status;
	}

	//!< Отправляем байты из буфера в сдвиговый регистр для отправки их на шину I2C и ждем, когда сдвиговый регистр освободится
	for(uint16_t i = 0; i < buffer_size; i++) {
		LL_I2C_TransmitData8(I2Cx, buffer[i]);

		if ((status = I2C__wait_flag(I2Cx, I2C_SR1_TXE, timeout)) != HAL_OK) {
			return status;
		}
	}

	//Отправляем сигнал Стоп
	LL_I2C_GenerateStopCondition(I2Cx);
	
	return status;
}