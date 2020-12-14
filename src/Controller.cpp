#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include "Controller.h"
#include "defines.h"
#include <ArduinoJson.h>
#include <WiFi101.h>
#include <SPI.h>

Controller ::Controller(int chipSelect, int slaveSelect, int rstPin, int clk, int data) : _drawer(clk, data), _userHandler(chipSelect, slaveSelect, rstPin) {}

//////////////////  Getter and Setter for all variables  ////////////////////////////

void Controller::SetCurrentStatus(char stat) { _currentStatus = stat; }
char &Controller::GetCurrentStatus() { return _currentStatus; }

void Controller::SetCurrentKeyFlag(char key) { this->key = key; }
char &Controller::GetCurrentKeyFlag() { return this->key; }

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
Drawer &Controller::GetDrawer(){return this->_drawer;}

int &Controller::GetUserAsInt(){ return this->credit;}
void Controller::SetUserAsInt(int credit){this->credit = credit;}

int &Controller::GetCreditAsInt(){return this->user;}
void Controller::SetCreditAsInt(int user){this->user = user;}

/////////////////////////////////////////////////////////////////////////////////

void Controller::Begin()
{
        // Initialize all variables with default values
        Reset();

        // Setup RelayPin as OUTPUT, this pin controlles the basis of the transistor
        pinMode(RelayPin, OUTPUT);

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
}

char Controller::StateTransitions()
{
        SetOldKeyFlag(GetCurrentKeyFlag());
        SetCurrentKeyFlag(GetUserHandler().ReadUserInput());

        if (((GetCurrentKeyFlag() == LEFT_KEY) || (GetCurrentKeyFlag() == RIGHT_KEY)) && (GetCurrentKeyFlag() == GetOldKeyFlag()))
        {
                return GetCurrentStatus();
        }

        if (((GetCurrentStatus() == WaitForUser) || (GetCurrentStatus() == ShowCredit) || (GetCurrentStatus() == SceenSaferState)) && (GetCurrentKeyFlag() == LEFT_KEY))
        {
                if (GetCurrentStatus() == SceenSaferState)
                        SetStartTime(millis());
                return StateBegin(PayOne);
        }

        else if (((GetCurrentStatus() == Einfach) || (GetCurrentStatus() == Doppelt) || (GetCurrentStatus() == FinishState)) && ((GetCurrentKeyFlag() == LEFT_KEY) || (GetCurrentKeyFlag() == RIGHT_KEY)))
        {

                if (GetCurrentStatus() == Einfach)
                {
                        SetTimeRemaning(GetTimeSingle());
                }
                else if (GetCurrentStatus() == Doppelt)
                {
                        SetTimeRemaning(GetTimeDouble());
                }
                else if (GetCurrentStatus() == FinishState)
                {
                        SetTimePassed(GetTimeDelta() + GetTimePassed());
                }

                if (GetCurrentStatus() != FinishState)
                {
                        SetTimePassed(GetTimeDelta());
                }
                SetTimeStopBegin(millis());
                SetTimeInStop(0);
                return StateBegin(StopState);
        }

        else if ((GetCurrentStatus() == StopState) && (GetCurrentKeyFlag() == LEFT_KEY))
        {
                SetTimeInStop(0);
                SetTimeStopBegin(0);
                return StateBegin(WaitForUser);
        }
        else if ((GetCurrentStatus() == StopState) && GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(FinishState);
        }

        else if (GetCurrentStatus() == FreePullState)
        {
                if (GetCurrentKeyFlag() == LEFT_KEY) return StateBegin(Einfach);
                else if (GetCurrentKeyFlag() == RIGHT_KEY) return StateBegin(Doppelt);
                else return GetCurrentStatus();
        }

        else if (((GetCurrentStatus() == WaitForUser) || (GetCurrentStatus() == ShowCredit) || (GetCurrentStatus() == SceenSaferState)) && (GetCurrentKeyFlag() == BOTH_KEY))
        {
                if (GetCurrentStatus() == SceenSaferState)
                        SetStartTime(millis());
                return StateBegin(ShowLastUser);
        }

        else if ((GetCurrentStatus() == LstUserState) && ((GetCurrentKeyFlag() == LEFT_KEY) || (GetCurrentKeyFlag() == RIGHT_KEY)))
        {
                return StateBegin(HoldState_2);
        }

        else if ((GetCurrentStatus() == SplitPaymentQ) && (GetCurrentKeyFlag() == BOTH_KEY))
        {
                return StateBegin(HoldState_2);
        }

        else if (((GetCurrentStatus() == WaitForUser) || (GetCurrentStatus() == ShowCredit) || (GetCurrentStatus() == SceenSaferState)) && (GetCurrentKeyFlag() == RIGHT_KEY))
        {
                if (GetCurrentStatus() == SceenSaferState)
                        SetStartTime(millis());
                return StateBegin(HoldState);

        }

        else if ((GetCurrentStatus() == SplitPaymentQ) && (GetCurrentKeyFlag() == LEFT_KEY))
        {
                return StateBegin(PayTwo);
        }

        else if (GetCurrentStatus() == SplitPaymentQ && (GetCurrentKeyFlag() == RIGHT_KEY))
        {
                return StateBegin(PayTwo_1);
        }

        else if (((GetCurrentStatus() == WaitForUser) || (GetCurrentStatus() == SceenSaferState)) && GetUserHandler().HasCardToRead())
        {
                if (GetCurrentStatus() == SceenSaferState)
                        SetStartTime(millis());
                return StateBegin(ReadCreditUser);
        }

        else if (GetCurrentStatus() == ReadCreditUser)
        {
                return StateBegin(ShowCredit);
        }

        else if (GetCurrentStatus() == PayOne && GetUserHandler().HasCardToRead())
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
                                return StateBegin(Einfach);
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
                                        return StateBegin(Einfach);
                                }
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }

        else if (GetCurrentStatus() == PayTwo && GetUserHandler().HasCardToRead())
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
                                return StateBegin(Doppelt);
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
                                        return StateBegin(Doppelt);
                                }
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }

        else if (GetCurrentStatus() == PayTwo_1 && GetUserHandler().HasCardToRead())
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
                        while (GetCurrentUser() == ZERO_STRING || GetCurrentUser()  == "")
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
        else if (GetCurrentStatus() == PayTwo_2 && GetUserHandler().HasCardToRead())
        {
                String _currentUser = GetUserHandler().GetCardId();
                int _credit = GetUserHandler().ReadCredit();

                if (_credit >= PRICE_SINGE && GetCurrentUser() != _currentUser && _currentUser != ZERO_STRING)
                {
                        GetUserHandler().WriteCredit(_credit - PRICE_SINGE, false);
                        delay(DELAY_MILL_ON);
                        return StateBegin(Doppelt);
                }
                else
                {
                        return StateBegin(PayTwo_2);
                }
        }
        else if (GetCurrentStatus() == ReplayState && GetUserHandler().HasCardToRead())
        {
                String _currentUser = GetUserHandler().GetCardId();
                int _credit = GetUserHandler().ReadCredit();
                if (_credit == INVALD_CREDIT)
                {
                        return StateBegin(ReplayState);
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
                this->StateBegin(ReplayState);
                return true;
        }
        return false;
}

