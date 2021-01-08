#include "Arduino.h"
#include "UserHandler.h"
#include "Controller_defines.h"
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include "UserHandler_defines.h"

byte PSWBuff[] = PW_BUFFER;
byte pACK[] = ACK_BUFFER;

int &UserHandler::GetChipSelectSD() { return ChipSelect_SD; }
void UserHandler::SetChipSelectSD(int PIN) { ChipSelect_SD = PIN; }
bool &UserHandler::GetSDStatus() { return SdStatus; }
void UserHandler::SetSDStatus(bool Status) { SdStatus = Status; }
bool &UserHandler::GetRTCStatus() { return RtcStatus; }
void UserHandler::SetRTCStatus(bool Status) { RtcStatus = Status; }
bool &UserHandler::GetNFCStatus() { return NfcStatus; }
void UserHandler::SetNFCStatus(bool Status) { NfcStatus = Status; }
String &UserHandler::GetUser() { return User; }
void UserHandler::SetUser(String user) { User = user; }
MFRC522 &UserHandler::GetNFCReader() { return _nfcReader; }
RTC_DS3231 &UserHandler::GetRTC() { return _rtc; }
File &UserHandler::GetLogFile() { return _logFile; }
void UserHandler::SetLogFile(File filename) { _logFile = filename; }
void UserHandler::SetUserKey(int key) { UserKey = key; }
int &UserHandler::GetUserKey() { return UserKey; }

bool &UserHandler::GetStLeft() { return KeyLeft; }
void UserHandler::SetStLeft(bool st) { KeyLeft = st; }
bool &UserHandler::GetStRigth() { return KeyRight; }
void UserHandler::SetStRight(bool st) { KeyRight = st; }
bool &UserHandler::GetStBoth() { return KeyBoth; }
void UserHandler::SetStBoth(bool st) { KeyBoth = st; }

void UserHandler::StartKeyDebounce() { debounce = millis(); };
byte UserHandler::DebounceFinished(unsigned long max, unsigned long min)
{
        if ((millis() - GetTimer()) > max)
        {
                return FALLING_EDGE_ALLOWED;
        }
        else if ((millis() - GetTimer()) < min)
        {
                return RISING_EDGE_NOT_ALLOWED;
        }
        else
        {
                return KEYS_BLOCKED;
        }
}
unsigned long &UserHandler::GetTimer() { return debounce; };
void UserHandler::SetTimer(long ti) { debounce = ti; }

bool UserHandler::KeyRight = false;
bool UserHandler::KeyLeft = false;
bool UserHandler::KeyBoth = false;

unsigned long UserHandler::debounce = 0;

UserHandler::UserHandler(int chipSelect, int slaveSelect, int rstPin) : _nfcReader(slaveSelect, rstPin)
{
        SetChipSelectSD(chipSelect);
        SetSDStatus(false);
        SetNFCStatus(false);
        SetRTCStatus(false);
        SetUser("");
}

void UserHandler::loadConfiguration()
{
        SD.begin(GetChipSelectSD());
        File _file = SD.open(CONFIG_FILE, FILE_READ);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, _file);
        if (error)
                Serial.println(F("Failed to read file, using default configuration"));

        config.single_time = doc[JSON_FLAG_TIMESINGLE] | DEFAULT_TIMESINGLE;
        config.double_time = doc[JSON_FLAG_TIMEDOUBLE] | DEFAULT_TIMEDOUBLE;
        config.ServerOn = doc[JSON_FLAG_SERVERSTATE] | DEFAULT_SERVERSTATE;
        config.SSID = doc[JSON_FLAG_SSID] | DEFAULT_SSID;
        config.PW = doc[JSON_FLAG_PASSWORD] | DEFAULT_PASSWORD;
        config.chipPage = doc[JSON_FLAG_CHIPPAGE] | DEFAULT_CHIPPAGE;
        config.key = doc[JSON_FLAG_KEY] | DEFAULT_KEY;

        _file.close();
}

void UserHandler::ResetInput()
{
        SetStBoth(false);
        SetStLeft(false);
        SetStRight(false);
}

