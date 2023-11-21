/*
 *******************************************************************************
 * 	@file			LSM6DS33_ll.c
 *	@brief			This file provides functions for configuration and taking
 *					measurements from the LSM6DS33 using LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		15.11.2023
 *******************************************************************************
 */

#include "LSM6DS33_hal.h"
#include "I2C_ll.h"

LSM6DS33_cfg LSM6DS33_config;
I2C_HandleTypeDef LSM6DS33_hi2c;
uint16_t FULL_SCALES_A[4] = {2, 16, 4, 8};
uint16_t FULL_SCALES_G[4] = {250, 500, 1000, 2000};
/*
 * @brief Auxiliary function for modifying registers.
 */
void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits) {
	*reg_data &= ~mask;
	*reg_data |= bits;
}

/*
 * @brief Initialization of LSM6DS33.
 * @param hi2c_ - I2C to which LSM6DS33 is connected.
 * @retval status:		-HAL_OK		Initialization complete.
 * 						-HAL_ERROR	Error of initialization.
 */
HAL_StatusTypeDef LSM6DS33_init(I2C_HandleTypeDef hi2c_) {
	uint8_t id;
	I2C_read_bytes(&hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_ID, &id, 1);

	if(id == 0x58) {
		LSM6DS33_hi2c = hi2c_;
		LSM6DS33_config.ORIENT_config = 0b0;
		LSM6DS33_config.CTRL1_config = 0b0;
		LSM6DS33_config.CTRL2_config = 0b0;
		LSM6DS33_config.CTRL3_config = 0b01000100;
		LSM6DS33_config.CTRL7_config = 0b0;
		LSM6DS33_config.CTRL8_config = 0b0;
		I2C_write_bytes(&hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1);
		return HAL_OK;
	}
	return HAL_ERROR;
}

/*
 * @brief Configuration sensor coordinate system.
 * @param	orient - orientation XYZ
 * 					@arg LSM6DS33_ORIENT_CFG_XYZ
 * 					@arg LSM6DS33_ORIENT_CFG_XZY
 * 					@arg LSM6DS33_ORIENT_CFG_YXZ
 * 					@arg LSM6DS33_ORIENT_CFG_YZX
 * 					@arg LSM6DS33_ORIENT_CFG_ZXY
 * 					@arg LSM6DS33_ORIENT_CFG_ZYX
 * @param 	signs for X, Y and Z axes
 * 					@example LSM6DS33_ORIENT_SIGN_POSITIVE_X | LSM6DS33_ORIENT_SIGN_POSITIVE_Y | LSM6DS33_ORIENT_SIGN_NEGATIVE_Z
 * @retval status:		-HAL_OK		Configuration complete.
 * 						-HAL_ERROR	Error of configuration or wrong parameters.
 */
HAL_StatusTypeDef LSM6DS33_config_orientation(uint8_t orient, uint8_t signs) {
	if(signs > 0b111) return HAL_ERROR;
	__LSM6DS33_modify_reg(&LSM6DS33_config.ORIENT_config, LSM6DS33_ORIENT_CFG_MASK, (signs<<3) + orient);
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_ORIENT_CFG, &LSM6DS33_config.ORIENT_config, 1);
	return HAL_OK;
}

/*
 * @brief Configuration filters for gyroscope and accelerometer sensors.
 * @param g_HPF - mode of high-pass filter for gyroscope.
 * 					@arg LSM6DS33_GYRO_HPF_OFF
 * 					@arg LSM6DS33_GYRO_HPF_ON
 * @param g_HPF_frequency - frequency for high-pass filter for gyroscope.
 * 					@arg LSM6DS33_GYRO_HPF_FREQ_1  - 0.0081 Hz
 * 					@arg LSM6DS33_GYRO_HPF_FREQ_2  - 0.0324 Hz
 * 					@arg LSM6DS33_GYRO_HPF_FREQ_3  - 2.07 Hz
 * 					@arg LSM6DS33_GYRO_HPF_FREQ_4  - 16.32 Hz
 * @param a_HPF - mode of high-pass filter for accelerometer.
 * 					@arg LSM6DS33_A_FILTER_MODE_1 - slope with ODR_XL/50 frequency
 * 					@arg LSM6DS33_A_FILTER_MODE_2 - high-pass with ODR_XL/100 frequency
 * 					@arg LSM6DS33_A_FILTER_MODE_3 - high-pass with ODR_XL/9 frequency
 * 					@arg LSM6DS33_A_FILTER_MODE_4 - high-pass with ODR_XL/400 frequency
 * @retval status:		-HAL_OK		Configuration complete.
 * 						-HAL_ERROR	Error of configuration.
 */
