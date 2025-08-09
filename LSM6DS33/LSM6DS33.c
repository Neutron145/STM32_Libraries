#include <stdint.h>
#include "LSM6DS33.h"

//!< LPF2:
//!< CTRL8_XL: LPF2_XL_EN (1)
//!< CTRL10_C (FUNC_EN) / TAP_CFG (SLOPE_FDS)

//! Структура для хранения значений регистров конфигурации датчика.
//! Упрощает работу с датчиком, т.к. избегаем чтения регистров с датчика перед его изменением и храним все в памяти МК.
LSM6DS33_cfg LSM6DS33_config;
I2C_HandleTypeDef* LSM6DS33_hi2c;


//! Используется в переводе значений АЦП в мс/c и градусы в зависимости от установленного full-scale акселерометра и гироскопа соотвественно 
float FULL_SCALES_A[4] = {0.061f, 0.488f, 0.122f, 0.244f};
float FULL_SCALES_G[4] = {8.75f, 17.5f, 35.0f, 70.0f};

float a_ref[3] = {0};
float g_ref[3] = {0};

float full_scale_A;
float full_scale_G;

uint32_t LSM6DS33_ADDRESS;

void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits) {
	//! Обнуляем бит, который должны изменить, с помощью маски
	*reg_data &= ~mask;
	//! Присваиваем обнуленному биту новое значение
	*reg_data |= bits;
}


HAL_StatusTypeDef LSM6DS33_init(I2C_HandleTypeDef* hi2c_) {
	HAL_StatusTypeDef status;
	uint8_t id;
	if((status = I2C_Mem_Read(hi2c_, 0xD7, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
		LSM6DS33_ADDRESS = 0xD7;
	}
	else if((status = I2C_Mem_Read(hi2c_, 0xD5, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
		LSM6DS33_ADDRESS = 0xD5;
	}

	full_scale_A = FULL_SCALES_A[0];
	full_scale_G = FULL_SCALES_G[0];

	//! Если считанный ID соответствует ID датчика, устанавливаем стандартную конфигурацию
	if(id == 0x69) {
		LSM6DS33_hi2c = hi2c_;
		LSM6DS33_config.ORIENT_config = 0b0;
		LSM6DS33_config.CTRL1_config;
		LSM6DS33_config.CTRL2_config = 0b0;
		LSM6DS33_config.CTRL3_config = 0b01000100;
		LSM6DS33_config.CTRL7_config = 0b11101000;
		LSM6DS33_config.CTRL8_config = 0b11000000;
		LSM6DS33_config.CTRL10_config = 0b00010000;

		//!< Включаем фильтр для акселерометра
		if((status = I2C_Mem_Write(hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL8, &LSM6DS33_config.CTRL8_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}

		//!< Включаем фильтр
		if((status = I2C_Mem_Write(hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_TAP_CFG, &LSM6DS33_config.CTRL10_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}

		//!< Включаем HPF для гироскопа
		if((status = I2C_Mem_Write(hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL7, &LSM6DS33_config.CTRL7_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}

		//!< Устанавливаем частоту гироскопа и акселерометра 52Гц
		LSM6DS33_config_perfomance_mode(LSM6DS33_ODR_52HZ, LSM6DS33_ODR_52HZ);
		if((status = I2C_Mem_Write(hi2c_, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef LSM6DS33_config_orientation(uint8_t orient, uint8_t signs) {
	HAL_StatusTypeDef status;
	//! Проверка на корректность введенных значений
	if(signs > 0b111) return HAL_ERROR;
	//! Изменяем значение соответствующего бита в значении регистра структуры и записываем в регистр устройства 
	__LSM6DS33_modify_reg(&LSM6DS33_config.ORIENT_config, LSM6DS33_ORIENT_CFG_MASK, (signs<<3) + orient);
	if((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_ORIENT_CFG, &LSM6DS33_config.ORIENT_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}
	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_config_filters(uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF) {
	HAL_StatusTypeDef status;

	//! Изменяем значения соответствующих битов в значении регистра структуры и записываем в регистр устройства 
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL7_config, LSM6DS33_GYRO_HPF_MASK, (g_HPF << 6) + (g_HPF_frequency << 4));
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL8_config, LSM6DS33_A_FILTER_MASK, a_HPF << 5);

	if((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL7, &LSM6DS33_config.CTRL7_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_config_full_scale(uint8_t a_FS, uint8_t g_FS) {
	HAL_StatusTypeDef status;

	//! Проверка на корректность введенных значений
	if(a_FS > 0b11 || a_FS < 0 || g_FS > 0b11 || g_FS < 0) return HAL_ERROR;

	full_scale_A = FULL_SCALES_A[a_FS];
	full_scale_G = FULL_SCALES_G[g_FS];

	//! Изменяем значения соответствующих битов в значении регистра структуры и записываем в регистр устройства
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL1_config, LSM6DS33_FULL_SCALE_MASK, a_FS << 2);
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL2_config, LSM6DS33_FULL_SCALE_MASK, g_FS << 2);
	if((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL1, &LSM6DS33_config.CTRL1_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_config_perfomance_mode(uint8_t a_ODR, uint8_t g_ODR) {
	HAL_StatusTypeDef status;

	//! Проверка на корректность введенных значений
	if(a_ODR > 0b1010 || a_ODR < 0b0 || g_ODR > 0b1000 || g_ODR < 0b0) return HAL_ERROR;

	//! Изменяем значения соответствующих битов в значении регистра структуры и записываем в регистр устройства 
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL1_config, LSM6DS33_ODR_MASK, a_ODR << 4);
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL2_config, LSM6DS33_ODR_MASK, g_ODR << 4);

	if((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL1, &LSM6DS33_config.CTRL1_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_reset() {
	HAL_StatusTypeDef status;

	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL3_config, 0b00000001, 0b0);

	if((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_A_get_measure(float *a) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	int16_t raw_data[3];
	if((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_A, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  //! Необходимо умножить считанное значение с АЦП на установленный full_scale. Для этого используем массивы 
		a[i] = (raw_data[i] * full_scale_A * 0.001f * 9.80665f) - a_ref[i];
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_G_get_measure(float *g) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	int16_t raw_data[3];
	if((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
		g[i] = (raw_data[i] * full_scale_G * 0.001f);
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_T_get_measure(float *t) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data;
	if((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	*t = (((float) raw_data)*12.)/(float)0x8000;

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_get_measure(float* a, float *g) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	int16_t raw_data[6];
	if((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 12, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  g[i] = (raw_data[i] * full_scale_G * 0.001f);
	  a[i] = (raw_data[i+3] * full_scale_A * 0.001f * 9.80665f);
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_get_all_measure(float *a, float *g, float *t) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	int16_t raw_data[7];
	if((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 14, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
		g[i] = (raw_data[i+1] * full_scale_G * 0.001f) - g_ref[i];
		a[i] = (raw_data[i+4] * full_scale_A * 0.001f * 9.80665f) - a_ref[i];
	}
	*t = ((float) raw_data[0])*125/(float)0x8000 + 26;

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS33_convert_measure(int16_t* buffer, float* a, float *g) {
	for(int i = 0; i < 3; i++) {
		  g[i] = (buffer[i] * full_scale_G * 0.001f);
		  a[i] = (buffer[i + 3] * full_scale_A * 0.001f * 9.80665f);
	}


	return HAL_OK;
}
