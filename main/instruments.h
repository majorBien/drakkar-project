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

double calculateAltitude(double pressure, double seaLevelPressure);
double updateVerticalVelocity(double deltaTime,double pressure);


#endif /* MAIN_INSTRUMENTS_H_ */
