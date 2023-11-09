/*
 *******************************************************************************
 * 	@file			BMP280_ll.c
 *	@brief			This file provides functions for configuration and taking
 *					measurements from the BMP280 using LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		08.11.2023
 *******************************************************************************
 */

#include "BMP280_ll.h"
#include "math.h"

BMP280_calib BMP280_calibration_data;
BMP280_settings BMP280_sensor_settings;
I2C_TypeDef *BMP280_I2C;
int32_t BMP280_t_fine;
uint32_t BMP280_refPressure;

/*
 * @brief	Calculates temparature by value from ADC.
 * @param 	adc_T Value from ADC.
 * @retval	Temperature in DegC.
 */
int32_t __BMP280_compensate_T_int32(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)BMP280_calibration_data.dig_T1<<1))) * ((int32_t)BMP280_calibration_data.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)BMP280_calibration_data.dig_T1)) * ((adc_T>>4) - ((int32_t)BMP280_calibration_data.dig_T1))) >> 12) *
			((int32_t)BMP280_calibration_data.dig_T3)) >> 14;
	BMP280_t_fine = var1 + var2;
	T = (BMP280_t_fine * 5 + 128) >> 8;
	return T;
}
/*
 * @brief	Calculates pressure by value from ADC.
 * @param	adc_P Value from ADC.
 * @retval Pressure in Pa.
 */
uint32_t __BMP280_compensate_P_int64(int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t)BMP280_t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)BMP280_calibration_data.dig_P6;
	var2 = var2 + ((var1*(int64_t)BMP280_calibration_data.dig_P5)<<17);
	var2 = var2 + (((int64_t)BMP280_calibration_data.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)BMP280_calibration_data.dig_P3)>>8) + ((var1 * (int64_t)BMP280_calibration_data.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)BMP280_calibration_data.dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)BMP280_calibration_data.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)BMP280_calibration_data.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)BMP280_calibration_data.dig_P7)<<4);
	return (uint32_t)p;
}


/*
 * @brief	Initialization of BMP280.
 * @param	hi2c_ - I2C to which BMP280 is connected.
 * @param	refPressure - Reference pressure for altitude calculations. 101325 Pa by default.
 * @retval 	status:		- HAL_OK 	Initialization complete.
 * 						- HAL_ERROR Error of initialization.
 */
HAL_StatusTypeDef BMP280_init(I2C_TypeDef *I2Cx_, uint32_t refPressure_) {
	uint8_t id;
	BMP280_I2C = I2Cx_;
	I2C_read_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_ID, &id, 1);

	if(id == 0x58) {

		BMP280_refPressure = refPressure_;
		I2C_read_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_CALIBRATION, (uint8_t *) &BMP280_calibration_data, 24);

		BMP280_config(1, 1, 0, 0);
		return HAL_OK;
	}
	else return HAL_ERROR;
}

