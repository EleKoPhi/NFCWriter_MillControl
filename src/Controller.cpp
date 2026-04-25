#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include "Controller.h"
#include "Controller_defines.h"
#include <SPI.h>
#include "LogManager.h"
#include "MillWiFi.h"
#include <esp_attr.h>
#include <esp_system.h>
#include <Preferences.h>

namespace
{
        static const uint32_t PULL_RECOVERY_MAGIC = 0x504C4C31UL; // "PLL1"
        static const char *RESET_STATS_NS = "reset_stats";
        static const char *RESET_STATS_KEY_WITH_PULL = "with_pull";
        static const char *RESET_STATS_KEY_WITHOUT_PULL = "no_pull";
        static const int PAY_RESULT_LOW_CREDIT = -2;
        static const int PAY_RESULT_WRITE_FAILED = -3;
        static const int PAY_RESULT_CARD_MISMATCH = -4;

        struct PendingPullRtcData
        {
                uint32_t magic;
                uint32_t durationMs;
                uint32_t crc;
                uint8_t active;
                char state;
        };

        RTC_DATA_ATTR PendingPullRtcData g_pendingPullRtc = {0, 0, 0, 0, WaitForUser};

        static uint32_t pendingPullRtcCrc(const PendingPullRtcData &data)
        {
                return data.magic ^ data.durationMs ^ (uint32_t)data.active ^ (uint32_t)(uint8_t)data.state ^ 0xA5A55A5AUL;
        }

        static bool hasValidPendingPullSnapshot()
        {
                return g_pendingPullRtc.magic == PULL_RECOVERY_MAGIC
                        && g_pendingPullRtc.crc == pendingPullRtcCrc(g_pendingPullRtc)
                        && g_pendingPullRtc.active != 0
                        && g_pendingPullRtc.durationMs != 0
                        && (g_pendingPullRtc.state == Single || g_pendingPullRtc.state == Double);
        }

        static bool shouldCountResetReason(esp_reset_reason_t reason)
        {
                switch (reason)
                {
                case ESP_RST_UNKNOWN:
                case ESP_RST_POWERON:
                case ESP_RST_DEEPSLEEP:
                        return false;
                default:
                        return true;
                }
        }

        static void recordResetCounters()
        {
                esp_reset_reason_t reason = esp_reset_reason();
                if (!shouldCountResetReason(reason))
                        return;

                bool interruptedPull = hasValidPendingPullSnapshot();
                Preferences prefs;
                prefs.begin(RESET_STATS_NS, false);

                uint32_t withPull = prefs.getULong(RESET_STATS_KEY_WITH_PULL, 0);
                uint32_t withoutPull = prefs.getULong(RESET_STATS_KEY_WITHOUT_PULL, 0);

                if (interruptedPull)
                {
                        withPull++;
                        prefs.putULong(RESET_STATS_KEY_WITH_PULL, withPull);
                }
                else
                {
                        withoutPull++;
                        prefs.putULong(RESET_STATS_KEY_WITHOUT_PULL, withoutPull);
                }

                prefs.end();

                Serial.println(
                        "[ResetStats] reason=" + String((int)reason)
                        + " interrupted_pull=" + String(interruptedPull ? 1 : 0)
                        + " with_pull=" + String(withPull)
                        + " without_pull=" + String(withoutPull)
                );
        }
}

static void serviceNetworkDelay(unsigned long ms)
{
        unsigned long start = millis();
        while ((millis() - start) < ms)
        {
                MillWiFi::getInstance().handle();
                delay(1);
        }
}

Controller::Controller(int chipSelect, int slaveSelect, int rstPin, int clk, int data) : MillDrawer(clk, data), MillUserHandler(chipSelect, slaveSelect, rstPin) {}

//////////////////  Getter and Setter for all variables  ////////////////////////////

void Controller::SetCurrentStatus(char stat) { currentStatus = stat; }
char &Controller::GetCurrentStatus() { return currentStatus; }

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

