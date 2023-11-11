#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

typedef enum {
    DISABLED,
    CHARGING,
    FULL,
    BALANCING,
    OVERHEATED
} CHARGER_MODE;

typedef enum {
    _2_POINT_5_WATT,
    _5_WATT,
    _10_WATT
} USB_MODE;

#define NUM_DISPLAY_ITEMS 35

typedef struct {
    bool itemEnabled[NUM_DISPLAY_ITEMS];
} display_config_t;

extern const char* DISPLAY_NAMES[NUM_DISPLAY_ITEMS];

/*================== TASK SETTINGS ===================*/

#define STORE_READINGS_TASK_DELAY ((uint32_t)10000 / portTICK_PERIOD_MS)
#define STATE_WRITE_DELAY ((uint32_t)3000 / portTICK_PERIOD_MS)
#define USB_CHARGER_DELAY ((uint32_t)5000 / portTICK_PERIOD_MS)
#define SOC_TASK_DELAY ((uint32_t)60000000 / portTICK_PERIOD_MS)
#define BATTERY_CHARGER_TASK_DELAY ((uint32_t)3000 / portTICK_PERIOD_MS)
#define ADC_TASK_DELAY ((uint32_t)100 / portTICK_PERIOD_MS)
#define ESP_COMM_RECEIVE_TASK_DELAY ((uint32_t)500 / portTICK_PERIOD_MS)

#define STORE_READINGS_TASK_MAX_GPS_READS 10

/*====================================================*/

/*=================== MISC ===========================*/
#define xSemaphorePoll(semaphore) xSemaphoreTake(semaphore, 1000 / portTICK_PERIOD_MS)

#define ESP_HANDLE_ERR(error, msg) \
    if (error != ESP_OK) {         \
        ESP_LOGE("", msg);         \
        return error;              \
    }

/*====================================================*/

/*=================== I2C SETTINGS ===================*/

#define I2C_MASTER_PORT I2C_NUM_0
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MS_TO_WAIT 5000

/*====================================================*/

/*================== COMPASS MODULE ==================*/

#define COMPASS_MODULE_ADDR 0x0D  // 7 bit address of GY-271
#define CM_MODE_STANDBY 0x00
#define CM_MODE_CONTINUOUS_MEASUREMENT 0x01

// axis output registers
#define CM_DOXL 0x00
#define CM_DOXH 0x01
#define CM_DOYL 0x02
#define CM_DOYH 0x03
#define CM_DOZL 0x04
#define CM_DOZH 0x05

#define CM_SR 0x06  // status register

// temperature output registers
#define CM_TOL 0x07
#define CM_TOH 0x08

// control registers
#define CM_CR0 0x09
#define CM_CR1 0x0A

#define CM_FBR 0x0B  // SET/RESET period

#define CM_UPDATE_DATA_RATE 0b00      // 0b00 -> 10Hz; 0b01 -> 50Hz; 0b10 -> 100Hz; 0b11 -> 200Hz
#define CM_MAGNETIC_FIELD_RANGE 0b00  // 0b00 -> 2G; 0b01 -> 8G
#define CM_OVER_SAMPLE_RATIO 0b00     // 0b00 -> 512; 0b01 -> 256; 0b10 -> 128; 0b11 -> 64
#define CM_ROL_PNT 1                  // enable rolling pointer for continuous data readout
#define CM_INT_DISABLE 1              // disable interrupt on DRDY PIN

/*====================================================*/

/*==================== BAROMETER =====================*/

#define BAROMETER_ADDR 0x77  // 7 bit address of GY-68
#define BMP_POWER_ON_RESET 0xB6
#define BMP_CHIP_ID 0x55

#define BMP_MEAS_CTRL_TEMP 0x2E
#define BMP_MEAS_CTRL_PRES 0x34

// output registers
#define BMP_DOXLSB 0xF8
#define BMP_DOLSB 0xF7
#define BMP_DOMSB 0xF6

#define BMP_CR 0xF4   // control register
#define BMP_RST 0xE0  // reset register
#define BMP_IDR 0xD0  // chip id register

