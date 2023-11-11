#ifndef GPS_H
#define GPS_H

#include <driver/gpio.h>
#include <driver/uart.h>
#include <stdlib.h>

#include "settings.h"

typedef enum GpsPositionFixIndicator {
    FIX_NOT_AVAILABLE_OR_INVALID,
    GNSS_FIX,
    DGPS_FIX,
    PPS_FIX,
    REAL_TIME_KINEMATIC_FIX,
    DEAD_RECKONING_FIX,
    MANUAL_INPUT_MODE_FIX,
    SIMULATION_MODE_FIX
} gps_position_fix_indicator_t;

typedef enum GpsUnit {
    METER_OR_DEGREES_MAGNETIC,
    DEGREES_TRUE,
    KNOTS,
    KILOMETERS_PER_HOUR
} gps_unit_t;

typedef enum GpsMode {
    AUTONOMOUS_MODE,
    DIFFERENTIAL_MODE,
    ESTIMATED_MODE,
    FLOAT_RTK_MODE,
    MANUAL_INPUT_MODE,
    NOT_VALID_MODE,
    PRECISE_MODE,
    REAL_TIME_KINEMATIC_MODE,
    SIMULATOR_MODE
} gps_mode_t;

typedef enum GpsStatus {
    ACTIVE_STATUS,
    VOID_STATUS,
} gps_status_t;

typedef enum GpsGsaMode1 {
    MANUAL_GSA_MODE,
    AUTOMATIC_GSA_MODE,
} gps_gsa_mode_1_t;

typedef enum GpsGsaMode2 {
    FIX_NOT_AVAILABLE_GSA_MODE,
    TWO_DIMENSION_GSA_MODE,
    THREE_DIMENSION_GSA_MODE,
} gps_gsa_mode_2_t;

typedef struct GpsDate {
    uint8_t day;
    uint8_t month;
    uint8_t year;
} gps_date_t;

typedef struct GpsTime {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
} gps_time_t;

typedef struct GGA {
    gps_time_t time;
    float latitude;
    // N for north or S for south
    char northSouthIndicator;
    float longitude;
    // E for east or W for west
    char eastWestIndicator;
    gps_position_fix_indicator_t fixIndicator;
    uint8_t satellitesNumber;
    // horizontal dilution of precision
    float hdop;
    // mean sea level altitude
    float mlsAltitude;
    gps_unit_t mlsAltitudeUnit;
    float geoidHeight;
    gps_unit_t geoidHeightUnit;
    // age of differential GPS data record in seconds
    uint8_t ageOfDiffCorr;
    // reference station ID
    uint16_t diffRefStationId;
} gga_t;

typedef struct GLL {
    float latitude;
    // N for north or S for south
    char northSouthIndicator;
    float longitude;
    // E for east or W for west
    char eastWestIndicator;
    gps_time_t time;
    gps_status_t status;
    gps_mode_t mode;
} gll_t;

typedef struct GSA {
    gps_gsa_mode_1_t mode1;
    gps_gsa_mode_2_t mode2;
    uint8_t satelliteUsedOnChannel1;
    uint8_t satelliteUsedOnChannel2;
    uint8_t satelliteUsedOnChannel3;
    uint8_t satelliteUsedOnChannel4;
    uint8_t satelliteUsedOnChannel5;
    uint8_t satelliteUsedOnChannel6;
    uint8_t satelliteUsedOnChannel7;
    uint8_t satelliteUsedOnChannel8;
    uint8_t satelliteUsedOnChannel9;
    uint8_t satelliteUsedOnChannel10;
    uint8_t satelliteUsedOnChannel11;
    uint8_t satelliteUsedOnChannel12;
    // position dilution of precision
    float pdop;
    // horizontal dilution of precision
    float hdop;
    // vertical dilution of precision;
    float vdop;
} gsa_t;

typedef struct RMC {
    gps_time_t time;
    gps_status_t status;
    float latitude;
    // N for north or S for south
    char northSouthIndicator;
    float longitude;
    // E for east or W for west
    char eastWestIndicator;
    // in knots
    float speedOverGround;
    // in degrees true
    float trackMadeGood;
    gps_date_t date;
    // in degrees
    float magneticVariation;
    char eastwestMagneticVariation;
} rmc_t;

typedef struct VTG {
    float trackDegrees1;
    gps_unit_t trackDegrees1Unit;
    float trackDegrees2;
    gps_unit_t trackDegrees2Unit;
    // in knots
    float speed1;
    gps_unit_t speed1Unit;
    // in km/h
    float speed2;
    gps_unit_t speed2Unit;
    gps_mode_t mode;
} vtg_t;

typedef struct GSV {
    uint8_t totalNumberOfMessages;
    uint8_t messageNumber;
    uint16_t numberOfSatellitesInView;
} gsv_t;

typedef union NmeaData {
    gga_t gga;
    gll_t gll;
    gsa_t gsa;
    rmc_t rmc;
    vtg_t vtg;
    gsv_t gsv;
} nmea_data_t;

typedef enum NmeaDataType {
    NONE,
    GGA,
    GLL,
    GSA,
    RMC,
    VTG,
    GSV
} nmea_data_type_t;

esp_err_t gpsInit();

esp_err_t gpsClearLocalUartBuffer();

esp_err_t gpsTransmitData(const char* data, size_t len);

// do not forget to free nmea; nmea must be NULL
esp_err_t gpsReceiveNmea(char** nmea, size_t* len);

esp_err_t gpsParseNmea(char* nmea, size_t len, nmea_data_t* nmeaData, nmea_data_type_t* nmeaDataType);

#endif  // GPS_H
