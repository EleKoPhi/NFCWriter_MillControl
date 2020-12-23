#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include "Controller.h"
#include "defines.h"
#include <ArduinoJson.h>
#include <WiFi101.h>
#include <SPI.h>
#include <WDTZero.h>

Controller ::Controller(int chipSelect, int slaveSelect, int rstPin, int clk, int data) : _drawer(clk, data), _userHandler(chipSelect, slaveSelect, rstPin), _watchDog() {}

//////////////////  Getter and Setter for all variables  ////////////////////////////

void Controller::SetCurrentStatus(char stat) { _currentStatus = stat; }
char &Controller::GetCurrentStatus() { return _currentStatus; }

void Controller::SetCurrentKeyFlag(char key) { this->key = key; }
char &Controller::GetCurrentKeyFlag() { return this->key; }

void Controller::SetTempKeyFlag(char key) { this->temp_oldKey = key; }
char &Controller::GetTempKeyFlag() { return this->temp_oldKey; }

void Controller::SetOldKeyFlag(char key) { this->oldKey = key; }
char &Controller::GetOldKeyFlag() { return this->oldKey; }

void Controller::SetStartTime(unsigned long time) { this->_startTime = time; }
unsigned long &Controller::GetStartTime() { return this->_startTime; }

void Controller::SetTimeSingle(unsigned long time) { this->T_einfach = time; }
unsigned long &Controller::GetTimeSingle() { return this->T_einfach; }

void Controller::SetTimeDouble(unsigned long time) { this->T_doppelt = time; }
unsigned long &Controller::GetTimeDouble() { return this->T_doppelt; }

void Controller::SetTimeRemaning(unsigned long time) { this->T_rest = time; }
unsigned long &Controller::GetTimeRemaning() { return this->T_rest; }

void Controller::SetTimePassed(unsigned long time) { this->_passedtime = time; }
unsigned long &Controller::GetTimePassed() { return this->_passedtime; }

void Controller::SetTimeDelta(unsigned long time) { this->_deltaTime = time; }
unsigned long &Controller::GetTimeDelta() { return this->_deltaTime; }

void Controller::SetTimeStopBegin(unsigned long time) { this->_stopBegin = time; }
unsigned long &Controller::GetTimeStopBegin() { return this->_stopBegin; }

void Controller::SetTimeInStop(unsigned long time) { this->_timeInStopState = time; }
unsigned long &Controller::GetTimeInStop() { return this->_timeInStopState; }

void Controller::SetCurrentUser(String userID) { this->_currentUser = userID; }
String &Controller::GetCurrentUser() { return this->_currentUser; }

UserHandler &Controller::GetUserHandler() { return this->_userHandler; }
Drawer &Controller::GetDrawer() { return this->_drawer; }
WDTZero &Controller::GetWatchDog() { return this->_watchDog; }

int &Controller::GetUserAsInt() { return this->credit; }
void Controller::SetUserAsInt(int credit) { this->credit = credit; }

int &Controller::GetCreditAsInt() { return this->user; }
void Controller::SetCreditAsInt(int user) { this->user = user; }

int &Controller::GetActiveKeyElement() { return this->activeKeyElement; }
void Controller::SetActiveKeyElement(int element)
{
        if (element >= KEY_LENGTH_CONTROLLER)
        {
                element = 0;
        }
        this->activeKeyElement = element;
}

int &Controller::GetLocalKey() { return this->localKey; }
void Controller::SetLocalKey(int key) { this->localKey = key; }

unsigned long &Controller::GetTiRight() { return this->_ti_r_down; }
void Controller::SetTiRight(unsigned long ti) { this->_ti_r_down = ti; }

unsigned long &Controller::GetTiLeft() { return this->_ti_l_down; }
void Controller::SetTiLeft(unsigned long ti) { this->_ti_l_down = ti; }

unsigned long &Controller::GetTiBoth() { return this->_ti_both_down; }
void Controller::SetTiBoth(unsigned long ti) { this->_ti_both_down = ti; }

unsigned long &Controller::GetDeltaTiRight() { return this->_delta_ti_r_down; }
void Controller::SetDeltaTiRight(unsigned long ti) { this->_delta_ti_r_down = ti; }

unsigned long &Controller::GetDeltaTiLeft() { return this->_delta_ti_l_down; }
void Controller::SetDeltaTiLeft(unsigned long ti) { this->_delta_ti_l_down = ti; }

unsigned long &Controller::GetDeltaTiBoth() { return this->_delta_ti_both_down; }
void Controller::SetDeltaTiBoth(unsigned long ti) { this->_delta_ti_both_down = ti; }

