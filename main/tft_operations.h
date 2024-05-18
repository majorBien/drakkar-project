/*
 * tft_operations.h
 *
 *  Created on: 18 maj 2024
 *      Author: majorbien
 */

#ifndef MAIN_TFT_OPERATIONS_H_
#define MAIN_TFT_OPERATIONS_H_


#include <stdio.h>
#include <inttypes.h>
#include <math.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/timers.h>
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

TickType_t FillTest(TFT_t * dev, int width, int height);



#endif /* MAIN_TFT_OPERATIONS_H_ */
