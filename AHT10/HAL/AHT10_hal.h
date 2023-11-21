/*
 *******************************************************************************
 * 	@file			AHT10_hal.c
 *	@brief			This file provides functions for taking
 *					measurements from the AHT10 using HAL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		21.11.2023
 *******************************************************************************
 */
#ifndef INC_AHT10_HAL_H_
#define INC_AHT10_HAL_H_

#include "main.h"

/* Instance I2C to which AHT10 is connected ----------------------------------*/
extern I2C_HandleTypeDef AHT10_hi2c;
/* Array from store measured data and status from AHT10 ----------------------*/
extern uint8_t AHT10_RxData[7];
/* Array with command and configuration to send in AHT10 for trigger ---------*/
extern uint8_t AHT10_trigger_data[3];

/* Address of AHT10 in I2C ---------------------------------------------------*/
#define AHT10_ADDRESS 					0x38<<1

/* Commands for control AHT10 ------------------------------------------------*/
#define AHT10_COMMAND_INIT 				0b11100001
#define AHT10_COMMAND_TRIGGER 			0b10101100
#define AHT10_COMMAND_RESET				0b10111010

/* Initialization of AHT10 ---------------------------------------------------*/
HAL_StatusTypeDef AHT10_init(I2C_HandleTypeDef hi2c_);

/* Start measuring humidity and temperature ----------------------------------*/
HAL_StatusTypeDef AHT10_trigger_measure();

/* Start reading measured data after trigger command -------------------------*/
HAL_StatusTypeDef AHT10_get_measure(float *t, float *hum);
HAL_StatusTypeDef AHT10_get_T(float *t);
HAL_StatusTypeDef AHT10_get_H(float *hum);

/* Soft reset of sensor ------------------------------------------------------*/
HAL_StatusTypeDef AHT10_soft_reset();

#endif /* INC_AHT10_HAL_H_ */
