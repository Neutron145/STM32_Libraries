#include <stdint.h>
#include "LSM6DS33.h"

float FULL_SCALES_A[4] = { 0.061f, 0.488f, 0.122f, 0.244f };
float FULL_SCALES_G[4] = { 8.75f, 17.5f, 35.0f, 70.0f };

float full_scale_A;
float full_scale_G;

uint32_t LSM6DS33_ADDRESS;

void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits) {
	*reg_data &= ~mask;
	*reg_data |= bits;
}

void __calibrate_accelerometer(LSM6DS33_handler_t* handler) {
	for(int i = 0; i < 3; i++) {
		handler->measured_data.a[i] -= handler->calibration_data.a_bias[i];
	}
}

void __calibrate_gyroscope(LSM6DS33_handler_t* handler) {
	for(int i = 0; i < 3; i++) {
		handler->measured_data.g[i] -= handler->calibration_data.g_bias[i];
	}
}

HAL_StatusTypeDef __get_measure_without_cal(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	int16_t raw_data[6];
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 12, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  handler->measured_data.g[i] = (raw_data[i] * full_scale_G * 0.001f);
	  handler->measured_data.a[i] = (raw_data[i+3] * full_scale_A * 0.001f * 9.80665f);
	}

	return HAL_OK;
}

void __get_calibration_data(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;
	
	for(int i = 0; i < 100; i++) {
		status = __get_measure_without_cal(handler);
		for(int j = 0; j < 3; j++) {
			handler->calibration_data.a_bias[j] += handler->measured_data.a[j];
			handler->calibration_data.g_bias[j] += handler->measured_data.g[j];
		}
	}
	for(int j = 0; j < 3; j++) {
		handler->calibration_data.a_bias[j] /= 100.0f;
		handler->calibration_data.g_bias[j] /= 100.0f;
		handler->measured_data.a[j] = 0.0f;
		handler->measured_data.g[j] = 0.0f;
	}
}


