/*
 *******************************************************************************
 * 	@file			BMP280_hal.h
 *	@brief			Header file for BMP280_hal.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		03.11.2023
 *******************************************************************************
 */

#ifndef BMP280_H_
#define BMP280_H_

#include "main.h"

/* Calibration data for temperature and pressure -----------------------------*/
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

/* Settings of BMP280 -----------------------------*/
typedef struct {
	uint8_t ctrl_meas;
	uint8_t config;
} BMP280_settings;

/* Reference pressure. Used to make altitude calculations accurate -----------*/
extern uint32_t BMP280_refPressure;
/* Instance I2C to which BMP280 is connected ---------------------------------*/
extern I2C_HandleTypeDef BMP280_hi2c;
/* Temperature value for pressure calculation --------------------------------*/
extern int32_t BMP280_t_fine;

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
#define BMP280_REGISTER_CALIBRATION		0x88
#define BMP280_REGISTER_RAW_DATA 		0xF7
#define BMP280_REGISTER_CONFIG 			0xF5
#define BMP280_REGISTER_CTRL_MEAS 		0xF4
#define BMP280_REGISTER_STATUS 			0xF3
#define BMP280_REGISTER_RESET 			0xE0
#define BMP280_REGISTER_ID 				0xD0

/* Compensation formula from datasheet ---------------------------------------*/
int32_t __BMP280_compensate_T_int32(int32_t adc_T);
uint32_t __BMP280_compensate_P_int64(int32_t adc_P);

/* Initialization of BMP280 --------------------------------------------------*/
HAL_StatusTypeDef BMP280_init(I2C_HandleTypeDef hi2c_, uint32_t refPressure_);

/* Configuration BMP280 ------------------------------------------------------*/
HAL_StatusTypeDef BMP280_config(uint8_t T_OS, uint8_t P_OS, uint8_t STDB, uint8_t IIRF);

/* Configuration power modes BMP280 ------------------------------------------*/
HAL_StatusTypeDef BMP280_forced_measure(float *temp, float *press, float *h);
HAL_StatusTypeDef BMP280_normal_measure();
HAL_StatusTypeDef BMP280_sleep();
HAL_StatusTypeDef BMP280_get_measure(float *temp, float *press, float *h);

#endif /* BMP280_H_ */
