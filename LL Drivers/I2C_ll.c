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
 * @brief Interntal function for error tracking
 * @param I2Cx The I2C instance to which the device is connected.
 * @param bit Bit whose value needs to check in SR1 register
 * @retval HAL_OK		- In 10ms I2C responded to the command
 * 		   HAL_ERROR 	- In 10ms I2C not responded to the command
 */
HAL_StatusTypeDef __wait_flag(I2C_TypeDef *I2Cx, uint8_t bit) {
	uint32_t start_wait = uwTick;
	while((READ_BIT(I2Cx->SR1, bit) != bit)) {
		if(HAL_GetTick() - start_wait > 10){
			return HAL_OK;
		}
	}
	return HAL_ERROR;
}

/*
 * @brief Read bytes from register in device on I2Cx or read data from DR
 * @param I2Cx The I2C instance to which the device is connected.
 * @param device_address Address of device from which you want to read data.
 * @param first_frame  Address of register from which you want to read data.
 * 						@arg 0x0000	for read data from DR
 * 						@arg other	for read data from register
 * @param buffer Buffer into which data is read.
 * @param bytes_count Number of bytes to read.
 * @retval HAL_OK		- Successful reading of data on I2C
 * 		   HAL_ERROR	- Error reading data on I2C
 */
HAL_StatusTypeDef LL_I2C_Master_Receive(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t first_frame, uint8_t *buffer, uint8_t bytes_count) {
	LL_I2C_DisableBitPOS(I2Cx);
	//Set ACK, send Start signal and waiting for the start bit to be set
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	//If register address is not 0, read data into buffer from register. Else read data from DR
	if(first_frame != 0) {
		LL_I2C_GenerateStartCondition(I2Cx);
		if(__wait_flag(I2Cx, I2C_SR1_SB) != HAL_OK) return HAL_ERROR;
		(void) I2Cx->SR1;
		//Send address of device from which you need to read data
		LL_I2C_TransmitData8(I2Cx, device_address | WRITE_SIGNAL);
		//Waiting for the device with the required address to be found
		if(__wait_flag(I2Cx, I2C_SR1_ADDR) != HAL_OK) return HAL_ERROR;
		(void) I2Cx->SR1;
		(void) I2Cx->SR2;
		//Send to device address of register from which you need to read data
		if(first_frame == (first_frame & 0b11111111)) {
			LL_I2C_TransmitData8(I2Cx, (uint8_t) first_frame);
		}
		else {
			LL_I2C_TransmitData8(I2Cx, (uint8_t) (first_frame >> 8));
			if(__wait_flag(I2Cx, I2C_SR1_TXE) != HAL_OK) return HAL_ERROR;
			LL_I2C_TransmitData8(I2Cx, (uint8_t) first_frame);
		}
		//Waiting for data from DR moved into shift register
		if(__wait_flag(I2Cx, I2C_SR1_TXE) != HAL_OK) return HAL_ERROR;
	}
	//Send Restart signal and waiting for the start bit to be set
	LL_I2C_GenerateStartCondition(I2Cx);
	if(__wait_flag(I2Cx, I2C_SR1_SB) != HAL_OK) return HAL_ERROR;
	(void) I2Cx->SR1;
	//Send a command to the device to send data from the register
	LL_I2C_TransmitData8(I2Cx, device_address | READ_SIGNAL);
	//Waiting for the device with the required address to be found
	if(__wait_flag(I2Cx, I2C_SR1_ADDR) != HAL_OK) return HAL_ERROR;
	LL_I2C_ClearFlag_ADDR(I2Cx);
	uint16_t i;
	//Save received data from device into buffer
	for(i=0;i<bytes_count-1;i++)
	{
	  if(__wait_flag(I2Cx, I2C_SR1_RXNE) != HAL_OK) return HAL_ERROR;
	  buffer[i] = LL_I2C_ReceiveData8(I2Cx);
	}
	//Set NACK and send Stop signal
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2Cx);
    //Waiting for last byte received from device and save it into buffer
    if(__wait_flag(I2Cx, I2C_SR1_RXNE) != HAL_OK) return HAL_ERROR;
    buffer[i] = LL_I2C_ReceiveData8(I2Cx);
    return HAL_OK;
}


/*
 * @brief Transmit data to device via I2Cx. Data can be opcode or data to write to register
 * @param I2Cx The I2C instance to which the device is connected.
 * @param device_address Address of the device to transmit data
 * @param first_packet Address of the register to write data or opcode
 * @param second_packet Data to be written to the register or params for opcode
 * @param bytes_count Number of bytes of second_packet array.
 * @retval HAL_OK		- Successful reading of data on I2C
 * 		   HAL_ERROR	- Error reading data on I2C
 */
HAL_StatusTypeDef LL_I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint8_t device_address, uint16_t first_frame, uint8_t *second_frame, uint8_t bytes_count) {
	LL_I2C_DisableBitPOS(I2Cx);
	//Set ACK, send Start signal and waiting for the start bit to be set
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(I2Cx);

	if(__wait_flag(I2Cx, I2C_SR1_SB) != HAL_OK) return HAL_ERROR;
	(void) I2Cx->SR1;
	//Sending the address of the device to which data needs to be written in 7 bit mode
	LL_I2C_TransmitData8(I2Cx, device_address | I2C_WRITE_SIGNAL);
	//Waiting for the device with the required address to be found
	if(__wait_flag(I2Cx, I2C_SR1_ADDR) != HAL_OK) return HAL_ERROR;
	(void) I2Cx->SR1;
	(void) I2Cx->SR2;
	//Send to device address of register to which data need to be written or opcode
	if(first_frame == (first_frame & 0b11111111)) {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) first_frame);
	}
	else {
		LL_I2C_TransmitData8(I2Cx, (uint8_t) (first_frame >> 8));
		if(__wait_flag(I2Cx, I2C_SR1_TXE) != HAL_OK) return HAL_ERROR;
		LL_I2C_TransmitData8(I2Cx, (uint8_t) first_frame);
	}
	//Waiting for data from DR moved into shift register
	if(__wait_flag(I2Cx, I2C_SR1_TXE) != HAL_OK) return HAL_ERROR;
	//Sending data or params for opcode from the second frame into DR to write data in device
	for(uint16_t i = 0; i < bytes_count; i++) {
		LL_I2C_TransmitData8(I2Cx, second_frame[i]);
		if(__wait_flag(I2Cx, I2C_SR1_TXE) != HAL_OK) return HAL_ERROR;
	}
	//Send Stop signal
	LL_I2C_GenerateStopCondition(I2Cx);
}
