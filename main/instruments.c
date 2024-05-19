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
double updateVerticalVelocity(double gyro_x, double gyro_y, double gyro_z, double delta_czasu, bool feet) {

	 double pr_predkosc_z = gyro_z;
	    double delta_kat_z = pr_predkosc_z * delta_czasu;
	    double predkosc_pionowa;

	    if (feet) {

	        predkosc_pionowa = delta_kat_z * 114.591559;
	    } else {

	        predkosc_pionowa = delta_kat_z;
	    }

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