// calibration register
#define BMP_CAL_AC1H 0xAA
#define BMP_CAL_AC1L 0xAB
#define BMP_CAL_AC2H 0xAC
#define BMP_CAL_AC2L 0xAD
#define BMP_CAL_AC3H 0xAE
#define BMP_CAL_AC3L 0xAF
#define BMP_CAL_AC4H 0xB0
#define BMP_CAL_AC4L 0xB1
#define BMP_CAL_AC5H 0xB2
#define BMP_CAL_AC5L 0xB3
#define BMP_CAL_AC6H 0xB4
#define BMP_CAL_AC6L 0xB5
#define BMP_CAL_B1H 0xB6
#define BMP_CAL_B1L 0xB7
#define BMP_CAL_B2H 0xB8
#define BMP_CAL_B2L 0xB9
#define BMP_CAL_MBH 0xBA
#define BMP_CAL_MBL 0xBB
#define BMP_CAL_MCH 0xBC
#define BMP_CAL_MCL 0xBD
#define BMP_CAL_MDH 0xBE
#define BMP_CAL_MDL 0xBF

#define BMP_OVER_SAMPLE_RATIO 0b00     // 0b00 -> ultra low power (single); 0b01 -> standard (2 times); 0b10 -> high resolution (4 times); 0b11 -> ultra high resolution (8 times)
#define BMP_MEASUREMENT_DELAY_US 5000  // OSR = 0b00 -> 5ms; OSR = 0b01 -> 8ms; OSR = 0b10 -> 14ms; OSR = 0b11 -> 26ms;

/*====================================================*/

/*==================== GYROSCOPE =====================*/

#define GYROSCOPE_ADDR_LOW 0x68   // 7 bit address of MPU6050 with address pin low (GND).
#define GYROSCOPE_ADDR_HIGH 0x69  // 7 bit address of MPU6050 with address pin high (VCC).
#define GYROSCOPE_ADDR GYROSCOPE_ADDR_LOW

#define GY_BAND_260_HZ 0
#define GY_BAND_184_HZ 1
#define GY_BAND_94_HZ 2
#define GY_BAND_44_HZ 3
#define GY_BAND_21_HZ 4
#define GY_BAND_10_HZ 5
#define GY_BAND_5_HZ 6

#define GY_GYRO_RANGE_250_DEG 0
#define GY_GYRO_RANGE_500_DEG 1
#define GY_GYRO_RANGE_1000_DEG 2
#define GY_GYRO_RANGE_2000_DEG 3

#define GY_ACCEL_RANGE_2_G 0
#define GY_ACCEL_RANGE_4_G 1
#define GY_ACCEL_RANGE_8_G 2
#define GY_ACCEL_RANGE_16_G 3

#define GY_RST_GYRO_PATH 0x04
#define GY_RST_ACCEL_PATH 0x02
#define GY_RST_TEMP_PATH 0x01
#define GY_STBY_XA 0x20  // puts the X axis accelerometer into standby mode
#define GY_STBY_YA 0x10  // puts the Y axis accelerometer into standby mode
#define GY_STBY_ZA 0x08  // puts the Z axis accelerometer into standby mode
#define GY_STBY_XG 0x04  // puts the X axis gyroscope into standby mode
#define GY_STBY_YG 0x02  // puts the Y axis gyroscope into standby mode
#define GY_STBY_ZG 0x01  // puts the Z axis gyroscope into standby mode

