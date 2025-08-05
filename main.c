#include "bme68x.h"
#include "bme68x_platfrom.h"
#include "bme68x_defs.h"

#include "si1151.h"

#include "PMSA003I.h"

#include "ads1015.h"
#include "ads1015_platform.h"

#include "RS485_wind_direction.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "[ERROR] %s:%d: Failed to open i2c\n", __FILE__, __LINE__);
        return 1;
    }

    struct bme68x_dev dev;
    int8_t rslt = BME68X_OK;
    bme68x_platform_init(&dev);
    rslt = bme68x_init(&dev);
    printf("\r\n BME68X Init Result is:%d \r\n",rslt);
    struct bme68x_data comp_data;
    bme68x_get_sensor_data(&dev, &comp_data);
    bme68x_print_sensor_data(&comp_data);

    si1151_t si1151;
    si1151_begin(&si1151, 1, fd);
    int result = si1151_read_visible(&si1151);
    printf("The visible result is: %d\n", result);

    pmsa003i_t pmsa003i;
    pmsa003i_begin(&pmsa003i, fd, 17, 27);
    result = pmsa003i_read_PM_atmospheric_environment(&pmsa003i, PMSA003I_PM_2_5);
    printf("The PM2.5 result is: %dµg/m³\n", result);

    ads1015_handler_t ads1015;
    ads1015_platform_init(&ads1015);
    if (ads1015_init(&ads1015, ADS1015_I2C_ADDR_GND, fd) != ADS1015_OK) {
        fprintf(stderr, "[ERROR] %s:%d: Failed to initialize sensor\n", __FILE__, __LINE__);
        return 1;
    }
    
    if (ads1015_set_mux(&ads1015, ADS1015_MUX_AIN0_AIN_GND) != ADS1015_OK) {
        fprintf(stderr, "[ERROR] %s:%d: Failed to change mux setting\n", __FILE__, __LINE__);
        return 1;
    }

    if (ads1015_start_single_meas(&ads1015) != ADS1015_OK) {
    fprintf(stderr, "[ERROR] %s:%d: Failed to start measurement\n", __FILE__, __LINE__);
    return 1;
    }
    sleep(5);
    ads1015_sample_t sample;
    if (ads1015_read_sample(&ads1015, &sample) != ADS1015_OK) {
        fprintf(stderr, "[ERROR] %s:%d: Failed to take sample\n", __FILE__, __LINE__);
        return 1;
    }

    fprintf(stdout, "[INFO] %s:%d: Result of sample is %d, %fV\n", __FILE__, __LINE__, sample.raw, sample.voltage);

    rs485_wind_direction_t wind_direction;
    rs485_wind_direction_begin(&wind_direction);
    rs485_wind_direction_read_register(&wind_direction);

    fprintf(stdout, "[INFO] %s:%d: Result of wind is %f degrees\n", __FILE__, __LINE__, wind_direction.wind_direction);

    printf("\n\n\n\n\n");

    //
    bme68x_get_sensor_data(&dev, &comp_data);
    bme68x_print_sensor_data(&comp_data);
    //
    result = si1151_read_visible(&si1151);
    printf("The visible result is: %d\n", result);
    //
    result = pmsa003i_read_PM_atmospheric_environment(&pmsa003i, PMSA003I_PM_2_5);
    printf("The PM2.5 result is: %dµg/m³\n", result);
    //
    if (ads1015_read_sample(&ads1015, &sample) != ADS1015_OK) {
        fprintf(stderr, "[ERROR] %s:%d: Failed to take sample\n", __FILE__, __LINE__);
        return 1;
    }
    fprintf(stdout, "[INFO] %s:%d: Result of sample is %d, %fV\n", __FILE__, __LINE__, sample.raw, sample.voltage);
    //
    rs485_wind_direction_read_register(&wind_direction);
    fprintf(stdout, "[INFO] %s:%d: Result of wind is %f degrees\n", __FILE__, __LINE__, wind_direction.wind_direction);
    //

    rs485_wind_direction_close(&wind_direction);
    return 0;
}