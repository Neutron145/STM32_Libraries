/*
 *******************************************************************************
 * 	@file			I2C_ll.h
 *	@brief			Header file for I2C_ll.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		08.11.2023
 *******************************************************************************
 */

#ifndef I2C_H_
#define I2C_H_

#include "main.h"

/* Commands to devices -------------------------------------------------------*/
#define I2C_READ_SIGNAL 	0x01
#define I2C_WRITE_SIGNAL 	0x00

/* Read bytes from device or from registers ----------------------------------*/
void LL_I2C_Master_Receive(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count);

/* Write bytes in registers of devices or send opcode with params ------------*/
void LL_I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count);

#endif /* I2C_H_ */