// registers
#define GY_XG_OFFS_TC (0x00)
#define GY_YG_OFFS_TC (0x01)
#define GY_ZG_OFFS_TC (0x02)
#define GY_X_FINE_GAIN (0x03)
#define GY_Y_FINE_GAIN (0x04)
#define GY_Z_FINE_GAIN (0x05)
#define GY_XA_OFFS_H (0x06)
#define GY_XA_OFFS_L_TC (0x07)
#define GY_YA_OFFS_H (0x08)
#define GY_YA_OFFS_L_TC (0x09)
#define GY_ZA_OFFS_H (0x0A)
#define GY_ZA_OFFS_L_TC (0x0B)
#define GY_SELF_TEST_X (0x0D)
#define GY_SELF_TEST_Y (0x0E)
#define GY_SELF_TEST_Z (0x0F)
#define GY_SELF_TEST_A (0x10)
#define GY_XG_OFFS_USRH (0x13)
#define GY_XG_OFFS_USRL (0x14)
#define GY_YG_OFFS_USRH (0x15)
#define GY_YG_OFFS_USRL (0x16)
#define GY_ZG_OFFS_USRH (0x17)
#define GY_ZG_OFFS_USRL (0x18)
#define GY_SMPRT_DIV (0x19)
#define GY_CONFIG (0x1A)
#define GY_GYRO_CONFIG (0x1B)
#define GY_ACCEL_CONFIG (0x1C)
#define GY_FIFO_EN (0x23)
#define GY_I2C_MST_CTRL (0x24)
#define GY_I2C_SLV0_ADDR (0x25)
#define GY_I2C_SLV0_REG (0x26)
#define GY_I2C_SLV0_CTRL (0x27)
#define GY_I2C_SLV1_ADDR (0x28)
#define GY_I2C_SLV1_REG (0x29)
#define GY_I2C_SLV1_CTRL (0x2A)
#define GY_I2C_SLV2_ADDR (0x2B)
#define GY_I2C_SLV2_REG (0x2C)
#define GY_I2C_SLV2_CTRL (0x2D)
#define GY_I2C_SLV3_ADDR (0x2E)
#define GY_I2C_SLV3_REG (0x2F)
#define GY_I2C_SLV3_CTRL (0x30)
#define GY_I2C_SLV4_ADDR (0x31)
#define GY_I2C_SLV4_REG (0x32)
#define GY_I2C_SLV4_DO (0x33)
#define GY_I2C_SLV4_CTRL (0x34)
#define GY_I2C_SLV4_DI (0x35)
#define GY_I2C_MST_STATUS (0x36)
#define GY_INT_PIN_CFG (0x37)
#define GY_INT_ENABLE (0x38)
#define GY_INT_STATUS (0x3A)
#define GY_ACCEL_XOUT_H (0x3B)
#define GY_ACCEL_XOUT_L (0x3C)
#define GY_ACCEL_YOUT_H (0x3D)
#define GY_ACCEL_YOUT_L (0x3E)
#define GY_ACCEL_ZOUT_H (0x3F)
#define GY_ACCEL_ZOUT_L (0x40)
#define GY_TEMP_OUT_H (0x41)
#define GY_TEMP_OUT_L (0x42)
#define GY_GYRO_XOUT_H (0x43)
#define GY_GYRO_XOUT_L (0x44)
#define GY_GYRO_YOUT_H (0x45)
#define GY_GYRO_YOUT_L (0x46)
#define GY_GYRO_ZOUT_H (0x47)
#define GY_GYRO_ZOUT_L (0x48)
#define GY_EXT_SENS_DATA_00 (0x49)
#define GY_EXT_SENS_DATA_01 (0x4A)
#define GY_EXT_SENS_DATA_02 (0x4B)
#define GY_EXT_SENS_DATA_03 (0x4C)
#define GY_EXT_SENS_DATA_04 (0x4D)
#define GY_EXT_SENS_DATA_05 (0x4E)
#define GY_EXT_SENS_DATA_06 (0x4F)
#define GY_EXT_SENS_DATA_07 (0x50)
#define GY_EXT_SENS_DATA_08 (0x51)
#define GY_EXT_SENS_DATA_09 (0x52)
#define GY_EXT_SENS_DATA_10 (0x53)
#define GY_EXT_SENS_DATA_11 (0x54)
#define GY_EXT_SENS_DATA_12 (0x55)
#define GY_EXT_SENS_DATA_13 (0x56)
#define GY_EXT_SENS_DATA_14 (0x57)
#define GY_EXT_SENS_DATA_15 (0x58)
#define GY_EXT_SENS_DATA_16 (0x59)
#define GY_EXT_SENS_DATA_17 (0x5A)
#define GY_EXT_SENS_DATA_18 (0x5B)
#define GY_EXT_SENS_DATA_19 (0x5C)
#define GY_EXT_SENS_DATA_20 (0x5D)
#define GY_EXT_SENS_DATA_21 (0x5E)
#define GY_EXT_SENS_DATA_22 (0x5F)
#define GY_EXT_SENS_DATA_23 (0x60)
#define GY_MOT_DETECT_STATUS (0x61)
#define GY_I2C_SLV0_DO (0x63)
#define GY_I2C_SLV1_DO (0x64)
#define GY_I2C_SLV2_DO (0x65)
#define GY_I2C_SLV3_DO (0x66)
#define GY_I2C_MST_DELAY_CTRL (0x67)
#define GY_SIGNAL_PATH_RESET (0x68)
#define GY_USER_CTRL (0x6A)
#define GY_PWR_MGMT_1 (0x6B)
#define GY_PWR_MGMT_2 (0x6C)
#define GY_FIFO_COUNTH (0x72)
#define GY_FIFO_COUNTL (0x73)
#define GY_FIFO_R_W (0x74)
#define GY_WHO_AM_I (0x75)

