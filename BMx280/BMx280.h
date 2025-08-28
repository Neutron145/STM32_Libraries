/**
 * @defgroup BMx280 
 * @brief Библиотека для работы с BME/BMP280. Позволяет проводить инициализацию, базовую конфигурацию,
 * 	чтение измеренных данных. 
 */
/**
 * 	@file BMx280.h
 *  @ingroup BMx280
 *	@brief API библиотеки для использования в своих проектах  
 *  
 *	@author Rafael Abeldinov
 *  @date 24.08.2025
 */

#ifndef BMx280_H_
#define BMx280_H_

#include "main.h"

/** @cond UNNECESSARY */
#ifdef BMx280_HAL
#define I2C_TypeDef 		I2C_HandleTypeDef
#define I2C_Mem_Write(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)		HAL_I2C_Mem_Write(ADR,DEV_ADR,REG_ADR,I2C_MEMADD_SIZE_8BIT,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Mem_Read(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)			HAL_I2C_Mem_Read(ADR,DEV_ADR,REG_ADR,I2C_MEMADD_SIZE_8BIT,BUF,BUF_SIZE,TIMEOUT)

#elif BMx280_LL
#include "I2C_ll.h"
#define I2C_TypeDef 		I2C_TypeDef
#define I2C_Mem_Write(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)		LL_I2C_Mem_Write(ADR,DEV_ADR,REG_ADR,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Mem_Read(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)			LL_I2C_Mem_Read(ADR,DEV_ADR,REG_ADR,BUF,BUF_SIZE,TIMEOUT)
#endif /** BMx280_LL */

/* Calibration data for temperature and pressure */
typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} BMx280_calibration_data;
/** @endcond */

extern uint32_t BMx280_refPressure; //!< Давление при инициализации датчика для вычисления высоты

extern I2C_TypeDef *BMx280_hi2c;  //!< Экземпляр интерфейса I2C, к которому подключен BMP280/BME280

/**
 * @defgroup STANDBY_MODE
 * @ingroup BMx280
 * @brief Конфигурация частоты получения данных с датчиков термометра, барометра, гигрометра
 * 
 * @{
 */
#define BMx280_STANDBY_0_5 				0b000		//!< 2 кГц
#define BMx280_STANDBY_62_5 			0b001		//!< 16 Гц
#define BMx280_STANDBY_125 				0b010		//!< 8 Гц
#define BMx280_STANDBY_250 				0b011		//!< 4 Гц
#define BMx280_STANDBY_500 				0b100		//!< 2 Гц
#define BMx280_STANDBY_1000 			0b101		//!< 1 Гц
#define BMx280_STANDBY_2000 			0b110		//!< 0.5 Гц
#define BMx280_STANDBY_4000 			0b111		//!< 0.25 Гц
/** @} */

/**
 * @defgroup OVERSAMPLING
 * @ingroup BMx280
 * @brief Конфигурация точности измеряемых показаний. Применимо для барометра и термометра
 * @details Данные значения передаются как аргумент в функцию @ref BMx280_config для трех разных датчиков:
 * 	термометра, барометра, гигрометра (актуально для BME280).  
 * Среднеквадратичные значения шумов при разных разрешениях oversampling для барометра в зависимости от
 * 	коэффициентов фильтра IIR:
 * @image html bmx_oversampling_pressure.png
 * 
 * Среднеквадратичные значения шумов при разных разрешениях oversampling для термометра:
 * @image html bmx_oversampling_temperature.png 
 * 
 * Среднеквадратичные значения шумов при разных разрешениях oversampling для гигрометра:
 * @image html bmx_oversampling_humidity.png 
 * @{
 */
#define BMx280_OVERSAMPLING_0 			0b000
#define BMx280_OVERSAMPLING_1 			0b001
#define BMx280_OVERSAMPLING_2 			0b010
#define BMx280_OVERSAMPLING_4 			0b011
#define BMx280_OVERSAMPLING_8 			0b100
#define BMx280_OVERSAMPLING_16 			0b101
/** @} */

/** 
 * @defgroup IIR_FILTER
 * @ingroup BMx280
 * @brief Конфигурация IIR-фильтра для барометра 
 * @details Чем выше коэффициент - тем меньше шумов, но медленнее реакция на изменения.  
 *  Зависимость коэффициента фильтра на скорость реакции отражена на графике:
 * @image html bmx_iir_response.png
 * То есть без фильтра при следующем измерении будет получено 100% от резкого изменения сигнала.
 * При коэффициенте 16 после резкого изменения сигнала будет получено 90% сигнала после 32 измерений.
 * 
 * @{
 */
#define BMx280_FILTER_OFF				0b000		//!< Ultra low-power
#define BMx280_FILTER_X2				0b001		//!< Low-power
#define BMx280_FILTER_X4				0b010		//!< Standart resolution
#define BMx280_FILTER_X8				0b011		//!< High resolution 
#define BMx280_FILTER_X16				0b100		//!< Ultra high resolution
/** @} */

/** @cond UNNECESSARY */
#define BMx280_REGISTER_CALIBRATION		0x88
#define BMx280_REGISTER_CALIBRATION_H4	0xE4
#define BMx280_REGISTER_CALIBRATION_H6	0xE7
#define BMx280_REGISTER_RAW_DATA 		0xF7
#define BMx280_REGISTER_CONFIG 			0xF5
#define BMx280_REGISTER_CTRL_MEAS 		0xF4
#define BMx280_REGISTER_STATUS 			0xF3
#define BMx280_REGISTER_CTRL_HUM 		0xF2
#define BMx280_REGISTER_RESET 			0xE0
#define BMx280_REGISTER_ID 				0xD0
/** @endcond */


