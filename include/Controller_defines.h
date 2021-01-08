#ifndef Controller_defines
#define Controller_defines

// PIN DEFINES

#define taster_LINKS_pin 		7
#define taster_RECHTS_pin 		6
#define nfc_RS_pin 				2	
#define display_CLK_pin 		0
#define display_DATA_pin 		1
#define nfc_SS_pin 				11
#define sd_CS_pin 				4
#define RelayPin 				5


// Key defines

#define LEFT_KEY				'l'
#define RIGHT_KEY				'r'
#define BOTH_KEY				'b'
#define NONE_KEY				'n'


// Price defines

#define PRICE_SINGE				1
#define PRICE_DOUBLE			2


// Timing define

#define DELAY_MILL_ON					100
#define DELAY_AFTER_ERROR				2000
#define DELAY_ACTIVATION_SCREENSAFTER 	3600000
#define TIMEOUT_DEFAULT					10000
#define TIMEOUT_LONG					60000
#define TIMEOUT_SHORT					5000
#define TIMEOUT_REPAY					30000
#define TIME_FOR_PW_ACTIVATION			3000


// Status defines

#define OK						0
#define FAILED 					-1
#define INVALD_CREDIT			-1


// ERROR RETRY

#define ERROR_RETRY_WRITING		10


// Init Values

#define DEFAULT_INT_INI			0
#define ZERO_STRING				"0"
#define HUNDRED_PERCENT			100
#define NO_TIME					0

#define KEY_LENGTH_CONTROLLER	5  
#define TIME_RESOLUTION			100
#define PROGRESS_RESOLUTION		10
#define TASK_100MS				100
#define TASK_50MS				50

// STATES

enum MillStates
{
	Double             		= 	1,
	Single             		= 	2,
	WaitForSplitBooking 	= 	3,
	RepayState         		= 	4,
	DoneState           	= 	5,
	SceenSaferState     	= 	6,
	LstUserState        	= 	7,
	FreePullState 			= 	8,
	StopState				= 	9,
	FinishState  	    	= 	10,
 	WaitForUser         	= 	11,
  	PayOne              	= 	12,
  	AskForSplitPayment      = 	13,
  	PayTwo              	= 	14,
  	ShowCredit          	= 	15,
  	PayTwo_1            	= 	16,
  	PayTwo_2            	= 	17,
  	LowCredit           	= 	18,
  	ReadCreditUser      	= 	19,
	ShowLastUser        	= 	20,
	EnterKey				=	21,
	WrongKey				= 	22,
	AdaptTiSingle			= 	23,
	AdaptTiDouble			=	24,
	SelectTiToAdapt			= 	25,
	WebHandlerStop			= 	26
};

#endif
