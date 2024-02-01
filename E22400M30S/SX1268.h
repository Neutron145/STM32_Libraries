/***************************************************************************//**
 * 	@file			SX1268.h
 *  @brief			Файл обеспечивает передачу данных с помощью стандарта LoRa приемопередатчика SX1268, подключенному по SPI
 * 	@author			Рафаэль Абельдинов
 *  @date 			12.01.2024
 ******************************************************************************/

/**
 * @defgroup SX1268_group SX1268
 * @brief Модуль акселерометра 
 * @details Модуль позволяет конфигурировать формат пакета и параметры модуляции и передавать данные. В будущем будет реализация приема данных.   
 *  		Модуль разрабатывался для LoRa модуля E22 400M30S, подключеному по SPI   
 * 			Перед отправкой данных необходимо настроить формат пакетов, по желанию сконфигурировать параметры модуляции.   
 *			Пример работы с датчиком   
 * 			**ЗДЕСЬ ДОЛЖЕН БЫТЬ КОД**   
 * @{
 */

#ifndef INC_SX1268_H_
#define INC_SX1268_H_

#include "main.h"

/**
 * @name Макрос для определения испольуземой библиотеки
 * @{
 */
#define SX1268_HAL 			//!< Указывает библиотеку STM32, с помощью которой управляется интерфейс SPI. Определите **SX1268_HAL** или **SX1268_LL** для соотвествующей библиотеки. 
/** @} */

#ifdef ISX1268_HAL

#define SPI_TypeDef 			SPI_HandleTypeDef
#define SPI_Transmit(ADR, BUF, BUF_SIZE, TIMEOUT)			HAL_SPI_Transmit(ADR, BUF, BUF_SIZE, TIMEOUT)
#define SPI_Receive(ADR, BUF,BUF_SIZE, TIMEOUT)				HAL_SPI_Receive(ADR, BUF, BUF_SIZE, TIMEOUT)


#elif SX1268_LL

#define SPI_TypeDef 			SPI_TypeDef
#define SPI_Transmit(ADR, BUF, BUF_SIZE, TIMEOUT)			LL_SPI_Transmit(ADR, BUF, BUF_SIZE, TIMEOUT)
#define SPI_Receive(ADR, BUF,BUF_SIZE, TIMEOUT)				LL_SPI_Receive(ADR, BUF, BUF_SIZE, TIMEOUT)

#endif /* SX1268_LL */

extern SPI_TypeDef SX1268_hspi;					//!< Экземпляр SPI, к которому подключен модуль

/**
 * @defgroup SX1268_OPCODES_STATES Коды команд для SX1268
 * @brief Команды для смены состояний, калибровки, конфигурации, чтения и записи данных. Присутсвует конфигурация пакетов только для протокола LoRa
 * @{
 */
#define SX1268_OP_SET_STANDBY 				0x80			//!< Команда перехода в состояние ожидания
#define SX1268_OP_SET_TX					0x83			//!< Команда перехода в состояние отправки данных
#define SX1268_OP_SET_RX					0x82			//!< Команда перехода в состояние приема данных
#define SX1268_OP_SET_SLEEP					0x84			//!< Команда перехода в состояние сна
#define SX1268_OP_CALIBRATE					0x89			//!< Команда калбировки приемопередатчика

#define SX1268_OP_WRITE_REGISTER			0x0D			//!< Команда записи данных в регистр
#define SX1268_OP_READ_REGISTER				0x1D			//!< Команда чтения данных из регистра
#define SX1268_OP_WRITE_BUFFER				0x0E			//!< Команда записи данных в буфер данных приемопередатчика
#define SX1268_OP_READ_BUFFER				0x1E			//!< Команда чтения данных из буфера данных приемопередатчика

#define SX1268_OP_SET_RF_FREQ				0x86			//!< Конфигурация радиочастоты приемопередатчика 
#define SX1268_OP_SET_PACKET_TYPE			0x8A			//!< Конфигурация протокола передачи пакетов (GFSK или LoRa)
#define SX1268_OP_SET_TX_PARAMS				0x8E			//!< Конфигурация состояния приема данных
#define SX1268_OP_SET_MODULATION_PARAMS		0x8B			//!< Конфигурация параметров модуляции
#define SX1268_OP_SET_PACKET_PARAMS			0x8C			//!< Конфигурация параметров пакета 
#define SX1268_OP_SET_BUF_BASE_ADDR			0x8F			//!< Установка адресов на области принятых и отправленных данных в буфере данных приемопередатчика 
/** @} */

