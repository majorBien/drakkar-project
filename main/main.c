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
#include "instruments.h"

//mpe
#ifdef CONFIG_EXAMPLE_I2C_ADDRESS_LOW
#define ADDR MPU6050_I2C_ADDRESS_LOW
#else
#define ADDR MPU6050_I2C_ADDRESS_HIGH
#endif

//bme
#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

//uart
#define TXD_PIN 22
#define RXD_PIN 21

//tft delay

#define INTERVAL 400
#define WAIT vTaskDelay(INTERVAL)

static const char *TAG = "MAIN";


//global variables

//gyro and acc globals


float acc_x = 0.0;
float acc_y = 0.0;
float acc_z = 0.0;

float gyro_x = 0.0;
float gyro_y = 0.0;
float gyro_z = 0.0;

//bme globals

float pressureG = 0.0;
float temperatureG = 0.0;

double velocityZ = 0.0;
double altitude = 0.0;


//menu globals
bool feet;
uint8_t menu = 0;
bool left_button;
bool right_button;
double seaLevelPressure = 1013.25;
uint8_t unit = 0;
uint8_t direction_flag = 2;
// You have to set these CONFIG value using menuconfig.
#if 0
#define CONFIG_WIDTH	240
#define CONFIG_HEIGHT 320
#define CONFIG_TFT_CS_GPIO 5
#define CONFIG_DC_GPIO 26
#define CONFIG_RESET_GPIO 2
#define CONFIG_BL_GPIO 2
#endif

//jackob parse to json uart data

typedef struct {
  char time[11];      // HHMMSS.SS
  char latitude[10];  // DDMM.MMMM
  char lat_dir;       // N or S
  char longitude[11]; // DDDMM.MMMM
  char lon_dir;       // E or W
  char date[7];       // DDMMYY
  float speed;        // Speed in knots
  float course;       // Course over ground
} GPSData_t;

typedef struct {
  GPSData_t gpsData;
  float temp;
  float pressure;
  float orientation;
  float verticalSpeed;
  bool thermal;
} parsedToStruct_t;

typedef union {

  char toPass[sizeof(parsedToStruct_t) + 1];
  parsedToStruct_t parsedToStruct;

} togetherApesStrong_u;



//GPIO macro

#define GPIO_INPUT_A	GPIO_NUM_39
#define GPIO_INPUT_B	GPIO_NUM_38
#define GPIO_INPUT_C	GPIO_NUM_37




//gyro tasks

void mpu6050_test(void *pvParameters)
{
    mpu6050_dev_t dev = { 0 };

   //ESP_ERROR_CHECK(mpu6050_init_desc(&dev, ADDR, 0, 21, 22)); //sda i scl
   ESP_ERROR_CHECK(mpu6050_init_desc(&dev, ADDR, 0, CONFIG_EXAMPLE_SDA_GPIO, CONFIG_EXAMPLE_SCL_GPIO));

    while (1)
    {
        esp_err_t res = i2c_dev_probe(&dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(mpu6050_init(&dev));

    ESP_LOGI(TAG, "Accel range: %d", dev.ranges.accel);
    ESP_LOGI(TAG, "Gyro range:  %d", dev.ranges.gyro);

    while (1)
    {
        float temp;
        mpu6050_acceleration_t accel = { 0 };
        mpu6050_rotation_t rotation = { 0 };

        ESP_ERROR_CHECK(mpu6050_get_temperature(&dev, &temp));
        ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &rotation));

        ESP_LOGI(TAG, "**********************************************************************");
        ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
        ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
        ESP_LOGI(TAG, "Temperature:  %.1f", temp);

        acc_x = accel.x;
        acc_y = accel.y;
        acc_z = accel.z;

        gyro_x = rotation.x;
        gyro_y = rotation.y;
        gyro_z = rotation.z;

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}



//bme task


void bmp280_test(void *pvParameters)
{
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, 5, 2));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
        if (bme280p)
            printf(", Humidity: %.2f\n", humidity);
        else
            printf("\n");
        pressureG = pressure;
        temperatureG = temperature;
    }
}




void bmp280_test2(void *pvParameters)
{
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_1, 0, 5, 2));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("Pressure2: %.2f Pa, Temperature2: %.2f C", pressure, temperature);
        if (bme280p)
            printf(", Humidity: %.2f\n", humidity);
        else
            printf("\n");
    }
}

