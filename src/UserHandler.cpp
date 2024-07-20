#include "Arduino.h"
#include "UserHandler.h"
#include "Controller_defines.h"
#include <SPI.h>
#include <MFRC522.h>
#include "UserHandler_defines.h"
#include <EEPROM.h>
#include <Preferences.h>

byte PSWBuff[] = PW_BUFFER;
byte pACK[] = ACK_BUFFER;

Preferences nvm_storage;

bool &UserHandler::GetNFCStatus() { return NfcStatus; }
void UserHandler::SetNFCStatus(bool Status) { NfcStatus = Status; }
String &UserHandler::GetUser() { return User; }
void UserHandler::SetUser(String user) { User = user; }
MFRC522 &UserHandler::GetNFCReader() { return _nfcReader; }
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
        SetNFCStatus(false);
        SetUser("");
}

bool UserHandler::loadConfiguration()
{

        nvm_storage.begin("mill_times", false);

        config.single_time = nvm_storage.getUInt("ti_single", DEFAULT_TIMESINGLE);
        config.double_time = nvm_storage.getUInt("ti_double", DEFAULT_TIMEDOUBLE);

        config.chipPage = DEFAULT_CHIPPAGE;
        config.key = DEFAULT_KEY;
        config.split = DEFAULT_Split;

        if (config.single_time == 0 || config.single_time == 0)
        {
                Serial.println("NVM Error");
                return false;
        }
        return true;
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
        _nfcReader.PCD_Init();
        _nfcReader.PCD_Init();
        SetNFCStatus(_nfcReader.PCD_PerformSelfTest());
        Serial.println(GetNFCStatus());
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

        return _moment;
}

void UserHandler::WriteToLog(String userID, String credit, bool isDouble)
{
}

int UserHandler::ReadCredit()
{
        byte _buffer[18];
        byte _byteCount;
        int _status = 0;

        _byteCount = sizeof(_buffer);

        _status = GetNFCReader().MIFARE_Read(0x06, _buffer, &_byteCount);

        Serial.println(_buffer[0]);
        Serial.println(_buffer[1]);
        Serial.println(_buffer[2]);
        Serial.println(_buffer[3]);

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
        _stat = GetNFCReader().MIFARE_Ultralight_Write(0x06, _writeBuffer, 4); // EE-5 KM CP = 0x04

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
                Serial.println("Invalid time setting!");
                return false;
        }

        nvm_storage.putUInt("ti_single", uint32_t(tiSingle));
        nvm_storage.putUInt("ti_double", uint32_t(tiDobule));
        nvm_storage.end();
        Serial.println("Saved mill time to NVM!");
        return true;
}
