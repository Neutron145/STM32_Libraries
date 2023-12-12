/*
 *******************************************************************************
 * 	@file			SX1268_hal.h
 *	@brief			Header file for SX1268_hal.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		08.12.2023
 *******************************************************************************
 */

#ifndef INC_SX1268_HAL_H_
#define INC_SX1268_HAL_H_

#include "main.h"

/* Opcodes for switching states ----------------------------------------------*/
#define SX1268_OP_SET_STANDBY 				0x80
#define SX1268_OP_SET_TX					0x83
#define SX1268_OP_SET_RX					0x82
#define SX1268_OP_SET_SLEEP					0x84
#define SX1268_OP_SET_FS					0xC1
#define SX1268_OP_CALIBRATE					0x89
#define SX1268_OP_CALIBRATE_IMAGE			0x98

/* Opcodes for read and write data -------------------------------------------*/
#define SX1268_OP_WRITE_REGISTER			0x0D
#define SX1268_OP_READ_REGISTER				0x1D
#define SX1268_OP_WRITE_BUFFER				0x0E
#define SX1268_OP_READ_BUFFER				0x1E

/* Opcodes for configure SX1268 ----------------------------------------------*/
#define SX1268_OP_SET_RF_FREQ				0x86
#define SX1268_OP_SET_PACKET_TYPE			0x8A
#define SX1268_OP_SET_TX_PARAMS				0x8E
#define SX1268_OP_SET_MODULATION_PARAMS		0x8B
#define SX1268_OP_SET_PACKET_PARAMS			0x8C
#define SX1268_OP_SET_BUF_BASE_ADDR			0x8F

/* Modulation params ---------------------------------------------------------*/
#define SX1268_SF5							0x05
#define SX1268_SF6							0x06
#define SX1268_SF7							0x07
#define SX1268_SF8							0x08
#define SX1268_SF9							0x09
#define SX1268_SF10							0x0A
#define SX1268_SF11							0x0B
#define SX1268_SF12							0x0C

#define SX1268_LORA_BW_7					0x00
#define SX1268_LORA_BW_10					0x08
#define SX1268_LORA_BW_15					0x01
#define SX1268_LORA_BW_20					0x09
#define SX1268_LORA_BW_31					0x02
#define SX1268_LORA_BW_41					0x0A
#define SX1268_LORA_BW_62					0x03
#define SX1268_LORA_BW_125					0x04
#define SX1268_LORA_BW_250					0x05
#define SX1268_LORA_BW_500					0x06

#define SX1268_LORA_CR_4_5					0x01
#define SX1268_LORA_CR_4_6					0x02
#define SX1268_LORA_CR_4_7					0x03
#define SX1268_LORA_CR_4_8					0x04

#define SX1268_LDRO_OFF						0x00
#define SX1268_LDRO_ON						0x01

/* Packet params -------------------------------------------------------------*/
#define SX1268_PREAMBLE_EXPLICIT			0x00
#define SX1268_PREAMBLE_IMPLICIT			0x01

#define SX1268_CRC_OFF						0x00
#define SX1268_CRC_ON						0x01

#define SX1268_STANDART_IQ					0x00
#define SX1268_INVERT_IQ 					0x01

/* Modes for SX1268 ----------------------------------------------------------*/
#define SX1268_PACKET_TYPE_GFSK				0x00
#define SX1268_PACKET_TYPE_LORA				0x01

/* Tx params -----------------------------------------------------------------*/
#define SX1268_SET_RAMP_10U					0x00
#define SX1268_SET_RAMP_20U					0x01
#define SX1268_SET_RAMP_40U					0x02
#define SX1268_SET_RAMP_80U					0x03
#define SX1268_SET_RAMP_200U				0x04
#define SX1268_SET_RAMP_800U				0x05
#define SX1268_SET_RAMP_1700U				0x06
#define SX1268_SET_RAMP_3400U				0x07

/* Register addresses --------------------------------------------------------*/
#define SX1268_RX_GAIN_REGISTER				0x08AC

/* Power modes for radio -----------------------------------------------------*/
#define SX1268_RX_POWER_SAVING_GAIN_MODE	0x94
#define SX1268_RX_BOOSTED_GAIN_MODE			0x96

/* Instance SPI to which SX1268 is connected ---------------------------------*/
extern SPI_HandleTypeDef SX1268_hspi;

/* Determining SPI to which SX1268 is connected and selecting LoRa mode ------*/
void SX1268_init(SPI_HandleTypeDef hspi_);
/* Radio module frequency configuration --------------------------------------*/
HAL_StatusTypeDef SX1268_config_rfFreq(uint8_t* rfFreq);
/* Configuration of the modulation parameters --------------------------------*/
HAL_StatusTypeDef SX1268_config_modulation_params(uint8_t* params);
/* Configuration of the address in the buffer for rx and tx data -------------*/
HAL_StatusTypeDef SX1268_config_baseAddr(uint8_t TxAddr, uint8_t RxAddr);
/* Configuration of the radio module for data transmission -------------------*/
HAL_Status SX1268_config_tx_params(uint8_t power, unit8_t rampTime);
/* Configuration of transmitted data packets ---------------------------------*/
HAL_StatusTypeDef SX1268_config_packet_params(uint8_t* params);
/* Transmit message ----------------------------------------------------------*/
HAL_StatusTypeDef SX1268_send_message(uint8_t* buffer, int buffer_size);

#endif /* INC_SX1268_HAL_H_ */