/**
 * @brief Инициализация BME/BMP280 
 * @ingroup BMx280
 * @note Автоматически определяет адрес датчика в зависимости от SDO. 
 * 	Данная библиотека поддерживает только один подключенный датчик BMP/BME280
 *
 * @param[in] hi2c_ Экземляр интерфейса I2C, к которому подключен датчик
 * @param[in] refPressure_ Начальное давление, относительно которого вычисляется высота по давлению 
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BMx280_init(I2C_TypeDef *hi2c_, uint32_t refPressure_);

/**
 * @brief Конфигурация датчика BMP/BME280 
 * @ingroup BMx280
 * @details Конфигурирует OVERSAMPLING для датчика температуры, давления и влажности, частоту работы,
 * 	коэффициент сглаживания для IIR фильтра.
 * 
 * @note Конфигурация датчика влажности актуальна только для BME280. BMP280 не имеет датчика влажности.
 * 
 * @param [in] T_OS OVERSAMPLING для термометра. Допустимые значения см. в @ref OVERSAMPLING 
 * @param [in] P_OS OVERSAMPLING для барометра. Допустимые значения см. в @ref OVERSAMPLING 
 * @param [in] H_OS OVERSAMPLING для гигрометра. Допустимые значения см. в @ref OVERSAMPLING 
 * @param [in] STDB Частоты работы. Допустимые значения см. в @ref STANDBY_MODE
 * @param [in] IIRF Коэффициент сглаживания для IIR-фильтра. Допустимые значения см. в @ref IIR_FILTER
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BMx280_config(uint8_t T_OS, uint8_t P_OS, uint8_t H_OS, uint8_t STDB, uint8_t IIRF);

/**
 * @brief Функция для разового измерения и чтения данных с датчика BME280.
 * @ingroup BMx280
 * @details Высота вычисляется по формуле @f$ 29.254 \cdot (T + 273.15) \cdot \log(\frac{pres_{ref}}{pres})@f$
 * 
 * @note Функции измерения данных не универсальны для датчиков BMP/BME280.
 * 
 * @param[out] temp Измеренная температура в градусах Цельсия
 * @param[out] press Измеренное давление в Паскалях
 * @param[out] hum Измеренная влажность в %
 * @param[out] h Вычисленная высота в метрах
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BME280_forced_measure(float *temp, float *press, float *hum, float *h);
/**
 * @brief Чтение измеренных данных из памяти BME280
 * @ingroup BMx280
 * @details Используется при включении режима измерений с помощью @ref BMx280_normal_measure.  
 * 	Высота вычисляется по формуле @f$ 29.254 \cdot (T + 273.15) \cdot \log(\frac{pres_{ref}}{pres})@f$
 * 
 * @note Функции чтения данных не универсальны для датчиков BMP/BME280.
 * 
 * @param[out] temp Измеренная температура в градусах Цельсия
 * @param[out] press Измеренное давление в Паскалях
 * @param[out] hum Измеренная влажность в %
 * @param[out] h Вычисленная высота в метрах
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BME280_get_measure(float *temp, float *press, float *hum, float *h);

/**
 * @brief Функция для разового измерения и чтения данных с датчика BMP280.
 * @ingroup BMx280
 * @details Высота вычисляется по формуле @f$ 29.254 \cdot (T + 273.15) \cdot \log(\frac{pres_{ref}}{pres})@f$
 * 
 * @note Функции измерения данных не универсальны для датчиков BMP/BME280.
 * 
 * @param[out] temp Измеренная температура в градусах Цельсия
 * @param[out] press Измеренное давление в Паскалях
 * @param[out] h Вычисленная высота в метрах
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */

HAL_StatusTypeDef BMP280_forced_measure(float *temp, float *press, float *h);
/**
 * @brief Чтение измеренных данных из памяти BMP280
 * @ingroup BMx280
 * @details Используется при включении режима измерений с помощью @ref BMx280_normal_measure.  
 * 	Высота вычисляется по формуле @f$ 29.254 \cdot (T + 273.15) \cdot \log(\frac{pres_{ref}}{pres})@f$
 * 
 * @note Функции чтения данных не универсальны для датчиков BMP/BME280.
 * 
 * @param[out] temp Измеренная температура в градусах Цельсия
 * @param[out] press Измеренное давление в Паскалях
 * @param[out] h Вычисленная высота в метрах
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BMP280_get_measure(float *temp, float *press, float *h);

/**
 * @brief Переход в режим сна
 * @ingroup BMx280
 * 
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BMx280_sleep();
/**
 * @brief Переход в режим измерений. 
 * @ingroup BMx280
 * @details Производит измерения с частотой @ref STANDBY_MODE, заданной с помощью @ref BMx280_config.  
 * 	Измерения записывает во внутреннюю память, которые можно считать функцией @ref BMP280_get_measure или 
 *  @ref BME280_get_measure.
 * 
 * @return HAL_StatusTypeDef Результат получения данных по I2C
 */
HAL_StatusTypeDef BMx280_normal_measure();

#endif /* BMx280_H_ */