#define GY_SENSOR_GRAVITY 9.80665f
#define GY_DEGPS_TO_RADPS 0.017453293f

/*====================================================*/

/*======================= GPS ========================*/

// UART settings

#define GPS_UART_NUM UART_NUM_2
#define GPS_UART_BAUD_RATE 9600
#define GPS_UART_DATA_BITS UART_DATA_8_BITS
#define GPS_UART_PARITY UART_PARITY_DISABLE
#define GPS_UART_STOP_BITS UART_STOP_BITS_1
#define GPS_UART_FLOW_CTRL UART_HW_FLOWCTRL_CTS_RTS
#define GPS_UART_RX_FLOW_CTRL_THRESH 122
#define GPS_UART_RX_BUFFER_SIZE 200
#define GPS_UART_TX_BUFFER_SIZE 0

#define GPS_GPIO_RX GPIO_NUM_16
#define GPS_GPIO_TX GPIO_NUM_17

#define GPS_NMEA_MAX_SENTENCE_LENGTH 120
#define GPS_NMEA_MAX_WORD_COUNT 20

#define GPS_NMEA_MIN_LEN 20

/**
 Different commands to set the update rate from once a second (1 Hz) to 10 times
 a second (10Hz) Note that these only control the rate at which the position is
 echoed, to actually speed up the position fix you must also send one of the
 position fix rate commands below too. */
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ "$PMTK220,10000*2F"  ///< Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ "$PMTK220,5000*1B"   ///< Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F"              ///<  1 Hz
#define PMTK_SET_NMEA_UPDATE_2HZ "$PMTK220,500*2B"               ///<  2 Hz
#define PMTK_SET_NMEA_UPDATE_5HZ "$PMTK220,200*2C"               ///<  5 Hz
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"              ///< 10 Hz
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ "$PMTK300,10000,0,0,0,0*2C"  ///< Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ "$PMTK300,5000,0,0,0,0*18"   ///< Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ "$PMTK300,1000,0,0,0,0*1C"              ///< 1 Hz
#define PMTK_API_SET_FIX_CTL_5HZ "$PMTK300,200,0,0,0,0*2F"               ///< 5 Hz
// Can't fix position faster than 5 times a second!

#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F"  ///< 115200 bps
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"    ///<  57600 bps
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"      ///<   9600 bps

#define PMTK_SET_NMEA_OUTPUT_GLLONLY "$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on only the GPGLL sentence
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on only the GPRMC sentence
#define PMTK_SET_NMEA_OUTPUT_VTGONLY "$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on only the GPVTG
#define PMTK_SET_NMEA_OUTPUT_GGAONLY "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on just the GPGGA
#define PMTK_SET_NMEA_OUTPUT_GSAONLY "$PMTK314,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on just the GPGSA
#define PMTK_SET_NMEA_OUTPUT_GSVONLY "$PMTK314,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*29"    ///< turn on just the GPGSV
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"     ///< turn on GPRMC and GPGGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGAGSA "$PMTK314,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"  ///< turn on GPRMC, GPGGA and GPGSA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"    ///< turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"        ///< turn off output