//uart globals GPS
/*
char time[11];      // HHMMSS.SS
char latitude[10];  // DDMM.MMMM
char lat_dir;       // N or S
char longitude[11]; // DDDMM.MMMM
char lon_dir;       // E or W
char date[7];       // DDMMYY
float speed;        // Speed in knots
float course;       // Course over ground
*/

//uart functions

void uart_init() {

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 1024, 1024, 0, NULL, 0);

	//uart_enable_pattern_det_baud_intr(UART_NUM_1, 0x0a, 1, 9, 0, 0); // pattern is LF
	//uart_pattern_queue_reset(UART_NUM_1, 20);
	ESP_LOGI(TAG, "Initializing UART done");

}

void uart_send_data(const char* data) {

    uart_write_bytes(UART_NUM_1, data, strlen(data));


}




uint8_t *shared_data = NULL;
volatile bool data_ready = false;


#define BUF_SIZE (1024)
uint8_t* data;
void uart_recieve(void * pvParameters)
{

	   shared_data = (uint8_t *) malloc(BUF_SIZE);
	    if (shared_data == NULL) {
	        ESP_LOGE(TAG, "Failed to allocate memory for UART buffer");
	        vTaskDelete(NULL);
	    }

	    while (1) {
	        int length = uart_read_bytes(UART_NUM_1, shared_data, BUF_SIZE - 1, pdMS_TO_TICKS(1000));
	        if (length > 0) {
	            shared_data[length] = '\0'; // Null-terminate the string
	            ESP_LOGI(TAG, "Received: %s", shared_data);
	            data_ready = true;
	        }
	    }

}


char string_uart[80];









void uart_send_bytes(const char* data, size_t length) {
    uart_write_bytes(UART_NUM_1, data, length);
}

void uart_send(void *pvParameters)
{


/*
	 while(1)
		  	{
		      uart_send_data("hello\n");
		      vTaskDelay(pdMS_TO_TICKS(1000)); // Send data every second
		  	}
*/
	    while (1) {
	        if (data_ready) {
	            uart_write_bytes(UART_NUM_1, (const char *)shared_data, strlen((const char *)shared_data));
	            data_ready = false; // Reset the flag after sending
	            ESP_LOGI(TAG, "Transmitted: %s", shared_data);
	        }
	        vTaskDelay(pdMS_TO_TICKS(1000)); // Check for data to send every second
	    }


}



/*
void buttons(void *pvParameters)
{

	gpio_pad_select_gpio(GPIO_INPUT_A);
	gpio_set_direction(GPIO_INPUT_A, GPIO_MODE_DEF_INPUT);
	gpio_pad_select_gpio(GPIO_INPUT_B);
	gpio_set_direction(GPIO_INPUT_B, GPIO_MODE_DEF_INPUT);
	gpio_pad_select_gpio(GPIO_INPUT_C);
	gpio_set_direction(GPIO_INPUT_C, GPIO_MODE_DEF_INPUT);

	while(1) {

		int level_a = gpio_get_level(GPIO_INPUT_A);
		int level_b = gpio_get_level(GPIO_INPUT_B);
		int level_c = gpio_get_level(GPIO_INPUT_C);
		if (level_a == 0) {
				ESP_LOGI(TAG, "Push Button A");
				if(menu==1)
				{
					seaLevelPressure--;
					vTaskDelay(1);
				}
				if(menu==2)
				{
					unit--;
					if(unit<0) unit = 0;
					vTaskDelay(1);

				}

			}
		if (level_b == 0) {
				ESP_LOGI(TAG, "Push Button B");
				menu++;
				vTaskDelay(100);
				if(menu>2) menu = 0;

			}
		if (level_c == 0) {
			ESP_LOGI(TAG, "Push Button C");
		}
		if(menu==1)
		{
			seaLevelPressure++;
			vTaskDelay(1);
		}

		if(menu==2)
		{
			unit++;
			if(unit>1) unit = 1;
			vTaskDelay(1);
		}

		}
		vTaskDelay(10);
}


*/

