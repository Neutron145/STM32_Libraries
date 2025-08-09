/**
 * @file LIS3MDL.h
 * @author Talha Sarı (talha.sari@outlook.com.tr)
 * @brief LIS3MDL 3-axis magnetometer I2C library for STM32 environment
 * @version v1.1
 * @date 2021-06-07
 * 
 * @copyright Copyright (c) 2021 - GNU General Public License v3
 * 
 */

#ifndef LIS3MDL_H
#define LIS3MDL_H

#include <stdint.h>
#include "main.h"

/* Structure and Enums */
typedef enum {
    LIS3MDL_OK = 0x00,
    LIS3MDL_ERROR
} LIS3MDL_Result_t;

typedef enum {
    LIS3MDL_Scale_4G = 0x00,    /* ±4 Gauss     */
    LIS3MDL_Scale_8G = 0x20,    /* ±8 Gauss     */
    LIS3MDL_Scale_12G = 0x40,   /* ±12 Gauss    */
    LIS3MDL_Scale_16G = 0x60    /* ±16 Gauss    */
} LIS3MDL_Scale_t;

typedef enum {
    LIS3MDL_Device_0 = 0x1C,
    LIS3MDL_Device_1 = 0x1E
} LIS3MDL_Device_t;

typedef enum {
    LIS3MDL_MODE_LOWPOWER = 0x00,
    LIS3MDL_MODE_MEDIUM = 0x01,
    LIS3MDL_MODE_HIGH = 0x02,
    LIS3MDL_MODE_ULTRAHIGH = 0x03
} LIS3MDL_OperationMode_t;

typedef enum {
    LIS3MDL_ODR_0 = 0x00,   /* 0.625 Hz */
    LIS3MDL_ODR_1 = 0x04,   /* 1.25 Hz  */
    LIS3MDL_ODR_2 = 0x08,   /* 2.5 Hz   */
    LIS3MDL_ODR_3 = 0x0C,   /* 5 Hz     */
    LIS3MDL_ODR_4 = 0x10,   /* 10 Hz    */
    LIS3MDL_ODR_5 = 0x14,   /* 20 Hz    */
    LIS3MDL_ODR_6 = 0x18,   /* 40 Hz    */
    LIS3MDL_ODR_7 = 0x1C,   /* 80 Hz    */
    /** Fast Output Data Rates
     * Low Power:               1000 Hz
     * Medium Performance:      560 Hz
     * High Performance:        300 Hz
     * Ultra-High Performance:  155 Hz
     */
    LIS3MDL_ODR_FAST = 0x02
} LIS3MDL_ODR_t;

typedef struct {
    float *mag_x;
    float *mag_y;
    float *mag_z;
    float temp;
    int16_t mag_raw[3];
    int16_t temp_raw;
    LIS3MDL_Scale_t scale;
    uint8_t addr;
} LIS3MDL_t;

/* Sensor Functions */
/**
 * @brief         Initializes the sensor according to the specified parameters.
 * 
 * @param hsensor Pointer to a LIS3MDL_t handler structure that contains the
 *                configuration and axis values information for specified sensor.
 * @param hi2c    Pointer to I2C_HandleTypeDef for the I2C bus.
 * @param dev     Specified sensor device no for the I2C address selection.
 * @param scale   Magnetometer full scale selection.
 * @param mode    Sensor's operation mode selection.
 * @param odr     Sensor's output data rate selection.
 * @return        LIS3MDL status
 */
LIS3MDL_Result_t LIS3MDL_Init(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c, LIS3MDL_Device_t dev, LIS3MDL_Scale_t scale, LIS3MDL_OperationMode_t mode, LIS3MDL_ODR_t odr);

/**
 * @brief         Reads the 3-axis magnetometer values for specified sensor.
 * 
 * @param hsensor Pointer to a LIS3MDL_t handler structure that contains the
 *                configuration and axis values information for specified sensor.
 * @param hi2c    Pointer to I2C_HandleTypeDef for the I2C bus.
 * @return        LIS3MDL status
 */
LIS3MDL_Result_t LIS3MDL_ReadMag(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c);

/**
 * @brief         Reads the temperature in Celcius for specified sensor.
 * 
 * @param hsensor Pointer to a LIS3MDL_t handler structure that contains the
 *                configuration and axis values information for specified sensor.
 * @param hi2c    Pointer to I2C_HandleTypeDef for the I2C bus.
 * @return        LIS3MDL status
 */
LIS3MDL_Result_t LIS3MDL_ReadTemp(LIS3MDL_t *hsensor, I2C_HandleTypeDef *hi2c);

#endif /* LIS3MDL_H */
