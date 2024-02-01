#include <SX1268.h>

SPI_TypeDef SX1268_hspi;


void SX1268_init(SPI_HandleTypeDef hspi_) {
	SX1268_hspi = hspi_;
	uint8_t packet_type_op[2] = {SX1268_OP_SET_PACKET_TYPE, SX1268_PACKET_TYPE_LORA};
	SPI_Transmit(&SX1268_hspi, packet_type_op, 2, 0xFF);

}


HAL_StatusTypeDef SX1268_config_rfFreq(uint8_t* rfFreq) {
	uint8_t rf_freq_op[4] = {SX1268_OP_SET_RF_FREQ, rfFreq[0], rfFreq[1], rfFreq[2], rfFreq[3]};
	return SPI_Transmit(&SX1268_hspi, rf_freq_op, 4, 0xFF);
}


HAL_StatusTypeDef SX1268_config_modulation_params(uint8_t SF, uint8_t BW, uint8_t CR, uint8_t LDRO) {
	uint8_t mod_params_op[9] = {SX1268_OP_SET_MODULATION_PARAMS, SF, BW, CR, LDRO, 0, 0, 0, 0 };
	return SPI_Transmit(&SX1268_hspi, mod_params_op, 9, 0xFF);
}

/*
 * @brief	Function sets the base addresses in the data buffer.
 * @param 	txAddr - base addresses for data to transmit.
 * @param 	rxAddr - base addresses for received data.
 * @retval	HAL_OK -	Command successfully sent to SX1268.
 * 			HAL_BUSY -	SPI line is currently busy.
 * 			HAL_ERROR - SPI line returned an error.
 */
HAL_StatusTypeDef SX1268_config_baseAddr(uint8_t txAddr, uint8_t rxAddr) {
	if((txAddr < 0 || txAddr > 0xFF) || (rxAddr < 0 || rxAddr > 0xFF)) { return HAL_ERROR; }
	uint8_t base_addres_op[3] = {SX1268_OP_SET_BUF_BASE_ADDR, txAddr, rxAddr};
	return SPI_Transmit(&SX1268_hspi, base_addres_op, 3, 0xFF);
} 


/*
 * @brief	Function sets the TX output power and the TX ramping time.
 * @param 	power - output power of radio in dBm.
 * 			@arg -17 (0xEF) to +14 (0x0E) dBm by step of 1dB if low power PA is selected
 * 			@arg 9 (0xF7) to +22 (0x16) dBm by step of 1dB if high power PA is selected
 * @retval	HAL_OK -	Command successfully sent to SX1268.
 * 			HAL_BUSY -	SPI line is currently busy.
 * 			HAL_ERROR - SPI line returned an error.
 */
HAL_Status SX1268_config_tx_params(uint8_t power, unit8_t rampTime) {
	uint8_t tx_params_op[3] = {SX1268_OP_SET_TX_PARAMS, power, rampTime};
	return SPI_Transmit(&SX1268_hspi, tx_params_op, 3, 0xFF);
}

/*
 * @brief	Used to set the parameters of the packet handling block.
 * 		 	params - packet parameters. An array of 6 elements.
 * @param	params[0-1] - Preamble length
 * 			@arg 0x0001-0xFFFF
 * @param   params[2] - Header type
 * 			@arg SX1268_PREAMBLE_EXPLICIT
 * 			@arg SX1268_PREAMBLE_IMPLICIT
 * @param	params[3] - Payload length
 * 			@arg - 0x00-0xFF
 * @param	params[4] - CRC type
 * 			@arg SX1268_CRC_ON
 * 			@arg SX1268_CRC_OFF
 * @param 	params[5] - Invert IQ
 * 			@arg SX1268_STANDART_IQ
 * 			@arg SX1268_INVERT_IQ
 * @retval	HAL_OK -	Command successfully sent to SX1268.
 * 			HAL_BUSY -	SPI line is currently busy.
 * 			HAL_ERROR - SPI line returned an error.
 */
HAL_StatusTypeDef SX1268_config_packet_params(uint16_t preamble_length, uint8_t header_type, uint8_t payload_length, uint8_t CRC, uint8_t invert_IQ) {
	uint8_t pack_params_op[9] = {SX1268_OP_SET_PACKET_PARAMS, preamble_length, header_type, payload_length, CRC, invert_IQ,
								0, 0 };
	return SPI_Transmit(&SX1268_hspi, pack_params_op, 9, 0xFF);
}

/*
 * @brief	Function write message into buffer and set TX mode for transmit this message
 * @param 	buffer - message
 * @param 	buffer_size - message size
 * @retval	HAL_OK -	Command successfully sent to SX1268.
 * 			HAL_ERROR - SPI line returned an error.
 */
HAL_StatusTypeDef SX1268_send_message(uint8_t* buffer, int buffer_size) {
	uint8_t write_buffer_op[2] = {SX1268_OP_WRITE_BUFFER, 0x00};
	if(SPI_Transmit(&SX1268_hspi, write_buffer_op , buffer_size, 0xFF) != HAL_OK){
		return HAL_ERROR;
	}
	if(SPI_Transmit(&SX1268_hspi, buffer, buffer_size, 0xFF) != HAL_OK){
		return HAL_ERROR;
	}
	uint8_t set_tx_op[4] = {SX1268_OP_SET_TX, 0x00, 0x00, 0x00};
	return SPI_Transmit(&SX1268_hspi, set_tx_op , 4, 0xFF);
}
