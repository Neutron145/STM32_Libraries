/*
 *******************************************************************************
 * 	@file			I2C_ll.h
 *	@brief			This file provides functions for using I2C via LL.
 *
 *	@author			Rafael Abeldinov
 *  @created 		08.11.2023
 *******************************************************************************
 */

#include "I2C_ll.h"


/*
 * @brief Read bytes from register in device on I2Cx
 * @param I2Cx The I2C instance to which the device is connected.
 * @param device_address Address of device from which you want to read data.
 * @param reg_address  Address of register from which you want to read data.
 * @param buffer Buffer into which read data is read.
 * @param bytes_count Number of bytes to read.
 * @retval None.
 */
void I2C_read_bytes(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count) {
	LL_I2C_DisableBitPOS(I2Cx);
	//Set ACK, send Start signal and waiting for the start bit to be set
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(I2Cx);
	while(!LL_I2C_IsActiveFlag_SB(I2Cx)){};
	(void) I2Cx->SR1;
	//Send address of device from which you need to read data
	LL_I2C_TransmitData8(I2Cx, device_address | WRITE_SIGNAL);
	//Waiting for the device with the required address to be found
	while(!LL_I2C_IsActiveFlag_ADDR(I2Cx)){};
	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	//Send to device address of register from which you need to read data
	LL_I2C_TransmitData8(I2Cx, (uint8_t) reg_address);
	//Waiting for data from DR moved into shift register
	while(!LL_I2C_IsActiveFlag_TXE(I2Cx)){};
	//Send Start signal and waiting for the start bit to be set
	LL_I2C_GenerateStartCondition(I2Cx);
	while(!LL_I2C_IsActiveFlag_SB(I2Cx)){};
	(void) I2Cx->SR1;
	//Send a command to the device to send data from the register
	LL_I2C_TransmitData8(I2Cx, device_address | READ_SIGNAL);
	//Waiting for the device with the required address to be found
	while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)){};
	LL_I2C_ClearFlag_ADDR(I2Cx);
	uint16_t i;
	//Save received data from device into buffer
	for(i=0;i<bytes_count-1;i++)
	{
	  while(!LL_I2C_IsActiveFlag_RXNE(I2Cx)){};
	  buffer[i] = LL_I2C_ReceiveData8(I2Cx);
	}
	//Set NACK and send Stop signal
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2Cx);
    //Waiting for last byte received from device and save it into buffer
    while(!LL_I2C_IsActiveFlag_RXNE(I2Cx)){};
    buffer[i] = LL_I2C_ReceiveData8(I2Cx);
}
/*
 * @brief Write bytes from register in device on I2Cx
 * @param I2Cx The I2C instance to which the device is connected.
 * @param device_address address of the device to write data to
 * @param reg_address  Address of the register to write data to
 * @param buffer Data to be written to the register
 * @param bytes_count Number of bytes to write.
 * @retval None.
 */
void I2C_write_bytes(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t reg_address, uint8_t *buffer, uint8_t bytes_count) {
	LL_I2C_DisableBitPOS(I2Cx);
	//Set ACK, send Start signal and waiting for the start bit to be set
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(I2Cx);
	while(!LL_I2C_IsActiveFlag_SB(I2Cx));
	(void) I2Cx->SR1;
	//Sending the address of the device to which data needs to be written
	LL_I2C_TransmitData8(I2Cx, device_address | WRITE_SIGNAL);
	//Waiting for the device with the required address to be found
	while(!LL_I2C_IsActiveFlag_ADDR(I2Cx));
	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	//Send to device address of register to which data need to be written
	LL_I2C_TransmitData8(I2Cx, (uint8_t) reg_address);
	//Waiting for data from DR moved into shift register
	while(!LL_I2C_IsActiveFlag_TXE(I2Cx)){};
	//Sending data from the buffer into DR to write data in device
	for(uint16_t i = 0; i < bytes_count; i++) {
		LL_I2C_TransmitData8(I2Cx, buffer[i]);
		while(!LL_I2C_IsActiveFlag_TXE(I2Cx));
	}
	//Send Stop signal
	LL_I2C_GenerateStopCondition(I2Cx);
}
