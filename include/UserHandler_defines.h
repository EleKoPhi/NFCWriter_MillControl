// FILE NAMES

#define CONFIG_FILE                 "config.txt"
#define LOG_FILE                    "UserLog.txt"


// JSON FLAGS
#define JSON_FLAG_TIMESINGLE        "Zeit_einfach"
#define JSON_FLAG_TIMEDOUBLE        "Zeit_doppelt"
#define JSON_FLAG_SERVERSTATE       "ServerOn"
#define JSON_FLAG_PASSWORD          "PW"
#define JSON_FLAG_CHIPPAGE          "ChipPage"
#define JSON_FLAG_SSID              "SSID"

#define DEFAULT_SERVERSTATE         0
#define DEFAULT_PASSWORD            "123456789"
#define DEFAULT_SSID                "TEST123TEST"
#define DEFAULT_TIMEDOUBLE          20000
#define DEFAULT_TIMESINGLE          10000
#define DEFAULT_CHIPPAGE            4


// LOG DEFINES

#define LOG_SERPERATOR              ";"
#define SINGLE                      "S"
#define DOUBLE                      "D"
#define UNKNOWN_USER_STRING         "Unbekannt"


// NFC KEYS

#define PW_BUFFER                   {0xFF, 0xAB, 0xBA, 0xFF}
#define ACK_BUFFER                  {0xE, 0x5}


// MIN MAX TIMES

#define TiSingleMin                 1000
#define TiSingleMax                 20000
#define TiDoubleMin                 1000
#define TiDoubleMax                 40000