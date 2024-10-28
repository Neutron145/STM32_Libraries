/***************************************************************************//**
 * 	@file			LSM6DS33.h
 *  @brief			Файл обеспечивает работу датчика LSM6DS33, подключенному к микроконтроллеру STM32F103 или STM32F411 по I2C.
 * 	@author			Рафаэль Абельдинов
 *  @date 			19.12.2023
 ******************************************************************************/

/**
 * @defgroup LSM6DS33_group LSM6DS33
 * @brief			Модуль для акселерометра и гироскопа LSM6DS33
 * @details 		Модуль позволяет конфигурировать датчик, снимать измерения гироскопа и акселерометра, программно сбрасывать датчик.  
 * 					Модуль работает с библиотеками LL и HAL. Для выбора библиотеки необходимо использовать макрос и при использовании LL добавить в проект модуль I2C LL.  
 * 					Перед измерением данных необходимо инициализировать датчик, сконфигурировать его, после чего можно снимать измерения.
 * 					Пример работы с датчиком:
 * 					\code{.c}  
 * 					float a[3];
 * 					float g[3];
 * 					float t;  
 * 					LSM6DS33_init(hi2c1);
 * 					LSM6DS33_get_all_measure(a, g, &t);   
 * 					}  
 * 					\endcode
 * @{
 */
#ifndef INC_LSM6DS33_H_
#define INC_LSM6DS33_H_

#include "main.h"

/**
 * @name Макрос определения используемой библиотеки
 * @{
 */
#define LSM6DS33_HAL			//!< Указывает библиотеку STM32, с помощью которой управляется интерфейс I2C. Определите **LSM6DS33_HAL** или **LSM6DS33_LL** в зависимости от используемой библиотеки.
/** @} */

/** @cond UNNECESSARY */
#ifdef LSM6DS33_HAL

#define I2C_TypeDef 		I2C_HandleTypeDef
#define I2C_Mem_Write(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)		HAL_I2C_Mem_Write(ADR,DEV_ADR,REG_ADR,I2C_MEMADD_SIZE_8BIT,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Mem_Read(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)			HAL_I2C_Mem_Read(ADR,DEV_ADR,REG_ADR,I2C_MEMADD_SIZE_8BIT,BUF,BUF_SIZE,TIMEOUT)


#elif LSM6DS33_LL

#include "I2C_ll.h"
#define I2C_TypeDef 		I2C_TypeDef
#define I2C_Mem_Write(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)		LL_I2C_Mem_Write(ADR,DEV_ADR,REG_ADR,BUF,BUF_SIZE,TIMEOUT)
#define I2C_Mem_Read(ADR, DEV_ADR, REG_ADR, BUF, BUF_SIZE, TIMEOUT)			LL_I2C_Mem_Read(ADR,DEV_ADR,REG_ADR,BUF,BUF_SIZE,TIMEOUT)

#endif /* LSM6DS33_LL */
/** @endcond */

extern I2C_TypeDef *LSM6DS33_hi2c; 			//!< Указатель на экземпляр I2C, к которому подключен датчиик влажности. Тип экземпляра зависит от библиотеки.
extern uint16_t FULL_SCALES_A[4]; 			//!< Хранит значения full-scale для акселерометра. Упрощает перевод из попугаев в слонов
extern uint16_t FULL_SCALES_G[4]; 			//!< Хранит значения full-scale для гироскопа. Упрощает перевод из попугаев в слонов 

extern uint32_t LSM6DS33_ADDRESS;			//!< Адрес LSM. Зависит от пина SDO, определяется автоматически

extern float LSM6DS33_a_ref[3];			//!< Содержит значения ускорения по трем осям на старте. Необходимо для получения корректных значений
extern float LSM6DS33_g_ref[3];			//!< Содержит значения угловых скоростей по трем осям на старте. Необходимо для поулчения корректных значений

/**
 * @brief Конфигурация датчика
 */
typedef struct {
	uint8_t ORIENT_config;					//!< Конфигурация регистра ORIENT_CFG_G 
	uint8_t CTRL1_config;					//!< Конфигурация регистра CTRL1_XL
	uint8_t CTRL2_config;					//!< Конфигурация регистра CTRL2_G
	uint8_t CTRL3_config;					//!< Конфигурация регистра CTRL3_C
	uint8_t CTRL7_config;					//!< Конфигурация регистра CTRL7_G
	uint8_t CTRL8_config;					//!< Конфигурация регистра CTRL8_XL
} LSM6DS33_cfg;

