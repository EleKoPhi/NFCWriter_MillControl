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
char Controller::GetCurrentKeyFlag()
{
        if (GetUserHandler().GetStBoth())
        {
                return BOTH_KEY;
        }
        else if (GetUserHandler().GetStLeft())
        {
                return LEFT_KEY;
        }
        else if (GetUserHandler().GetStRigth())
        {
                return RIGHT_KEY;
        }
        else
        {
                return NONE_KEY;
        }
}

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

int &Controller::GetLocalKeyDisplayed() { return this->localKey_displayed; }
void Controller::SetLocalKeyDisplayed(int key) { this->localKey_displayed = key; }

int &Controller::GetActiveKeyElementDisplayed() { return this->activeKeyElement_displayed; }
void Controller::SetActiveKeyElementDispayed(int element) { this->activeKeyElement_displayed = element; }

unsigned long &Controller::GetTiRight() { return this->_ti_r_down; }
void Controller::SetTiRight(unsigned long ti) { this->_ti_r_down = ti; }

unsigned long &Controller::GetTiLeft() { return this->_ti_l_down; }
void Controller::SetTiLeft(unsigned long ti) { this->_ti_l_down = ti; }

unsigned long &Controller::GetTiBoth() { return this->_ti_both_down; }
void Controller::SetTiBoth(unsigned long ti) { this->_ti_both_down = ti; }

unsigned long &Controller::GetTimer100ms() { return this->timer100ms; }
void Controller::SetTimer100ms(unsigned long ti) { this->timer100ms = ti; }

unsigned long &Controller::GetTimer50ms() { return this->timer10ms; }
void Controller::SetTimer50ms(unsigned long ti) { this->timer10ms = ti; }

unsigned long &Controller::GetDeltaTiRight() { return this->_delta_ti_r_down; }
void Controller::SetDeltaTiRight(unsigned long ti) { this->_delta_ti_r_down = ti; }

unsigned long &Controller::GetDeltaTiLeft() { return this->_delta_ti_l_down; }
void Controller::SetDeltaTiLeft(unsigned long ti) { this->_delta_ti_l_down = ti; }

unsigned long &Controller::GetDeltaTiBoth() { return this->_delta_ti_both_down; }
void Controller::SetDeltaTiBoth(unsigned long ti) { this->_delta_ti_both_down = ti; }

bool &Controller::GetUpdateDisplay() { return this->updateDisplay; }
void Controller::SetUpdateDisplay(bool st) { this->updateDisplay = st; }

int &Controller::GetDisplayedProgress() { return this->progress; }
void Controller::SetDisplayedProgress(int prog) { this->progress = prog; }

int &Controller::GetProgress() { return this->progress_temp; }
void Controller::SetProgress(int prog) { this->progress_temp = prog; }

/////////////////////////////////////////////////////////////////////////////////