void buttons(void *pvParameters)
{
    // Konfiguracja pinów z pull-up
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;       // Wyłącz przerwania
    io_conf.mode = GPIO_MODE_INPUT;              // Ustaw jako wejścia
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_A) | (1ULL << GPIO_INPUT_B) | (1ULL << GPIO_INPUT_C);
    io_conf.pull_down_en = 0;                    // Wyłącz pull-down
    io_conf.pull_up_en = 1;                      // Włącz pull-up

    gpio_config(&io_conf);

    // Zmienna stanu do kontrolowania automatycznego zwiększania
    bool incrementing = false;
    TickType_t lastIncrementTime = xTaskGetTickCount();

    while (1)
    {
        int level_a = gpio_get_level(GPIO_INPUT_A);
        int level_b = gpio_get_level(GPIO_INPUT_B);
        int level_c = gpio_get_level(GPIO_INPUT_C);

        if (level_a == 0)
        {
            ESP_LOGI(TAG, "Push Button A");
            incrementing = false;
            if (menu == 1)
            {
                seaLevelPressure--;
                vTaskDelay(pdMS_TO_TICKS(200)); // Opóźnienie 200 ms
            }
            else if (menu == 2)
            {
                unit--;
                if (unit < 0) unit = 0;
                vTaskDelay(pdMS_TO_TICKS(200)); // Opóźnienie 200 ms
            }
        }

        if (level_b == 0)
        {
            ESP_LOGI(TAG, "Push Button B");
            incrementing = false;
            menu++;
            vTaskDelay(pdMS_TO_TICKS(200)); // Opóźnienie 200 ms
            if (menu > 2) menu = 0;
        }

        if (level_c == 0)
        {
            ESP_LOGI(TAG, "Push Button C");
            incrementing = false;
            // Możesz dodać tutaj kod obsługi przycisku C
        }

        // Automatyczne zwiększanie zmiennych na podstawie aktywnego menu co 1 sekundę
        if (xTaskGetTickCount() - lastIncrementTime > pdMS_TO_TICKS(1000))
        {
            if (menu == 1)
            {
                seaLevelPressure++;
                incrementing = true;
            }
            else if (menu == 2)
            {
                unit++;
                if (unit > 1) unit = 1;
                incrementing = true;
            }
            lastIncrementTime = xTaskGetTickCount();
        }

        // Resetowanie stanu incrementing po naciśnięciu przycisku
        if (!incrementing)
        {
            vTaskDelay(pdMS_TO_TICKS(10)); // Opóźnienie 10 ms na koniec pętli
        }

        if(unit == 0) feet = 0;
        else feet = 1;
    }
}


void instruments(void *pvParameters)
{
    Vector3 rawAcceleration = {0.0, 0.0, 0.0}; // przyspieszenie w m/s^2
    Vector3 orientation = {0.0, 0.00, 0.0}; // orientacja w radianach

    while(1)
    {

	altitude = calculateAltitude(pressureG, seaLevelPressure);
	velocityZ = updateVerticalVelocity(gyro_x, gyro_y, gyro_z, 0.1,feet);



    Vector3 rawAcceleration = {acc_x, acc_y, acc_z};
    Vector3 orientation = {gyro_x, gyro_y, gyro_z};

    Vector3 horizontalVelocity = updateHorizontalVelocity(rawAcceleration, 0.1);
    Vector3 correctedAcceleration = correctAcceleration(rawAcceleration, orientation);
    Vector3 velocity = calculateVelocity(rawAcceleration, orientation, pressureG, 0.1);
    ESP_LOGI("SENSOR_DATA", "Altitude: %.2f meters", altitude);
    ESP_LOGI("SENSOR_DATA", "Vertical Velocity: %.2f m/s", velocityZ);
    vTaskDelay(100);
	/*
	updateHorizontalVelocity(Vector3, 2);
	correctAcceleration(Vector3, Vector3 orientation);
	calculateVelocity(Vector3, 2, pressureG, 2);
	*/
    }
}




//thermal chimney detect


void sample_sensor_data(sensor_data_t *data) {

    data->height = altitude;
    data->pitch = atan2(acc_y, sqrt(acc_x * acc_x + acc_z * acc_z)) * 180 / M_PI;
    data->roll = atan2(-acc_x, acc_z) * 180 / M_PI;
    data->yaw = atan2(gyro_z, sqrt(gyro_x * gyro_x + gyro_y * gyro_y)) * 180 / M_PI;
}

