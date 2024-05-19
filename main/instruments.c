/*
 * instruments.c
 *
 *  Created on: 19 maj 2024
 *      Author: majorbien
 */

#include "instruments.h"


const double g = 9.80665;



double calculateAltitude(double pressure, double seaLevelPressure) {
	 //return 145366.45 (1-(pressure/seaLevelPressure));
    const double L = 0.0065; // Temperatura dryfu w metrach
    const double T0 = 288.15; // Temperatura na poziomie morza w Kelvinach
    const double R = 287.05; // Stała gazowa dla powietrza w J/(kg*K)
    const double g = 9.80665; // Przyspieszenie ziemskie w m/s^2

    double altitude = (T0 / L) * (1 - pow(((pressure/100) / seaLevelPressure), (R * L / g)));
    return altitude;
}
/*
double updateVerticalVelocity(double deltaTime, double pressure) {
    static double previousAltitude = 0.0;
    static int firstRun = 1;


    double currentAltitude = calculateAltitude(pressure, 1013.25);

    if (firstRun) {

        previousAltitude = currentAltitude;
        firstRun = 0;
    }


    double velocityZ = (currentAltitude - previousAltitude) / deltaTime;


    previousAltitude = currentAltitude;

    return velocityZ;
}

*/
double updateVerticalVelocity(double gyro_x, double gyro_y, double gyro_z, double delta_czasu) {
    // Załóżmy, że odczyty są w radianach na sekundę (rad/s)

    // Prędkość kątowa wokół osi Z (pionowej)
    double pr_predkosc_z = gyro_z;

    // Obliczanie zmiany kąta w czasie przez całkowanie prędkości kątowej
    double delta_kat_z = pr_predkosc_z * delta_czasu;

    // Tutaj możesz zaimplementować różne metody całkowania, np. metodę prostokątów, trapezów, itp.

    // Obliczanie prędkości pionowej (vertical speed) zgodnie z kątem zmiany w czasie
    double predkosc_pionowa = delta_kat_z; // Tutaj możesz dodać dodatkowe korekty lub przeliczenia

    return predkosc_pionowa;
}




// Funkcja do aktualizacji prędkości poziomej na podstawie odczytów akcelerometru
Vector3 updateHorizontalVelocity(Vector3 acceleration, double deltaTime) {
    static Vector3 velocity = {0.0, 0.0, 0.0};

    // Integracja przyspieszenia w celu uzyskania prędkości
    velocity.x += acceleration.x * deltaTime;
    velocity.y += acceleration.y * deltaTime;
    velocity.z += acceleration.z * deltaTime;

    return velocity;
}

// Korekcja odczytów akcelerometru o wpływ grawitacji
Vector3 correctAcceleration(Vector3 rawAcceleration, Vector3 orientation) {
    Vector3 correctedAcceleration;

    // Orientacja w postaci kąta (pitch, roll)
    double pitch = orientation.x;
    double roll = orientation.y;

    correctedAcceleration.x = rawAcceleration.x;
    correctedAcceleration.y = rawAcceleration.y * cos(pitch) - rawAcceleration.z * sin(pitch);
    correctedAcceleration.z = rawAcceleration.y * sin(pitch) + rawAcceleration.z * cos(pitch) - g;

    return correctedAcceleration;
}

// Główna funkcja do obliczania prędkości
Vector3 calculateVelocity(Vector3 rawAcceleration, Vector3 orientation, double pressure, double deltaTime) {
	/*
    Vector3 correctedAcceleration = correctAcceleration(rawAcceleration, orientation);

    Vector3 horizontalVelocity = updateHorizontalVelocity(correctedAcceleration, deltaTime);
    double verticalVelocity = updateVerticalVelocity(deltaTime, pressure);

    Vector3 velocity = {horizontalVelocity.x, horizontalVelocity.y, verticalVelocity};
    return velocity;
    */
	   const double g = 9.80665;  // przyspieszenie ziemskie w m/s^2
	    Vector3 correctedAcceleration;

	    double pitch = orientation.x;
	    double roll = orientation.y;

	    correctedAcceleration.x = rawAcceleration.x;
	    correctedAcceleration.y = rawAcceleration.y * cos(pitch) - rawAcceleration.z * sin(pitch);
	    correctedAcceleration.z = rawAcceleration.y * sin(pitch) + rawAcceleration.z * cos(pitch) - g;

	    // Obliczanie prędkości pionowej
	    //double verticalVelocity = updateVerticalVelocity(deltaTime, pressure);

	    // Obliczanie prędkości poziomej
	    static Vector3 velocity = {0.0, 0.0, 0.0};
	    velocity.x += correctedAcceleration.x * deltaTime;
	    velocity.y += correctedAcceleration.y * deltaTime;
	    velocity.z += correctedAcceleration.z * deltaTime;

	    // Obliczanie wysokości
	    double altitude = calculateAltitude(pressure, 1013.25);

	    // Logowanie danych


	    Vector3 result = {0.0, 0.0, 0.0};
	    return result;
}

/*
    Vector3 rawAcceleration = {0.01, 0.02, 9.75}; // przyspieszenie w m/s^2
    Vector3 orientation = {0.1, 0.05, 0.0}; // orientacja w radianach
    double pressure = 1000.0; // ciśnienie w hPa
    double deltaTime = 0.1; // czas w sekundach między odczytami

    Vector3 velocity = calculateVelocity(rawAcceleration, orientation, pressure, deltaTime);

    printf("Prędkość: X = %f, Y = %f, Z = %f\n", velocity.x, velocity.y, velocity.z);

    return 0;
*/


void sample_sensor_data(sensor_data_t *data) {
    // Tutaj powinna być implementacja pobierania danych z czujników
    // Na przykład:
    // data->height = get_height_from_sensor();
    // data->pitch = get_pitch_from_sensor();
    // data->roll = get_roll_from_sensor();
    // data->yaw = get_yaw_from_sensor();

    // Na potrzeby przykładu, wypełnimy dane losowymi wartościami:
    data->height = (float) (rand() % 1000) / 10.0; // np. wysokość w metrach
    data->pitch = (float) (rand() % 360) - 180.0; // -180 do 180 stopni
    data->roll = (float) (rand() % 360) - 180.0; // -180 do 180 stopni
    data->yaw = (float) (rand() % 360); // 0 do 360 stopni
}
/*
void detect_thermal(sensor_data_t *data, size_t sample_count, char *direction) {
    float height_sum = 0;
    float left_height = 0;
    float right_height = 0;
    size_t left_count = 0;
    size_t right_count = 0;

    for (size_t i = 0; i < sample_count; i++) {
        height_sum += data[i].height;
    }

    float avg_height = height_sum / sample_count;

    for (size_t i = 0; i < sample_count; i++) {
        if (data[i].yaw <= 180) {
            left_height += data[i].height;
            left_count++;
        } else {
            right_height += data[i].height;
            right_count++;
        }
    }

    float avg_left_height = left_height / left_count;
    float avg_right_height = right_height / right_count;

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
*/