void Controller::SetTempKeyFlag(char key) { this->tempOldKey = key; }
char &Controller::GetTempKeyFlag() { return this->tempOldKey; }

void Controller::SetOldKeyFlag(char key) { this->oldKey = key; }
char &Controller::GetOldKeyFlag() { return this->oldKey; }

void Controller::SetStartTime(unsigned long time) { this->tiStart = time; }
unsigned long &Controller::GetStartTime() { return this->tiStart; }

void Controller::SetTimeSingle(unsigned long time) { this->tiSingle = time; }
unsigned long &Controller::GetTimeSingle() { return this->tiSingle; }

void Controller::SetTimeDouble(unsigned long time) { this->tiDouble = time; }
unsigned long &Controller::GetTimeDouble() { return this->tiDouble; }

void Controller::SetTimeRemaning(unsigned long time) { this->tiRemaining = time; }
unsigned long &Controller::GetTimeRemaning() { return this->tiRemaining; }

void Controller::SetTimePassed(unsigned long time) { this->tiPassed = time; }
unsigned long &Controller::GetTimePassed() { return this->tiPassed; }

void Controller::SetTimeDelta(unsigned long time) { this->tiDelat = time; }
unsigned long &Controller::GetTimeDelta() { return this->tiDelat; }

void Controller::SetTimeStopBegin(unsigned long time) { this->tiStopBegin = time; }
unsigned long &Controller::GetTimeStopBegin() { return this->tiStopBegin; }

void Controller::SetTimeInStop(unsigned long time) { this->tiInStopState = time; }
unsigned long &Controller::GetTimeInStop() { return this->tiInStopState; }

void Controller::SetCurrentUser(String userID) { this->currentUser = userID; }
String &Controller::GetCurrentUser() { return this->currentUser; }

UserHandler &Controller::GetUserHandler() { return this->MillUserHandler; }
Drawer &Controller::GetDrawer() { return this->MillDrawer; }

int &Controller::GetUserAsInt() { return this->activeCredit; }
void Controller::SetUserAsInt(int credit) { this->activeCredit = credit; }

int &Controller::GetCreditAsInt() { return this->activeUser; }
void Controller::SetCreditAsInt(int user) { this->activeUser = user; }

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

int &Controller::GetLocalKeyDisplayed() { return this->localKeyDisplayed; }
void Controller::SetLocalKeyDisplayed(int key) { this->localKeyDisplayed = key; }

int &Controller::GetActiveKeyElementDisplayed() { return this->activeKeyElementDisplayed; }
void Controller::SetActiveKeyElementDispayed(int element) { this->activeKeyElementDisplayed = element; }

unsigned long &Controller::GetTiRight() { return this->tiRightDown; }
void Controller::SetTiRight(unsigned long ti) { this->tiRightDown = ti; }

unsigned long &Controller::GetTiLeft() { return this->tiLeftDown; }
void Controller::SetTiLeft(unsigned long ti) { this->tiLeftDown = ti; }

unsigned long &Controller::GetTiBoth() { return this->tiBothDown; }
void Controller::SetTiBoth(unsigned long ti) { this->tiBothDown = ti; }

unsigned long &Controller::GetTimer100ms() { return this->tiTimer100ms; }
void Controller::SetTimer100ms(unsigned long ti) { this->tiTimer100ms = ti; }

unsigned long &Controller::GetTimer50ms() { return this->tiTimer50ms; }
void Controller::SetTimer50ms(unsigned long ti) { this->tiTimer50ms = ti; }

unsigned long &Controller::GetDeltaTiRight() { return this->tiDeltaRightDown; }
void Controller::SetDeltaTiRight(unsigned long ti) { this->tiDeltaRightDown = ti; }

unsigned long &Controller::GetDeltaTiLeft() { return this->tiDeltaLeftDown; }
void Controller::SetDeltaTiLeft(unsigned long ti) { this->tiDeltaLeftDown = ti; }

