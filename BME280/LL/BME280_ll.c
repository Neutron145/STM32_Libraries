/*
 *******************************************************************************
 * 	@file			BME280_ll.c
 *	@brief			This file provides functions for configuration and taking
 *					measurements from the BME280 using LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		09.11.2023
 *******************************************************************************
 */

#include "BME280_ll.h"
#include "math.h"

BME280_calib BME280_calibration_data;
BME280_settings BME280_sensor_settings;
I2C_TypeDef *BME280_I2C;
int32_t BME280_t_fine;
uint32_t BME280_refPressure;

/*
 * @brief	Calculates temparature by value from ADC.
 * @param 	adc_T Value from ADC.
 * @retval	Temperature in DegC.
 */
int32_t __BME280_compensate_T_int32(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t)BME280_calibration_data.dig_T1 << 1))) * ((int32_t)BME280_calibration_data.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)BME280_calibration_data.dig_T1)) * ((adc_T >> 4) - ((int32_t)BME280_calibration_data.dig_T1))) >> 12) *
		((int32_t)BME280_calibration_data.dig_T3)) >> 14;
	BME280_t_fine = var1 + var2;
	T = (BME280_t_fine * 5 + 128) >> 8;
	return T;
}
/*
 * @brief	Calculates pressure by value from ADC.
 * @param	adc_P Value from ADC.
 * @retval  Pressure in Pa.
 */
uint32_t __BME280_compensate_P_int64(int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t)BME280_t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)BME280_calibration_data.dig_P6;
	var2 = var2 + ((var1 * (int64_t)BME280_calibration_data.dig_P5) << 17);
	var2 = var2 + (((int64_t)BME280_calibration_data.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)BME280_calibration_data.dig_P3) >> 8) + ((var1 * (int64_t)BME280_calibration_data.dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)BME280_calibration_data.dig_P1) >> 33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)BME280_calibration_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)BME280_calibration_data.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)BME280_calibration_data.dig_P7) << 4);
	return (uint32_t)p;
}
/*
 * @brief	Calculates humidity by value from ADC.
 * @param	hum_P Value from ADC.
 * @retval  Humidity in %.
 */
uint32_t __BME280_compensate_H_int32(int32_t adc_H)
{
	int32_t v_x1_u32r;
	v_x1_u32r = (BME280_t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)BME280_calibration_data.dig_H4) << 20) - (((int32_t)BME280_calibration_data.dig_H5) *
		v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
		((int32_t)BME280_calibration_data.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)BME280_calibration_data.dig_H3)) >> 11) +
		((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)BME280_calibration_data.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)BME280_calibration_data.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r>>12);
}

/*
 * @brief	Initialization of BME280.
 * @param	hi2c_ - I2C to which BME280 is connected.
 * @param	refPressure - Reference pressure for altitude calculations. 101325 Pa by default.
 * @retval 	status:		- HAL_OK 	Initialization complete.
 * 						- HAL_ERROR Error of initialization.
 */
HAL_StatusTypeDef BME280_init(I2C_TypeDef *I2Cx_, uint32_t refPressure_){
	uint8_t id;
	BME280_I2C = I2Cx_;
	I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_ID, &id, 1);

	if (id == 0x60) {
		BME280_refPressure = refPressure_;
		I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CALIBRATION, (uint8_t *) &BME280_calibration_data, 28);
		uint8_t H_bytes[3];
		I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CALIBRATION_H4, H_bytes, 3);
		BME280_calibration_data.dig_H4 = (H_bytes[0] << 4) | (H_bytes[1] & 0xF);
		BME280_calibration_data.dig_H5 = (H_bytes[2] << 4) | (H_bytes[1] >> 4);
		I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CALIBRATION_H6, (uint8_t*) &BME280_calibration_data.dig_H6, 1);
		BME280_config(1, 1, 1, 0, 0);
		return HAL_OK;
	}
	else return HAL_ERROR;
}

/*
 * @brief	Sets the specified configuration to BME.
 * @param	T_OS - value for the oversampling of data from the temperature sensor (X1 by default).
 * @param	P_OS - value for the oversampling of data from the pressure sensor (X1 by default).
 * @param   H_OS - value for the oversampling of data from the humidity sensor (X1 by default).
 * 						@arg BME280_OVERSAMPLING_0
 * 						@arg BME280_OVERSAMPLING_1
 * 						@arg BME280_OVERSAMPLING_2
 * 						@arg BME280_OVERSAMPLING_4
 * 						@arg BME280_OVERSAMPLING_8
 * 						@arg BME280_OVERSAMPLING_16
 * @param	STDB - value of standby time between measurements (62.5 ms by default).
 * 						@arg BME280_STANDBY_0_5
 * 						@arg BME280_STANDBY_62_5
 * 						@arg BME280_STANDBY_125
 * 						@arg BME280_STANDBY_250
 * 						@arg BME280_STANDBY_500
 * 						@arg BME280_STANDBY_1000
 * 						@arg BME280_STANDBY_2000
 * 						@arg BME280_STANDBY_4000
* @param	IIRF - setting of the IIR filter coefficient (off by default).
 * 						@arg BME280_FILTER_OFF
 * 						@arg BME280_FILTER_X1
 * 						@arg BME280_FILTER_X2
 * 						@arg BME280_FILTER_X4
 * 						@arg BME280_FILTER_X8
 * 						@arg BME280_FILTER_X16
 * @retval	status:		HAL_OK -	Configuration parameters successfully sent to BME.
 *						HAL_BUSY -	I2C line is currently busy.
 *						HAL_ERROR - I2C line returned an error.
*/
HAL_StatusTypeDef BME280_config(uint8_t T_OS, uint8_t P_OS, uint8_t H_OS, uint8_t STDB, uint8_t IIRF) {
	if (T_OS < 0) T_OS = 0;
	else if (T_OS > 0b101) T_OS = 0b101;
	if (P_OS < 0) P_OS = 0;
	else if (P_OS > 0b101) P_OS = 0b101;
	if (H_OS < 0) H_OS = 0;
	else if (H_OS > 0b101) H_OS = 0b101;
	if (STDB < 0) STDB = 0;
	else if (STDB > 0b111) STDB = 0b111;
	if (IIRF < 0) IIRF = 0;
	else if (IIRF > 0b100) IIRF = 0b100;

	BME280_sensor_settings.ctrl_meas = (T_OS << 5) | (P_OS << 2);
	BME280_sensor_settings.config = (STDB << 5) | (IIRF << 2);
	BME280_sensor_settings.ctrl_hum = H_OS;
	I2C_write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CTRL_MEAS, (uint8_t *) &BME280_sensor_settings, 2);
	I2C_write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CTRL_HUM, (uint8_t *) &BME280_sensor_settings.ctrl_hum, 1);
	return HAL_OK;
}