void UserHandler::begin()
{
        SPI.begin();
        GetNFCReader().PCD_Init();

        SetRTCStatus(GetRTC().begin());

#ifdef SETRTC // Set RTC to compile TIME
        GetRTC().adjust(DateTime(__DATE__, __TIME__));
#endif

        _nfcReader.PCD_Init();
        SetSDStatus(SD.begin(GetChipSelectSD()));
        _nfcReader.PCD_Init();
        SetNFCStatus(GetNFCReader().PCD_PerformSelfTest());
        _nfcReader.PCD_Init();

        pinMode(taster_LINKS_pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(taster_LINKS_pin), ISR_Left, CHANGE);
        pinMode(taster_RECHTS_pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(taster_RECHTS_pin), ISR_Right, CHANGE);

        loadConfiguration();
}

bool UserHandler::AuthenticateUser(int localKey)
{
        return (config.key == localKey);
}

void UserHandler::ISR_Left()
{
        if (digitalRead(taster_LINKS_pin) && DebounceFinished(DEBOUNCE_KEYS_MS_MAX, DEBOUNCE_KEYS_MS_MIN) == RISING_EDGE_NOT_ALLOWED)
        {
                return;
        }
        else if (!digitalRead(taster_LINKS_pin) && DebounceFinished(DEBOUNCE_KEYS_MS_MAX, DEBOUNCE_KEYS_MS_MIN) == FALLING_EDGE_ALLOWED)
        {
                if (digitalRead(taster_RECHTS_pin))
                {
                        SetStBoth(true);
                }
                else
                {
                        SetStLeft(true);
                }
                StartKeyDebounce();
                return;
        }
        else
        {
                return;
        }
}

void UserHandler::ISR_Right()
{
        if (digitalRead(taster_RECHTS_pin) && DebounceFinished(DEBOUNCE_KEYS_MS_MAX, DEBOUNCE_KEYS_MS_MIN) == RISING_EDGE_NOT_ALLOWED)
        {
                return;
        }
        else if (!digitalRead(taster_RECHTS_pin) && DebounceFinished(DEBOUNCE_KEYS_MS_MAX, DEBOUNCE_KEYS_MS_MIN) == FALLING_EDGE_ALLOWED)
        {
                if (digitalRead(taster_LINKS_pin))
                {
                        SetStBoth(true);
                }
                else
                {
                        SetStRight(true);
                }
                StartKeyDebounce();
                return;
        }
        else
        {
                return;
        }
}

char UserHandler::ReadUserInput()
{

        if (digitalRead(taster_LINKS_pin))
        {
                if (digitalRead(taster_RECHTS_pin))
                {
                        return BOTH_KEY;
                }
                else
                {
                        return LEFT_KEY;
                }
        }
        else if (digitalRead(taster_RECHTS_pin))
        {
                if (digitalRead(taster_LINKS_pin))
                {
                        return BOTH_KEY;
                }
                else
                {
                        return RIGHT_KEY;
                }
        }
        else
        {
                return NONE_KEY;
        }
}

bool UserHandler::HasCardToRead()
{
        if (!GetNFCReader().PICC_IsNewCardPresent())
        {
                return false;
        }
        else
        {
                return true;
        }
}

String UserHandler::GetCardId()
{
        long _code = 0;

        if (GetNFCReader().PICC_ReadCardSerial())
        {
                for (byte i = 0; i < GetNFCReader().uid.size; i++)
                {
                        _code = ((_code + GetNFCReader().uid.uidByte[i]) * 10);
                }
        }

        return String(_code, DEC);
}

String UserHandler::GetTimeStamp()
{
        String _moment = "";
        GetRTC().begin();
        DateTime _timeStamp = GetRTC().now();
        GetNFCReader().PCD_Init(); // Shit :O

        _moment = String(_timeStamp.year(), DEC) + "-";
        if (_timeStamp.month() < 10)
                _moment += "0";
        _moment += String(_timeStamp.month(), DEC) + "-";
        if (_timeStamp.day() < 10)
                _moment += "0";
        _moment += String(_timeStamp.day(), DEC) + LOG_SERPERATOR;
        if (_timeStamp.hour() < 10)
                _moment += "0";
        _moment += String(_timeStamp.hour(), DEC) + ":";
        if (_timeStamp.minute() < 10)
                _moment += "0";
        _moment += String(_timeStamp.minute(), DEC) + "";

        return _moment;
}