// to generate your own sentences, check out the MTK command datasheet and use a
// checksum calculator such as the awesome
// http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG "$PMTK185,0*22"          ///< Start logging data
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"           ///< Stop logging data
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"  ///< Acknowledge the start or stop command
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"        ///< Query the logging status
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"       ///< Erase the log flash data
#define LOCUS_OVERLAP 0                              ///< If flash is full, log will overwrite old data with new logs
#define LOCUS_FULLSTOP 1                             ///< If flash is full, logging will stop

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"  ///< Enable search for SBAS satellite (only works with 1Hz output rate)
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"  ///< Use WAAS for DGPS correction data

#define PMTK_STANDBY "$PMTK161,0*28"  ///< standby command & boot successful message
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"
#define PMTK_AWAKE "$PMTK010,002*2D"  ///< Wake up

#define PMTK_Q_RELEASE "$PMTK605*31"  ///< ask for the release and version

#define PGCMD_ANTENNA "$PGCMD,33,1*6C"    ///< request for updates on antenna status
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D"  ///< don't show antenna status messages

#define MAXWAITSENTENCE 10  ///< how long to wait when we're looking for a response

/*====================================================*/

/*=============== WIFI SERVICE SETTINGS ==============*/

#define WIFI_FACTORY_SSID "SunStorage"
#define WIFI_FACTORY_PASSWORD "sunstorage123"
#define WIFI_FACTORY_CHANNEL 1                 // use channel 1 as default wifi channel
#define WIFI_FACTORY_MAX_STA_CONN 4            // allow ony up to 4 connections to access point
#define WIFI_FACTORY_IP_ADDRESS "192.168.1.1"  // default ip for access point
#define WIFI_FACTORY_GATEWAY_ADDRESS \
    "192.168.1.1"  // default gateway for access point
#define WIFI_FACTORY_SUBNETMASK \
    "255.255.255.0"  // default subnetmask for access point

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_MAXIMUM_CONNECTION_RETRY 5
#define WIFI_DEFAULT_SCAN_LIST_SIZE 10
#define WIFI_CONFIG_FILE "wifi.txt"

/*====================================================*/

/*============= SD CARD SERVICE SETTINGS =============*/

#define SDC_MOUNT_POINT "/sdcard"
#define SDC_PIN_NUM_MISO GPIO_NUM_5
#define SDC_PIN_NUM_MOSI GPIO_NUM_18
#define SDC_PIN_NUM_CLK GPIO_NUM_19
#define SDC_PIN_NUM_CS GPIO_NUM_23

/*====================================================*/

/*=============== HTTP SERVICE SETTINGS ==============*/

#define HTTP_FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define HTTP_SCRATCH_BUFSIZE (10240)
#define HTTP_CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)
#define HTTP_MAX_URI_HANDLERS 15

/*====================================================*/

/*=============== SPIFFS SETTINGS ==============*/

#define SPIFFS_BASE_PATH "/spiffs"

/*====================================================*/

/*================ DATABASE SETTINGS =================*/

#define DB_FOLDER "readings"

#define DB_TIMESTAMP_DATE_DEVIDER 1000000
#define DB_MAX_LINE_LENGTH 300