/**
 * @defgroup LSM6DS33_ODR Частота обновления измерений
 * @brief Значения битов ODR в регистре CTRL1
 * @{
 */
#define LSM6DS33_ODR_MASK						0b11110000			
#define LSM6DS33_ODR_POWER_DOWN 				0b0000				
#define LSM6DS33_ODR_12_5HZ						0b0001				
#define LSM6DS33_ODR_26HZ						0b0010				
#define LSM6DS33_ODR_52HZ						0b0011				
#define LSM6DS33_ODR_104HZ						0b0100				
#define LSM6DS33_ODR_208HZ						0b0101				
#define LSM6DS33_ODR_416HZ						0b0110				
#define LSM6DS33_ODR_833HZ						0b0111				
#define LSM6DS33_ODR_1660HZ						0b1000				
#define LSM6DS33_ODR_3330HZ						0b1001				
#define LSM6DS33_ODR_6660HZ						0b1010				
/** @} */


/**
 * @defgroup LSM6DS33_ORIENT Порядок расположения осей системы координат
 * @brief Значения битов Orient_2, Orient_1 и Orient_0 в регистре ORIENT_CFG_G
 * @{
 */
#define LSM6DS33_ORIENT_CFG_MASK				0b00111111			
#define LSM6DS33_ORIENT_CFG_XYZ					0b000
#define LSM6DS33_ORIENT_CFG_XZY					0b001
#define LSM6DS33_ORIENT_CFG_YXZ					0b010
#define LSM6DS33_ORIENT_CFG_YZX					0b011
#define LSM6DS33_ORIENT_CFG_ZXY					0b100
#define LSM6DS33_ORIENT_CFG_ZYX					0b101
/** @} */

/**
 * @defgroup LSM6DS33_ORIENT_SIGN Направления осей системы координат
 * @brief Значения битов SignX_G, SignY_G и SignZ_G в регистре ORIENT_CFG_G
 * @{ 
 */
#define LSM6DS33_ORIENT_SIGN_POSITIVE_X			0b0<<2
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_X			0b1<<2
#define LSM6DS33_ORIENT_SIGN_POSITIVE_Y			0b0<<1
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_Y			0b1<<1
#define LSM6DS33_ORIENT_SIGN_POSITIVE_Z			0b0
#define LSM6DS33_ORIENT_SIGN_NEGATIVE_Z			0b1
/** @} */

/**
 * @defgroup LSM6DS33_GYRO_HPF Фильтр high-pass frequency для гироскопа
 * @brief Значения битов HPFC_G (частота фильтра) и HPF_G_E (включен или выключен фильтр) регистра CTRL7_G
 * @{
 */
#define LSM6DS33_GYRO_HPF_MASK 					0b01110000
#define LSM6DS33_GYRO_HPF_OFF					0b0
#define LSM6DS33_GYRO_HPF_ON					0b1
#define LSM6DS33_GYRO_HPF_FREQ_1				0b00		//!< Частота фильтра 0.0081 Гц
#define LSM6DS33_GYRO_HPF_FREQ_2				0b01		//!< Частота фильтра 0.0324 Гц
#define LSM6DS33_GYRO_HPF_FREQ_3				0b00		//!< Частота фильтра 2.07 Гц
#define LSM6DS33_GYRO_HPF_FREQ_4				0b01		//!< Частота фильтра 16.32 Гц
/** @} */

/**
 * @defgroup LSM6DS33_A_FILTER Фильтр high-pass frequency для акселерометра 
 * @brief Значения HPCF_XL регистра CTRL8_XL
 * @{
 */
#define LSM6DS33_A_FILTER_MASK					0b01100000
#define LSM6DS33_A_FILTER_MODE_1				0b00			//!< Применяется фильтр Slope. Частота ODR_XL/50
#define LSM6DS33_A_FILTER_MODE_2				0b01			//!< Применяется фильтр HPF. Частота ODR_XL/100
#define LSM6DS33_A_FILTER_MODE_3				0b10			//!< Применяется фильтр HPF. Частота ODR_XL/9
#define LSM6DS33_A_FILTER_MODE_4				0b11			//!< Применяется фильтр HPF. Частота ODR_XL/400
/** @} */

