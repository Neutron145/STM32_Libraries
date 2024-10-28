/***************************************************************************//**
 * 	@file			AHT10.h
 *  @brief			Файл обеспечивает работу датчика AHT10, подключенному к микроконтроллеру STM32F103 или STM32F411 по I2C.
 * 	@author			Рафаэль Абельдинов
 *  @date 			16.12.2023
 ******************************************************************************/

/**
 * @defgroup AHT10_group AHT10
 * @brief			Модуль для датчика AHT10. Обеспечивает программную перезагрузку датчика и измерение данных. Датчик подключается к I2C.
 * @details 		Модуль работает с библиотеками LL и HAL. Для выбора библиотеки необходимо использовать макрос и при использовании LL добавить в проект модуль I2C LL.  
 * 					Для работы достаточно вызвать функцию инициализации, вызвать измерение и считать полученные данные:
 * 					\code{.c}  
 * 					float t, hum;  
 * 					AHT10_init();  
 * 					AHT10_trigger_measure();  
 * 					while(1) {  
 * 						AHT10_get_measure(&t, &hum);  
 *							HAL_Delay(1000);   
 * 					}  
 * 					\endcode
 * @{
 */
#ifndef INC_AHT10_HAL_H_
#define INC_AHT10_HAL_H_

#include "main.h"

/** 
 * @name Макрос определения используемой библиотеки
 * @{
 */
#define AHT10_HAL 			//!< Указывает библиотеку STM32, с помощью которой управляется интерфейс I2C. Определите **AHT10_HAL** или **AHT10_LL** в зависимости от используемой библиотеки.
/** @} */	

#ifdef AH10_HAL

#define I2C_TypeDef 	I2C_HandleTypeDef
#define I2C_Master_Receive(I2C, DEV_ADR, BUF,BUF_SIZE, TIMEOUT) 		HAL_I2C_Master_Receive(I2C,DEV_ADR,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Master_Transmit(I2C, DEV_ADR, BUF, BUF_SIZE, TIMEOUT)		HAL_I2C_Master_Transmit(I2C,DEV_ADR,BUF,BUF_SIZE,TIMEOUT)


#elif AHT10_LL

#include "I2C_ll.h"
#define I2C_TypeDef 	I2C_TypeDef
#define I2C_Master_Receive(I2C, DEV_ADR, BUF, BUF_SIZE, TIMEOUT)		LL_I2C_Master_Receive(I2C,DEV_ADR,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Master_Transmit(I2C, DEV_ADR, BUF, BUF_SIZE, TIMEOUT)		LL_I2C_Master_Transmit(I2C,DEV_ADR,BUF,BUF_SIZE,TIMEOUT)

#endif /* AHT10_LL */


extern I2C_TypeDef* AHT10_hi2c;					//!< Экземпляр I2C, к которому подключен датчиик влажности. Тип экземпляра зависит от библиотеки.

extern uint8_t AHT10_RxData[7];					//!< Массив для получения данных с датчика.
extern uint8_t AHT10_trigger_data[3];			//!< Массив для отправки данных датчику.

/**
 * @name Макросы управления
 * @{
 */
#define AHT10_ADDRESS 					0x38<<1 			//!< Адрес датчика на шине I2C.
#define AHT10_COMMAND_INIT 				0b11100001			//!< Код операции инициации измерений влажности и температуры.
#define AHT10_COMMAND_TRIGGER 			0b10101100			//!< Код операции инициации измерений влажности и температуры.
#define AHT10_COMMAND_RESET				0b10111010			//!< Код операции программной перезагрузки датчика.
/** @} */

/** 
 * @brief Инициализация датчика.
 * @details Функция инициализации датчика. Используется перед измерением данных. Отправляет код операции инициализации датчику.  
 * @param hi2c_ Экземпляр I2C_TypeDef, к которому подключен датчик.
 * @retval status Статус I2C после инициализации датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_init(I2C_HandleTypeDef *hi2c_);

/** 
 * @brief Функция выполнения измерений влажности и темепратуры датчиком.  
 * @details Функция отправляет код операции измерения влажности и температуры вместе со стандартными параметрами. Данные измеряются ~75мс.
 * @retval status Статус I2C после отправки кода операции датчику. Может быть **HAL_OK**. **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_trigger_measure();

/** 
 * @brief Чтения измеренных влажности и температуры с датчика.
 * @details Функция считывает данные из AHT10 в буфер **AHT10_RxData**. Данные из этого буфера преобразуются в величины температуры и влажности по стандартным формулам. 
 * @param t Возвращаемая величина температуры в градусах Цельсия
 * @param hum Возвращаемая величина влажности в %
 * @retval status Статус I2C после сохранения полученных данных по I2C в буфер. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_get_measure(float *t, float *hum);

/** 
 * @brief Чтения измеренной температуры с датчика.
 * @details Функция считывает из AHT10 в буфер **AHT10_RxData**. Данные из этого буфера преобразуются в величину температуры по стандртным формулам.
 * @param t Возвращаемая величина температуры в градусах Цельсия
 * @retval status Статус I2C после сохранения данных температуры по I2C в буфер. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_get_T(float *t);

/** 
 * @brief Чтения измеренной влажности с датчика.
 * @details Функция считывает из AHT10 в буфер **AHT10_RxData**. Данные из этого буфера преобразуются в величину влажности по стандртным формулам.
 * @param t Возвращаемая величина влажности в %
 * @retval status  Статус I2C после сохранения данных влажности по I2C в буфер. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_get_H(float *hum);

/** 
 * @brief Программная перезагрузка датчика.
 * @details Отправляет код операции перезагрузки датчика. Перезагрузка длится ~20мс.
 * @retval status Статус I2C после отправки кода операции датчику. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef AHT10_soft_reset();

#endif /* INC_AHT10_HAL_H_ */

/** @} */