#define DB_COL_COUNT 18
#define DB_READINGS_COL_TIMESTAMP "TimestampID"
#define DB_READINGS_COL_CHARGER_STATE "ChargerState"
#define DB_READINGS_COL_CURRENT_IN "CurrentIn"
#define DB_READINGS_COL_CURRENT_OUT "CurrentOut"
#define DB_READINGS_COL_BATTERY_LEVEL "BatteryLevel"
#define DB_READINGS_COL_VOLTAGE_LOWER_CELL "VoltageLowerCell"
#define DB_READINGS_COL_VOLTAGE_HIGHER_CELL "VoltageHigherCell"
#define DB_READINGS_COL_BATTERY_TEMPERATURE "BatteryTemperature"
#define DB_READINGS_COL_GYROSCOPE_ACCEL_X "GyroscopeAccelX"
#define DB_READINGS_COL_GYROSCOPE_ACCEL_Y "GyroscopeAccelY"
#define DB_READINGS_COL_GYROSCOPE_ACCEL_Z "GyroscopeAccelZ"
#define DB_READINGS_COL_GYROSCOPE_TEMPERATURE "GyroscopeTemperature"
#define DB_READINGS_COL_GPS_LAT "GpsLat"
#define DB_READINGS_COL_GPS_LON "GpsLon"
#define DB_READINGS_COL_GPS_USED_SATELLITES "GpsUsedSatellites"
#define DB_READINGS_COL_GPS_SATELLITES_IN_VIEW "GpsSatellitesInView"
#define DB_READINGS_COL_COMPASS_BEARING "CompassBearing"
#define DB_READINGS_COL_MAGNETIC_VARIATION "MagneticVariation"

/*====================================================*/

/*================ CHARGER SETTINGS ==================*/

#define CHARGER_LEDC_TIMER_SOLAR LEDC_TIMER_0
#define CHARGER_LEDC_MODE LEDC_LOW_SPEED_MODE
#define CHARGER_LEDC_OUTPUT_SOLAR (27)
#define CHARGER_LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define CHARGER_LEDC_FREQUENCY (5000)
#define CHARGER_GPIO_LOWER_CELL_BALANCING GPIO_NUM_12
#define CHARGER_GPIO_HIGHER_CELL_BALANCING GPIO_NUM_14
#define CHARGER_GPIO_HIGH_POWER_DISABLE GPIO_NUM_2

/*====================================================*/

/*============ Night Shutdown SETTINGS ===============*/

#define MIN_CHARGE_LEFT 20   // min. charge in % that has to be left in battery to engage night shutdown
#define SUNRISE_BUFFER -1.0  // in hours; here one hour before sunrise
#define SUNSET_BUFFER 1.0    // in hours; here one hour after sunset

#define NIGHT_SHUTDOWN_EVENT 0x01

/*====================================================*/

/*================== ADC SETTINGS ====================*/

#define ADC_CH_CURRENT_SOLAR ADC_CHANNEL_5
#define ADC_CH_CURRENT_SYSTEM ADC_CHANNEL_6
#define ADC_CH_VOLTAGE_LOWER_CELL ADC_CHANNEL_7
#define ADC_CH_VOLTAGE_BATTERY ADC_CHANNEL_4

/*====================================================*/

/*================== SERVO SETTINGS ==================*/

#define OUTPUT_MOTOR_ROTATE (4)
#define OUTPUT_MOTOR_TILT (5)
#define DELAY_VALUE (50)

/*====================================================*/

/*============ Night Shutdown SETTINGS ===============*/

#define MIN_CHARGE_LEFT 20   // min. charge in % that has to be left in battery to engage night shutdown
#define TILT_SHUTDOWN 90     // in Degree
#define ROTATE_SHUTDOWN 0    // in Degree
#define SUNRISE_BUFFER -1.0  // in hours; here one hour before sunrise
#define SUNSET_BUFFER 1.0    // in hours; here one hour after sunset

#define HOURS_TO_SECONDS 3600

#define NIGHT_SHUTDOWN_EVENT 0x01

/*====================================================*/

/*============ STATE SETTINGS ========================*/

// general
#define STATE_FILE "state.txt"

// object names, define one per file
#define STATE_OBJ_CHARGER "charger"

// //////////
// item names

