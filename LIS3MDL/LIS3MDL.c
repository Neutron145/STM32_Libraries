/**
 * @file LIS3MDL.c
 * @author Talha Sarı (talha.sari@outlook.com.tr)
 * @brief LIS3MDL 3-axis magnetometer I2C library for STM32 environment
 * @version v1.1
 * @date 2021-06-07
 * 
 * @copyright Copyright (c) 2021 - GNU General Public License v3
 * 
 */

#include "LIS3MDL_Registers.h"
#include "LIS3MDL.h"

/* I2C R/W Function Prototypes */
static HAL_StatusTypeDef writeByte(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t data);
static HAL_StatusTypeDef readByte(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t *data);
static HAL_StatusTypeDef readMultiBytes(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t *data, uint16_t count);

/* Sensor Functions */
LIS3MDL_Result_t LIS3MDL_Init(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c, LIS3MDL_Device_t dev, LIS3MDL_Scale_t scale, LIS3MDL_OperationMode_t mode, LIS3MDL_ODR_t odr)
{
    uint8_t data;
    hsensor->addr = (uint8_t)(dev << 1);
    hsensor->scale = (LIS3MDL_Scale_t)scale;

    if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)hsensor->addr, 2, 5) != HAL_OK)
        if (HAL_I2C_IsDeviceReady(hi2c, ((uint16_t)hsensor->addr) + 1, 2, 5) != HAL_OK)
            return LIS3MDL_ERROR;

    readByte(hi2c, hsensor->addr, WHO_AM_I, &data);
    if (data != 0x3D)
        return LIS3MDL_ERROR;
    data = 0x00;

    readByte(hi2c, hsensor->addr, CTRL_REG1, &data);
    data &= ~0xFE;
    data |= 0x80 | ((uint8_t)mode << 5) | (uint8_t)odr;
    writeByte(hi2c, hsensor->addr, CTRL_REG1, data);

    readByte(hi2c, hsensor->addr, CTRL_REG2, &data);
    data &= ~0x60;
    data |= (uint8_t)scale;
    writeByte(hi2c, hsensor->addr, CTRL_REG2, data);

    readByte(hi2c, hsensor->addr, CTRL_REG3, &data);
    data &= ~0x03;
    data |= 0x00;
    writeByte(hi2c, hsensor->addr, CTRL_REG3, data);

    readByte(hi2c, hsensor->addr, CTRL_REG4, &data);
    data &= ~0x0C;
    data |= (uint8_t)(mode << 2);
    writeByte(hi2c, hsensor->addr, CTRL_REG4, data);

    readByte(hi2c, hsensor->addr, CTRL_REG5, &data);
    data &= ~0xC0;
    data |= 0x00;
    writeByte(hi2c, hsensor->addr, CTRL_REG5, data);

    readByte(hi2c, hsensor->addr, INT_CFG, &data);
    data &= ~0xFF;
    data |= 0x00;
    writeByte(hi2c, hsensor->addr, INT_CFG, data);

    return LIS3MDL_OK;
}

LIS3MDL_Result_t LIS3MDL_ReadMag(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c)
{
    uint8_t data[6];

    readMultiBytes(hi2c, hsensor->addr, OUT_X_L, data, 6);

    hsensor->mag_raw[0] = ((int16_t)data[1] << 8) | data[0];
    hsensor->mag_raw[1] = ((int16_t)data[3] << 8) | data[2];
    hsensor->mag_raw[2] = ((int16_t)data[5] << 8) | data[4];

    switch (hsensor->scale) {
        case LIS3MDL_Scale_4G:
            *hsensor->mag_x = (float)(hsensor->mag_raw[0] / 6842.0f);
            *hsensor->mag_y = (float)(hsensor->mag_raw[1] / 6842.0f);
            *hsensor->mag_z = (float)(hsensor->mag_raw[2] / 6842.0f);
            break;
        case LIS3MDL_Scale_8G:
            *hsensor->mag_x = (float)(hsensor->mag_raw[0] / 3421.0f);
            *hsensor->mag_y = (float)(hsensor->mag_raw[1] / 3421.0f);
            *hsensor->mag_z = (float)(hsensor->mag_raw[2] / 3421.0f);
            break;
        case LIS3MDL_Scale_12G:
            *hsensor->mag_x = (float)(hsensor->mag_raw[0] / 2281.0f);
            *hsensor->mag_y = (float)(hsensor->mag_raw[1] / 2281.0f);
            *hsensor->mag_z = (float)(hsensor->mag_raw[2] / 2281.0f);
            break;
        case LIS3MDL_Scale_16G:
            *hsensor->mag_x = (float)(hsensor->mag_raw[0] / 1711.0f);
            *hsensor->mag_y = (float)(hsensor->mag_raw[1] / 1711.0f);
            *hsensor->mag_z = (float)(hsensor->mag_raw[2] / 1711.0f);
            break;
    }
    return LIS3MDL_OK;
}

LIS3MDL_Result_t LIS3MDL_ReadTemp(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c)
{
    uint8_t data[2];
    
    readMultiBytes(hi2c, hsensor->addr, TEMP_OUT_L, data, 2);

    hsensor->temp_raw = ((int16_t)data[1] << 8) | data[0];
    hsensor->temp = (float)(hsensor->temp_raw / 8.0f);

    return LIS3MDL_OK;
}

/* I2C R/W Functions */
static HAL_StatusTypeDef writeByte(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = register_addr;
    buffer[1] = data;
    if (HAL_I2C_Master_Transmit(hi2c, (uint16_t)device_addr, (uint8_t *)buffer, 2, 1000) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {}
        return HAL_ERROR;
    }
    return HAL_OK;
}

static HAL_StatusTypeDef readByte(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t *data)
{
    if (HAL_I2C_Master_Transmit(hi2c, (uint16_t)device_addr, &register_addr, 1, 1000) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {}
        return HAL_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, (uint16_t)device_addr, data, 1, 1000) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {}
        return HAL_ERROR;
    }
    return HAL_OK;
}

static HAL_StatusTypeDef readMultiBytes(I2C_HandleTypeDef *hi2c, uint8_t device_addr, uint8_t register_addr, uint8_t *data, uint16_t count)
{
    if (HAL_I2C_Master_Transmit(hi2c, (uint16_t)device_addr, &register_addr, 1, 1000) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {}
        return HAL_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, (uint16_t)device_addr, data, count, 1000) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {}
        return HAL_ERROR;
    }
    return HAL_OK;
}
