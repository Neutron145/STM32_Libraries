&/*
 *******************************************************************************
 * 	@file			BME280_ll.h
 *	@brief			Header file for BME280_ll.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		09.11.2023
 *******************************************************************************
 */

#ifndef BME280_H_
#define BME280_H_

#include "main.h"
#include "I2C_ll.h"

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
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} BME280_calib;

/* Settings of BME280 -----------------------------*/
typedef struct {
	uint8_t ctrl_meas;
	uint8_t config;
	uint8_t ctrl_hum;
} BME280_settings;

/* Reference pressure. Used to make altitude calculations accurate --------*/
extern uint32_t BME280_refPressure;
/* Instance I2C to which BMP280 is connected ---------------------------------*/
extern I2C_TypeDef *BME280_I2Cx;
/* Temperature value for pressure calculation --------------------------------*/
extern int32_t BME280_t_fine;

/* Periods between measurements in normal mode--------------------------------*/
#define BME280_STANDBY_0_5 				0b000
#define BME280_STANDBY_62_5 			0b001
#define BME280_STANDBY_125 				0b010
#define BME280_STANDBY_250 				0b011
#define BME280_STANDBY_500 				0b100
#define BME280_STANDBY_1000 			0b101
#define BME280_STANDBY_2000 			0b110
#define BME280_STANDBY_4000 			0b111

/* Oversampling for temperature and pressure measurements --------------------*/
#define BME280_OVERSAMPLING_0 			0b000
#define BME280_OVERSAMPLING_1 			0b001
#define BME280_OVERSAMPLING_2 			0b010
#define BME280_OVERSAMPLING_4 			0b011
#define BME280_OVERSAMPLING_8 			0b100
#define BME280_OVERSAMPLING_16 			0b101

/* Coefficient of the IIR filter ---------------------------------------------*/
#define BME280_FILTER_OFF				0b000
#define BME280_FILTER_X2				0b001
#define BME280_FILTER_X4				0b010
#define BME280_FILTER_X8				0b011
#define BME280_FILTER_X16				0b100

/* Addres of BMP280 on I2C ---------------------------------------------------*/
#define BME280_ADDRESS 					0x76 << 1

/* Addresses of BMP280 registers ---------------------------------------------*/
#define BME280_REGISTER_CALIBRATION		0x88
#define BME280_REGISTER_CALIBRATION_H4	0xE4
#define BME280_REGISTER_CALIBRATION_H6	0xE7
#define BME280_REGISTER_RAW_DATA 		0xF7
#define BME280_REGISTER_CONFIG 			0xF5
#define BME280_REGISTER_CTRL_MEAS 		0xF4
#define BME280_REGISTER_STATUS 			0xF3
#define BME280_REGISTER_CTRL_HUM 		0xF2
#define BME280_REGISTER_RESET 			0xE0
#define BME280_REGISTER_ID 				0xD0

/* Compensation formula from datasheet ---------------------------------------*/
int32_t __BME280_compensate_T_int32(int32_t adc_T);
uint32_t __BME280_compensate_P_int64(int32_t adc_P);
uint32_t __BME280_compensate_H_int32(int32_t adc_H);

/* Initialization of BMP280 --------------------------------------------------*/
HAL_StatusTypeDef BME280_init(I2C_TypeDef  *I2Cx_, uint32_t refPressure_);

/* Configuration BMP280 ------------------------------------------------------*/
HAL_StatusTypeDef BME280_config(uint8_t T_OS, uint8_t P_OS, uint8_t H_OS, uint8_t STDB, uint8_t IIRF);

/* Configuration power modes BMP280 -------------------------------------------------------*/
HAL_StatusTypeDef BME280_forced_measure(float *temp, float *press, float *hum, float *h);
HAL_StatusTypeDef BME280_normal_measure();
HAL_StatusTypeDef BME280_sleep();
HAL_StatusTypeDef BME280_get_measure(float *temp, float *press, float *hum, float *h);

#endif /* BME280_H_ */