void Controller::UpDateTime()
{
        SetTimeDelta(millis() - GetStartTime());
}

void Controller::States(char Status)
{

        if (Status == Einfach)
        {
                MillOn();
                GetDrawer().DisplayProgress((GetTimeDelta()) / (GetTimeSingle() / 100));
                TimeOut(GetTimeSingle());
        }

        else if (Status == Doppelt)
        {
                MillOn();
                GetDrawer().DisplayProgress((GetTimeDelta()) / (GetTimeDouble() / 100));
                TimeOut(GetTimeDouble());
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

        else if (Status == PayOne)
        {
                GetDrawer().DrawPayOne();
                TimeOut(TIMEOUT_DEFAULT);
        }

        else if (Status == SceenSaferState)
        {
                GetDrawer().DrawScreenSafer(GetTimeDelta());
        }

        else if (Status == SplitPaymentQ)
        {
                GetDrawer().DrawSplitQ2();
                TimeOut(TIMEOUT_DEFAULT);
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
        else if (Status == HoldState)
        {
                if (GetCurrentKeyFlag() == NONE_KEY)
                {
                        StateBegin(SplitPaymentQ);
                }
        }
        else if (Status == HoldState_2)
        {
                if (GetCurrentKeyFlag() == NONE_KEY)
                {
                        StateBegin(WaitForUser);
                }
        }
        else if (Status == ReplayState)
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
                SetTimeInStop(millis()-GetTimeStopBegin());
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
        SetOldKeyFlag('n');
        SetCurrentKeyFlag('n');
}

char Controller::StateBegin(char state)
{
        SetCurrentStatus(state);
        SetStartTime(millis());
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