HAL_StatusTypeDef LSM6DS33_init(LSM6DS33_handler_t* handler, I2C_HandleTypeDef* hi2c_) {
	HAL_StatusTypeDef status;
	uint8_t id;

	if((status = I2C_Mem_Read(hi2c_, 0xD7, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
		handler->address = 0xD7;
		handler->SAO_pin_state = 1;
	}
	else if((status = I2C_Mem_Read(hi2c_, 0xD5, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
		handler->address = 0xD5;
		handler->SAO_pin_state = 0;
	}
	handler->id = id;

	full_scale_A = FULL_SCALES_A[0];
	full_scale_G = FULL_SCALES_G[0];

	if(handler->id == 0x69) {
		handler->hi2c_ = hi2c_;

		//!< Записываем дефолтные значения
		handler->registers.ORIENT_config = 0b0;
		handler->registers.CTRL1_config = 0b0;
		handler->registers.CTRL2_config = 0b0;
		handler->registers.CTRL3_config = 0b00000100;
		handler->registers.CTRL7_config = 0b0;
		handler->registers.CTRL8_config = 0b0;
		handler->registers.CTRL10_config = 0b00111000;
		handler->registers.TAP_config = 0b0;

		//!< Записываем дефолтные конфигурации
		handler->config.ODR = LSM6DS33_ODR_52HZ;
		handler->config.ORIENTATION = LSM6DS33_ORIENT_XYZ;
		handler->config.ORIENT_SIGN = LSM6DS33_ORIENT_SIGN_POSITIVE_X | LSM6DS33_ORIENT_SIGN_POSITIVE_Y | LSM6DS33_ORIENT_SIGN_POSITIVE_Z;
		handler->config.GYRO_HPF = LSM6DS33_GYRO_HPF_DISABLED;
		handler->config.ACCEL_HPF = LSM6DS33_A_FILTER_MODE_1;
		handler->config.GYRO_FULL_SCALE = LSM6DS33_FULL_SCALE_245DPS;
		handler->config.ACCEL_FULL_SCALE = LSM6DS33_FULL_SCALE_2G;


		LSM6DS33_config_perfomance_mode(handler, LSM6DS33_ODR_52HZ, LSM6DS33_ODR_52HZ);
		if((status = I2C_Mem_Write(hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &handler->registers.CTRL3_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}
		return HAL_OK;
	}

	__get_calibration_data(handler);
	
	return HAL_ERROR;
}

HAL_StatusTypeDef LSM6DS33_config_orientation(LSM6DS33_handler_t* handler, uint8_t orient, uint8_t signs) {
	HAL_StatusTypeDef status;
	if(signs > 0b111) return HAL_ERROR;

	handler->config.ORIENTATION = orient;
	handler->config.ORIENT_SIGN = signs;
	__LSM6DS33_modify_reg(&handler->registers.ORIENT_config, LSM6DS33_ORIENT_CFG_MASK, (signs<<3) + orient);
	if((status = I2C_Mem_Write(handler->hi2c_, handler->address, LSM6DS33_REGISTER_ORIENT_CFG, &handler->registers.ORIENT_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}
	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_config_filters(LSM6DS33_handler_t* handler, uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF) {
	HAL_StatusTypeDef status;

	handler->config.GYRO_HPF_ON = g_HPF;
	handler->config.GYRO_HPF_FREQ = g_HPF_frequency;
	handler->config.ACCEL_HPF = a_HPF;

	__LSM6DS33_modify_reg(&handler->registers.CTRL7_config, LSM6DS33_GYRO_HPF_MASK, (g_HPF << 6) + (g_HPF_frequency << 4));
	__LSM6DS33_modify_reg(&handler->registers.CTRL8_config, LSM6DS33_A_FILTER_MASK, a_HPF << 5);
	
	if((status = I2C_Mem_Write(handler->hi2c_, handler->address, LSM6DS33_REGISTER_CTRL7, &handler->registers.CTRL7_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_config_full_scale(LSM6DS33_handler_t* handler, uint8_t a_FS, uint8_t g_FS) {
	HAL_StatusTypeDef status;

	if(a_FS > 0b11 || a_FS < 0 || g_FS > 0b11 || g_FS < 0) return HAL_ERROR;

	handler->config.ACCEL_FULL_SCALE = FULL_SCALES_A[a_FS];
	handler->config.GYRO_FULL_SCALE = FULL_SCALES_G[g_FS];

	__LSM6DS33_modify_reg(&handler->registers.CTRL1_config, LSM6DS33_FULL_SCALE_MASK, a_FS << 2);
	__LSM6DS33_modify_reg(&handler->registers.CTRL2_config, LSM6DS33_FULL_SCALE_MASK, g_FS << 2);
	if((status = I2C_Mem_Write(handler->hi2c_, handler->address, LSM6DS33_REGISTER_CTRL1, &handler->registers.CTRL1_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_config_perfomance_mode(LSM6DS33_handler_t* handler, uint8_t a_ODR, uint8_t g_ODR) {
	HAL_StatusTypeDef status;

	if(a_ODR > 0b1010 || a_ODR < 0b0 || g_ODR > 0b1000 || g_ODR < 0b0) return HAL_ERROR;

	handler->config.ACCEL_ODR = a_ODR;
	handler->config.GYRO_ODR = g_ODR;

	__LSM6DS33_modify_reg(&handler->registers.CTRL1_config, LSM6DS33_ODR_MASK, a_ODR << 4);
	__LSM6DS33_modify_reg(&handler->registers.CTRL2_config, LSM6DS33_ODR_MASK, g_ODR << 4);
	if((status = I2C_Mem_Write(handler->hi2c_, handler->address, LSM6DS33_REGISTER_CTRL1, &handler->registers.CTRL1_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_reset(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	__LSM6DS33_modify_reg(&handler->registers.CTRL3_config, 0b00000001, 0b0);

	if((status = I2C_Mem_Write(handler->hi2c_, handler->address, LSM6DS33_REGISTER_CTRL3, &handler->registers.CTRL3_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_A_get_measure(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	int16_t raw_data[3];
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_A, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
		handler->measured_data.a[i] = (raw_data[i] * full_scale_A * 0.001f * 9.80665f);
	}
	__calibrate_accelerometer(handler);

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_G_get_measure(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	int16_t raw_data[3];
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
		handler->measured_data.g[i] = (raw_data[i] * full_scale_G * 0.001f);
	}
	__calibrate_gyroscope(handler);

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_T_get_measure(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	uint16_t raw_data;
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	handler->measured_data.t = (((float) raw_data)*12.)/(float)0x8000;

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_get_measure(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	int16_t raw_data[6];
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 12, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  handler->measured_data.g[i] = (raw_data[i] * full_scale_G * 0.001f);
	  handler->measured_data.a[i] = (raw_data[i+3] * full_scale_A * 0.001f * 9.80665f);
	}
	__calibrate_gyroscope(handler);
	__calibrate_accelerometer(handler);

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_get_all_measure(LSM6DS33_handler_t* handler) {
	HAL_StatusTypeDef status;

	int16_t raw_data[7];
	if((status = I2C_Mem_Read(handler->hi2c_, handler->address, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 14, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
		handler->measured_data.g[i] = (raw_data[i+1] * full_scale_G * 0.001f);
		handler->measured_data.a[i] = (raw_data[i+4] * full_scale_A * 0.001f * 9.80665f);
	}
	handler->measured_data.t = ((float) raw_data[0])*125/(float)0x8000 + 26;

	__calibrate_gyroscope(handler);
	__calibrate_accelerometer(handler);

	return HAL_OK;
}
