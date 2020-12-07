#include "Arduino.h"
#include "Drawer.h"
#include <U8g2lib.h>
#include "display_defines.h"

Drawer::Drawer(int clk, int data) : _display(U8G2_R0, clk, data, U8X8_PIN_NONE)
{
        _display.begin();
        _display.setFlipMode(TARGETFLIPMODE);
        this->screensafer_x = _display.getWidth()/2;
        this->screensafer_y = _display.getHeight()/2;
        this->size = SAFTERINITIALSIZE;
}

void Drawer::Clear()
{
        _display.clearBuffer();
        _display.sendBuffer();
}

void Drawer::DrawCenter(String txt, int y)
{
        char pos = (_display.getWidth() - _display.getStrWidth(txt.c_str()))/2;
        _display.drawStr(pos, y, txt.c_str());
}

void Drawer::DrawScreenSafer(int time)
{
        _display.clearBuffer();
        if (time-this->_oldTime>1000)
        {
                this->size = random(0,_display.getHeight()/2-1);
                this->screensafer_x = random(this->size, _display.getWidth()-this->size);
                this->screensafer_y = random(this->size, _display.getHeight()-this->size);
                this->_oldTime = time;
        }
        _display.drawCircle(this->screensafer_x, this->screensafer_y,this->size);
        _display.sendBuffer();
}

void Drawer::DrawMain()
{
        
        int pos = 0;

        while(pos <= 64 && STARTANIMATION)
        {
                _display.clearBuffer();
                _display.setFont(KMNEXT_font);
                _display.setDrawColor(1);
                this->DrawCenter(KMNEXT_txt, KMNEXT_y);
                _display.setDrawColor(0);
                _display.drawBox(0,0,64-pos,32);
                _display.drawBox(64+pos,0,64-pos,32);
                _display.sendBuffer();
                _display.setDrawColor(1);
                pos = pos + 4;
        }
        while(pos > 0) {pos = pos - 4;}
}

void Drawer::DrawErr(bool sdStatus, bool nfcStatus, bool rtcStatus)
{
        _display.clearBuffer();
        _display.setFont(STATE_font);

        if (!sdStatus)
        {
                this->DrawCenter(SDERROR_txt, SDSTATE_y);
        }
        else
        {
                this->DrawCenter(SDOK_txt, SDSTATE_y);
        }

        if (!nfcStatus)
        {
                this->DrawCenter(NFCERROR_txt, NFCSTATE_y);
        }
        else
        {
                this->DrawCenter(NFCOK_txt, NFCSTATE_y);
        }
        if (!rtcStatus)
        {
                this->DrawCenter(RTCERROR_txt, RTCSTATE_y);
        }
        else
        {
                this->DrawCenter(RTCOK_txt, RTCSTATE_y);
        }

        if (!sdStatus |!nfcStatus | !rtcStatus)
        {
                _display.sendBuffer();
                while (true);
        }
}

void Drawer::DrawLastUser(String lastUser)
{
        _display.clearBuffer();
        _display.setFont(LASTID_font);
        this->DrawCenter(LASTID_txt, LASTID_y);
        _display.setFont(LASTID_value_font);
        this->DrawCenter(lastUser, LASTID_value_y);
        _display.sendBuffer();
}

void Drawer::DisplayProgress(int progress)
{

        _display.clearBuffer();
        _display.setFont(ZUBEREITUNG_font);
        this->DrawCenter(ZUBEREITUNG_txt,ZUBEREITUNG_y);

        for(int i=0; i<=progress/PROGRESS_division; i++)
        {
                _display.drawBox(i*13+1, PROGRESS_y, PROGRESS_width, PROGRESS_height);
        }
        _display.sendBuffer();
}

void Drawer::DrawUnknown()
{
        _display.clearBuffer();
        _display.setFont(UNKNOWNID_font);
        this->DrawCenter(UNKNOWNIDFILL_txt,UNKNOWNIDFILL_y1);
        this->DrawCenter(UNKNOWNID_txt,UNKNOWNID_y);
        this->DrawCenter(UNKNOWNIDFILL_txt,UNKNOWNIDFILL_y2);
        _display.sendBuffer();
}

void Drawer::DrawWaitForUser()
{
        _display.clearBuffer();
        _display.setFont(BITTEWAEHLEN_font);
        this->DrawCenter(BITTEWAEHLEN_txt, BITTEWAEHLEN_y);
        _display.setFont(ARROWS_font);
        this->DrawCenter(ARROWS_txt, ARROWS_y);
        _display.sendBuffer();
}

