#ifndef UserHandler_defines
#define UserHandler_defines

// FILE NAMES

#define CONFIG_FILE "config.txt"
#define LOG_FILE "UserLog.txt"

// JSON FLAGS
#define JSON_FLAG_TIMESINGLE "Zeit_einfach"
#define JSON_FLAG_TIMEDOUBLE "Zeit_doppelt"
#define JSON_FLAG_SERVERSTATE "ServerOn"
#define JSON_FLAG_PASSWORD "PW"
#define JSON_FLAG_CHIPPAGE "ChipPage"
#define JSON_FLAG_SSID "SSID"
#define JSON_FLAG_KEY "KEY"
#define JSON_FLAG_SPLIT "Split"

#define DEFAULT_SERVERSTATE 0
#define DEFAULT_PASSWORD "123456789"
#define DEFAULT_SSID "TEST123TEST"
#define DEFAULT_TIMEDOUBLE 20000
#define DEFAULT_TIMESINGLE 10000
#define DEFAULT_TIMEDOUBLE_MAX 40000
#define DEFAULT_TIMESINGLE_MAX 40000
#define DEFAULT_CHIPPAGE 0x06 // V1 :4 // V2 :5 // SN007: 6 // Default: 0
#define DEFAULT_KEY 10
#define DEFAULT_Split 1
#define DEFAULT_CRC 0x00

// LOG DEFINES

#define LOG_SERPERATOR ";"
#define SINGLE "S"
#define DOUBLE "D"
#define UNKNOWN_USER_STRING "Unbekannt"

// NFC KEYS

#define PW_BUFFER \
    {             \
        0xFF, 0xAB, 0xBA, 0xFF}
#define ACK_BUFFER \
    {              \
        0xE, 0x5}

// MIN MAX TIMES

#define TI_SINGLE_MIN 1000
#define TI_SINGLE_MAX 20000
#define TI_DOUBLE_MIN 1000
#define TI_DOUBLE_MAX 40000

// Delayes

#define DEBOUNCE_KEYS_MS_MAX 300
#define DEBOUNCE_KEYS_MS_MIN 50

#define RISING_EDGE_NOT_ALLOWED 0x1
#define FALLING_EDGE_ALLOWED 0x2
#define KEYS_BLOCKED 0x3

#endif