/////////////////////////////////////////////////////////////////////////////////

void Controller::Begin()
{
        // Initialize all variables with default values
        Reset();

        // Setup RelayPin as OUTPUT, this pin controlles the basis of the transistor
        pinMode(RelayPin, OUTPUT);

        // Initialize time
        SetTiRight(NO_TIME);
        SetTiLeft(NO_TIME);

        // Make sure the mill if off
        MillOff();

        // Initialize the user handler for for NFC, SD and RTC functionallity
        GetUserHandler().begin();

        //Read time for singe and double from config file
        SetTimeSingle(GetUserHandler().config.single_time);
        SetTimeDouble(GetUserHandler().config.double_time);

        // If there is a problem with NFC, SD or RTC, display the error states
        GetDrawer().DrawErr(GetUserHandler().GetSDStatus(), GetUserHandler().GetNFCStatus(), GetUserHandler().GetRTCStatus());

        // Draw the startup animation
        GetDrawer().DrawMain();

        // Setup watchdog time to 2s. One cycle is around 300ms
        GetWatchDog().setup(WDT_HARDCYCLE2S);
}

char Controller::StateTransitions()
{
        SetOldKeyFlag(GetTempKeyFlag());
        SetTempKeyFlag(GetUserHandler().ReadUserInput());

        if (GetOldKeyFlag() == GetTempKeyFlag())
        {
                if (GetTempKeyFlag() == LEFT_KEY)
                {
                        if (GetDeltaTiLeft() != DEFAULT_INT_INI)
                        {
                                SetTiLeft(((millis() - GetDeltaTiLeft()) + GetTiLeft()));
                        }
                        else
                                SetTiLeft(DEFAULT_INT_INI);

                        SetDeltaTiLeft(millis());
                }
                else
                {
                        SetTiLeft(DEFAULT_INT_INI);
                        SetDeltaTiLeft(DEFAULT_INT_INI);
                }

                if (GetTempKeyFlag() == RIGHT_KEY)
                {
                        if (GetDeltaTiRight() != DEFAULT_INT_INI)
                        {
                                SetTiRight(((millis() - GetDeltaTiRight()) + GetTiRight()));
                        }
                        else
                                SetTiRight(DEFAULT_INT_INI);

                        SetDeltaTiRight(millis());
                }
                else
                {
                        SetTiRight(DEFAULT_INT_INI);
                        SetDeltaTiRight(DEFAULT_INT_INI);
                }

                if (GetTempKeyFlag() == BOTH_KEY)
                {
                        if (GetDeltaTiBoth() != DEFAULT_INT_INI)
                        {
                                SetTiBoth(((millis() - GetDeltaTiBoth()) + GetTiBoth()));
                        }
                        else
                                SetTiBoth(DEFAULT_INT_INI);

                        SetDeltaTiBoth(millis());
                }
                else
                {
                        SetTiBoth(DEFAULT_INT_INI);
                        SetDeltaTiBoth(DEFAULT_INT_INI);
                }
        }

        if ((GetOldKeyFlag() == LEFT_KEY) && (GetTempKeyFlag() == NONE_KEY))
        {
                SetCurrentKeyFlag(LEFT_KEY);
        }
        else if ((GetOldKeyFlag() == RIGHT_KEY) && (GetTempKeyFlag() == NONE_KEY))
        {
                SetCurrentKeyFlag(RIGHT_KEY);
        }
        else if ((GetOldKeyFlag() == BOTH_KEY) && (GetTempKeyFlag() == NONE_KEY))
        {
                SetCurrentKeyFlag(BOTH_KEY);
        }
        else if ((GetOldKeyFlag() == NONE_KEY) && (GetTempKeyFlag() == NONE_KEY))
        {
                SetCurrentKeyFlag(NONE_KEY);
        }

        if (GetCurrentStatus() == WaitForUser)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(PayOne);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(AskForSplitPayment);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(ShowLastUser);
                }
                if (GetUserHandler().HasCardToRead())
                {
                        return StateBegin(ReadCreditUser);
                }
                if (GetTiLeft() > TIME_FOR_PW_ACTIVATION)
                {
                        return StateBegin(EnterKey);
                }
        }

        else if (GetCurrentStatus() == EnterKey)
        {
                if (GetUserHandler().AuthenticateUser(GetLocalKey()))
                {
                        return StateBegin(SelectTiToAdapt);
                }
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetActiveKeyElement(GetActiveKeyElement() + 1);
                        return (EnterKey);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetLocalKey(GetLocalKey() ^= 1UL << GetActiveKeyElement());
                        return (EnterKey);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == SelectTiToAdapt)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(AdaptTiSingle);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(AdaptTiDouble);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == ShowCredit)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(PayOne);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(AskForSplitPayment);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(ShowLastUser);
                }
                if (GetUserHandler().HasCardToRead())
                {
                        return StateBegin(ReadCreditUser);
                }
        }

        else if (GetCurrentStatus() == SceenSaferState)
        {
                SetStartTime(millis());

                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(PayOne);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(AskForSplitPayment);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(ShowLastUser);
                }
                if (GetUserHandler().HasCardToRead())
                {
                        return StateBegin(ReadCreditUser);
                }
        }

        else if (GetCurrentStatus() == Single)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimeRemaning(GetTimeSingle());
                        SetTimePassed(GetTimeDelta());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }

                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetTimeRemaning(GetTimeSingle());
                        SetTimePassed(GetTimeDelta());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }
        }

        else if (GetCurrentStatus() == Double)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimeRemaning(GetTimeDouble());
                        SetTimePassed(GetTimeDelta());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }

                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetTimeRemaning(GetTimeDouble());
                        SetTimePassed(GetTimeDelta());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }
        }

        else if (GetCurrentStatus() == FinishState)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimePassed(GetTimeDelta() + GetTimePassed());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }

                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetTimePassed(GetTimeDelta() + GetTimePassed());
                        SetTimeStopBegin(millis());
                        SetTimeInStop(NO_TIME);
                        return StateBegin(StopState);
                }
        }

        else if ((GetCurrentStatus() == StopState))
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimeInStop(NO_TIME);
                        SetTimeStopBegin(NO_TIME);
                        return StateBegin(WaitForUser);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(FinishState);
                }
        }

        else if (GetCurrentStatus() == FreePullState)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(Single);
                }
                else if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(Double);
                }
                else
                {
                        return GetCurrentStatus();
                }
        }

        else if ((GetCurrentStatus() == AskForSplitPayment))
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        return StateBegin(PayTwo);
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        return StateBegin(PayTwo_1);
                }
                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == ReadCreditUser)
        {
                return StateBegin(ShowCredit);
        }

        else if (GetCurrentStatus() == AdaptTiSingle)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimeSingle(GetTimeSingle() - TIME_RESOLUTION);
                        return AdaptTiSingle;
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetTimeSingle(GetTimeSingle() + TIME_RESOLUTION);
                        return AdaptTiSingle;
                }

                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                        return StateBegin(WaitForUser);
                }
        }
        else if (GetCurrentStatus() == AdaptTiDouble)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY)
                {
                        SetTimeDouble(GetTimeDouble() - TIME_RESOLUTION);
                        return AdaptTiDouble;
                }
                if (GetCurrentKeyFlag() == RIGHT_KEY)
                {
                        SetTimeSingle(GetTimeDouble() + TIME_RESOLUTION);
                        return AdaptTiDouble;
                }

                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == PayOne)
        {
                if (GetUserHandler().HasCardToRead())
                {
                        int _credit = GetUserHandler().ReadCredit();
                        int _status = DEFAULT_INT_INI;
                        int _counter = DEFAULT_INT_INI;

                        if (_credit == INVALD_CREDIT)
                        {
                                return StateBegin(PayOne);
                        }

                        if (_credit >= PRICE_SINGE)
                        {
                                _status = GetUserHandler().WriteCredit(_credit - PRICE_SINGE, false);

                                if (_status == OK)
                                {
                                        delay(DELAY_MILL_ON);
                                        return StateBegin(Single);
                                }
                                else
                                {
                                        while ((_status != OK) && (_counter <= ERROR_RETRY_WRITING))
                                        {
                                                _status = GetUserHandler().WriteCredit(_credit - PRICE_SINGE, false);
                                                if (_status == OK)
                                                        break;
                                                _counter++;
                                        }

                                        if (_counter >= ERROR_RETRY_WRITING)
                                        {
                                                GetDrawer().Err();
                                                delay(DELAY_AFTER_ERROR);
                                                return StateBegin(WaitForUser);
                                        }
                                        else
                                        {
                                                delay(DELAY_MILL_ON);
                                                return StateBegin(Single);
                                        }
                                }
                        }
                        else
                        {
                                return StateBegin(LowCredit);
                        }
                }

                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == PayTwo)
        {
                if (GetUserHandler().HasCardToRead())
                {
                        int _credit = GetUserHandler().ReadCredit();
                        int _status = DEFAULT_INT_INI;
                        int _counter = DEFAULT_INT_INI;

                        if (_credit == INVALD_CREDIT)
                        {
                                return StateBegin(WaitForUser);
                        }

                        if (_credit >= PRICE_DOUBLE)
                        {
                                _status = GetUserHandler().WriteCredit(_credit - PRICE_DOUBLE, true);

                                if (_status == OK)
                                {
                                        delay(DELAY_MILL_ON);
                                        return StateBegin(Double);
                                }
                                else
                                {
                                        while ((_status != OK) && (_counter <= ERROR_RETRY_WRITING))
                                        {
                                                _status = GetUserHandler().WriteCredit(_credit - PRICE_DOUBLE, true);
                                                if (_status == OK)
                                                        break;
                                                _counter++;
                                        }

                                        if (_counter >= ERROR_RETRY_WRITING)
                                        {
                                                GetDrawer().Err();
                                                delay(DELAY_AFTER_ERROR);
                                                return StateBegin(WaitForUser);
                                        }
                                        else
                                        {
                                                delay(DELAY_MILL_ON);
                                                return StateBegin(Double);
                                        }
                                }
                        }
                        else
                        {
                                return StateBegin(LowCredit);
                        }
                }

                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }

        else if (GetCurrentStatus() == PayTwo_1)
        {
                if (GetUserHandler().HasCardToRead())
                {
                        int _credit = GetUserHandler().ReadCredit();

                        if (_credit == INVALD_CREDIT)
                        {
                                return StateBegin(PayTwo_1);
                        }

                        if (_credit >= PRICE_SINGE)
                        {
                                _userHandler.WriteCredit(_credit - PRICE_SINGE, false);
                                _userHandler.newRead();
                                while (GetCurrentUser() == ZERO_STRING || GetCurrentUser() == "")
                                {
                                        SetCurrentUser(GetUserHandler().GetCardId());
                                        GetUserHandler().ReadCredit();
                                }
                                return StateBegin(PayTwo_2);
                        }
                        else
                        {
                                return StateBegin(LowCredit);
                        }
                }

                if (GetCurrentKeyFlag() == BOTH_KEY)
                {
                        return StateBegin(WaitForUser);
                }
        }
        else if (GetCurrentStatus() == PayTwo_2 && GetUserHandler().HasCardToRead())
        {

                String _currentUser = GetUserHandler().GetCardId();
                int _credit = GetUserHandler().ReadCredit();

                if (_credit >= PRICE_SINGE && GetCurrentUser() != _currentUser && _currentUser != ZERO_STRING)
                {
                        GetUserHandler().WriteCredit(_credit - PRICE_SINGE, false);
                        delay(DELAY_MILL_ON);
                        return StateBegin(Double);
                }
                else
                {
                        return StateBegin(PayTwo_2);
                }
        }
        else if (GetCurrentStatus() == RepayState && GetUserHandler().HasCardToRead())
        {
                String _currentUser = GetUserHandler().GetCardId();
                int _credit = GetUserHandler().ReadCredit();
                if (_credit == INVALD_CREDIT)
                {
                        return StateBegin(RepayState);
                }
                else
                {
                        GetUserHandler().WriteCredit(_credit + PRICE_SINGE, false);
                        return StateBegin(DoneState);
                }
        }
        else if (GetCurrentStatus() == DoneState && !(GetUserHandler().HasCardToRead()))
        {
                if (!(GetUserHandler().HasCardToRead()))
                {
                        return StateBegin(WaitForUser);
                }
                else
                {
                        return StateBegin(DoneState);
                }
        }
        else
        {
                return GetCurrentStatus();
        }
}