void Drawer::DrawPayOne()
{
        _display.clearBuffer();
        _display.setFont(PAYONE_font);
        this->DrawCenter(PAYONE_txt, PAYONE_y);
        _display.setFont(ADDCARD_font);
        this->DrawCenter(ADDCARD_txt, ADDCARD_y);
        _display.sendBuffer();

}

void Drawer::DrawSplitQ2()
{
        _display.clearBuffer();
        _display.setFont(SPLITQ_font);
        this->DrawCenter(SPLITQ_txt, SPLITQ_y);
        _display.setFont(SPLITQARRWOS_font);
        this->DrawCenter(SPLITQARROWS_txt, SPLITQARROWS_y);
        _display.sendBuffer();
}

void Drawer::DrawPay2 ()
{
        _display.clearBuffer();
        _display.setFont(PAY2_font);
        this->DrawCenter(PAY2_txt, PAY2_y);
        _display.setFont(ADDCARD_font);
        this->DrawCenter(ADDCARD_txt, ADDCARD_y);
        _display.sendBuffer();

}

void Drawer::DrawPay2_1()
{
        _display.clearBuffer();
        _display.setFont(PAY2_1_font);
        this->DrawCenter(PAY2_1_txt, PAY2_1_y);
        _display.setFont(ADD1CARD_font);
        this->DrawCenter(ADD1CARD_y, ADD1CARD_txt);
        _display.sendBuffer();

}

void Drawer::DrawPay2_2()
{
        _display.clearBuffer();
        _display.setFont(PAY2_2_font);
        this->DrawCenter(PAY2_2_txt, PAY2_2_y);
        _display.setFont(ADD2CARD_font);
        this->DrawCenter(ADD2CARD_txt, ADD2CARD_y);
        _display.sendBuffer();
}

void Drawer::DrawLowCredit()
{
        _display.clearBuffer();
        _display.setFont(LOWCREDIT1_font);
        this->DrawCenter(LOWCREDIT1_txt, LOWCREDIT1_y);
        _display.setFont(LOWCREDIT2_font);
        this->DrawCenter(LOWCREDIT2_txt, LOWCREDIT2_y);
        _display.sendBuffer();
}

void Drawer::DrawReplay(int progress)
{
        _display.clearBuffer();
        _display.setFont(REPAY1_font);
        this->DrawCenter(REPAY1_txt, REPAY1_y);
        _display.setFont(REPAY2_font);
        this->DrawCenter(REPAY2_txt, REPAY2_y);
        _display.drawLine(0,_display.getHeight()/2+1, _display.getWidth()*((progress)*0.01),_display.getHeight()/2+1);
        _display.drawLine(_display.getWidth(),_display.getHeight()/2-1, (_display.getWidth()-_display.getWidth()*((progress)*0.01)),_display.getHeight()/2-1);
        _display.sendBuffer();
}

void Drawer::DrawCredit(int ID, int Credit)
{
        String IDs = ID_txt + String(ID);
        String Cred = CREDITS_txt + String(Credit);

        _display.clearBuffer();
        _display.setFont(ID_font);
        this->DrawCenter(IDs, ID_y);
        _display.setFont(CREDITS_font);
        this->DrawCenter(Cred, CREDITS_y);
        _display.sendBuffer();

}

void Drawer::DrawDoneState()
{
        _display.clearBuffer();
        _display.setFont(DONE_font);
        this->DrawCenter(DONE_txt, DONE_y);
        _display.sendBuffer();
}

void Drawer::Drawer::Err()
{
        _display.clearBuffer();
        _display.setFont(ERR_font);
        this->DrawCenter(ERR_txt, ERR_y);
        _display.sendBuffer();
}

void Drawer::DrawFreeState()
{
        _display.clearBuffer();
        _display.setFont(FREE_font);
        this->DrawCenter(FREE1_txt, FREE1_y);
        this->DrawCenter(FREE2_txt, FREE2_y);
        _display.sendBuffer();
}

void Drawer::DrawStopState()
{
        _display.clearBuffer();
        _display.setFont(STOP_font);
        this->DrawCenter(STOP1_txt, STOP1_y);
        this->DrawCenter(STOP2_txt, STOP2_y);
        _display.sendBuffer();
}