HAL_StatusTypeDef LSM6DS33_config_filters(uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF) {
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL7_config, LSM6DS33_GYRO_HPF_MASK, (g_HPF << 6) + (g_HPF_frequency << 4));
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL8_config, LSM6DS33_A_FILTER_MASK, a_HPF << 5);
	I2C_write_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL7, &LSM6DS33_config.CTRL7_config, 2);
	return HAL_OK;
}

/*
 * @brief Configure full-scale for gyroscope and accelerometer.
 * @param a_FS - full-scale for accelerometer
 * 					@arg LSM6DS33_FULL_SCALE_2G
 * 					@arg LSM6DS33_FULL_SCALE_4G
 * 					@arg LSM6DS33_FULL_SCALE_8G
 * 					@arg LSM6DS33_FULL_SCALE_16G
 * @param g_FS - full-scale for gyroscope.
 * 					@arg LSM6DS33_FULL_SCALE_250DPS
 * 					@arg LSM6DS33_FULL_SCALE_500DPS
 * 					@arg LSM6DS33_FULL_SCALE_1000DPS
 * 					@arg LSM6DS33_FULL_SCALE_2000DPS
 * @retval status:		-HAL_OK		Configuration complete.
 * 						-HAL_ERROR	Error of configuration or wrong parameters.
 */
HAL_StatusTypeDef LSM6DS33_config_full_scale(uint8_t a_FS, uint8_t g_FS) {
	if(a_FS > 0b11 || a_FS < 0 || g_FS > 0b11 || g_FS < 0) return HAL_ERROR;
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL1_config, LSM6DS33_FULL_SCALE_MASK, a_FS << 2);
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL2_config, LSM6DS33_FULL_SCALE_MASK, g_FS << 2);
	I2C_write_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL1, &LSM6DS33_config.CTRL1_config, 2);
	return HAL_OK;
}

/*
 * @brief Set perfomance mode and output data rate for gyroscope and accelerometer sensors
 * @param a_ODR - output data rate for accelerometer
 * 					@arg LSM6DS33_ODR_POWER_DOWN
 * 					@arg LSM6DS33_ODR_12_5HZ
 * 					@arg LSM6DS33_ODR_26HZ
 * 					@arg LSM6DS33_ODR_52HZ
 * 					@arg LSM6DS33_ODR_104HZ
 * 					@arg LSM6DS33_ODR_208HZ
 * 					@arg LSM6DS33_ODR_416HZ
 * 					@arg LSM6DS33_ODR_833HZ
 * 					@arg LSM6DS33_ODR_1660HZ
 * 					@arg LSM6DS33_ODR_3330HZ
 * @param g_ODR - output data rate for gyroscope
 * 					@arg LSM6DS33_ODR_POWER_DOWN
 * 					@arg LSM6DS33_ODR_12_5HZ
 * 					@arg LSM6DS33_ODR_26HZ
 * 					@arg LSM6DS33_ODR_52HZ
 * 					@arg LSM6DS33_ODR_104HZ
 * 					@arg LSM6DS33_ODR_208HZ
 * 					@arg LSM6DS33_ODR_416HZ
 * 					@arg LSM6DS33_ODR_833HZ
 * @retval status:		-HAL_OK		Configuration complete.
 * 						-HAL_ERROR	Error of configuration or wrong parameters.
 */
