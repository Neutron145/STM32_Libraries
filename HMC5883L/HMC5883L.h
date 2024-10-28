/*
 *******************************************************************************
 * 	@file			HMC5883L.h
 *	@brief			Header file for HMC5883L.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		25.09.2024
 *******************************************************************************
 */

#ifndef HMC5883L_H_
#define HMC5883L_H_

#include "main.h"

/* Instance I2C to which HMC5883L is connected ---------------------------------*/
extern I2C_HandleTypeDef *HMC5883L_hi2c;


#define HMC5883L_ADDRESS 				0x1E<<1

/* */
#define HMC5883L_REG_A					0x00
#define HMC5883L_REG_B 					0x01
#define HMC5883L_REG_MODE				0x02
#define HMC5883L_REG_MODE				0x03 
#define HMC5883L_REG_STATUS				0x09
#define HMC5883L_REG_IND_A				0x0A
#define HMC5883L_REG_IND_B				0x0B
#define HMC5883L_REG_IND_C				0x0C

#define HMC5883L_IND_A					0b01001000
#define HMC5883L_IND_B 					0b00110100
#define HMC5883L_IND_C 					0b00110011

HAL_StatusTypeDef HMC5883L_init(I2C_HandleTypeDef *hi2c_);

HAL_StatusTypeDef HMC5883L_measure();