#include "sunpos.h"

#define DEG_TO_RAD (3.1415 / 180)
#define PI 3.1415
#define radiansPerDegree 0.0174533

const int millisecondsPerSecond = 1000;
const int secondsPerMinute = 60;
const int minutesPerHour = 60;
const int hoursPerDay = 24;
const double earthRotationDegreesPerHour = 15;
const double daysPerYear = 365.25;
const double fullCircleDegrees = 360;
const double halfCircleDegrees = 180;
const int march22DayOfYear = 81;
const double maxDeclinationAngle = 23.45;
const int solarNoonHours = 12;

const int secondsPerHour = secondsPerMinute * minutesPerHour;
const int millisecondsPerDay = hoursPerDay * secondsPerHour * millisecondsPerSecond;

int getDayOfYear(int year, int month, int day) {
    int i;
    int dayOfYear = 0;
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    for (i = 0; i < month - 1; i++) {
        dayOfYear += daysInMonth[i];
    }
    dayOfYear += day;
    return dayOfYear;
}

sun_direction_t calculateSunDirection(location_t loc, int year, int month, int day, int hour, int minute, int second, int timeZoneOffsetInHours) {
    double dayOfYear = getDayOfYear(year, month, day);
    double LT = hour + minute / (double)minutesPerHour + second / (double)secondsPerHour;  // hours

    double deltaTutc = timeZoneOffsetInHours;
    double LSTM = earthRotationDegreesPerHour * deltaTutc;

    double B = (fullCircleDegrees / daysPerYear) * (dayOfYear - march22DayOfYear);
    double EoT = 9.87 * sin(2 * B * radiansPerDegree) - 7.53 * cos(B * radiansPerDegree) - 1.5 * sin(B * radiansPerDegree);  // minutes

    double TC = 4 * (loc.longitude - LSTM) + EoT;  // minutes

    double LST = LT + TC / (double)minutesPerHour;  // hours

    // NOTE: LST cannot be negative
    // but it can happen when TC is big and LT small (new day)
    // only small negative numbers tho
    // HOTFIX: Normalize value hours
    if (LST < 0)
        LST += hoursPerDay;

    double HRA = earthRotationDegreesPerHour * (LST - solarNoonHours);

    // HRA is also used for azimuth -> hra need to be between [180, -180]
    // here something like 188° can happen -> normalize angle
    if (HRA > halfCircleDegrees)
        HRA -= fullCircleDegrees;
    if (HRA < -halfCircleDegrees)
        HRA += fullCircleDegrees;

    double phi = loc.latitude;

    double delta = asin(sin(maxDeclinationAngle * radiansPerDegree) * sin(B * radiansPerDegree)) / radiansPerDegree;

    double elevation = asin(sin(phi * radiansPerDegree) * sin(delta * radiansPerDegree) + cos(phi * radiansPerDegree) * cos(delta * radiansPerDegree) * cos(HRA * radiansPerDegree)) / radiansPerDegree;

    double azimuth = acos((sin(delta * radiansPerDegree) * cos(phi * radiansPerDegree) - cos(delta * radiansPerDegree) * sin(phi * radiansPerDegree) * cos(HRA * radiansPerDegree)) / cos(elevation * radiansPerDegree)) / radiansPerDegree;

    if (HRA <= 0) {
        azimuth = fullCircleDegrees - azimuth;
    }

    sun_direction_t result;
    result.azimuthAngle = azimuth;
    result.elevationAngle = elevation;

    return result;
}

double toRadians(double degrees) {
    return degrees * (PI / 180.0);
}

double toDegrees(double radians) {
    return radians * (180.0 / PI);
}

/*
 * Based on this algorithm: http://edwilliams.org/sunrise_sunset_algorithm.htm
 */
float calculateSunTimeExtrems(bool isRise, float latitude, float longitude, uint8_t month, uint8_t day, uint8_t tz, uint8_t dts) {
    int sunMov = 6;
    if (!isRise) {
        sunMov = 18;
    }

    double zenith = -0.83;

    int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int n = days[month - 1] + day;

    double longHour = longitude / 15.0;
    double t = n + ((sunMov - longHour) / 24);

    double M = (0.9856 * t) - 3.289;

    double L = fmod(M + (1.916 * sin(toRadians(M))) + (0.020 * sin(2 * toRadians(M))) + 282.634, 360.0);

    double RA = fmod(toDegrees(atan(0.91764 * tan(toRadians(L)))), 360.0);

    double Lquadrant = (floor(L / 90.0)) * 90.0;
    double RAquadrant = (floor(RA / 90.0)) * 90.0;
    RA = RA + (Lquadrant - RAquadrant);

    RA = RA / 15.0;

    double sinDec = 0.39782 * sin(toRadians(L));
    double cosDec = cos(asin(sinDec));

    double cosH = (sin(toRadians(zenith)) - (sinDec * sin(toRadians(latitude)))) / (cosDec * cos(toRadians(latitude)));

    double H = 360.0 - toDegrees(acos(cosH));

    if (!isRise) {
        H = toDegrees(acos(cosH));
    }

    H = H / 15.0;

    double T = H + RA - (0.06571 * t) - 6.622;

    float UT = T - longHour + tz + dts;

    if (UT > 24.0)
        UT = UT - 24.0;
    else if (UT < 0.0)
        UT = UT + 24.0;

    return UT;
}
