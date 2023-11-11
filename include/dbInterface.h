#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <cJSON.h>
#include <esp_err.h>
#include <stdbool.h>

typedef struct ReadingsDataSet {
    bool timestampExists;
    int64_t timestamp;
    bool chargerStateExists;
    uint8_t chargerState;
    bool currentInExists;
    uint16_t currentIn;
    bool currentOutExists;
    uint16_t currentOut;
    bool batteryLevelExists;
    uint8_t batteryLevel;
    bool voltageLowerCellExists;
    uint16_t voltageLowerCell;
    bool voltageHigherCellExists;
    uint16_t voltageHigherCell;
    bool batteryTemperatureExists;
    float batteryTemperature;
    bool gyroscopeAccelXExists;
    float gyroscopeAccelX;
    bool gyroscopeAccelYExists;
    float gyroscopeAccelY;
    bool gyroscopeAccelZExists;
    float gyroscopeAccelZ;
    bool gyroscopeTemperatureExists;
    float gyroscopeTemperature;
    bool gpsLatExists;
    float gpsLat;
    bool gpsLonExists;
    float gpsLon;
    bool gpsUsedSatellitesExists;
    uint8_t gpsUsedSatellites;
    bool gpsSatellitesInViewExists;
    uint8_t gpsSatellitesInView;
    bool compassBearingExists;
    float compassBearing;
    bool magneticVariationExists;
    float magneticVariation;
} readings_data_set_t;

typedef struct ReadingsReadConfig {
    // if 0 all readings from start
    int64_t startTimestamp;
    // if 0 all readings till the last
    int64_t endTimestamp;
    bool getTimestamp;
    bool getChargerState;
    bool getCurrentIn;
    bool getCurrentOut;
    bool getBatteryLevel;
    bool getVoltageLowerCell;
    bool getVoltageHigherCell;
    bool getBatteryTemperature;
    bool getGyroscopeAccelX;
    bool getGyroscopeAccelY;
    bool getGyroscopeAccelZ;
    bool getGyroscopeTemperature;
    bool getGpsLat;
    bool getGpsLon;
    bool getGpsUsedSatellites;
    bool getGpsSatellitesInView;
    bool getCompassBearing;
    bool getMagneticVariation;
} readings_read_config_t;

esp_err_t dbInit();

esp_err_t dbStoreReadings(readings_data_set_t readings);

// jsonOut must be NULL and will be set to the returned JSON object if successful (the user is responsible to free the memory)
esp_err_t dbGetReadings(readings_read_config_t config, cJSON** jsonOut);

esp_err_t dbClearReadings();

// delete all entries below or equal date
esp_err_t dbTrimBelowReadings(int32_t date);

#endif  // DBINTERFACE_H