/**
 * @defgroup SX1268_SF Значения параметра Spreading Factor 
 * @brief Spreadinf Factor позволяет размазывать передачу данных в диапазоне BandWith. Чем выше значение SF, тем ниже скорость передачи (и потребление энергии) и выше сила сигнала.
 * @{
 */
#define SX1268_SF5							0x05			
#define SX1268_SF6							0x06
#define SX1268_SF7							0x07
#define SX1268_SF8							0x08
#define SX1268_SF9							0x09
#define SX1268_SF10							0x0A
#define SX1268_SF11							0x0B
#define SX1268_SF12							0x0C
/** @} */


/**
 * @defgroup SX1268_BW Значения параметра BandWith
 * @brief Bandwith определяет диапазон частот, в котором передается сигнал. Чем шире Bandwith, тем выше скорость передачи, но ниже сила сигнала. 
 * @{
 */
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
/** @} */

/**
 * @defgroup SX1268_CR Значения параметра Coding Rate
 * @brief Coding Rate определяет скорость кодирования при методе Forward Error Correction. Чем выше скорость, тем сильнее помехоустойчивость, но ниже скорость передачи данных. В типичных условиях достаточно скорость 4_5
 * @{
 */
#define SX1268_LORA_CR_4_5					0x01
#define SX1268_LORA_CR_4_6					0x02
#define SX1268_LORA_CR_4_7					0x03
#define SX1268_LORA_CR_4_8					0x04
/** @} */

/**
 * @defgroup SX1268_LDRO Оптимизация передачи данных при низком потоке данных
 * @brief Оптимизация повышает надежность передачи данных при отправке и приеме больших, но редких сообщениях
 * @{
 */
#define SX1268_LDRO_OFF						0x00		
#define SX1268_LDRO_ON						0x01	
/** @} */

/**
 * @defgroup SX1268_HEADER_TYPE Тип заголовка пакета
 * @brief Заголовок может быть явным или неявным. Неявный заголовок используется если длина сообщения, CR и CRC известны. Он повышает скорость передачи сообщений
 * @{
 */
#define SX1268_PREAMBLE_EXPLICIT			0x00		//!< Явный заголовок. В нем указывается длина сообщения, CR и CRC сообщения.
#define SX1268_PREAMBLE_IMPLICIT			0x01		//!< Неявный заголовок. Т.е. отсутсвие заголовка в пакете
/** @} */

/**
 * @defgroup SX1268_CRC Контрольная сумма пакета
 * @brief Контрольная сумма позволяет проверить целостность файлов пакета
 * @{
 */
#define SX1268_CRC_OFF						0x00		//!< Отключение проверки контрольной суммы пакеты
#define SX1268_CRC_ON						0x01		//!< Включение проверки контрольной суммы пакета
/** @} */

/** 
 * @defgroup SX1268_IQ IQ
 * @brief Неизвестно что это 
 * @{
 */
#define SX1268_STANDART_IQ					0x00		//!< ??
#define SX1268_INVERT_IQ 					0x01		//!< ??
/** @} */


#define SX1268_PACKET_TYPE_LORA				0x01		//!< В данном модуле пристуствует только протокол LoRa

/**
 * @defgroup SX1268_RAMP_TIME Значения параметра Ramp Time
 * @brief Непонятно на что влияющий параметр
 * @{
 */
#define SX1268_SET_RAMP_10U					0x00
#define SX1268_SET_RAMP_20U					0x01
#define SX1268_SET_RAMP_40U					0x02
#define SX1268_SET_RAMP_80U					0x03
#define SX1268_SET_RAMP_200U				0x04
#define SX1268_SET_RAMP_800U				0x05
#define SX1268_SET_RAMP_1700U				0x06
#define SX1268_SET_RAMP_3400U				0x07
/** @} */ 

#define SX1268_RX_GAIN_REGISTER				0x08AC				//!< Адрес регистра режима получения данных приемопередатчиком

/**
 * @defgroup SX1268_GAIN_MODE Режимы получения данных приемопередатчиком
 * @brief От режима зависит потребление энергии и чувствительность приемопередатчика в состоянии Rx
 * @{
 */
#define SX1268_RX_POWER_SAVING_GAIN_MODE	0x94				//!< Режим сбережения энергии и пониженной чувствительности
#define SX1268_RX_BOOSTED_GAIN_MODE			0x96				//!< Режим повышенного потребления энергии и высокой чувствительности
/** @} */