/*
 * @brief	Perfoms forced measurements.
 * @param	*temp 	Reference to variable to store temperature value.
 * @param	*press 	Reference to variable to store pressure value.
 * @param	*hum	Reference to varibale to store humidity value.
 * @param	*h		Reference to variable to store altitude value.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of measuring.
 */
HAL_StatusTypeDef BME280_forced_measure(float* temp, float* press, float* hum, float* h) {
	BME280_sensor_settings.ctrl_meas = (BME280_sensor_settings.ctrl_meas & 0xFC) | 0b10;
	I2C_write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CTRL_MEAS, (uint8_t*)&BME280_sensor_settings.ctrl_meas, 1);
	I2C_write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CTRL_HUM, (uint8_t*)&BME280_sensor_settings.ctrl_hum, 1);
	uint8_t status = 1;
	while (status >> 3 != 0) {
		I2C_write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_STATUS, &status, 1);
	}

	uint8_t raw_data[8];
	I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_RAW_DATA, raw_data, 8);
	int32_t ADC_data[3];
	for (int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t)(((uint32_t)raw_data[3 * i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}
	ADC_data[2] = (int32_t)(((uint16_t)raw_data[7] << 8) | ((uint16_t)raw_data[6] & 0xFF));
	*temp = __BME280_compensate_T_int32(ADC_data[1]) / 100.;
	*press = __BME280_compensate_P_int64(ADC_data[0]) / 256.;
	*hum = __BME280_compensate_H_int32(ADC_data[2]) / 1024.;
	*h = 29.254 * ((*temp) + 273.15) * log(BME280_refPressure / (*press));
	return HAL_OK;
}

/*
 * @brief	Set normal mode for measuring.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BME280_normal_measure() {
	BME280_sensor_settings.ctrl_meas = (BME280_sensor_settings.ctrl_meas & 0xFC) | 0b11;
	I2C__write_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_CTRL_MEAS, (uint8_t*)&BME280_sensor_settings, 1);
}

/*
 * @brief	Set sleep mode.
 * @retval 	status:		- HAL_OK	Configuration complete.
 * 						- HAL_ERROR Error of configuration.
 */
HAL_StatusTypeDef BME280_sleep() {
	BME280_sensor_settings.ctrl_meas = (BME280_sensor_settings.ctrl_meas & 0xFC) | 0b00;
	I2C__write_bytes(BME280_I2C, BME280_REGISTER_CTRL_MEAS, (uint8_t*)&BME280_sensor_settings, 1);
}

/*
 * @brief	Get measurements from normal mode.
 * @param	*temp 	Reference to variable to store temperature value.
 * @param	*press 	Reference to variable to store pressure value.
 * @param	*hum	Reference to varibale to store humidity value.
 * @param	*h		Reference to variable to store altitude value.
 * @retval 	status:		- HAL_OK	Measurements received.
 * 						- HAL_ERROR Error in receiving measurements.
 */
HAL_StatusTypeDef BME280_get_measure(float *temp, float *press, float *hum, float *h) {
	uint8_t raw_data[8];
	I2C_read_bytes(BME280_I2C, BME280_ADDRESS, BME280_REGISTER_RAW_DATA, raw_data, 8);
	int32_t ADC_data[3];
	for (int i = 0; i < 2; i++) {
		ADC_data[i] = (int32_t)(((uint32_t)raw_data[3 * i + 0] << 12) | ((uint32_t)raw_data[3 * i + 1] << 4) | ((uint32_t)raw_data[3 * i + 2] >> 4));
	}
	ADC_data[2] = (int32_t)(((uint16_t)raw_data[7] << 8) | ((uint16_t)raw_data[6] & 0xFF));
	*temp = __BME280_compensate_T_int32(ADC_data[1]) / 100.;
	*press = __BME280_compensate_P_int64(ADC_data[0]) / 256.;
	*hum = __BME280_compensate_H_int32(ADC_data[2]) / 1024.;
	*h = 29.254 * ((*temp) + 273.15) * log(BME280_refPressure / (*press));
	return HAL_OK;
}