unsigned long &Controller::GetDeltaTiBoth() { return this->tiDeltaBothDown; }
void Controller::SetDeltaTiBoth(unsigned long ti) { this->tiDeltaBothDown = ti; }

bool &Controller::GetUpdateDisplay() { return this->updateDisplay; }
void Controller::SetUpdateDisplay(bool st) { this->updateDisplay = st; }

int &Controller::GetDisplayedProgress() { return this->activeProgress; }
void Controller::SetDisplayedProgress(int prog) { this->activeProgress = prog; }

int &Controller::GetProgress() { return this->tempProgress; }
void Controller::SetProgress(int prog) { this->tempProgress = prog; }

/////////////////////////////////////////////////////////////////////////////////

bool Controller::IsSelectionReleased(char keyCode, unsigned long minHoldMs, unsigned long maxHoldMs)
{
        char currentInput = GetUserHandler().ReadUserInput();
        unsigned long heldMs = 0;

        if (keyCode == LEFT_KEY)
        {
                heldMs = GetTiLeft();
        }
        else if (keyCode == RIGHT_KEY)
        {
                heldMs = GetTiRight();
        }
        else
        {
                return false;
        }

        if (GetCurrentKeyFlag() != keyCode || currentInput == keyCode || heldMs < minHoldMs)
        {
                return false;
        }
        if (maxHoldMs > 0 && heldMs >= maxHoldMs)
        {
                return false;
        }
        return true;
}

bool Controller::IsSelectionPressed(char keyCode, unsigned long minHoldMs)
{
        char currentInput = GetUserHandler().ReadUserInput();
        if (GetCurrentKeyFlag() != keyCode || currentInput != keyCode)
        {
                return false;
        }

        if (keyCode == LEFT_KEY)
        {
                return GetTiLeft() >= minHoldMs;
        }
        if (keyCode == RIGHT_KEY)
        {
                return GetTiRight() >= minHoldMs;
        }
        return false;
}

bool Controller::WriteCreditWithRetry(int targetCredit, bool paymentType)
{
        int status = FAILED;
        for (int attempt = 0; attempt <= ERROR_RETRY_WRITING; attempt++)
        {
                status = GetUserHandler().WriteCredit(targetCredit, paymentType);
                if (status == _Ok)
                {
                        return true;
                }
                MillWiFi::getInstance().handle();
                delay(1);
        }
        return false;
}

int Controller::DebitPresentedCard(int price, bool paymentType, int &creditAfter)
{
        int credit = GetUserHandler().ReadCredit();
        creditAfter = INVALD_CREDIT;

        if (credit == INVALD_CREDIT)
        {
                return INVALD_CREDIT;
        }

        if (credit < price)
        {
                return PAY_RESULT_LOW_CREDIT;
        }

        creditAfter = credit - price;
        if (!WriteCreditWithRetry(creditAfter, paymentType))
        {
                return PAY_RESULT_WRITE_FAILED;
        }

        GetUserHandler().WriteToLog(-price, creditAfter);
        return _Ok;
}

int Controller::RefundPresentedCard(String expectedUser, int refundCredits, int &creditAfter)
{
        String currentUser = GetUserHandler().GetCardId();
        currentUser.trim();
        expectedUser.trim();
        creditAfter = INVALD_CREDIT;

        if (expectedUser == ZERO_STRING || expectedUser == "" || currentUser != expectedUser)
        {
                return PAY_RESULT_CARD_MISMATCH;
        }

        int credit = GetUserHandler().ReadCredit();
        if (credit == INVALD_CREDIT || credit + refundCredits > 255)
        {
                return INVALD_CREDIT;
        }

        creditAfter = credit + refundCredits;
        if (!WriteCreditWithRetry(creditAfter, false))
        {
                return PAY_RESULT_WRITE_FAILED;
        }

        GetUserHandler().WriteToLog(+refundCredits, creditAfter);
        return _Ok;
}