bool Controller::TimeOut(unsigned long time)
{
        if (_deltaTime >= time)
        {
                this->Reset();
                return true;
        }
        return false;
}

bool Controller::TimeOutWithBackPay(unsigned long time)
{
        if (GetTimeDelta() >= time)
        {
                this->StateBegin(RepayState);
                return true;
        }
        return false;
}

void Controller::UpDateTime()
{
        GetWatchDog().clear();
        SetTimeDelta(millis() - GetStartTime());
}

void Controller::States(char Status)
{

        if (Status == Single)
        {
                MillOn();
                GetDrawer().DisplayProgress((GetTimeDelta()) / (GetTimeSingle() / 100));
                TimeOut(GetTimeSingle());
        }

        else if (Status == Double)
        {
                MillOn();
                GetDrawer().DisplayProgress((GetTimeDelta()) / (GetTimeDouble() / 100));
                TimeOut(GetTimeDouble());
        }

        else if (Status == EnterKey)
        {
                GetDrawer().DrawKeyInput(GetLocalKey(), GetActiveKeyElement());
                TimeOut(TIMEOUT_LONG);
        }

        else if (Status == WaitForUser)
        {
                GetDrawer().DrawWaitForUser();
                if (GetTimeDelta() > DELAY_ACTIVATION_SCREENSAFTER)
                {
                        StateBegin(SceenSaferState);
                }
        }

        else if (Status == FreePullState)
        {
                GetDrawer().DrawFreeState();
                TimeOut(TIMEOUT_LONG);
        }

        else if (Status == AdaptTiDouble)
        {
                GetDrawer().DrawTime(GetTimeDouble(), true);
                TimeOut(TIMEOUT_LONG);
        }

        else if (Status == AdaptTiSingle)
        {
                GetDrawer().DrawTime(GetTimeSingle(), false);
                TimeOut(TIMEOUT_LONG);
        }

        else if (Status == PayOne)
        {
                GetDrawer().DrawPayOne();
                TimeOut(TIMEOUT_DEFAULT);
        }

        else if (Status == SceenSaferState)
        {
                GetDrawer().DrawScreenSafer(GetTimeDelta());
        }

        else if (Status == AskForSplitPayment)
        {
                GetDrawer().DrawSplitQ2();
                TimeOut(TIMEOUT_DEFAULT);
        }

        else if (Status == SelectTiToAdapt)
        {
                GetDrawer().DrawTimeSelect();
                TimeOut(TIMEOUT_LONG);
        }

        else if (Status == PayTwo)
        {
                GetDrawer().DrawPay2();
                TimeOut(TIMEOUT_DEFAULT);
        }

        else if (Status == PayTwo_1)
        {
                GetDrawer().DrawPay2_1();
                TimeOut(TIMEOUT_DEFAULT);
        }

        else if (Status == PayTwo_2)
        {
                GetDrawer().DrawPay2_2();
                this->TimeOutWithBackPay(TIMEOUT_LONG);
        }

        else if (Status == LowCredit)
        {
                GetDrawer().DrawLowCredit();
                this->TimeOut(TIMEOUT_SHORT);
        }

        else if (Status == ReadCreditUser)
        {
                SetUserAsInt(GetUserHandler().GetCardId().toInt());
                SetCreditAsInt(GetUserHandler().ReadCredit());
        }

        else if (Status == ShowCredit)
        {
                if (GetUserAsInt() == 0)
                {
                        TimeOut(0);
                        return;
                }
                GetDrawer().DrawCredit(GetUserAsInt(), GetCreditAsInt());
                this->TimeOut(TIMEOUT_SHORT);
        }
        else if (Status == RepayState)
        {
                GetDrawer().DrawReplay(GetTimeDelta() / (TIMEOUT_REPAY / HUNDRED_PERCENT));
                TimeOut(TIMEOUT_REPAY);
        }
        else if (Status == DoneState)
        {
                GetDrawer().DrawDoneState();
                TimeOut(TIMEOUT_DEFAULT);
        }
        else if (Status == LstUserState)
        {
                GetDrawer().DrawLastUser(GetUserHandler().getLastUser());
                TimeOut(TIMEOUT_DEFAULT);
        }
        else if (Status == StopState)
        {
                MillOff();
                SetTimeInStop(millis() - GetTimeStopBegin());
                GetDrawer().DrawStopState();
                TimeOut(TIMEOUT_LONG);
        }
        else if (Status == FinishState)
        {
                int _progress = ((_passedtime + _deltaTime) / (this->T_rest / 100));
                MillOn();
                GetDrawer().DisplayProgress(_progress);
                TimeOut(GetTimeRemaning() - GetTimePassed());
        }
}

void Controller::Reset()
{
        MillOff();
        SetCurrentStatus(WaitForUser);
        SetTimeDelta(0);
        SetStartTime(millis());
        SetCurrentUser("");
        SetUserAsInt(0);
        SetCreditAsInt(0);
        SetCurrentKeyFlag(NONE_KEY);
        SetOldKeyFlag(NONE_KEY);
}

char Controller::StateBegin(char state)
{
        SetCurrentStatus(state);
        SetStartTime(millis());
        SetCurrentKeyFlag(NONE_KEY);
        SetOldKeyFlag(NONE_KEY);
        SetLocalKey(DEFAULT_INT_INI);
        SetActiveKeyElement(DEFAULT_INT_INI);
        UpDateTime();
        return state;
}

void Controller::MillOn()
{
        digitalWrite(RelayPin, HIGH);
}

void Controller::MillOff()
{
        digitalWrite(RelayPin, LOW);
}
