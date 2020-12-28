#include "Arduino.h"
#include "Drawer.h"
#include <U8g2lib.h>
#include "Drawer_defines.h"

#ifdef SIXTYFOUR
U8G2_SSD1306_128X64_NONAME_F_SW_I2C &Drawer::GetDisplay() { return _display; }
#else
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C &Drawer::GetDisplay() { return _display; }
#endif

Drawer::Drawer(int clk, int data) : _display(U8G2_R0, clk, data, U8X8_PIN_NONE)
{
        GetDisplay().begin();
        GetDisplay().setFlipMode(TARGETFLIPMODE);
        SetScreensaverX(GetDisplay().getWidth() / 2);
        SetScreensaverY(GetDisplay().getHeight() / 2);
        SetScreensaverSize(SAFTERINITIALSIZE);
}

int Drawer::GetScreensaverSize() { return size; }
void Drawer::SetScreensaverSize(int Size) { size = Size; }
int Drawer::GetScreensaverX() { return screensafer_x; }
void Drawer::SetScreensaverX(int X) { screensafer_x = X; }
int Drawer::GetScreensaverY() { return screensafer_y; }
void Drawer::SetScreensaverY(int Y) { screensafer_y = Y; }
int Drawer::GetOldTime() { return _oldTime; }
void Drawer::SetOldTime(int time) { _oldTime = time; }

void Drawer::Clear()
{
        GetDisplay().clearBuffer();
        GetDisplay().sendBuffer();
}

void Drawer::DrawCenter(String txt, int y)
{
        char _pos = (GetDisplay().getWidth() - GetDisplay().getStrWidth(txt.c_str())) / 2;
        GetDisplay().drawStr(_pos, y, txt.c_str());
}

void Drawer::DrawScreenSafer(int time)
{
        GetDisplay().clearBuffer();
        if (time - GetOldTime() > UPDATETIME)
        {
                SetScreensaverSize(random(0, GetDisplay().getHeight() / 2 - 1));
                SetScreensaverX(random(GetScreensaverSize(), GetDisplay().getWidth() - GetScreensaverSize()));
                SetScreensaverY(random(GetScreensaverSize(), GetDisplay().getHeight() - GetScreensaverSize()));
                SetOldTime(time);
        }
        GetDisplay().drawCircle(GetScreensaverX(), GetScreensaverY(), GetScreensaverSize());
        GetDisplay().sendBuffer();
}

void Drawer::DrawMain()
{

        int pos = 0;

        while (pos <= GetDisplay().getWidth() && STARTANIMATION)
        {
                GetDisplay().clearBuffer();
                GetDisplay().setFont(KMNEXT_font);
                GetDisplay().setDrawColor(1);
                DrawCenter(KMNEXT_txt, KMNEXT_y);
                GetDisplay().setDrawColor(0);
                GetDisplay().drawBox(0, 0, GetDisplay().getWidth() - pos, GetDisplay().getWidth() / 2);
                GetDisplay().drawBox(GetDisplay().getWidth() + pos, 0, GetDisplay().getWidth() - pos, GetDisplay().getWidth() / 2);
                GetDisplay().sendBuffer();
                GetDisplay().setDrawColor(1);
                pos = pos + STEPSIZE;
        }
        while (pos > 0)
        {
                pos = pos - STEPSIZE;
        }
}

void Drawer::DrawErr(bool sdStatus, bool nfcStatus, bool rtcStatus)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(STATE_font);

        if (!sdStatus)
        {
                DrawCenter(SDERROR_txt, SDSTATE_y);
        }
        else
        {
                DrawCenter(SDOK_txt, SDSTATE_y);
        }

        if (!nfcStatus)
        {
                DrawCenter(NFCERROR_txt, NFCSTATE_y);
        }
        else
        {
                DrawCenter(NFCOK_txt, NFCSTATE_y);
        }
        if (!rtcStatus)
        {
                DrawCenter(RTCERROR_txt, RTCSTATE_y);
        }
        else
        {
                DrawCenter(RTCOK_txt, RTCSTATE_y);
        }

        if (!sdStatus | !nfcStatus | !rtcStatus)
        {
                GetDisplay().sendBuffer();
                while (true)
                        ;
        }
}

void Drawer::DrawLastUser(String lastUser)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(LASTID_font);
        DrawCenter(LASTID_txt, LASTID_y);
        GetDisplay().setFont(LASTID_value_font);
        DrawCenter(lastUser, LASTID_value_y);
        GetDisplay().sendBuffer();
}

void Drawer::DisplayProgress(int progress)
{

        GetDisplay().clearBuffer();
        GetDisplay().setFont(ZUBEREITUNG_font);
        DrawCenter(ZUBEREITUNG_txt, ZUBEREITUNG_y);

        for (int i = 0; i <= progress / PROGRESS_division; i++)
        {
                GetDisplay().drawBox(i * 13 + 1, PROGRESS_y, PROGRESS_width, PROGRESS_height);
        }
        GetDisplay().sendBuffer();
}

