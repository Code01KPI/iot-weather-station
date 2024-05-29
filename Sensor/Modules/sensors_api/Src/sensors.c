#include <stm32l475e_iot01.h>
#if 0
#include <stm32l475e_iot01_accelero.h>
#include <stm32l475e_iot01_gyro.h>
#endif 
#include <stm32l475e_iot01_hsensor.h>
#include <stm32l475e_iot01_magneto.h>
#include <stm32l475e_iot01_psensor.h>
#include <stm32l475e_iot01_tsensor.h>
#include <logging_api.h>
#include <rtos_common.h>
#include "sensors.h"

// todecide: add more log preample as error, warn, info ets
#define LOG_PREAMPLE "[SENSORS]"
#define MAX_INIT_CNT (5)

CONFIGURE_TASK(sensors, osPriorityHigh, 2048 * 2, sensors_task, NULL);
// TODO: Add event flag for sensors status

static void init_sensors() {
    uint8_t init_cnt = 0;

    CONSOLE_LOG("%s Init start", LOG_PREAMPLE);

    while(BSP_PSENSOR_Init() != PSENSOR_OK) {
        ++init_cnt;
        osDelay(500);

        if (init_cnt == MAX_INIT_CNT) {
            CONSOLE_LOG("%s barometer initialization error!", LOG_PREAMPLE);
            break;
        }
    }
    init_cnt = 0;
    CONSOLE_LOG("%s barometer initialized", LOG_PREAMPLE);

    while (BSP_TSENSOR_Init() != TSENSOR_OK) {
        ++init_cnt;
        osDelay(500);

        if (init_cnt == MAX_INIT_CNT) {
            CONSOLE_LOG("%s temperature sensor initialization error!", LOG_PREAMPLE);
            break;
        }
    }
    init_cnt = 0;
    CONSOLE_LOG("%s temperature sensor initialized", LOG_PREAMPLE);

    while (BSP_HSENSOR_Init() != HSENSOR_OK) {
        ++init_cnt;
        osDelay(500);

        if (init_cnt == MAX_INIT_CNT) {
            CONSOLE_LOG("%s humidity sensor initialization error!", LOG_PREAMPLE);
            break;
        }
    }
    init_cnt = 0;
    CONSOLE_LOG("%s humidity sensor initialized", LOG_PREAMPLE);
}

static inline void read_data_from_sensors(struct sensors_data *s_data) {
    // TODO: Add check to init status of sensors
    // TODO: Check is data correct

    float_to_fixed_point(&s_data->pressure, BSP_PSENSOR_ReadPressure());
    float_to_fixed_point(&s_data->temp, BSP_TSENSOR_ReadTemp());
    float_to_fixed_point(&s_data->humidity, BSP_HSENSOR_ReadHumidity());
}

static void sensors_task(void *arguments) {
    struct sensors_data sensors_data;

    init_sensors();

    while (1) {
        read_data_from_sensors(&sensors_data);
        CONSOLE_LOG("%s Pressure: %u.%02u, Temp: %u.%02u, Humidity: %u.%02u", LOG_PREAMPLE, sensors_data.pressure.integer_part, (sensors_data.pressure.mantissa_part * 100) / UINT16_MAX_VALUE, sensors_data.temp.integer_part, (sensors_data.temp.mantissa_part * 100) / UINT16_MAX_VALUE, sensors_data.humidity.integer_part, (sensors_data.humidity.mantissa_part * 100) / UINT16_MAX_VALUE);
        osDelay(500);
    }
}