void UserHandler::WriteToLog(String userID, String credit, bool isDouble)
{
        SD.begin(GetChipSelectSD());
        SetLogFile(SD.open(LOG_FILE, FILE_WRITE));
        if (GetLogFile())
        {
                String _logLine = "";
                if (isDouble)
                {
                        _logLine = GetTimeStamp() + LOG_SERPERATOR + userID + LOG_SERPERATOR + credit + LOG_SERPERATOR + DOUBLE;
                }
                else
                {
                        _logLine = GetTimeStamp() + LOG_SERPERATOR + userID + LOG_SERPERATOR + credit + LOG_SERPERATOR + SINGLE;
                }
                GetLogFile().println(_logLine);
        }

        GetLogFile().close();
}

int UserHandler::ReadCredit()
{
        byte _buffer[18];
        byte _byteCount;
        int _status = 0;

        _byteCount = sizeof(_buffer);

        _status = GetNFCReader().MIFARE_Read(config.chipPage, _buffer, &_byteCount);

        int _key_AND = (_buffer[0] & _buffer[1] & _buffer[2] & _buffer[3]);
        int _key_SUM = ((_buffer[0] + _buffer[1] + _buffer[2] + _buffer[3]) / 4);

        if ((_status == 0) && (_key_AND == _key_SUM))
        {
                return (_buffer[0] + _buffer[1] + _buffer[2] + _buffer[3]) / 4;
        }
        else
        {
                return -1;
        }
}

int UserHandler::WriteCredit(int newCredit, bool paymentType)
{

        byte _pwBufer[] = PW_BUFFER;
        byte _pACK[] = ACK_BUFFER;
        byte _writeBuffer[] = {newCredit, newCredit, newCredit, newCredit};
        int _stat = 0;

        ReadCredit();
        GetNFCReader().PCD_NTAG216_AUTH(&_pwBufer[0], _pACK);
        _stat = GetNFCReader().MIFARE_Ultralight_Write(config.chipPage, _writeBuffer, 4); // EE-5 KM CP = 0x04

        if (_stat != 0)
        {
                return -1;
        }

        ReadCredit();

        int _testCount = 0;

        while (!HasCardToRead())
        {
                _testCount++;
                if (_testCount > 10)
                        return -1;
        }

        SetUser(String(GetCardId().toInt()));
        WriteToLog(GetUser().c_str(), String(newCredit, DEC).c_str(), paymentType);
        return 0;
}

String UserHandler::getLastUser()
{
        if (GetUser() != "" && GetUser() != "0")
        {
                return GetUser();
        }
        else
        {
                return UNKNOWN_USER_STRING;
        }
}

String UserHandler::ID()
{
        long _code = 0;

        GetNFCReader().PICC_ReadCardSerial();
        for (byte i = 0; i < GetNFCReader().uid.size; i++)
        {
                _code = ((_code + GetNFCReader().uid.uidByte[i]) * 10);
        }
        return String(_code, DEC);
}

void UserHandler::newRead()
{
        while (!GetNFCReader().PICC_IsNewCardPresent())
        {
        };
}

bool UserHandler::saveConfiguration(int tiSingle, int tiDobule)
{

        if (tiSingle < TI_SINGLE_MIN || tiSingle > TI_SINGLE_MAX || tiSingle >= tiDobule || tiDobule < TI_DOUBLE_MIN || tiDobule > TI_DOUBLE_MAX)
        {
                return false;
        }

        SD.remove(CONFIG_FILE);

        File _file = SD.open(CONFIG_FILE, FILE_WRITE);
        if (!_file)
        {
                return false;
        }

        StaticJsonDocument<512> doc;

        doc[JSON_FLAG_SSID] = config.SSID;
        doc[JSON_FLAG_PASSWORD] = config.PW;
        doc[JSON_FLAG_CHIPPAGE] = config.chipPage;
        doc[JSON_FLAG_TIMESINGLE] = tiSingle;
        doc[JSON_FLAG_TIMEDOUBLE] = tiDobule;
        doc[JSON_FLAG_SERVERSTATE] = config.ServerOn;
        doc[JSON_FLAG_CHIPPAGE] = config.chipPage;
        doc[JSON_FLAG_KEY] = config.key;

        if (serializeJson(doc, _file) == 0)
        {
                Serial.println(F("Failed to write to file"));
        }

        _file.close();
        return true;
}
