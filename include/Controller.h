#ifndef CONTROLLER_h
#define CONTROLLER_h

#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include "WebHandler.h"
#include <WiFi101.h>
#include <SPI.h>
#include <WDTZero.h>

class Controller
{
public:
    Controller(int chipSelect, int slaveSelect, int rstPin, int clk, int data);

    void Begin();

    void States(char Status);
    char StateBegin(char state);
    char StateTransitions();

    void UpDateTime();

    bool TimeOut(unsigned long time);
    bool TimeOutWithBackPay(unsigned long time);

    void Reset();

    void ProcessInput();

    // State transitions

    char tr_WaitForUser();
    char tr_EnterKey();
    char tr_SelectToToAdapt();
    char tr_ShowCredit();
    char tr_Screensafer();
    char tr_Single();
    char tr_Dobule();
    char tr_StopState();
    char tr_FinishState();
    char tr_FreePullState();
    char tr_AskForSplitPayment();
    char tr_ReadCreditUser();
    char tr_AdaptTiSingle();
    char tr_AdaptTiDouble();
    char tr_PayOne();
    char tr_PayTwo();
    char tr_PayTwo_1();
    char tr_PayTwo_2();
    char tr_RepayState();
    char tr_DoneState();
    char tr_ShowLastUser();

    // Activation of mill

    void MillOn();
    void MillOff();

    // &Getter and Setter

    UserHandler &GetUserHandler();
    Drawer &GetDrawer();
    WDTZero &GetWatchDog();
    WebHandler &GetWebHandler();

    char &GetCurrentStatus();
    void SetCurrentStatus(char stat);

    void SetCurrentKeyFlag(char key);
    char GetCurrentKeyFlag();

    void SetTempKeyFlag(char key);
    char &GetTempKeyFlag();

    void SetOldKeyFlag(char key);
    char &GetOldKeyFlag();

    void SetStartTime(unsigned long Time);
    unsigned long &GetStartTime();

    void SetTimeSingle(unsigned long time);
    unsigned long &GetTimeSingle();

    void SetTimeDouble(unsigned long time);
    unsigned long &GetTimeDouble();

    void SetTimeRemaning(unsigned long time);
    unsigned long &GetTimeRemaning();

    void SetTimePassed(unsigned long time);
    unsigned long &GetTimePassed();

    void SetTimeDelta(unsigned long time);
    unsigned long &GetTimeDelta();

    void SetTimeStopBegin(unsigned long time);
    unsigned long &GetTimeStopBegin();

    void SetTimeInStop(unsigned long time);
    unsigned long &GetTimeInStop();

    void SetCurrentUser(String userID);
    String &GetCurrentUser();

    int &GetCreditAsInt();
    void SetCreditAsInt(int credit);

    int &GetUserAsInt();
    void SetUserAsInt(int user);

    unsigned long &GetTiRight();
    void SetTiRight(unsigned long ti);

    unsigned long &GetTiLeft();
    void SetTiLeft(unsigned long ti);

    unsigned long &GetTiBoth();
    void SetTiBoth(unsigned long ti);

    unsigned long &GetDeltaTiRight();
    void SetDeltaTiRight(unsigned long ti);

    unsigned long &GetTimer100ms();
    void SetTimer100ms(unsigned long ti);

    unsigned long &GetTimer50ms();
    void SetTimer50ms(unsigned long ti);

    unsigned long &GetDeltaTiLeft();
    void SetDeltaTiLeft(unsigned long ti);

    unsigned long &GetDeltaTiBoth();
    void SetDeltaTiBoth(unsigned long ti);

    int &GetLocalKey();
    void SetLocalKey(int key);

    int &GetActiveKeyElement();
    void SetActiveKeyElement(int element);

    int &GetLocalKeyDisplayed();
    void SetLocalKeyDisplayed(int key);

    int &GetActiveKeyElementDisplayed();
    void SetActiveKeyElementDispayed(int element);

    bool &GetUpdateDisplay();
    void SetUpdateDisplay(bool st);

    bool &GetWebHandlerActive();
    void SetWebHandlerActive(bool st);

    int &GetDisplayedProgress();
    void SetDisplayedProgress(int progress);

    int &GetProgress();
    void SetProgress(int prog);

private:

    Drawer                          MillDrawer;
    UserHandler                     MillUserHandler;
    WDTZero                         MillWatchDog;
    WebHandler                      MillWebHandler;

    String                          currentUser;

    char                            key;
    char                            oldKey;
    char                            tempOldKey;
    char                            currentStatus;

    bool updateDisplay              = true;
    bool webHandlerActive           = false;

    int localKey                    = 0;
    int localKeyDisplayed           = 0;
    int activeKeyElement            = 0;
    int activeKeyElementDisplayed   = 0;

    int activeProgress              = 0;
    int tempProgress                = 0;

    int activeUser                  = 0;
    int activeCredit                = 0;

    unsigned long tiSingle          = 0;
    unsigned long tiDouble          = 0;
    unsigned long tiRemaining       = 0;

    unsigned long tiStart           = 0;
    unsigned long tiCurrentTime     = 0;
    unsigned long tiDelat           = 0;
    unsigned long tiInStopState     = 0;
    unsigned long tiStopBegin       = 0;
    unsigned long tiPassed          = 0;

    unsigned long tiLeftDown        = 0;
    unsigned long tiDeltaLeftDown   = 0;
    unsigned long tiRightDown       = 0;
    unsigned long tiDeltaRightDown  = 0;
    unsigned long tiBothDown        = 0;
    unsigned long tiDeltaBothDown   = 0;

    unsigned long tiTimer100ms      = 0;
    unsigned long tiTimer50ms       = 0;
};

#endif
