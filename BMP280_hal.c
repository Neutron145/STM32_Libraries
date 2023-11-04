/*
 *******************************************************************************
 * 	@file			BMP280_hal.h
 *	@brief			This file provides functions for configuration and taking.
 *					measurements from the BMP280 using HAL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		03.11.2023
 *******************************************************************************
 */

#include "BMP280_hal.h"
#include "math.h"

BMP280_calib calibration_data;
I2C_HandleTypeDef hi2c;
int32_t t_fine;
uint8_t ctrl_meas = 0b00100100;
uint8_t config = 0b000000000;

/*
 * @brief	Calculates temparature by value from ADC.
 * @param 	adc_T Value from ADC.
 * @retval	Temperature in DegC.
 */
int32_t __BMP280_compensate_T_int32(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)calibration_data.dig_T1<<1))) * ((int32_t)calibration_data.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)calibration_data.dig_T1)) * ((adc_T>>4) - ((int32_t)calibration_data.dig_T1))) >> 12) *
			((int32_t)calibration_data.dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
/*
 * @brief	Calculates pressure by value from ADC.
 * @param	adc_P Value from ADC.
 * @retval Pressure in Pa.
 */
uint32_t __BMP280_compensate_P_int64(int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)calibration_data.dig_P6;
	var2 = var2 + ((var1*(int64_t)calibration_data.dig_P5)<<17);
	var2 = var2 + (((int64_t)calibration_data.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)calibration_data.dig_P3)>>8) + ((var1 * (int64_t)calibration_data.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calibration_data.dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)calibration_data.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)calibration_data.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)calibration_data.dig_P7)<<4);
	return (uint32_t)p;
}


/*
 * @brief	Initialization of BMP280.
 * @param	hi2c_ I2C to which BMP280 is connected.
 * @retval 	status:		- HAL_OK 	Initialization complete
 * 						- HAL_ERROR Error of initialization
 */
HAL_StatusTypeDef BMP280_init(I2C_HandleTypeDef hi2c_) {
	uint8_t id;
	HAL_I2C_Mem_Read(&hi2c_, BMP280_ADDRESS, BMP280_ID_REGISTER, I2C_MEMADD_SIZE_8BIT, &id, 1, 0xFF);
	if(id == 0x60) {
		hi2c = hi2c_;
		HAL_I2C_Mem_Read(&hi2c, BMP280_ADDRESS, BMP280_CALIBRATION_REGISTER, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &calibration_data, 24, 0xFF);
		return HAL_OK;
	}
	else return HAL_ERROR;
}


/*
 * @brief	Set oversampling value for temperature measuring.
 * @param	oversampling Value of oversampling for temperature.
 * 			This parameter can be one of the following values:
 * 						@arg BMP280_OVERSAMPLING_0
 * 						@arg BMP280_OVERSAMPLING_1
 * 						@arg BMP280_OVERSAMPLING_2
 * 						@arg BMP280_OVERSAMPLING_4
 * 						@arg BMP280_OVERSAMPLING_8
 * 						@arg BMP280_OVERSAMPLING_16
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Incorrect value.
 */
HAL_StatusTypeDef BMP280_set_T_oversampling(uint8_t oversampling){
	if(oversampling > 5) {
		return HAL_ERROR;
	}
	ctrl_meas = (ctrl_meas & 0x1F) | (oversampling << 5);
	return HAL_OK;
}


/*
 * @brief	Set oversampling value for pressure measuring.
 * @param	oversampling Value of oversampling for pressure.
 * 			This parameter can be one of the following values:
 * 						@arg BMP280_OVERSAMPLING_0
 * 						@arg BMP280_OVERSAMPLING_1
 * 						@arg BMP280_OVERSAMPLING_2
 * 						@arg BMP280_OVERSAMPLING_4
 * 						@arg BMP280_OVERSAMPLING_8
 * 						@arg BMP280_OVERSAMPLING_16
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Incorrect value.
 */
HAL_StatusTypeDef BMP280_set_P_oversampling(uint8_t oversampling){
	if(oversampling > 5) {
		return HAL_ERROR;
	}
	ctrl_meas = (ctrl_meas & 0xE3) | (oversampling << 2);
	return HAL_OK;
}


/*
 * @brief	Set periods between measuring in normal mode.
 * @param	t Time in ms between measuring.
 * 			This parameter can be one of the following values:
 * 						@arg BMP280_STANDBY_0_5
 * 						@arg BMP280_STANDBY_62_5
 * 						@arg BMP280_STANDBY_125
 * 						@arg BMP280_STANDBY_250
 * 						@arg BMP280_STANDBY_500
 * 						@arg BMP280_STANDBY_1000
 * 						@arg BMP280_STANDBY_2000
 * 						@arg BMP280_STANDBY_4000
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Incorrect value.
 */