void detect_thermal(sensor_data_t *data, size_t sample_count, char *direction) {
    double height_sum = 0;
    double left_height = 0;
    double right_height = 0;
    size_t left_count = 0;
    size_t right_count = 0;

    for (size_t i = 0; i < sample_count; i++) {
        height_sum += data[i].height;
    }

    double avg_height = height_sum / sample_count;

    for (size_t i = 0; i < sample_count; i++) {
        if (data[i].yaw <= 180) {
            left_height += data[i].height;
            left_count++;
        } else {
            right_height += data[i].height;
            right_count++;
        }
    }

    double avg_left_height = left_height / left_count;
    double avg_right_height = right_height / right_count;

    if (fabs(avg_left_height - avg_height) > TURN_THRESHOLD || fabs(avg_right_height - avg_height) > TURN_THRESHOLD) {
        if (avg_left_height > avg_right_height) {
            *direction = 'L';
        } else {
            *direction = 'R';
        }
    } else {
        *direction = 'U'; // Nieokreślony kierunek
    }
}






void thermo_detect(void *pvParameters)
{

    //thermal detect variables
	  sensor_data_t sensor_data;
	  size_t sample_count = CIRCLE_ANGLE / (SAMPLE_RATE_MS / 1000.0 * 360.0 / 60.0); // Oblicz liczba próbek dla pełnego obrotu
	  sensor_data_t *samples = malloc(sample_count * sizeof(sensor_data_t));
	  char direction = 'U'; // 'L' - lewo, 'R' - prawo, 'U' - nieokreślone



	 for (size_t i = 0; i < sample_count; i++) {
	            sample_sensor_data(&sensor_data);
	            samples[i] = sensor_data;
	            vTaskDelay(pdMS_TO_TICKS(SAMPLE_RATE_MS));
	        }

	        // Detekcja komina termicznego i określenie kierunku
	        detect_thermal(samples, sample_count, &direction);

	        if (direction == 'L') {
	            ESP_LOGI(TAG, "Zalecany kierunek: lewo");
	            direction_flag = 0;
	        } else if (direction == 'R') {
	            ESP_LOGI(TAG, "Zalecany kierunek: prawo");
	            direction_flag = 1;
	        } else {
	            ESP_LOGI(TAG, "Nieokreślony kierunek");
	            direction_flag = 2;
	        }



	        void set_gpio_high(gpio_num_t gpio_num, uint8_t direction_flag) {
	        	if(direction_flag==0||direction_flag==1)
	        	{
	        		 gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
	        	     gpio_set_level(gpio_num, 3);
	        	}

	        }


	vTaskDelay(pdMS_TO_TICKS(5000));


}






void ILI9341(void *pvParameters)
{





	// set font file
	FontxFile fx16G[2];
	FontxFile fx24G[2];
	FontxFile fx32G[2];
	FontxFile fx32L[2];
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
	InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latinc

	FontxFile fx16M[2];
	FontxFile fx24M[2];
	FontxFile fx32M[2];
	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo
	bool flag1 = 0;
	TFT_t dev;
#if CONFIG_XPT2046_ENABLE_SAME_BUS
	ESP_LOGI(TAG, "Enable Touch Contoller using the same SPI bus as TFT");
	int XPT_MISO_GPIO = CONFIG_XPT_MISO_GPIO;
	int XPT_CS_GPIO = CONFIG_XPT_CS_GPIO;
	int XPT_IRQ_GPIO = CONFIG_XPT_IRQ_GPIO;
	int XPT_SCLK_GPIO = -1;
	int XPT_MOSI_GPIO = -1;
#elif CONFIG_XPT2046_ENABLE_DIFF_BUS
	ESP_LOGI(TAG, "Enable Touch Contoller using the different SPI bus from TFT");
	int XPT_MISO_GPIO = CONFIG_XPT_MISO_GPIO;
	int XPT_CS_GPIO = CONFIG_XPT_CS_GPIO;
	int XPT_IRQ_GPIO = CONFIG_XPT_IRQ_GPIO;
	int XPT_SCLK_GPIO = CONFIG_XPT_SCLK_GPIO;
	int XPT_MOSI_GPIO = CONFIG_XPT_MOSI_GPIO;
#else
	ESP_LOGI(TAG, "Disable Touch Contoller");
	int XPT_MISO_GPIO = -1;
	int XPT_CS_GPIO = -1;
	int XPT_IRQ_GPIO = -1;
	int XPT_SCLK_GPIO = -1;
	int XPT_MOSI_GPIO = -1;
#endif
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_TFT_CS_GPIO, CONFIG_DC_GPIO,
		CONFIG_RESET_GPIO, CONFIG_BL_GPIO, XPT_MISO_GPIO, XPT_CS_GPIO, XPT_IRQ_GPIO, XPT_SCLK_GPIO, XPT_MOSI_GPIO);

#if CONFIG_ILI9225
	uint16_t model = 0x9225;