/**
 * @defgroup LSM5DS33_FULL_SCALE_A Full-scale для акселерометра
 * @brief Значения битов FS_XL регистра CTRL1_XL
 * @{
 */
#define LSM6DS33_FULL_SCALE_MASK				0b00001100
#define LSM6DS33_FULL_SCALE_2G					0b00
#define LSM6DS33_FULL_SCALE_4G					0b10
#define LSM6DS33_FULL_SCALE_8G					0b11
#define LSM6DS33_FULL_SCALE_16G					0b01
/** @} */

/**
 * @defgroup LSM6DS33_FULL_SCALE_G Full-scale для гироскопа
 * @brief Значения битов FS_G регистра CTRL2_G
 * @{
 */
#define LSM6DS33_FULL_SCALE_250DPS				0b000
#define LSM6DS33_FULL_SCALE_500DPS				0b001
#define LSM6DS33_FULL_SCALE_1000DPS				0b010
#define LSM6DS33_FULL_SCALE_2000DPS				0b011
/** @} */

/**
 * @defgroup LSM5DS33_REG_ADR Адреса регистров датчика 
 * @{
 */
#define LSM6DS33_REGISTER_CTRL1					0x10
#define LSM6DS33_REGISTER_CTRL2					0x11
#define LSM6DS33_REGISTER_CTRL3					0x12
#define LSM6DS33_REGISTER_CTRL7					0x16
#define LSM6DS33_REGISTER_CTRL8					0x17
#define LSM6DS33_REGISTER_ORIENT_CFG			0x0B
#define LSM6DS33_REGISTER_STATUS_REG			0x1E
#define LSM6DS33_REGISTER_OUT_T					0x20
#define LSM6DS33_REGISTER_OUT_G					0x22
#define LSM6DS33_REGISTER_OUT_A					0x28
#define LSM6DS33_REGISTER_ID					0x0F
/** @} */


/**
 * @brief Вспомогательная функция для изменения бита в байте 
 * @details Используется для модификации регистров датчика, используя маску бита и новое значение
 * @param reg_data Значение регистра 
 * @param mask Маска изменяемого бита
 * @param bits Новое значение бита
 */
void __LSM6DS33_modify_reg(uint8_t *reg_data, uint8_t mask, uint8_t bits);



