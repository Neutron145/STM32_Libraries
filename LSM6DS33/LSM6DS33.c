#include "LSM6DS33.h"


//! Структура для хранения значений регистров конфигурации датчика.
//! Упрощает работу с датчиком, т.к. избегаем чтения регистров с датчика перед его изменением и храним все в памяти МК.
LSM6DS33_cfg LSM6DS33_config;
I2C_TypeDef *LSM6DS33_hi2c;

//! Используется в переводе значений АЦП в мс/c и градусы в зависимости от установленного full-scale акселерометра и гироскопа соотвественно 
uint16_t FULL_SCALES_A[4] = {2, 16, 4, 8};
uint16_t FULL_SCALES_G[4] = {250, 500, 1000, 2000};

//!<
uint32_t LSM6DS33_ADDRESS;

float LSM6DS33_a_ref[3];
float LSM6DS33_g_ref[3];

void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits) {
	//! Обнуляем бит, который должны изменить, с помощью маски
	*reg_data &= ~mask;

	//! Присваиваем обнуленному биту новое значение
	*reg_data |= bits;
}


HAL_StatusTypeDef LSM6DS33_init(I2C_TypeDef *hi2c_) {
	HAL_StatusTypeDef status;

	uint8_t id;

	if ((status = I2C_Mem_Read(hi2c_, 0xD7, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
		LSM6DS33_ADDRESS = 0xD7;
	}
	else if ((status = I2C_Mem_Read(hi2c_, 0xD5, LSM6DS33_REGISTER_ID, &id, 1, 0xFF)) == HAL_OK) {
			LSM6DS33_ADDRESS = 0xD5;
	}
	else {
		return status;
	}

	//! Если считанный ID соответствует ID датчика, устанавливаем стандартную конфигурацию и записываем начальные значения
	if (id == 0x69) {
		LSM6DS33_hi2c = hi2c_;
		LSM6DS33_config.ORIENT_config = 0b0;
		LSM6DS33_config.CTRL1_config = 0b0;
		LSM6DS33_config.CTRL2_config = 0b0;
		LSM6DS33_config.CTRL3_config = 0b01000100;
		LSM6DS33_config.CTRL7_config = 0b0;
		LSM6DS33_config.CTRL8_config = 0b0;

		if ((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1, 0xFF)) != HAL_OK) {
			return status;
		}

		LSM6DS33_get_measure(LSM6DS33_a_ref, LSM6DS33_g_ref);

		return HAL_OK;
	}
	return HAL_ERROR;
}


HAL_StatusTypeDef LSM6DS33_config_orientation(uint8_t orient, uint8_t signs) {
	HAL_StatusTypeDef status;

	//! Проверка на корректность введенных значений
	if (signs > 0b111) return HAL_ERROR;

	//! Изменяем значение соответствующего бита в значении регистра структуры и записываем в регистр устройства 
	__LSM6DS33_modify_reg(&LSM6DS33_config.ORIENT_config, LSM6DS33_ORIENT_CFG_MASK, (signs<<3) + orient);

	if ((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_ORIENT_CFG, &LSM6DS33_config.ORIENT_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_config_filters(uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF) {
	HAL_StatusTypeDef status;

	//! Изменяем значения соответствующих битов в значении регистра структуры и записываем в регистр устройства 
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL7_config, LSM6DS33_GYRO_HPF_MASK, (g_HPF << 6) + (g_HPF_frequency << 4));
	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL8_config, LSM6DS33_A_FILTER_MASK, a_HPF << 5);

	if ((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL7, &LSM6DS33_config.CTRL7_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_config_full_scale(uint8_t a_FS, uint8_t g_FS) {
	HAL_StatusTypeDef status;

	//! Проверка на корректность введенных значений
	if(a_FS > 0b11 || a_FS < 0 || g_FS > 0b11 || g_FS < 0) return HAL_ERROR;

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

	if ((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL1, &LSM6DS33_config.CTRL1_config, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_reset() {
	HAL_StatusTypeDef status;

	__LSM6DS33_modify_reg(&LSM6DS33_config.CTRL3_config, 0b00000001, 0b0);

	if ((status = I2C_Mem_Write(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_CTRL3, &LSM6DS33_config.CTRL3_config, 1, 0xFF)) != HAL_OK) {
		return status;
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_A_get_measure(float *a) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data[3];
	if ((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_A, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for (int i = 0; i < 3; i++) {
	  //! Необходимо умножить считанное значение с АЦП на установленный full_scale. Для этого используем массивы 
	  a[i] = ((float)raw_data[i]) * FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  a[i] = a[i] - LSM6DS33_a_ref[i];
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_G_get_measure(float *g) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data[3];
	if ((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 6, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  g[i] = ((float)raw_data[i]) * FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  g[i] = g[i] - LSM6DS33_g_ref[i];
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_T_get_measure(float *t) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data;
	if ((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 2, 0xFF)) != HAL_OK) {
		return status;
	}

	*t = (((float)raw_data) * 12.) / (float)0x8000;

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_get_measure(float *a, float *g) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data[6];
	if ((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_G, (uint8_t*)&raw_data, 12, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  g[i] = ((float)raw_data[i]) * FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  g[i] = g[i] - LSM6DS33_g_ref[i];
	  a[i] = ((float)raw_data[i + 3]) * FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  a[i] = a[i] - LSM6DS33_a_ref[i];
	}

	return HAL_OK;
}


HAL_StatusTypeDef LSM6DS33_get_all_measure(float *a, float *g, float *t) {
	HAL_StatusTypeDef status;

	//! Сохраняем в массив uint16_t. Таким образом избегаем объединения uint8_t в uint16_t
	uint16_t raw_data[7];
	if ((status = I2C_Mem_Read(LSM6DS33_hi2c, LSM6DS33_ADDRESS, LSM6DS33_REGISTER_OUT_T, (uint8_t*)&raw_data, 14, 0xFF)) != HAL_OK) {
		return status;
	}

	for(int i = 0; i < 3; i++) {
	  g[i] = ((float)raw_data[i + 1]) * FULL_SCALES_G[(LSM6DS33_config.CTRL2_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  g[i] = g[i] - LSM6DS33_g_ref[i];
	  a[i] = ((float)raw_data[i + 4]) * FULL_SCALES_A[(LSM6DS33_config.CTRL1_config & LSM6DS33_FULL_SCALE_MASK) >> 2] / (float)0x8000;
	  a[i] = a[i] - LSM6DS33_a_ref[i];
	}

	*t = ((float)raw_data[0]) * 125 / (float)0x8000 + 26;

	return HAL_OK;
}
