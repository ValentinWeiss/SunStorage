# API

This file is used to define the endpoints of the GUI-API.

## REST Endpoints V1

### System Data

Description: Returns system information about the current state of the ESP32\
url: /api/v1/system\
method: GET\
Body Type: JSON string\
Ok StatusCode: 200\
Values:\
{\
 availableHeap: number;\
 availableInternalHeap: number;\
 cores: number;\
 features: number;\
 mac: string;\
 minimumHeap: number;\
 model: number;\
 version: string;\
}

### Access Points

Description: Returns a list of all close wifi accesspoints\
url: /api/v1/wifi/list\
method: GET\
Body Type: JSON string\
Ok StatusCode: 200\
Values:\
{\
 accessPoints: [\
 {\
 ssid: string;\
 rssi: number;\
 authmode: number;\
 channel: number;\
 }\
 ]\
}

### Set Wifi Settings

Description: Returns a list of all close wifi accesspoints\
url: /api/v1/wifi/configure\
method: POST\
Body Type: JSON string\
Ok StatusCode: 204\
Values:\
{\
 ssid: string;\
 pw: string;\
}

### Get State

Description: get solar panel config\
url: /api/v1/state\
method: GET\
Body Type: JSON string\
Ok StatusCode: 200\
Values:\
{\
 "charger": {\
 usbPowerMode: number; // value in W\
 enableBatteryCharger: boolean;\
 chargingCurrent: number; // value in mA\
 chargerEnableHysteresis: number;\
 maximumChargeVoltage: number; // value in mV\
 trickleThreshold: number; // value in mV
},\
"positioning": {\
 latitude?: number;\
 logitude?: number;\
 azimuth?: number;\
 elevation?: number;\
 angleToNorth?: number;\
 "manualMode": true/false\
 },\
 "battery": {\
 batterySize: number;\
 maxBatteryTemperature: number;\
 },\
 "display": {\
 "currentPower": bool,\
 "dailyProduction": bool,\
 "soc": bool,\
 "voltageLowerCell": bool\
 }
}

### Set State

alle werte optional nur wenn gesetzt wird er auch geupdated!
Description: Configures state\
url: /api/v1/state\
method: POST\
Body Type: JSON string\
Ok StatusCode: 204\
Values:\
{\
 usbPowerMode: number; // value in W\
 enableBatteryCharger: boolean;\
 chargingCurrent: number; // value in mA\
 chargerEnableHysteresis: number;\
 maximumChargeVoltage: number; // value in mV\
 trickleThreshold: number; // value in mV
"manualControl": true/false\
 latitude?: number;\
 logitude?: number;\
 azimuth?: number;\
 elevation?: number;\
 angleToNorth?: number;\
 batterySize: number;\
 maxBatteryTemperature: number;\
 "currentPower": bool,\
 "dailyProduction": bool,\
 "soc": bool,\
 "voltageLowerCell": bool\
}

### Reclaibrate State of Charge

Description: Triggers Reclaibratetion of State of Charge\
url: /api/v1/battery/calibrate\
method: GET\
Ok StatusCode: 204\
Body Type: No Content

### Shutdown Modules

alle werte optional, nur gesetzte werden ausgeschaltet
Description: Shuts Server down for XXXX Seconds\
url: /api/v1/shutdown
method: POST\
Ok StatusCode: 204\
Body Type: JSON string\
Values:\
{\
 "http": number\ // anzahl sekunden die ausgeschaltet wird
"gps": number\
 "wifi": number\
 "dcf77": number\
}

### Database

alle werte optional, nur gesetzte werden ausgeschaltet
Extracts information from databse, endTimestamp is optional-> Careful with load on esp. Keep timestamps close to each other
url: /api/v1/database
method: POST\
Body Type: JSON string\
Ok StatusCode: 204\
Values:\
{\
 startTimestamp: 20230613, // from timestamp
endTimestamp: 20230614, // until you want the values -> Aufpassen verursacht viel load auf dem ESP. Kleine Timestamps
getTimestamp: bool, // if you want this value from the database or not
getGpsLat: bool, // if you want this value from the database or not
getGpsLon: bool, // if you want this value from the database or not
getBatteryLevel: bool, // if you want this value from the database or not
getBatteryTemperature: bool, // if you want this value from the database or not
getChargerState: bool, // if you want this value from the database or not
getCompassBearing: bool, // if you want this value from the database or not
getCurrentIn: bool, // if you want this value from the database or not
getCurrentOut: bool, // if you want this value from the database or not
getGpsSatellitesInView: bool, // if you want this value from the database or not
getGpsUsedSatellites: bool, // if you want this value from the database or not
getGyroscopeAccelX: bool, // if you want this value from the database or not
getGyroscopeAccelY: bool, // if you want this value from the database or not
getGyroscopeAccelZ: bool, // if you want this value from the database or not
getGyroscopeTemperature: bool, // if you want this value from the database or not
getMagneticVariation: bool, // if you want this value from the database or not
getVoltageHigherCell: bool, // if you want this value from the database or not
getVoltageLowerCell: bool, // if you want this value from the database or not
}
Response with selected values!

### timestamp

Description: Set timestamp for database\
url: /api/v1/timestamp
method: POST\
Ok StatusCode: 204\
Body Type:\
{\
 "timestamp": "202306014" string
}

# Inputs from HTTP to Backend

| Suggested Frontend Name         | Suggested GUI Element                        | Frontend Values            | Backend Method                  | Backend Type                              |
| ------------------------------- | -------------------------------------------- | -------------------------- | ------------------------------- | ----------------------------------------- | --- |
| USB Power Mode                  | Combo                                        | 2.5 W, 5 W, 10 W           | setUsbMode() charger.h          | USB_MODE                                  |
| Enable Battery Charger          | Switch                                       | on, off                    | setSolarChargerState charger.h  | CHARGER_MODE (only CHARGING and DISABLED) |
| Charging Current (mA)           | Text                                         | 0 - 2000                   | setSolarCurrentTarget charger.h | uint16_t                                  |
| Charger Enable Hysteresis       | Text + Explanation                           | 0 - 2000                   | setThresholdVoltage charger.h   | uint16_t                                  |
| Maximum Charge Voltage (mV)     | Text + Explanation                           | 3000 - 4200                | setMaximumVoltage charger.h     | uint16_t                                  |
| Trickle Threshold (mV)          | Text + Explanation                           | 3000 - 4200                | setTrickleThreshold charger.h   | uint16_t                                  |
| Set Battery Size                | Text + Explanation                           | 500 - 20000                | setBatterySize soc.h            | uint16_t                                  |
| Horizontal Panel Angle          | Text/Slider                                  | 0 - 360                    |                                 |                                           |
| Vertical Panel Angle            | Text/Slider                                  | 0 - 90                     |                                 |                                           |
| Auto Adjust Panel Angle         | Switch (auto off when manual angle selected) | on, off                    |                                 |                                           |
| Turn off at night               | Switch                                       | on, off                    | TODO: what can be turned off?   |                                           |
| Manually Set Location           | Text                                         | Some GPS coordinate values |                                 |                                           |
| Manually Set Angle to North     | Text/Slider + Explanation                    | 0-360                      |                                 |                                           |
| Shutdown Server                 | Text (time in s) + button                    |                            |                                 |                                           |
| Reclaibrate State of Charge     | Push Button                                  |                            | recalibrateSoC(); soc.h         |                                           |
| Set Maximum Battery Temperature | Text/Slider                                  | Celsius                    |                                 |                                           |     |
