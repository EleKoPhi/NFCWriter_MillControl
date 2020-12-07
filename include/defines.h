// COMPILER DEFINES 

#define DEBUG


// PIN DEFINES

#define taster_LINKS_pin 7
#define taster_RECHTS_pin 6
#define nfc_RS_pin 2
#define display_CLK_pin 0
#define display_DATA_pin 1
#define nfc_SS_pin 11
#define sd_CS_pin 4
#define RelayPin 5


// STATES

enum MillStates
{
	WaitForCard         = 0,
	ReadCard            = 1,
	WaitForInput        = 2,
	Doppelt             = 3,
	Einfach             = 4,
	WaitForSplitBooking = 5,
	IdentSecondPayer    = 6,
	UnknownUserState    = 7,
	LastUserState       = 8,
	CurrentDrawsState   = 9,
	KaffeeKingState     = 10,
	HoldState           = 20,
	ReplayState         = 21,
	DoneState           = 22,
	SceenSaferState     = 23,
	LstUserState        = 24,
	HoldState_2         = 25,
	FreePullState 		= 26,
	StopState			= 27,
	FinishState  	    = 28,


  	WaitForUser         = 11,
  	PayOne              = 12,
  	SplitPaymentQ       = 13,
  	PayTwo              = 14,
  	ShowCredit          = 15,
  	PayTwo_1            = 16,
  	PayTwo_2            = 17,
  	LowCredit           = 18,
  	ReadCreditUser      = 19

};