#endif
#if CONFIG_ILI9225G
	uint16_t model = 0x9226;
#endif
#if CONFIG_ILI9340
	uint16_t model = 0x9340;
#endif
#if CONFIG_ILI9341
	uint16_t model = 0x9341;
#endif
#if CONFIG_ST7735
	uint16_t model = 0x7735;
#endif
#if CONFIG_ST7796
	uint16_t model = 0x7796;
#endif
	lcdInit(&dev, model, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

#if CONFIG_INVERSION
	ESP_LOGI(TAG, "Enable Display Inversion");
	lcdInversionOn(&dev);
#endif

#if CONFIG_RGB_COLOR
	ESP_LOGI(TAG, "Change BGR filter to RGB filter");
	lcdBGRFilter(&dev);
#endif

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
#if CONFIG_XPT_CHECK
	TouchPosition(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif
#endif

#if 0
	// for test
	while(1) {
#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
		TouchCalibration(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		TouchPenTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchKeyTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMenuTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMoveTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchIconTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif

		ArrowTest(&dev, fx16G, model, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;
	}
#endif

	while(1) {

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
		TouchCalibration(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		TouchPenTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchKeyTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMenuTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMoveTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);

#ifdef ENABLE_PNG
		TouchIconTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif

#endif









		char file[32];
		if(flag1==0)
		{
		strcpy(file, "/images/logo_drakkar.png");
		PNGTest(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;
		flag1=1;
		}

		if(menu==0)
		{
		//ArrowInteractions(&dev, fx16G, model, CONFIG_WIDTH, CONFIG_HEIGHT, 1);
		ArrowInteractions2(&dev, fx24G, model, CONFIG_WIDTH, CONFIG_HEIGHT, direction_flag, altitude, velocityZ);
		vTaskDelay(200);
		}

		if(menu==1||menu==2)
		{
		//displaySettingsMenu(&dev, fx24G, model, CONFIG_WIDTH, CONFIG_HEIGHT,  seaLevelPressure ,unit);
		Menu(&dev, fx24G, model, CONFIG_WIDTH, CONFIG_HEIGHT, direction_flag, seaLevelPressure ,unit);
		vTaskDelay(200);
		}



	} // end while

	// never reach here
	vTaskDelete(NULL);
}











static void listSPIFFS(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

esp_err_t mountSPIFFS(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

#if 0
	ESP_LOGI(TAG, "Performing SPIFFS_check().");
	ret = esp_spiffs_check(conf.partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		return ret;
	} else {
			ESP_LOGI(TAG, "SPIFFS_check() successful");
	}
#endif

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Mount %s to %s success", path, label);
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	return ret;
}


void app_main(void)
{
	//uart init
    uart_init();



	//mpu
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6, NULL, 5, NULL);

    //bme
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreatePinnedToCore(bmp280_test, "bmp280_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(bmp280_test2, "bmp280_test2", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);

	// Initialize NVS
	ESP_LOGI(TAG, "Initialize NVS");
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	ESP_LOGI(TAG, "Initializing SPIFFS");
	esp_err_t ret;
	ret = mountSPIFFS("/spiffs", "storage0", 10);
	if (ret != ESP_OK) return;
	listSPIFFS("/spiffs/");

	// Image file borrowed from here
	// https://www.flaticon.com/packs/social-media-343
	ret = mountSPIFFS("/icons", "storage1", 10);
	if (ret != ESP_OK) return;
	listSPIFFS("/icons/");

	ret = mountSPIFFS("/images", "storage2", 14);
	if (ret != ESP_OK) return;
	listSPIFFS("/images/");

	xTaskCreate(ILI9341, "ILI9341", 1024*6, NULL, 2, NULL);

	xTaskCreate(uart_send, "uart_event_task", 2048, NULL, 12, NULL);
	xTaskCreate(uart_recieve, "uart_event_task", 2048, NULL, 16, NULL);
	xTaskCreate(buttons, "BUTTON-A", 1024*2, NULL, 1, NULL);

	xTaskCreatePinnedToCore(instruments, "avionics", configMINIMAL_STACK_SIZE * 32, NULL, 5, NULL, 1);
	xTaskCreatePinnedToCore(thermo_detect, "thermal_chimney_detect", configMINIMAL_STACK_SIZE * 32, NULL, 2, NULL, 1);
	//logs
	//xTaskCreate(ILI9341, "ILI9341", 1024*6, NULL, 2, NULL);



}
