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
#include "stdbool.h"

TickType_t FillTest(TFT_t * dev, int width, int height);
TickType_t ColorBarTest(TFT_t * dev, int width, int height);
TickType_t ArrowTest(TFT_t * dev, FontxFile *fx, uint16_t model, int width, int height);
TickType_t DirectionTest(TFT_t * dev, FontxFile *fx, int width, int height);
TickType_t HorizontalTest(TFT_t * dev, FontxFile *fx, int width, int height);
TickType_t VerticalTest(TFT_t * dev, FontxFile *fx, int width, int height);
TickType_t LineTest(TFT_t * dev, int width, int height);
TickType_t CircleTest(TFT_t * dev, int width, int height);
TickType_t RectAngleTest(TFT_t * dev, int width, int height);
TickType_t TriangleTest(TFT_t * dev, int width, int height);
TickType_t RoundRectTest(TFT_t * dev, int width, int height);
TickType_t FillRectTest(TFT_t * dev, int width, int height);
TickType_t ColorTest(TFT_t * dev, int width, int height);
TickType_t ScrollTest(TFT_t * dev, FontxFile *fx, int width, int height);
void ScrollReset(TFT_t * dev, int width, int height);
TickType_t BMPTest(TFT_t * dev, char * file, int width, int height);
TickType_t JPEGTest(TFT_t * dev, char * file, int width, int height);
TickType_t PNGTest(TFT_t * dev, char * file, int width, int height);
TickType_t CodeTest(TFT_t * dev, FontxFile *fx, int width, int height, uint16_t start, uint16_t end);

TickType_t PNGshow(TFT_t * dev, char * file, int width, int height);
void PNGshowPos(TFT_t * dev, char * file, int width, int height, int xpos, int ypos);
TickType_t ArrowInteractions(TFT_t * dev, FontxFile *fx, uint16_t model, int width, int height, uint8_t arrow);
TickType_t DisplayData(TFT_t *dev, FontxFile *fx, int width, int height, double altitude, double verticalVelocity);
TickType_t ArrowInteractions2(TFT_t * dev, FontxFile *fx, uint16_t model, int width, int height, uint8_t arrow, double altitude, double verticalVelocity);
//TickType_t displaySettingsMenu(TFT_t *dev, FontxFile *fx, uint16_t model, int width, int height, double localPressureOffset, uint8_t unit);
TickType_t Menu(TFT_t * dev, FontxFile *fx, uint16_t model, int width, int height, uint8_t arrow, double localPressureOffset, uint8_t unit);
#endif /* MAIN_TFT_OPERATIONS_H_ */
