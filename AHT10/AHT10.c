#include "AHT10.h"


I2C_TypeDef *AHT10_hi2c;

uint8_t AHT10_RxData[7] = {0,};
uint8_t AHT10_trigger_data[3] = { AHT10_COMMAND_TRIGGER, 0b00110011, 0 };


HAL_StatusTypeDef AHT10_init(I2C_HandleTypeDef *hi2c_) {
	HAL_StatusTypeDef status;

	if ((status = HAL_I2C_Master_Transmit(hi2c_, AHT10_ADDRESS, (uint8_t*)AHT10_COMMAND_INIT, 1, 50)) == HAL_OK) {
		AHT10_hi2c = hi2c_;
	}

	return status;
}


HAL_StatusTypeDef AHT10_trigger_measure() {
	return HAL_I2C_Master_Transmit(AHT10_hi2c, AHT10_ADDRESS, (uint8_t*)AHT10_trigger_data, 3, 50);
}


HAL_StatusTypeDef AHT10_get_measure(float *t, float *hum) {
	HAL_StatusTypeDef status;

	if ((status = I2C_Master_Receive(AHT10_hi2c, AHT10_ADDRESS, (uint8_t*)AHT10_RxData, 7, 50)) == HAL_OK) {
		//! 0b10000000 - код, который вернет AHT10, если он занят(еще выполняет измерения, например). 
		if (!(AHT10_RxData[0] & 0b10000000)) {
			//! Объединяем 3 последних байта в uint32_t для получения необработанных данных с АЦП о температуре
			uint32_t AHT10_ADC = (((uint32_t)AHT10_RxData[3] & 15) << 16) | ((uint32_t)AHT10_RxData[4] << 8) | AHT10_RxData[5];
			
			//! Стандартная формула преобразования попугаев в слонов
			*t = (float)(AHT10_ADC * 200. / 1048576.) - 50.;

			//! Для влажности аналогично объединяем 3 первых байта
			AHT10_ADC = ((uint32_t)AHT10_RxData[1] << 12) | ((uint32_t)AHT10_RxData[2] << 4) | (AHT10_RxData[3] >> 4);
			
			*hum = (float)(AHT10_ADC * 100. / 1048576.);
			
			return status;
		}

		return HAL_BUSY;
	}

	return status;
}


HAL_StatusTypeDef AHT10_get_T(float *t) {
	HAL_StatusTypeDef status;

	if ((status = I2C_Master_Receive(AHT10_hi2c, AHT10_ADDRESS, (uint8_t*)AHT10_RxData, 7, 50)) == HAL_OK) {
		//! Аналогичные действия из функциии AHT10_get_measure
		if (!(AHT10_RxData[0] & 0b10000000)) {
			uint32_t AHT10_ADC = (((uint32_t)AHT10_RxData[3] & 15) << 16) | ((uint32_t)AHT10_RxData[4] << 8) | AHT10_RxData[5];
			
			*t = (float)(AHT10_ADC * 200.00 / 1048576.00) - 50.;
			
			return status;
		}

		return HAL_BUSY;
	}

	return status;
}


HAL_StatusTypeDef AHT10_get_H(float *hum) {
	HAL_StatusTypeDef status;

	if ((status = HAL_I2C_Master_Receive(AHT10_hi2c, AHT10_ADDRESS, (uint8_t*)AHT10_RxData, 7, 50)) == HAL_OK) {
		//! Аналогичные действия из функциии AHT10_get_measure
		if (!(AHT10_RxData[0] & 0b10000000)) {
			uint32_t AHT10_ADC = ((uint32_t)AHT10_RxData[1] << 12) | ((uint32_t)AHT10_RxData[2] << 4) | (AHT10_RxData[3] >> 4);
			
			*hum = (float)(AHT10_ADC * 100. / 1048576.);
			
			return status;
		}

		return HAL_BUSY;
	}
	
	return status;
}


HAL_StatusTypeDef AHT10_soft_reset() {
	return I2C_Master_Transmit(AHT10_hi2c, AHT10_ADDRESS, (uint8_t*)AHT10_COMMAND_RESET, 1, 50);
}
