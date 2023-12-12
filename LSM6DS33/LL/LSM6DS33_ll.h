/*
 *******************************************************************************
 * 	@file			LSM6DS33_ll.h
 *	@brief			Header file for LSM6DS33_ll.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		15.11.2023
 *******************************************************************************
 */

#ifndef INC_LSM6DS33_HAL_H_
#define INC_LSM6DS33_HAL_H_

#include "main.h"

/* Data from config registers of LSM6DS33 ------------------------------------*/
typedef struct {
	uint8_t ORIENT_config;
	uint8_t CTRL1_config;
	uint8_t CTRL2_config;
	uint8_t CTRL3_config;
	uint8_t CTRL7_config;
	uint8_t CTRL8_config;
} LSM6DS33_cfg;

/* Instance I2C to which LSM6DS33 is connected -------------------------------*/
extern I2C_HandleTypeDef LSM6DS33_hi2c;

extern uint16_t FULL_SCALES_A[4];
extern uint16_t FULL_SCALES_G[4];

/* Comment, if pin SA0 is RESET ----------------------------------------------*/
#define SA0

/* Select address of LSM6DS33 ------------------------------------------------*/
#ifdef SA0
#define LSM6DS33_ADDRESS			0xD7
#else
#define LSM6DS33_ADDRESS			0xD5
#endif /* SA0 */

/* Output data rate selection ------------------------------------------------*/
#define LSM6DS33_ODR_MASK						0b11110000
#define LSM6DS33_ODR_POWER_DOWN 				0b0000
#define LSM6DS33_ODR_12_5HZ						0b0001
#define LSM6DS33_ODR_26HZ						0b0010
#define LSM6DS33_ODR_52HZ						0b0011
#define LSM6DS33_ODR_104HZ						0b0100
#define LSM6DS33_ODR_208HZ						0b0101
#define LSM6DS33_ODR_416HZ						0b0110
#define LSM6DS33_ODR_833HZ						0b0111
#define LSM6DS33_ODR_1660HZ						0b1000
#define LSM6DS33_ODR_3330HZ						0b1001
#define LSM6DS33_ODR_6660HZ						0b1010

/* Orientation of LSM6DS33 ---------------------------------------------------*/
#define LSM6DS33_ORIENT_CFG_MASK				0b00111111
#define LSM6DS33_ORIENT_CFG_XYZ					0b000
#define LSM6DS33_ORIENT_CFG_XZY					0b001
#define LSM6DS33_ORIENT_CFG_YXZ					0b010
#define LSM6DS33_ORIENT_CFG_YZX					0b011
#define LSM6DS33_ORIENT_CFG_ZXY					0b100
#define LSM6DS33_ORIENT_CFG_ZYX					0b101

/* Signs of axes -------------------------------------------------------------*/
#define LSM6DS33_ORIENT_SIGN_POSITIVE_X			0b0<<2
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_X			0b1<<2
#define LSM6DS33_ORIENT_SIGN_POSITIVE_Y			0b0<<1
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_Y			0b1<<1
#define LSM6DS33_ORIENT_SIGN_POSITIVE_Z			0b0
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_Z			0b1

/* Filters for gyroscope -------------------------------------------------------*/
#define LSM6DS33_GYRO_HPF_MASK 					0b01110000
#define LSM6DS33_GYRO_HPF_OFF					0b0
#define LSM6DS33_GYRO_HPF_ON					0b1
#define LSM6DS33_GYRO_HPF_FREQ_1				0b00
#define LSM6DS33_GYRO_HPF_FREQ_2				0b01
#define LSM6DS33_GYRO_HPF_FREQ_3				0b00
#define LSM6DS33_GYRO_HPF_FREQ_4				0b01

/* Filters for accelerometer -------------------------------------------------*/
#define LSM6DS33_A_FILTER_MASK					0b01100000
#define LSM6DS33_A_FILTER_MODE_1				0b00
#define LSM6DS33_A_FILTER_MODE_2				0b01
#define LSM6DS33_A_FILTER_MODE_3				0b10
#define LSM6DS33_A_FILTER_MODE_4				0b11

/* Full scale for measurements of sensors ------------------------------------*/
#define LSM6DS33_FULL_SCALE_MASK				0b00001100
#define LSM6DS33_FULL_SCALE_2G					0b00
#define LSM6DS33_FULL_SCALE_4G					0b10
#define LSM6DS33_FULL_SCALE_8G					0b11
#define LSM6DS33_FULL_SCALE_16G					0b01

#define LSM6DS33_FULL_SCALE_250DPS				0b000
#define LSM6DS33_FULL_SCALE_500DPS				0b001
#define LSM6DS33_FULL_SCALE_1000DPS				0b010
#define LSM6DS33_FULL_SCALE_2000DPS				0b011

/* Addresses of LSM6DS33 registers -------------------------------------------*/
#define LSM6DS33_REGISTER_CTRL1					0x10
#define LSM6DS33_REGISTER_CTRL2					0x11
#define LSM6DS33_REGISTER_CTRL3					0x12
#define LSM6DS33_REGISTER_CTRL7					0x16
#define LSM6DS33_REGISTER_CTRL8					0x17
#define LSM6DS33_REGISTER_ORIENT_CFG			0x0B
#define LSM6DS33_REGISTER_STATUS_REG			0x1E
#define LSM6DS33_REGISTER_OUT_T					0x20
#define LSM6DS33_REGISTER_OUT_G					0x22
#define LSM6DS33_REGISTER_OUT_A					0x28
#define LSM6DS33_REGISTER_ID					0x0F

/* Auxiliary function --------------------------------------------------------*/
void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits);

/* Initialization of LSM6DS33 ------------------------------------------------*/
HAL_StatusTypeDef LSM6DS33_init(I2C_HandleTypeDef hi2c_);

/* Configuration LSM6DS33 ----------------------------------------------------*/
HAL_StatusTypeDef LSM6DS33_config_orientation(uint8_t orient, uint8_t signs);
HAL_StatusTypeDef LSM6DS33_config_filters(uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF);
HAL_StatusTypeDef LSM6DS33_config_full_scale(uint8_t a_FS, uint8_t g_FS);
HAL_StatusTypeDef LSM6DS33_config_perfomance_mode(uint8_t a_ODR, uint8_t g_ODR);

/* Reset sensor --------------------------------------------------------------*/
HAL_StatusTypeDef LSM6DS33_reset();

/* Get measurement from LSM6DS33 ---------------------------------------------*/
HAL_StatusTypeDef LSM6DS33_A_get_measure(float *a);
HAL_StatusTypeDef LSM6DS33_G_get_measure(float *g);
HAL_StatusTypeDef LSM6DS33_T_get_measure(float *t);
HAL_StatusTypeDef LSM6DS33_get_measure(float* a, float *g);
HAL_StatusTypeDef LSM6DS33_get_all_measure(float* a, float *g, float* t);

#endif /* INC_LSM6DS33_HAL_H_ */