char Controller::tr_WaitForUser()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(PayOne);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(AskForSplitPayment);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(ShowLastUser);
        }
        else if (GetUserHandler().HasCardToRead())
        {
                return StateBegin(ReadCreditUser);
        }
        else if (GetTiLeft() > TIME_FOR_PW_ACTIVATION)
        {
                return StateBegin(EnterKey);
        }
        else if (GetTimeDelta() > DELAY_ACTIVATION_SCREENSAFTER)
        {
                return StateBegin(SceenSaferState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_EnterKey()
{
        if (GetUserHandler().AuthenticateUser(GetLocalKey()))
        {
                return StateBegin(SelectTiToAdapt);
        }
        else if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetActiveKeyElement(GetActiveKeyElement() + 1);
                GetUserHandler().ResetInput();
                return (EnterKey);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetLocalKey(GetLocalKey() ^= 1UL << GetActiveKeyElement());
                GetUserHandler().ResetInput();
                return (EnterKey);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_SelectToToAdapt()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(AdaptTiSingle);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(AdaptTiDouble);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_ShowCredit()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(PayOne);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(AskForSplitPayment);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(ShowLastUser);
        }
        else if (GetUserHandler().HasCardToRead())
        {
                return StateBegin(ReadCreditUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_Screensafer()
{
        SetStartTime(millis());

        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(PayOne);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(AskForSplitPayment);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(ShowLastUser);
        }
        else if (GetUserHandler().HasCardToRead())
        {
                return StateBegin(ReadCreditUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_Single()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimeRemaning(GetTimeSingle());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetTimeRemaning(GetTimeSingle());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_Dobule()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimeRemaning(GetTimeDouble());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetTimeRemaning(GetTimeDouble());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_FinishState()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimePassed(GetTimeDelta() + GetTimePassed());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetTimePassed(GetTimeDelta() + GetTimePassed());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_StopState()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimeInStop(NO_TIME);
                SetTimeStopBegin(NO_TIME);
                return StateBegin(WaitForUser);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                MillOn();
                return StateBegin(FinishState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_FreePullState()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                MillOn();
                return StateBegin(Single);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                MillOn();
                return StateBegin(Double);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_AskForSplitPayment()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(PayTwo);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(PayTwo_1);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_ReadCreditUser()
{
        return StateBegin(ShowCredit);
}
char Controller::tr_AdaptTiDouble()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimeDouble(GetTimeDouble() - TIME_RESOLUTION);
                SetUpdateDisplay(true);
                GetUserHandler().ResetInput();
                return AdaptTiDouble;
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetTimeDouble(GetTimeDouble() + TIME_RESOLUTION);
                SetUpdateDisplay(true);
                GetUserHandler().ResetInput();
                return AdaptTiDouble;
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_AdaptTiSingle()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                SetTimeSingle(GetTimeSingle() - TIME_RESOLUTION);
                SetUpdateDisplay(true);
                GetUserHandler().ResetInput();
                return AdaptTiSingle;
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetTimeSingle(GetTimeSingle() + TIME_RESOLUTION);
                SetUpdateDisplay(true);
                GetUserHandler().ResetInput();
                return AdaptTiSingle;
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_PayOne()
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
                                MillOn();
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
                                        MillOn();
                                        return StateBegin(Single);
                                }
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_PayTwo()
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
                                MillOn();
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
                                        MillOn();
                                        return StateBegin(Double);
                                }
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_PayTwo_1()
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
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_PayTwo_2()
{
        if (GetUserHandler().HasCardToRead())
        {

                String _currentUser = GetUserHandler().GetCardId();
                int _credit = GetUserHandler().ReadCredit();

                if (_credit >= PRICE_SINGE && GetCurrentUser() != _currentUser && _currentUser != ZERO_STRING)
                {
                        GetUserHandler().WriteCredit(_credit - PRICE_SINGE, false);
                        delay(DELAY_MILL_ON);
                        MillOn();
                        return StateBegin(Double);
                }
                else
                {
                        return StateBegin(PayTwo_2);
                }
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_RepayState()
{
        if (GetUserHandler().HasCardToRead())
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
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_DoneState()
{
        if (!(GetUserHandler().HasCardToRead()))
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

        // Draw initial screen
        SetUpdateDisplay(true);
}

void Controller::ProcessInput()
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
}

char Controller::StateTransitions()
{
        if ((millis() - GetTimer50ms()) > TASK_50MS)
        {
                SetTimer50ms(millis());

                switch (GetCurrentStatus())
                {
                case WaitForUser:
                        return (tr_WaitForUser());
                case EnterKey:
                        return (tr_EnterKey());
                case SelectTiToAdapt:
                        return (tr_SelectToToAdapt());
                case ShowCredit:
                        return (tr_ShowCredit());
                case SceenSaferState:
                        return (tr_Screensafer());
                case Single:
                        return (tr_Single());
                case Double:
                        return (tr_Dobule());
                case FinishState:
                        return (tr_FinishState());
                case StopState:
                        return (tr_StopState());
                case FreePullState:
                        return (tr_FreePullState());
                case AskForSplitPayment:
                        return (tr_AskForSplitPayment());
                case ReadCreditUser:
                        return (tr_ReadCreditUser());
                case AdaptTiSingle:
                        return (tr_AdaptTiSingle());
                case AdaptTiDouble:
                        return (tr_AdaptTiDouble());
                case PayOne:
                        return (tr_PayOne());
                case PayTwo:
                        return (tr_PayTwo());
                case PayTwo_1:
                        return (tr_PayTwo_1());
                case RepayState:
                        return (tr_RepayState());
                case DoneState:
                        return (tr_DoneState());
                default:
                        return (tr_WaitForUser());
                };
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
        if ((millis() - GetTimer100ms()) > TASK_100MS)
        {
                SetTimer100ms(millis());

                ProcessInput();

                if (Status == WaitForUser)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawWaitForUser();
                                SetUpdateDisplay(false);
                        }
                }

                else if (Status == Single)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DisplayProgress(GetProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DisplayProgress(GetDisplayedProgress());
                                SetDisplayedProgress(GetProgress());
                        }
                        else
                        {
                                SetProgress((GetTimeDelta()) / (GetTimeSingle() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeSingle());
                }

                else if (Status == Double)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DisplayProgress(GetProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DisplayProgress(GetDisplayedProgress());
                                SetDisplayedProgress(GetProgress());
                        }
                        else
                        {
                                SetProgress((GetTimeDelta()) / (GetTimeDouble() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeDouble());
                }

                else if (Status == EnterKey)
                {
                        if (((GetLocalKey() != GetLocalKeyDisplayed()) || (GetActiveKeyElement() != GetActiveKeyElementDisplayed())) || GetUpdateDisplay())
                        {
                                SetLocalKeyDisplayed(GetLocalKey());
                                SetActiveKeyElementDispayed(GetActiveKeyElement());
                                GetDrawer().DrawKeyInput(GetLocalKeyDisplayed(), GetActiveKeyElementDisplayed());
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (Status == FreePullState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawFreeState();
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (Status == AdaptTiDouble)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawTime(GetTimeDouble(), true);
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (Status == AdaptTiSingle)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawTime(GetTimeSingle(), false);
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (Status == PayOne)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPayOne();
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (Status == SceenSaferState)
                {
                        GetDrawer().DrawScreenSafer(GetTimeDelta());
                }

                else if (Status == AskForSplitPayment)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawSplitQ2();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (Status == SelectTiToAdapt)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawTimeSelect();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_LONG);
                }

                else if (Status == PayTwo)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPay2();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (Status == PayTwo_1)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPay2_1();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (Status == PayTwo_2)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPay2_2();
                                SetUpdateDisplay(false);
                        }
                        TimeOutWithBackPay(TIMEOUT_LONG);
                }

                else if (Status == LowCredit)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawLowCredit();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_SHORT);
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
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawCredit(GetUserAsInt(), GetCreditAsInt());
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_SHORT);
                }
                else if (Status == RepayState)
                {
                        GetDrawer().DrawReplay(GetTimeDelta() / (TIMEOUT_REPAY / HUNDRED_PERCENT));
                        TimeOut(TIMEOUT_REPAY);
                }
                else if (Status == DoneState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawDoneState();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }
                else if (Status == LstUserState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawLastUser(GetUserHandler().getLastUser());
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }
                else if (Status == StopState)
                {
                        MillOff();
                        SetTimeInStop(millis() - GetTimeStopBegin());
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawStopState();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_LONG);
                }
                else if (Status == FinishState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DisplayProgress(GetDisplayedProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DisplayProgress(GetDisplayedProgress());
                        }
                        else
                        {
                                SetProgress((GetTimePassed() + GetTimeDelta()) / (GetTimeRemaning() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeRemaning() - GetTimePassed());
                }
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
        SetUpdateDisplay(true);
}

char Controller::StateBegin(char state)
{
        GetUserHandler().ResetInput();
        SetCurrentStatus(state);
        SetStartTime(millis());
        SetCurrentKeyFlag(NONE_KEY);
        SetOldKeyFlag(NONE_KEY);
        SetLocalKey(DEFAULT_INT_INI);
        SetActiveKeyElement(DEFAULT_INT_INI);
        UpDateTime();
        SetUpdateDisplay(true);
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
