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
#define READ_SIGNAL 	0x01
#define WRITE_SIGNAL 	0x00

/* Read bytes from device ----------------------------------------------------*/
void I2C_read_bytes(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count);
/* Write bytes in devices ----------------------------------------------------*/
void I2C_write_bytes(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count);

#endif /* I2C_H_ */