HAL_StatusTypeDef BMP280_set_T_standby(uint8_t t) {
	if(t > 7) {
		return HAL_ERROR;
	}
	config = (config & 0x1F) | (t << 5);
	return HAL_OK;
}


/*
 * @brief	Set filter for processing measurements.
 * @param	filter Coefficient of filter.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Incorrect value.
 */
HAL_StatusTypeDef BMP280_set_IRR_filter(uint8_t filter) {
	if(filter > 8) {
		return HAL_ERROR;
	}
	config = (config & 0xE3) | (filter << 2);
	return HAL_OK;
}


/*
 * @brief	Perfoms forced measurements.
 * @param	*temp 	Reference to variable to store temperature value.
 * @param	*press 	Reference to variable to store pressure value.
 * @param	*h		Reference to variable to store altitude value.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of measuring.
 */
HAL_StatusTypeDef BMP280_forced_measure(double *temp, double *press, double *h) {
	ctrl_meas = (ctrl_meas & 0xFC) | 0b10;
	if(HAL_I2C_Mem_Write(&hi2c, BMP280_ADDRESS, BMP280_CTRL_MEAS_REGISTER, I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1, 0xFF) != HAL_OK) {
		return HAL_ERROR;
	}
	uint8_t status = 1;
	while(status >> 3 != 0) {
		if(HAL_I2C_Mem_Read(&hi2c, BMP280_ADDRESS, BMP280_STATUS_REGISTER, I2C_MEMADD_SIZE_8BIT, &status, 1, 0xFF) != HAL_OK){
			return HAL_ERROR;
		}
	}

	uint8_t raw_data[6];

	if(HAL_I2C_Mem_Read(&hi2c, BMP280_ADDRESS, BMP280_RAW_DATA_REGISTER, I2C_MEMADD_SIZE_8BIT, raw_data, 6, 0xFF) != HAL_OK) {
		return HAL_ERROR;
	}
	int32_t ADC_data[2];
	for(int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t) (((uint32_t)raw_data[3* i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}
	*temp = __BMP280_compensate_T_int32(ADC_data[0])/100.;
	*press = __BMP280_compensate_P_int64(ADC_data[1])/256.;
	*h = ((8.314 * (*temp))/(0.029 * 9.8)) * log(101325/(*press));
	return HAL_OK;
}


/*
 * @brief	Set normal mode for measuring.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BMP280_normal_measure() {
	ctrl_meas = (ctrl_meas & 0xFC) | 0b11;
	if(HAL_I2C_Mem_Write(&hi2c, BMP280_ADDRESS, BMP280_CTRL_MEAS_REGISTER, I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1, 0xFF)!= HAL_OK){
		return HAL_ERROR;
	}
	return HAL_OK;
}


/*
 * @brief	Set sleep mode.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BMP280_sleep() {
	ctrl_meas = (ctrl_meas & 0xFC) | 0b00;
	if(HAL_I2C_Mem_Write(&hi2c, BMP280_ADDRESS, BMP280_CTRL_MEAS_REGISTER, I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1, 0xFF)!= HAL_OK){
		return HAL_ERROR;
	}
	return HAL_OK;
}


/*
 * @brief	Get measurements from normal mode.
 * @param	*temp 	Reference to variable to store temperature value.
 * @param	*press 	Reference to variable to store pressure value.
 * @param	*h		Reference to variable to store altitude value.
 * @retval 	status:		- HAL_OK	Measurements received.
 * 						- HAL_ERROR Error in receiving measurements.
 */
HAL_StatusTypeDef BMP280_get_measure(double *temp, double *press, double *h) {
	uint8_t raw_data[6];
	if(HAL_I2C_Mem_Read(&hi2c, BMP280_ADDRESS, BMP280_RAW_DATA_REGISTER, I2C_MEMADD_SIZE_8BIT, raw_data, 6, 0xFF)!= HAL_OK) {
		return HAL_ERROR;
	}
	int32_t ADC_data[2];
	for(int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t) (((uint32_t)raw_data[3* i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}

	*temp = __BMP280_compensate_T_int32(ADC_data[0])/100.;
	*press = __BMP280_compensate_P_int64(ADC_data[1])/256.;
	*h = ((8.314 * (*temp))/(0.029 * 9.8)) * log(101325/(*press));
	return HAL_OK;
}