// charger
#define STATE_CHARGER_USBMODE "usbMode"
#define STATE_CHARGER_USBMODE_DEFAULT _2_POINT_5_WATT
#define STATE_CHARGER_CHARGER_MODE "chargerMode"
#define STATE_CHARGER_CHARGER_MODE_DEFAULT CHARGING
#define STATE_CHARGER_CURRENT_TARGET "currentTarget"
#define STATE_CHARGER_CURRENT_TARGET_DEFAULT 100
#define STATE_CHARGER_THRESHOLD_VOLTAGE "thresholdVoltage"
#define STATE_CHARGER_THRESHOLD_VOLTAGE_DEFAULT 200
#define STATE_CHARGER_MAXIMUM_VOLTAGE "maximumVoltage"
#define STATE_CHARGER_MAXIMUM_VOLTAGE_DEFAULT 4050
#define STATE_CHARGER_TRICKLE_THRESHOLD "trickleThreshold"
#define STATE_CHARGER_TRICKLE_THRESHOLD_DEFAULT 3900
#define STATE_CHARGER_BATTERY_SIZE "batterySize"
#define STATE_CHARGER_BATTERY_SIZE_DEFAULT 1500
#define STATE_CHARGER_OVERHEATED_TEMPERATURE "overheatedTemperature"
#define STATE_CHARGER_OVERHEATED_TEMPERATURE_DEFAULT 40
#define STATE_CHARGER_HIGH_POWER_ENABLED "highPowerEnabled"
#define STATE_CHARGER_HIGH_POWER_ENABLED_DEFAULT true
#define STATE_CHARGER_OVERHEATED_HYSTERESESIS "overheatedHysteresis"
#define STATE_CHARGER_OVERHEATED_HYSTERESESIS_DEFAULT 5

/*====================================================*/

/*================== ESP COMMUNICATION SETTINGS ==================*/
#define COM_SENDER_NUM UART_NUM_0
#define COM_RECEIVER_NUM UART_NUM_2
#define COM_SENDER_RX GPIO_NUM_3
#define COM_SENDER_TX GPIO_NUM_1
#define COM_RECEIVER_TX GPIO_NUM_17
#define COM_RECEIVER_RX GPIO_NUM_16
#define COM_BAUDRATE 115200
#define COM_BUFFER (1024 * 2)
/*====================================================*/

/*=================== LCD MENU SETTINGS ===================*/

#define LCD_MENU_TIMEOUT (60000 / portTICK_PERIOD_MS)  // 1 min Timeout

extern display_config_t DEFAULT_DISPLAY_CONFIG;

#define MAX_NAME_LENGTH 28

/*====================================================*/

/*=================== LCD DISPLAY SETTINGS ===================*/

#define LCD_PIN_E GPIO_NUM_18
#define LCD_PIN_RS GPIO_NUM_19
#define LCD_PIN_D7 GPIO_NUM_26
#define LCD_PIN_D6 GPIO_NUM_15
#define LCD_PIN_D5 GPIO_NUM_27
#define LCD_PIN_D4 GPIO_NUM_25
#define LCD_PIN_ON_OFF GPIO_NUM_13

/*====================================================*/

/*=================== LCD BUTTONS SETTINGS ===================*/

#define LCD_MENU_TIMEOUT (60000 / portTICK_PERIOD_MS)  // 1 min Timeout
#define LCD_MENU_BUTTON_0 GPIO_NUM_2                   // next
#define LCD_MENU_BUTTON_1 GPIO_NUM_36                  // previous

#define BUTTONS_POLLING_RATE (200 / portTICK_PERIOD_MS)  // under 250 ACTION_CONF propably won't trigger
#define BOUNCE_DELAY (10 / portTICK_PERIOD_MS)
#define DOUBLE_CLICK_DELAY pdMS_TO_TICKS(300)

/*====================================================*/

/*=================== Coulomb Counter SETTINGS ===================*/

#define ADC_CH_COULOMB_COUNTER ADC_CHANNEL_3
#define COULOMB_COUNTER_DISCHARGE_PIN GPIO_NUM_13
#define VCC 3300        // Vcc in milivolt
#define RESISTOR 24000  // in Ohms
#define COULOMB_TO_AMPERE_HOURS 3600
/*====================================================*/

/*================== DCF77 MODULE SETTINGS ====================*/

#define DCF77_PIN GPIO_NUM_15

#define DCF77_DELAY (20 / portTICK_PERIOD_MS)

/*====================================================*/

/*================== DCF77 MODULE SETTINGS ====================*/
#define STATS_CALC_DELAY (600000 / portTICK_PERIOD_MS)
/*====================================================*/

#endif  // SETTINGS_H