/**
 * @brief Инициализация датчика LSM6DS33
 * @details Функция вызывается перед конфигурацией и измерением данных датчиком. В ней проверяется ID датчика и загружается стандартная конфигурация. Важно, чтобы датчик в момент инициализации находился в покое.
 * @param hi2c_ Экземпляр I2C_TypeDef, к которому подключен датчик.
 * @retval status Статус I2C после инициализации датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_init(I2C_TypeDef *hi2c_);



/**
 * @brief Конфигурация ориентации датчика
 * @details Конфигурируется регистр ORIENT_CFG_G. Определяется порядок расположения осей системы координат и их знаки
 * @param orient Порядок расположение осей системы координат. Принимает значения макросов @ref LSM6DS33_ORIENT "порядка расположения осей системы координат".
 * @param signs Знаки осей в системе координат. Принимает значения макросов @ref LSM6DS33_ORIENT_SIGN "направлений осей системы координат".
 * @retval status Статус I2C после конфигурации ориентации датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_config_orientation(uint8_t orient, uint8_t signs);

/**
 * @brief Конфигурация HPF фильтров акселерометра и гироскопа
 * @details Конфигурируется через бит HPCF_XL в регистре CRTL8_XL и биты HPCF_G, HP_G_EN в регистре CTRL7_G 
 * @param g_HPF Режим работы фильтра HPF для гироскопа. Принимает значения макросов @ref LSM6DS33_GYRO_HPF "режима работы фильтра".
 * @param g_HPF_frequency Частота фильтра HPF для гироскопа. Принимает значения макросов @ref LSM6DS33_GYRO_HPF "частоты фильтра для гироскопа".
 * @param a_HPF Частота и режим работы HPF для акселерометра. Принимает значения макросов @ref LSM6DS33_A_FILTER "фильтра high-pass frequency для акселерометра".
 * @retval status Статус I2C после конфигурации фильтров датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_config_filters(uint8_t g_HPF, uint8_t g_HPF_frequency, uint8_t a_HPF);

/**
 * @brief Конфигурация full-scale для гироскопа и акселерометра
 * @details Конфигурируется через бит FS_XL регистра CTRL1_XL и бит FS_G регистра CTRL2_G
 * @param a_FS Full-scale для акселерометра. Принимает значения марокосов @ref LSM5DS33_FULL_SCALE_A "full-scale для акселерометра".
 * @param g_FS Full-scale для гироскопа. Принимает значения макросов @ref LSM6DS33_FULL_SCALE_G "full-scale Для гироскопа".
 * @retval status Статус I2C после конфигурации full scale датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_config_full_scale(uint8_t a_FS, uint8_t g_FS);

/**
 * @brief Конфигурация режима работы датчика 
 * @details Конфигурируется через бит ODR_XL регистра CTRL1_XL и бит ODR_G регистра CTRL2_G. Позволяет включать или выключать гироскопа и акселерометра и задавать частоту их работы. 
 * 			Акселерометр и гироскоп работают независимо друг от друга, каждый на своих частотах.
 * @param a_ODR Частота работы акселерометра. Принимает значения макросов @ref LSM6DS33_ODR "частоты и питания датчика".
 * @param g_ODR Частота работы гироскопа. Принимает значения макросов @ref LSM6DS33_ODR "частоты и питания датчика".
 * @retval status Статус I2C после конфигурации режима работы датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_config_perfomance_mode(uint8_t a_ODR, uint8_t g_ODR);



/**
 * @brief Програмнная перезагрузка датчика
 * @details Выполняется записью 1 в бит SW_RESET в регистр CTRL3_C
 * @retval status Статус I2C после сброса датчика. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_reset();




/**
 * @brief Снятие измерений акселерометра
 * @details Измерения получаются путем считывания значений из 6 регистров, начиная с OUTX_L_XL, и преобразованием значений АЦП в мс/c.
 * @param a Массив, куда записываются значения ускорений по трем осям
 * @retval status Статус I2C после снятия измерений. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_A_get_measure(float *a);

/**
 * @brief Снятие измерений гироскопа
 * @details Измерения получаются путем считывания значений из 6 регистров, начиная с OUTX_L_G, и преобразованием значениий АЦП в градусы.
 * @param g Массив, куда записываются значений угла отклонения по трем осям
 * @retval status Статус I2C после снятия измерений. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_G_get_measure(float *g);

/**
 * @brief Снятие измерений термометра
 * @details Измерения получаются путем считывания значений из регистров OUT_TEMP_L и OUT_TEMP и преобразование значений АЦП в градусы Цельсия.
 * @param t Переменная, куда записывается значение температуры в градусах Цельсиях
 * @retval status Статус I2C после снятия измерений. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_T_get_measure(float *t);

/**
 * @brief Снятие измерений акселерометра и гирокскопа
 * @details Измерениия получаются путем считывания значений из 12 регистров, начиная с OUTX_L_G и преобразованием значений АЦП в мс/с и градусы. 
 * @param a Массив, куда записываются значения ускорений по трем осям
 * @param g Массив, куда записываются значений угла отклонения по трем осям
 * @retval status Статус I2C после снятия измерений. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_get_measure(float *a, float *g);

/**
 * @brief Снятие измерений акселерометра, гирокскопа и термометра
 * @details Измерения получаются путем считывания значений из 14 регистров, начиная с OUT_TEMP_L и преобразованием значений АЦП в мс/c, градусы и градусы Цельсия. 
 * @param a Массив, куда записываются значения ускорений по трем осям
 * @param g Массив, куда записываются значений угла отклонения по трем осям
 * @param t Переменная, куда записывается значение температуры в градусах Цельсиях
 * @retval status Статус I2C после снятия измерений. Может быть **HAL_OK**, **HAL_ERROR**, **HAL_BUSY**
 */
HAL_StatusTypeDef LSM6DS33_get_all_measure(float *a, float *g, float *t);

#endif /* INC_LSM6DS33_H_ */

/** @} */
