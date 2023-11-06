/*
 *******************************************************************************
 * 	@file			BMP280_hal.h
 *	@brief			Header file for BMP280_hal.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		03.11.2023
 *******************************************************************************
 */

#ifndef BME280_H_
#define BME280_H_

#include "stm32f1xx_hal.h"


/* Calibration data for temperature and pressure -------- --------------------*/
typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
} BMP280_calib;

typedef struct {
	uint8_t ctrl_meas;
	uint8_t config;
}BMP280_settings;

/* Periods between measurements in normal mode--------------------------------*/
#define BMP280_STANDBY_0_5 				0b000
#define BMP280_STANDBY_62_5 			0b001
#define BMP280_STANDBY_125 				0b010
#define BMP280_STANDBY_250 				0b011
#define BMP280_STANDBY_500 				0b100
#define BMP280_STANDBY_1000 			0b101
#define BMP280_STANDBY_2000 			0b110
#define BMP280_STANDBY_4000 			0b111

/* Oversampling for temperature and pressure measurements --------------------*/
#define BMP280_OVERSAMPLING_0 			0b000
#define BMP280_OVERSAMPLING_1 			0b001
#define BMP280_OVERSAMPLING_2 			0b010
#define BMP280_OVERSAMPLING_4 			0b011
#define BMP280_OVERSAMPLING_8 			0b100
#define BMP280_OVERSAMPLING_16 			0b101

/* Coefficient of the IIR filter ---------------------------------------------*/
#define BMP280_FILTER_OFF				0b000
#define BMP280_FILTER_X2				0b001
#define BMP280_FILTER_X4				0b010
#define BMP280_FILTER_X8				0b011
#define BMP280_FILTER_X16				0b100

/* Addres of BMP280 on I2C ---------------------------------------------------*/
#define BMP280_ADDRESS 					0x76 << 1

/* Addresses of BMP280 registers ---------------------------------------------*/
#define BMP280_CALIBRATION_REGISTER 	0x88
#define BMP280_CONFIG_REGISTER 			0xF5
#define BMP280_RAW_DATA_REGISTER 		0xF7
#define BMP280_STATUS_REGISTER 			0xF3
#define BMP280_CTRL_MEAS_REGISTER 		0xF4
#define BMP280_RESET_REGISTER 			0xE0
#define BMP280_ID_REGISTER 				0xD0

/* Reference pressure. Used to make altitude calculations accurate --------*/
uint32_t refPressure;

/* Compensation formula from datasheet ---------------------------------------*/
int32_t __BMP280_compensate_T_int32(int32_t adc_T);
uint32_t __BMP280_compensate_P_int64(int32_t adc_P);

/* Initialization of BMP280 --------------------------------------------------*/
HAL_StatusTypeDef BMP280_init(I2C_HandleTypeDef hi2c_, uint32_t refPressure_);

/* Configuration BMP280 ------------------------------------------------------*/
HAL_StatusTypeDef BMP280_Config(uint8_t T_OS, uint8_t P_OS, uint8_t STDB, uint8_t IIRF);

/* Configuration power modes BMP280 -------------------------------------------------------*/
HAL_StatusTypeDef BMP280_forced_measure(double* temp, double* press, double *h);
HAL_StatusTypeDef BMP280_normal_measure();
HAL_StatusTypeDef BMP280_sleep();
HAL_StatusTypeDef BMP280_get_measure(double *temp, double *press, double *h);

#endif /* BME280_H_ */
