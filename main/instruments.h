/*
 * instruments.h
 *
 *  Created on: 19 maj 2024
 *      Author: majorbien
 */

#ifndef MAIN_INSTRUMENTS_H_
#define MAIN_INSTRUMENTS_H_


#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "ili9340.h"
#include "fontx.h"
#include "bmpfile.h"
#include "decode_jpeg.h"
#include "decode_png.h"
#include "pngle.h"

#include "driver/gpio.h"
#include "tft_operations.h"


#include <mpu6050.h>
#include <bmp280.h>
#include <string.h>

#include "driver/uart.h"
#include "string.h"
#include "stdbool.h"
#include <math.h>


#define SAMPLE_RATE_MS 100
#define CIRCLE_ANGLE 360
#define TURN_THRESHOLD 10

typedef struct {
    float height;
    float pitch;
    float roll;
    float yaw;
} sensor_data_t;


typedef struct {
    double x;
    double y;
    double z;
} Vector3;

double calculateAltitude(double pressure, double seaLevelPressure);
//double updateVerticalVelocity(double deltaTime,double pressure);
Vector3 updateHorizontalVelocity(Vector3 acceleration, double deltaTime);
Vector3 correctAcceleration(Vector3 rawAcceleration, Vector3 orientation);
Vector3 calculateVelocity(Vector3 rawAcceleration, Vector3 orientation, double pressure, double deltaTime);
double updateVerticalVelocity(double gyro_x, double gyro_y, double gyro_z, double delta_czasu);


void sample_sensor_data(sensor_data_t *data);
void detect_thermal(sensor_data_t *data, size_t sample_count, char *direction);

#endif /* MAIN_INSTRUMENTS_H_ */