void Drawer::DrawUnknown()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(UNKNOWNID_font);
        DrawCenter(UNKNOWNIDFILL_txt, UNKNOWNIDFILL_y1);
        DrawCenter(UNKNOWNID_txt, UNKNOWNID_y);
        DrawCenter(UNKNOWNIDFILL_txt, UNKNOWNIDFILL_y2);
        GetDisplay().sendBuffer();
}

void Drawer::DrawWaitForUser()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(BITTEWAEHLEN_font);
        DrawCenter(BITTEWAEHLEN_txt, BITTEWAEHLEN_y);
        GetDisplay().setFont(ARROWS_font);
        DrawCenter(ARROWS_txt, ARROWS_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawPayOne()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAYONE_font);
        DrawCenter(PAYONE_txt, PAYONE_y);
        GetDisplay().setFont(ADDCARD_font);
        DrawCenter(ADDCARD_txt, ADDCARD_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawSplitQ2()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(SPLITQ_font);
        DrawCenter(SPLITQ_txt, SPLITQ_y);
        GetDisplay().setFont(SPLITQARRWOS_font);
        DrawCenter(SPLITQARROWS_txt, SPLITQARROWS_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawPay2()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_font);
        DrawCenter(PAY2_txt, PAY2_y);
        GetDisplay().setFont(ADDCARD_font);
        DrawCenter(ADDCARD_txt, ADDCARD_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawPay2_1()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_1_font);
        DrawCenter(PAY2_1_txt, PAY2_1_y);
        GetDisplay().setFont(ADD1CARD_font);
        DrawCenter(ADD1CARD_y, ADD1CARD_txt);
        GetDisplay().sendBuffer();
}

void Drawer::DrawPay2_2()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_2_font);
        DrawCenter(PAY2_2_txt, PAY2_2_y);
        GetDisplay().setFont(ADD2CARD_font);
        DrawCenter(ADD2CARD_txt, ADD2CARD_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawLowCredit()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(LOWCREDIT1_font);
        DrawCenter(LOWCREDIT1_txt, LOWCREDIT1_y);
        GetDisplay().setFont(LOWCREDIT2_font);
        DrawCenter(LOWCREDIT2_txt, LOWCREDIT2_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawReplay(int progress)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(REPAY1_font);
        DrawCenter(REPAY1_txt, REPAY1_y);
        GetDisplay().setFont(REPAY2_font);
        DrawCenter(REPAY2_txt, REPAY2_y);
        GetDisplay().drawLine(0, GetDisplay().getHeight() / 2 + 1, GetDisplay().getWidth() * ((progress)*0.01), GetDisplay().getHeight() / 2 + 1);
        GetDisplay().drawLine(GetDisplay().getWidth(), GetDisplay().getHeight() / 2 - 1, (GetDisplay().getWidth() - GetDisplay().getWidth() * ((progress)*0.01)), GetDisplay().getHeight() / 2 - 1);
        GetDisplay().sendBuffer();
}

void Drawer::DrawCredit(int ID, int Credit)
{
        String _id = ID_txt + String(ID);
        String _credit = CREDITS_txt + String(Credit);

        GetDisplay().clearBuffer();
        GetDisplay().setFont(ID_font);
        DrawCenter(_id, ID_y);
        GetDisplay().setFont(CREDITS_font);
        DrawCenter(_credit, CREDITS_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawDoneState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(DONE_font);
        DrawCenter(DONE_txt, DONE_y);
        GetDisplay().sendBuffer();
}

void Drawer::Drawer::Err()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(ERR_font);
        DrawCenter(ERR_txt, ERR_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawFreeState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(FREE_font);
        DrawCenter(FREE1_txt, FREE1_y);
        DrawCenter(FREE2_txt, FREE2_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawStopState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(STOP_font);
        DrawCenter(STOP1_txt, STOP1_y);
        DrawCenter(STOP2_txt, STOP2_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawKeyInput(int actualKey, int activeKeyElement)
{
        GetDisplay().clearBuffer();

        String _selector = "";
        String _key = "";

        for (int i = 0; i < KEY_LENGTH; i++)
        {
                if (activeKeyElement == i)
                {
                        _selector = _selector + UNDERSCORE;
                }
                else
                {
                        _selector = _selector + SPACE;
                }
                _key = _key + String(actualKey >> i & 0x01);
        }

        GetDisplay().setFont(KEYINPUT_font);
        DrawCenter(KEYINPUT_txt, KEYINPUT_y);
        GetDisplay().setFont(KEY_FONT);
        DrawCenter(_key, KEY_y);
        DrawCenter(_selector, KEY_SELECTOR_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawTimeSelect()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(TIMESELECT_font);
        DrawCenter(DOPPELT_txt, DOPPELT_y);
        DrawCenter(EINFACH_txt, EINFACH_y);
        GetDisplay().sendBuffer();
}

void Drawer::DrawTime(int time, bool type)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(TIME_font);

        if (type)
        {
                DrawCenter(TIME_DOUBLE_txt, TIME_y);
        }
        else
        {
                DrawCenter(TIME_SINGLE_txt, TIME_y);
        }

        DrawCenter(String(time) + String(" ms"), TIME_VALUE_y);
        GetDisplay().sendBuffer();
}
