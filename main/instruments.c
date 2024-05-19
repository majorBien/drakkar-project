/*
 * instruments.c
 *
 *  Created on: 19 maj 2024
 *      Author: majorbien
 */

#include "instruments.h"


#include <math.h>

double calculateAltitude(double pressure, double seaLevelPressure) {
    return 44330.0 * (1.0 - pow((pressure / seaLevelPressure), 0.1903));
}

double updateVerticalVelocity(double deltaTime, double pressure) {
    static double previousAltitude = 0.0;
    static int firstRun = 1;

    // Obliczanie aktualnej wysokości na podstawie ciśnienia
    double currentAltitude = calculateAltitude(pressure, 1013.25);

    if (firstRun) {
        // Pierwsze uruchomienie, ustawienie wartości początkowych
        previousAltitude = currentAltitude;
        firstRun = 0;
    }

    // Obliczanie prędkości pionowej (przybliżenie przez różniczkowanie)
    double velocityZ = (currentAltitude - previousAltitude) / deltaTime;

    // Aktualizacja stanu
    previousAltitude = currentAltitude;

    return velocityZ;
}
