/*
 *******************************************************************************
 * 	@file			AHT10_hal.c
 *	@brief			This file provides functions for taking
 *					measurements from the AHT10 using LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		21.11.2023
 *******************************************************************************
 */

#include "AHT10_hal.h"

I2C_HandleTypeDef AHT10_hi2c;
uint8_t AHT10_RxData[7] = {0,};
uint8_t AHT10_trigger_data[2] = { 0b00110011, 0 };


/*
 * @brief Initialization of LSM6DS33.
 * @param hi2c_ - I2C to which AHT10 is connected.
 * @retval status:		-HAL_OK		Initialization complete.
 * 						-HAL_ERROR	Error of initialization.
 */
HAL_StatusTypeDef AHT10_init(I2C_HandleTypeDef hi2c_) {
	if(LL_I2C_Master_Transmit(&hi2c_, AHT10_ADDRESS, AHT10_COMMAND_INIT, NULL, 0) == HAL_OK) {
		AHT10_hi2c = hi2c_;
		return HAL_OK;
	}
	return HAL_ERROR;
}


/*
 * @brief Initiates the measurement of humidity and temperature.
 * @retval status:		-HAL_OK		Command has been sent successfully.
 * 						-HAL_ERROR	Error of sending command.
 */
HAL_StatusTypeDef AHT10_trigger_measure() {
	return LL_I2C_Master_Transmit(&AHT10_hi2c, AHT10_ADDRESS, AHT10_COMMAND_TRIGGER, AHT10_trigger_data, 2);
	
}


/*
 * @brief Reading measured temperature and humidity from AHT10. Delay between reading data and sending command - 75ms.
 * @param *t Variable to store temperature.
 * @param *hum Variable to store humidity.
 * @retval status:		-HAL_OK		Reading measured data complete.
 * 						-HAL_ERROR	Error of reading data.
 */
HAL_StatusTypeDef AHT10_get_measure(float *t, float *hum) {
	if(LL_I2C_Master_Receive(&AHT10_hi2c, AHT10_ADDRESS, 0, AHT10_RxData, 7) == HAL_OK) {
		if(!(AHT10_RxData[0] & 0b10000000)) {
			uint32_t AHT10_ADC = (((uint32_t)AHT10_RxData[3] & 15) << 16) | ((uint32_t)AHT10_RxData[4] << 8) | AHT10_RxData[5];
			*t = (float)(AHT10_ADC * 200. / 1048576.) - 50.;
			AHT10_ADC = ((uint32_t)AHT10_RxData[1] << 12) | ((uint32_t)AHT10_RxData[2] << 4) | (AHT10_RxData[3] >> 4);
			*hum = (float)(AHT10_ADC*100./1048576.);
			return HAL_OK;
		}
		return HAL_BUSY;
	}
	return HAL_ERROR;
}


/*
 * @brief Reading measured temperature and humidity from AHT10. Delay between reading data and sending command - 75ms.
 * @param *t Variable to store temperature.
 * @param *hum Variable to store humidity.
 * @retval status:		-HAL_OK		Reading measured data complete.
 * 						-HAL_ERROR	Error of reading data.
 */
HAL_StatusTypeDef AHT10_get_T(float *t) {
	if(LL_I2C_Master_Receive(&AHT10_hi2c, AHT10_ADDRESS, 0, AHT10_RxData, 7) == HAL_OK) {
		if(!(AHT10_RxData[0] & 0b10000000)) {
			uint32_t AHT10_ADC = (((uint32_t)AHT10_RxData[3] & 15) << 16) | ((uint32_t)AHT10_RxData[4] << 8) | AHT10_RxData[5];
			*t = (float)(AHT10_ADC * 200.00 / 1048576.00) - 50.;
			return HAL_OK;
		}
		return HAL_BUSY;
	}
	return HAL_ERROR;
}


/*
 * @brief Reading measured temperature and humidity from AHT10. Delay between reading data and sending command - 75ms.
 * @param *t Variable to store temperature.
 * @param *hum Variable to store humidity.
 * @retval status:		-HAL_OK		Reading measured data complete.
 * 						-HAL_ERROR	Error of reading data.
 */
HAL_StatusTypeDef AHT10_get_H(float *hum) {
	if(LL_I2C_Master_Receive(&AHT10_hi2c, AHT10_ADDRESS, 0, AHT10_RxData, 7) == HAL_OK) {
		if(!(AHT10_RxData[0] & 0b10000000)) {
			uint32_t AHT10_ADC = ((uint32_t)AHT10_RxData[1] << 12) | ((uint32_t)AHT10_RxData[2] << 4) | (AHT10_RxData[3] >> 4);
			*hum = (float)(AHT10_ADC*100./1048576.);
			return HAL_OK;
		}
		return HAL_BUSY;
	}
	return HAL_ERROR;
}


/*
 * @brief Soft reset of AHT10. Soft reset takes less than 20ms
 * @retval status:		-HAL_OK		Resetting complete.
 * 						-HAL_ERROR	Error of soft reset.
 */
HAL_StatusTypeDef AHT10_soft_reset() {
	if(LL_I2C_Master_Transmit(&AHT10_hi2c, AHT10_ADDRESS, AHT10_COMMAND_RESET, NULL, 0) == HAL_OK) {
		return HAL_OK;
	}
	return HAL_ERROR;
}
