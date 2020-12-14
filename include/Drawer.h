#ifndef Drawer_h
#define Drawer_h

#include "Arduino.h"
#include <U8g2lib.h>

#define DEBUG

class Drawer
{
public:
Drawer(int clk, int data);
void DrawMain();
void DrawErr(bool sdStatus, bool nfcStatus, bool rtcStatus);
void DrawDes(String user);
void DrawSplitQ();
void DrawUnknown();
void DisplayProgress(int progress);
void Clear();
void DrawCenter(String txt, int y);
void DrawReplay(int progress);
void DrawDoneState();
void DrawScreenSafer(int time);
void DrawLastUser(String lastUser);
void DrawKaffeeKing(String King);
void DrawCurrentAmount(int amount);
void DrawWaitForUser();
void DrawPayOne();
void DrawSplitQ2();
void DrawPay2 ();
void DrawPay2_1();
void DrawPay2_2();
void DrawLowCredit();
void DrawCredit(int ID, int Credit);
void Err();
void DrawFreeState();
void DrawStopState();

int GetScreensaverSize();
void SetScreensaverSize(int Size);
int GetScreensaverX();
void SetScreensaverX(int Size);
int GetScreensaverY();
void SetScreensaverY(int Size);
int GetOldTime();
void SetOldTime(int time);

#ifdef SIXTYFOUR
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C &GetDisplay();
#else
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C &GetDisplay();
#endif


private:

#ifdef SIXTYFOUR
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C _display;
#else
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C _display;
#endif

int _oldTime;
int screensafer_x;
int screensafer_y;
int size;

};

#endif