/** 
 * @brief Инициализация приемопередатчик
 * @details На этапе инициализации определяется экземпляр SPI, к которому подключен SX1268 и устанавливается протокол LoRa. 
 * @param hspi_ Экземпляр SPI, к которому подключен подключен приемопередатчик.
 * @retval status Результат инициализации SPI. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
void SX1268_init(SPI_HandleTypeDef hspi_);


/** 
 * @brief Конфигурация радиочастоты приемопередатчика
 * @details Отправляет команду и указанную частоту, состоящую из 4 байтов
 * @param rfFreq Массив, определяющий частоту, размеров в 4 байта
 * @retval status Статус выполнения конфигурации. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef SX1268_config_rfFreq(uint8_t* rfFreq);

/**
 * @brief Конфигурация параметров модуляции протокола LoRa
 * @param SF Значение Spreading factor. Принимает значения @ref SX1268_SF "CF"
 * @param BW Значение Badnwith. Принимает значения @ref SX1268_BW "BW"
 * @param CR Значение Coding rate. Принимает значения @ref SX1268_CR "CR"
 * @param LDRO Значение Low data rate optimization. Принимает значения @ref SX1268_LDRO "LDRO"
 * @retval status Статус выполнения конфигурации. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef SX1268_config_modulation_params(uint8_t SF, uint8_t BW, uint8_t CR, uint8_t LDRO);

/**
 * @brief Определение указателя на принимаемые и отправляемые данные в буфере данных SX1268
 * @details Позволяет разделить буфер данных на буфер принимаемых данных (в этот буфер будут приходить все принятые данные) и буфер отправляемых данных (сюда помещаются все отправляемые данные). По стандарту оба указателя указывают на нулевой адрес буфера   
 * 			Размер буфера - 256 байт. Соотвественно изначально мы можем отправлять или принимать пакеты до 256 байт. В случае изменения адресов, мы изменяем максимальный размер пакетов, которые может отправить или принять.
 * @param TxAddr Адрес области памяти для хранения отправлениях данных. Принимаемые значния от 0 до 256
 * @param RxAddr Адрес области памяти для хранения принятых данных. Принимаемые значения от 0 до 256
 * @retval status Статус выполнения конфигурации. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */ 
HAL_StatusTypeDef SX1268_config_baseAddr(uint8_t TxAddr, uint8_t RxAddr);

/**
 * @brief Конфигурация мощности сигнала и ramping time для радиопередатчика в режиме TX
 * @details 
 * @param power Мощность передачи сигнала, измеряется в dBm.   
 * 				Принимаемые значения:   
 * 				от 0xEF (-17 dBm) до 0x0E (+14 dBm) с шагом 1dBm, если выбран режим low power PA   
 * 				от 0xF7 (+9 dBm) до 0x16 (+22 dBm) с шагом 1 dBm, если выбран режим high power PA
 * @param rampTime Принимает значения @ref SX1268_RAMP_TIME "ramp time"
 * @retval status Статус выполнения конфигурации. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_Status SX1268_config_tx_params(uint8_t power, unit8_t rampTime);

/**
 * @brief Конфигурация параметров пакетов
 * @details Длина преамбулы используется для синхронизации входящего пакета с приемником. Если длина преамбулы неизвеста или варьируется, то устанавливается максимально возможное значение.   
 * @param preamble_length Длина преамбулы сообщения. По умолчнанию используется 12 символов. Принимаемые значения: от 0 до 65535
 * @param header_type Тип заголовка. Принимает значения @ref SX1268_HEADER_TYPE "header type"
 * @param payload_length Длина сообщения. В даташите нет ограничений на это значение
 * @param CRC Контрольная сумма пакета. Добавляется к концу пакета после сообщения. Принимает значения @ref SX1268_CRC "CRC"
 * @param invert_IQ Непонятно что это
 * @retval status Статус выполнения конфигурации. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef SX1268_config_packet_params(uint16_t preamble_length, uint8_t header_type, uint8_t payload_length, uint8_t CRC, uint8_t invert_IQ);

/**
 * @brief
 * @details
 * @param buffer
 * @param buffer_size
 * @retval
 */
HAL_StatusTypeDef SX1268_send_message(uint8_t* buffer, int buffer_size);

#endif /* INC_SX1268_H_ */

/** @} */