char Controller::tr_WaitForUser()
{
        bool leftSelectionReleased = IsSelectionReleased(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS, TIME_FOR_PW_ACTIVATION);
        bool rightSelectionReleased = IsSelectionReleased(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool leftSelectionPressed = IsSelectionPressed(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionPressed = IsSelectionPressed(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);

        if (GetTiLeft() > TIME_FOR_PW_ACTIVATION)
        {
                MillWiFi::getInstance().activateFor(WIFI_AP_ACTIVE_MS);
                return StateBegin(EnterKey);
        }
        else if (leftSelectionReleased || leftSelectionPressed)
        {
                return StateBegin(PayOne);
        }
        else if (rightSelectionReleased || rightSelectionPressed)
        {
#ifdef SPLIT_ENABLED
                return StateBegin(AskForSplitPayment);
#else
                return StateBegin(PayTwo);
#endif
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(ShowLastUser);
        }
        else if (GetUserHandler().HasCardToRead())
        {
                return StateBegin(ReadCreditUser);
        }
        else if (GetTimeDelta() > DELAY_ACTIVATION_SCREENSAFTER)
        {
                return StateBegin(SceenSaferState);
        }
        else if (GetUserHandler().GetConfigStatus() == false)
        {
                return StateBegin(NVMError);
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
                SetUpdateDisplay(true);
                return (EnterKey);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                SetLocalKey(GetLocalKey() ^= 1UL << GetActiveKeyElement());
                GetUserHandler().ResetInput();
                SetUpdateDisplay(true);
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
        bool leftSelectionReleased = IsSelectionReleased(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionReleased = IsSelectionReleased(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool leftSelectionPressed = IsSelectionPressed(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionPressed = IsSelectionPressed(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);

        if (leftSelectionReleased || leftSelectionPressed)
        {
                return StateBegin(PayOne);
        }
        else if (rightSelectionReleased || rightSelectionPressed)
        {
#ifdef SPLIT_ENABLED
                return StateBegin(AskForSplitPayment);
#else
                return StateBegin(PayTwo);
#endif
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
char Controller::tr_ShowLastUser()
{
        bool leftSelectionReleased = IsSelectionReleased(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionReleased = IsSelectionReleased(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool leftSelectionPressed = IsSelectionPressed(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionPressed = IsSelectionPressed(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);

        if (leftSelectionReleased || leftSelectionPressed)
        {
                return StateBegin(PayOne);
        }
        else if (rightSelectionReleased || rightSelectionPressed)
        {
#ifdef SPLIT_ENABLED
                return StateBegin(AskForSplitPayment);
#else
                return StateBegin(PayTwo);
#endif
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
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

char Controller::tr_NVMError()
{
        if (GetCurrentKeyFlag() == LEFT_KEY)
        {
                return StateBegin(AdaptTiSingle);
        }
        else if (GetCurrentKeyFlag() == RIGHT_KEY)
        {
                return StateBegin(AdaptTiSingle);
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

char Controller::tr_Screensafer()
{
        bool leftSelectionReleased = IsSelectionReleased(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionReleased = IsSelectionReleased(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool leftSelectionPressed = IsSelectionPressed(LEFT_KEY, PAYMENT_SELECTION_HOLD_MS);
        bool rightSelectionPressed = IsSelectionPressed(RIGHT_KEY, PAYMENT_SELECTION_HOLD_MS);

        if (leftSelectionReleased || leftSelectionPressed)
        {
                return StateBegin(PayOne);
        }
        else if (rightSelectionReleased || rightSelectionPressed)
        {
#ifdef SPLIT_ENABLED
                return StateBegin(AskForSplitPayment);
#else
                return StateBegin(PayTwo);
#endif
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
        bool stopLeftRequested = GetTiLeft() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == LEFT_KEY;
        bool stopRightRequested = GetTiRight() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == RIGHT_KEY;

        if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopLeftRequested)
        {
                SetTimeRemaning(GetTimeSingle());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopRightRequested)
        {
                SetTimeRemaning(GetTimeSingle());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_Dobule()
{
        bool stopLeftRequested = GetTiLeft() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == LEFT_KEY;
        bool stopRightRequested = GetTiRight() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == RIGHT_KEY;

        if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopLeftRequested)
        {
                SetTimeRemaning(GetTimeDouble());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopRightRequested)
        {
                SetTimeRemaning(GetTimeDouble());
                SetTimePassed(GetTimeDelta());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_FinishState()
{
        bool stopLeftRequested = GetTiLeft() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == LEFT_KEY;
        bool stopRightRequested = GetTiRight() >= STOP_MENU_HOLD_MS && GetUserHandler().ReadUserInput() == RIGHT_KEY;

        if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopLeftRequested)
        {
                SetTimePassed(GetTimeDelta() + GetTimePassed());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else if (GetTimeDelta() > STOP_INPUT_GUARD_MS && stopRightRequested)
        {
                SetTimePassed(GetTimeDelta() + GetTimePassed());
                SetTimeStopBegin(millis());
                SetTimeInStop(NO_TIME);
                MillOff();
                return StateBegin(StopState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
char Controller::tr_StopState()
{
        char actionKey = GetCurrentKeyFlag();
        if (actionKey == NONE_KEY)
        {
                actionKey = GetUserHandler().ReadUserInput();
        }

        if (actionKey == LEFT_KEY)
        {
                SetTimeInStop(NO_TIME);
                SetTimeStopBegin(NO_TIME);
                ClearPendingPull();
                return StateBegin(WaitForUser);
        }
        else if (actionKey == RIGHT_KEY)
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
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else
        {
                return GetCurrentStatus();
        }
}
#ifdef SPLIT_ENABLED
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
#endif  // SPLIT_ENABLED
char Controller::tr_ReadCreditUser()
{
        //SetUserAsInt(GetUserHandler().GetCardId().toInt());
        //SetCreditAsInt(GetUserHandler().ReadCredit());
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
        else if (GetCurrentKeyFlag() == BOTH_KEY && GetUserHandler().GetConfigStatus() == false)
        {
                GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                GetUserHandler().SetConfigStatus(true);
                return StateBegin(WaitForUser);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY && GetUserHandler().GetConfigStatus() == true)
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
        else if (GetCurrentKeyFlag() == BOTH_KEY && GetUserHandler().GetConfigStatus() == false)
        {
                // GetUserHandler().saveConfiguration(GetTimeSingle(), GetTimeDouble());
                return StateBegin(AdaptTiDouble);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY && GetUserHandler().GetConfigStatus() == true)
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
                int creditAfter = INVALD_CREDIT;
                int status = DebitPresentedCard(PRICE_SINGE, false, creditAfter);

                if (status == INVALD_CREDIT)
                {
                        return StateBegin(PayOne);
                }

                if (status == _Ok)
                {
                        SavePendingPull(Single, GetTimeSingle());
                        serviceNetworkDelay(DELAY_MILL_ON);
                        MillOn();
                        return StateBegin(Single);
                }

                if (status == PAY_RESULT_LOW_CREDIT)
                {
                        return StateBegin(LowCredit);
                }

                GetDrawer().DrawSystemError();
                serviceNetworkDelay(DELAY_AFTER_ERROR);
                ClearPendingPull();
                return StateBegin(WaitForUser);
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(WaitForUser);
        }
        else if (GetTiLeft() > TIME_FOR_PW_ACTIVATION && GetUserHandler().ReadUserInput() == LEFT_KEY)
        {
                MillWiFi::getInstance().activateFor(WIFI_AP_ACTIVE_MS);
                return StateBegin(EnterKey);
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
                int creditAfter = INVALD_CREDIT;
                int status = DebitPresentedCard(PRICE_DOUBLE, true, creditAfter);

                if (status == INVALD_CREDIT)
                {
                        return StateBegin(WaitForUser);
                }

                if (status == _Ok)
                {
                        SavePendingPull(Double, GetTimeDouble());
                        serviceNetworkDelay(DELAY_MILL_ON);
                        MillOn();
                        return StateBegin(Double);
                }

                if (status == PAY_RESULT_LOW_CREDIT)
                {
                        return StateBegin(LowCredit);
                }

                GetDrawer().DrawSystemError();
                serviceNetworkDelay(DELAY_AFTER_ERROR);
                ClearPendingPull();
                return StateBegin(WaitForUser);
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
#ifdef SPLIT_ENABLED
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
                        String firstUser = GetUserHandler().GetCardId();
                        int creditAfter = INVALD_CREDIT;
                        int status = DebitPresentedCard(PRICE_SINGE, false, creditAfter);
                        if (status != _Ok)
                        {
                                if (status == PAY_RESULT_LOW_CREDIT)
                                {
                                        return StateBegin(LowCredit);
                                }
                                GetDrawer().DrawSystemError();
                                serviceNetworkDelay(DELAY_AFTER_ERROR);
                                return StateBegin(WaitForUser);
                        }
                        SetCurrentUser(firstUser);
                        if (!MillUserHandler.newRead(15000UL))
                                return StateBegin(RepayState);
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
                int creditAfter = INVALD_CREDIT;

                if ((GetCurrentUser() != _currentUser) && (_currentUser != ZERO_STRING))
                {
                        int status = DebitPresentedCard(PRICE_SINGE, false, creditAfter);
                        if (status == _Ok)
                        {
                                serviceNetworkDelay(DELAY_MILL_ON);
                                MillOn();
                                return StateBegin(Double);
                        }
                        if (status == PAY_RESULT_LOW_CREDIT)
                        {
                                return StateBegin(LowCredit);
                        }
                        GetDrawer().DrawSystemError();
                        serviceNetworkDelay(DELAY_AFTER_ERROR);
                        return StateBegin(RepayState);
                }
                else
                {
                        return StateBegin(PayTwo_2);
                }
        }
        else if (GetCurrentKeyFlag() == BOTH_KEY)
        {
                return StateBegin(RepayState);
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
                int creditAfter = INVALD_CREDIT;
                int status = RefundPresentedCard(GetCurrentUser(), PRICE_SINGE, creditAfter);
                if (status == INVALD_CREDIT)
                {
                        return StateBegin(RepayState);
                }
                else if (status == _Ok)
                {
                        SetCurrentUser("");
                        return StateBegin(DoneState);
                }
                return StateBegin(RepayState);
        }
        else
        {
                return GetCurrentStatus();
        }
}
#endif  // SPLIT_ENABLED
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
        Serial.println("[Boot] reset reason=" + String((int)esp_reset_reason()));
        recordResetCounters();

        GetDrawer().Begin();

        // Initialize all variables with default values
        Reset();

        // Setup RelayPin as OUTPUT, this pin controlles the basis of the transistor
        pinMode(RelayPin, OUTPUT);

        // Initialize time
        SetTiRight(NO_TIME);
        SetTiLeft(NO_TIME);

        // Make sure the mill if off
        MillOff();

        // Initialize the user handler for for NFC functionallity
        GetUserHandler().begin();

        // Read time for singe and double from config file
        SetTimeSingle(GetUserHandler().config.single_time);
        SetTimeDouble(GetUserHandler().config.double_time);

        // If there is a problem with NFC, display the error states
        GetDrawer().DrawSystemStatus(GetUserHandler().GetNFCStatus());

        // Draw the startup animation
        GetDrawer().DrawStartUpAnimation();

        // Setup watchdog time to 2s. One cycle is around 30ms
        // GetWatchDog().setup(WDT_HARDCYCLE2S);

        if (!RestorePendingPull())
        {
                // Draw initial screen
                SetUpdateDisplay(true);
        }
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
                ProcessInput();

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
#ifdef SPLIT_ENABLED
                case AskForSplitPayment:
                        return (tr_AskForSplitPayment());
#endif
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
#ifdef SPLIT_ENABLED
                case PayTwo_1:
                        return (tr_PayTwo_1());
                case PayTwo_2:
                        return (tr_PayTwo_2());
                case RepayState:
                        return (tr_RepayState());
#endif
                case DoneState:
                        return (tr_DoneState());
                case ShowLastUser:
                        return (tr_ShowLastUser());
                case NVMError:
                        return (tr_NVMError());
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
        if (tiDelat >= time)
        {
                ClearPendingPull();
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
        SetTimeDelta(millis() - GetStartTime());
}

void Controller::States(char state)
{

        if ((millis() - GetTimer100ms()) > TASK_100MS)
        {
                SetTimer100ms(millis());

                if (state == WaitForUser)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawWaitForUser();
                                SetUpdateDisplay(false);
                        }
                }

                else if (state == Single)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawProgress(GetProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DrawProgress(GetDisplayedProgress());
                                SetDisplayedProgress(GetProgress());
                        }
                        else
                        {
                                SetProgress((GetTimeDelta()) / (GetTimeSingle() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeSingle());
                }

                else if (state == Double)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawProgress(GetProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DrawProgress(GetDisplayedProgress());
                                SetDisplayedProgress(GetProgress());
                        }
                        else
                        {
                                SetProgress((GetTimeDelta()) / (GetTimeDouble() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeDouble());
                }

                else if (state == FinishState)
                {
                        if (GetUpdateDisplay())
                        {
                                SetProgress((GetTimePassed() + GetTimeDelta()) / (GetTimeRemaning() / HUNDRED_PERCENT));
                                SetDisplayedProgress(GetProgress());
                                GetDrawer().DrawProgress(GetProgress());
                                SetUpdateDisplay(false);
                        }
                        else if (GetProgress() >= GetDisplayedProgress() + PROGRESS_RESOLUTION)
                        {
                                GetDrawer().DrawProgress(GetDisplayedProgress());
                                SetDisplayedProgress(GetProgress());
                        }
                        else
                        {
                                SetProgress((GetTimePassed() + GetTimeDelta()) / (GetTimeRemaning() / HUNDRED_PERCENT));
                        }

                        TimeOut(GetTimeRemaning() - GetTimePassed());
                }

                else if (state == EnterKey)
                {
                        if (GetUpdateDisplay())
                        {
                                SetLocalKeyDisplayed(GetLocalKey());
                                SetActiveKeyElementDispayed(GetActiveKeyElement());
                                GetDrawer().DrawKeyInput(GetLocalKeyDisplayed(), GetActiveKeyElementDisplayed());
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (state == FreePullState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawFreeState();
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (state == AdaptTiDouble)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawTime(GetTimeDouble(), true);
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (state == AdaptTiSingle)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawTime(GetTimeSingle(), false);
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_LONG);
                }

                else if (state == PayOne)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPayOne();
                                SetUpdateDisplay(false);
                        }

                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (state == SceenSaferState)
                {
                        GetDrawer().DrawScreenSafer(millis());
                }

#ifdef SPLIT_ENABLED
                else if (state == AskForSplitPayment)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawSplitQuestion();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }
#endif

                else if (state == SelectTiToAdapt)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawSelectTime();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_LONG);
                }

                else if (state == PayTwo)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPayTwo();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }

#ifdef SPLIT_ENABLED
                else if (state == PayTwo_1)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPayTwo_First();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }

                else if (state == PayTwo_2)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawPayTwo_Second();
                                SetUpdateDisplay(false);
                        }
                        TimeOutWithBackPay(TIMEOUT_LONG);
                }
#endif

                else if (state == LowCredit)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawLowCredit();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_SHORT);
                }

                else if (state == ReadCreditUser)
                {
                        // Credit and ID are captured in tr_ReadCreditUser to avoid timing races
                        // between 50ms transition ticks and 100ms display ticks.
                        SetUserAsInt(GetUserHandler().GetCardId().toInt());
                        SetCreditAsInt(GetUserHandler().ReadCredit());
                }

                else if (state == ShowCredit)
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
#ifdef SPLIT_ENABLED
                else if (state == RepayState)
                {
                        GetDrawer().DrawReplay(GetTimeDelta() / (TIMEOUT_REPAY / HUNDRED_PERCENT));
                        TimeOut(TIMEOUT_REPAY);
                }
#endif
                else if (state == DoneState)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawDoneState();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }
                else if (state == ShowLastUser)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawLastUser(GetUserHandler().getLastUser());
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_DEFAULT);
                }
                else if (state == StopState)
                {
                        SetTimeInStop(millis() - GetTimeStopBegin());
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawStopState();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_LONG);
                }
                else if (state == NVMError)
                {
                        if (GetUpdateDisplay())
                        {
                                GetDrawer().DrawNvmErrorState();
                                SetUpdateDisplay(false);
                        }
                }
                else
                {
                        if (GetUpdateDisplay())
                        {
                                Serial.println(int(GetCurrentStatus()));
                                GetDrawer().DrawSystemError();
                                SetUpdateDisplay(false);
                        }
                        TimeOut(TIMEOUT_SHORT);
                }
        }
}

void Controller::Reset()
{
        pendingPullActive = false;
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
        SetDisplayedProgress(DEFAULT_INT_INI);
        SetProgress(DEFAULT_INT_INI);
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

void Controller::SavePendingPull(char state, unsigned long durationMs)
{
        g_pendingPullRtc.magic = PULL_RECOVERY_MAGIC;
        g_pendingPullRtc.durationMs = durationMs;
        g_pendingPullRtc.active = 1;
        g_pendingPullRtc.state = state;
        g_pendingPullRtc.crc = pendingPullRtcCrc(g_pendingPullRtc);
        pendingPullActive = true;
}

void Controller::ClearPendingPull()
{
        if (!pendingPullActive && g_pendingPullRtc.active == 0)
                return;

        g_pendingPullRtc.magic = 0;
        g_pendingPullRtc.durationMs = 0;
        g_pendingPullRtc.active = 0;
        g_pendingPullRtc.state = WaitForUser;
        g_pendingPullRtc.crc = pendingPullRtcCrc(g_pendingPullRtc);
        pendingPullActive = false;
}

bool Controller::IsRecoverableResetReason(esp_reset_reason_t reason) const
{
        switch (reason)
        {
        case ESP_RST_BROWNOUT:
        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
                return true;
        default:
                return false;
        }
}

bool Controller::RestorePendingPull()
{
        bool valid = g_pendingPullRtc.magic == PULL_RECOVERY_MAGIC
                && g_pendingPullRtc.crc == pendingPullRtcCrc(g_pendingPullRtc);

        pendingPullActive = valid && (g_pendingPullRtc.active != 0);
        if (!pendingPullActive)
                return false;

        char state = g_pendingPullRtc.state;
        unsigned long durationMs = g_pendingPullRtc.durationMs;
        esp_reset_reason_t reason = esp_reset_reason();
        Serial.println("[Recovery] pending pull found, reset reason=" + String((int)reason));

        if (!IsRecoverableResetReason(reason) || durationMs == 0 || (state != Single && state != Double))
        {
                Serial.println("[Recovery] pending pull discarded");
                ClearPendingPull();
                return false;
        }

        SetDisplayedProgress(DEFAULT_INT_INI);
        SetProgress(DEFAULT_INT_INI);
        SetTimePassed(0);
        SetTimeRemaning(durationMs);
        SetCurrentStatus(FinishState);
        SetCurrentKeyFlag(NONE_KEY);
        SetOldKeyFlag(NONE_KEY);
        SetTempKeyFlag(NONE_KEY);
        SetStartTime(millis());
        SetUpdateDisplay(true);
        MillOn();
        Serial.println("[Recovery] resuming interrupted pull");
        return true;
}