HAL_StatusTypeDef LSM6DS33_config_perfomance_mode(uint8_t a_ODR, uint8_t g_ODR) {
	if(a_ODR > 0b1010 || a_ODR < 0b0 || g_ODR > 0b1000 || g_ODR < 0b0) return HAL_ERROR;
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL1_config, LSM6DS33_ODR_MASK, a_ODR << 4);
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL2_config, LSM6DS33_ODR_MASK, g_ODR << 4);
	I2C_write_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL1, &LSM6DS33_config.CTRL1_config, 2);
	return HAL_OK;
}

/*
 * @brief Software reset of sensor
 * @retval status:		-HAL_OK		Reset complete.
 * 						-HAL_ERROR	Error of reset.
 */
HAL_StatusTypeDef LSM6DS33_reset() {
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL3_config, 0b00000001, 0b0);
	I2C_write_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1);
	return HAL_OK;
}

/*
 * @brief Read measurements from accelerometer
 * @param *a - Array to store acceleration on 3 axes
 * @retval status:		-HAL_OK		Read measurements complete.
 * 						-HAL_ERROR	Error of reading registers.
 */
HAL_StatusTypeDef LSM6DS33_A_get_measure(float *a) {
	uint16_t raw_data[6];
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_A, (uint8_t*)&raw_data, 6);
	for(int i = 0; i < 3; i++) {
	  a[i] = ((float) raw_data[i])*FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	}
	return HAL_OK;
}

/*
 * @brief Read measurements from gyroscope
 * @param *g - Array to store angular rate on 3 axes
 * @retval status:		-HAL_OK		Read measurements complete.
 * 						-HAL_ERROR	Error of reading registers.
 */
HAL_StatusTypeDef LSM6DS33_G_get_measure(float *g) {
	uint16_t raw_data[3];
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 6);
	for(int i = 0; i < 3; i++) {
	  g[i] = ((float) raw_data[i])*FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	}
	return HAL_OK;
}

/*
 * @brief Read measurements from termometer
 * @param *t - Value to store temperature
 * @retval status:		-HAL_OK		Read measurements complete.
 * 						-HAL_ERROR	Error of reading registers.
 */
HAL_StatusTypeDef LSM6DS33_T_get_measure(float *t) {
	uint16_t raw_data;
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 2);
	*t = (((float) raw_data)*12.)/(float)0x8000;
	return HAL_OK;
}

/*
 * @brief Read measurements from accelerometer and gyroscope
 * @param *a - Array to store acceleration on 3 axes
 * @param *g - Array to store angular rate on 3 axes
 * @retval status:		-HAL_OK		Read measurements complete.
 * 						-HAL_ERROR	Error of reading registers.
 */
HAL_StatusTypeDef LSM6DS33_get_measure(float* a, float *g) {
	uint16_t raw_data[6];
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 12);
	for(int i = 0; i < 3; i++) {
	  g[i] = ((float) raw_data[i])*FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	  a[i] = ((float) raw_data[i + 3])*FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	}
	return HAL_OK;
}

/*
 * @brief Read measurements from all sensors
 * @param *a - Array to store acceleration on 3 axes
 * @param *g - Array to store angular rate on 3 axes
 * @param *t - Value to store temperature
 * @retval status:		-HAL_OK		Read measurements complete.
 * 						-HAL_ERROR	Error of reading registers.
 */
HAL_StatusTypeDef LSM6DS33_get_all_measure(float *a, float *g, float *t) {
	uint16_t raw_data[7];
	I2C_read_bytes(&LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 14);
	for(int i = 0; i < 3; i++) {
	  g[i] = ((float) raw_data[i+1])*FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	  a[i] = ((float) raw_data[i + 4])*FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2]/(float)0x8000;
	}
	*t = ((float) raw_data[0])*125/(float)0x8000 + 26;
	return HAL_OK;
}