/*
 * @brief	Sets the specified configuration to BMP.
 * @param	T_OS - value for the oversampling of data from the temperature sensor (X1 by default).
 * @param	P_OS - value for the oversampling of data from the pressure sensor (X1 by default).
 * 						@arg BMP280_OVERSAMPLING_0
 * 						@arg BMP280_OVERSAMPLING_1
 * 						@arg BMP280_OVERSAMPLING_2
 * 						@arg BMP280_OVERSAMPLING_4
 * 						@arg BMP280_OVERSAMPLING_8
 * 						@arg BMP280_OVERSAMPLING_16
 * @param	STDB - value of standby time between measurements (62.5 ms by default).
 * 						@arg BMP280_STANDBY_0_5
 * 						@arg BMP280_STANDBY_62_5
 * 						@arg BMP280_STANDBY_125
 * 						@arg BMP280_STANDBY_250
 * 						@arg BMP280_STANDBY_500
 * 						@arg BMP280_STANDBY_1000
 * 						@arg BMP280_STANDBY_2000
 * 						@arg BMP280_STANDBY_4000
* @param	IIRF - setting of the IIR filter coefficient (off by default).
 * 						@arg BMP280_FILTER_OFF
 * 						@arg BMP280_FILTER_X1
 * 						@arg BMP280_FILTER_X2
 * 						@arg BMP280_FILTER_X4
 * 						@arg BMP280_FILTER_X8
 * 						@arg BMP280_FILTER_X16
 * @retval	status:		HAL_OK -	Configuration parameters successfully sent to BMP.
 *						HAL_BUSY -	I2C line is currently busy.
 *						HAL_ERROR - I2C line returned an error.
*/
HAL_StatusTypeDef BMP280_config(uint8_t T_OS, uint8_t P_OS, uint8_t STDB, uint8_t IIRF) {
	if (T_OS < 0) T_OS = 0;
	else if (T_OS > 0b101) T_OS = 0b101;
	if (P_OS < 0) P_OS = 0;
	else if (P_OS > 0b101) P_OS = 0b101;
	if (STDB < 0) STDB = 0;
	else if (STDB > 0b111) STDB = 0b111;
	if (IIRF < 0) IIRF = 0;
	else if (IIRF > 0b100) IIRF = 0b100;

	BMP280_sensor_settings.ctrl_meas = (T_OS << 5) | (P_OS << 2);
	BMP280_sensor_settings.config = (STDB << 5) | (IIRF << 2);
	I2C_write_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_CTRL_MEAS, (uint8_t *) &BMP280_sensor_settings, 2);
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
HAL_StatusTypeDef BMP280_forced_measure(float *temp, float *press, float *h) {
	BMP280_sensor_settings.ctrl_meas = (BMP280_sensor_settings.ctrl_meas & 0xFC) | 0b10;
	I2C_write_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_CTRL_MEAS, (uint8_t*)&BMP280_sensor_settings.ctrl_meas, 1);
	uint8_t status = 1;
	while(status >> 3 != 0) {
		I2C_write_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_STATUS, &status, 1);
	}

	uint8_t raw_data[6];
	I2C_read_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_RAW_DATA, raw_data, 6);
	int32_t ADC_data[2];
	for(int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t) (((uint32_t)raw_data[3* i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}
	*temp = __BMP280_compensate_T_int32(ADC_data[1])/100.;
	*press = __BMP280_compensate_P_int64(ADC_data[0])/256.;
	*h = 29.254 * ((*temp) + 273.15) * log(BMP280_refPressure / (*press));
	return HAL_OK;
}


/*
 * @brief	Set normal mode for measuring.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BMP280_normal_measure() {
	BMP280_sensor_settings.ctrl_meas = (BMP280_sensor_settings.ctrl_meas & 0xFC) | 0b11;
	I2C__write_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_CTRL_MEAS, (uint8_t*)&BMP280_sensor_settings, 1);
	//return HAL_I2C_Mem_Write(&BMP280_hi2c, BMP280_ADDRESS, BMP280_REGISTER_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&BMP280_sensor_settings, 1, 0xFF);
}


/*
 * @brief	Set sleep mode.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BMP280_sleep() {
	BMP280_sensor_settings.ctrl_meas = (BMP280_sensor_settings.ctrl_meas & 0xFC) | 0b00;
	I2C__write_bytes(BMP280_I2C, BMP280_REGISTER_CTRL_MEAS, (uint8_t*)&BMP280_sensor_settings, 1);
}


/*
 * @brief	Get measurements from normal mode.
 * @param	*temp 	Reference to variable to store temperature value.
 * @param	*press 	Reference to variable to store pressure value.
 * @param	*h		Reference to variable to store altitude value.
 * @retval 	status:		- HAL_OK	Measurements received.
 * 						- HAL_ERROR Error in receiving measurements.
 */
HAL_StatusTypeDef BMP280_get_measure(float *temp, float *press, float *h) {
	uint8_t raw_data[6];
	I2C_read_bytes(BMP280_I2C, BMP280_ADDRESS, BMP280_REGISTER_RAW_DATA, raw_data, 6);
	int32_t ADC_data[2];
	for(int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t) (((uint32_t)raw_data[3* i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}

	*temp = __BMP280_compensate_T_int32(ADC_data[0])/100.;
	*press = __BMP280_compensate_P_int64(ADC_data[1])/256.;
	*h = 29.254 * ((*temp) + 273.15) * log(BMP280_refPressure/(*press));
	return HAL_OK;
}
