#ifndef DT_LIB
#define DT_LIB

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

//structs
//enum EstrpcmFormat {SPF_PCM=0,SPF_ADPCM=1,SPF_GSM=2};

typedef struct sTransferRegion3 {
	uint32 heartbeat;          // counts frames

	uint16 touchX,   touchY;        // TSC X, Y
	uint16 touchXpx, touchYpx;      // TSC X, Y pixel values
	sint16 touchZ1,  touchZ2;       // TSC x-panel measurements
	uint16 tdiode1,  tdiode2;      // TSC temperature diodes
	uint32 temperature;            // TSC comp
	uint16 buttons;            // X, Y, /PENIRQ buttons
 
	union {
		uint8 curtime[8];        // current time response from RTC

		struct {
		  u8 rtc_command;
		  u8 rtc_year;           //add 2000 to get 4 digit year
		  u8 rtc_month;          //1 to 12
		  u8 rtc_day;            //1 to (days in month)

		  u8 rtc_incr;
		  u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
		  u8 rtc_minutes;        //0 to 59
		  u8 rtc_seconds;        //0 to 59
		};
	};
	bool curtimeFlag;

	uint16 battery;            // battery life ??  hopefully.  :)
	uint16 aux;                // i have no idea...

	// Don't rely on these below, will change or be removed in the future
	vuint32 mailAddr;
	vuint32 mailData;
	vuint8 mailRead;
	vuint8 mailBusy;
	vuint8 mailSize;

	u32 LCDPowerControl;

	u32 ReqVsyncUpdate;

	u32 strpcmControl;
	u32 strpcmFreq,strpcmSamples,strpcmChannels;
	u32 strpcmVolume8;
	u32 strpcmWriteRequest;
	u32 strpcmFormat;
	s16 *strpcmLBuf,*strpcmRBuf;
} TransferRegion3, * pTransferRegion3;


#endif


#ifdef __cplusplus
extern "C" {
#endif

// date/time retrieval
extern int getDay();
extern int getMonth();
extern int getYear();
extern int getHour(bool military);
extern int getMinute();
extern int getSecond();
extern int getDayOfWeek();
extern int isPM();
extern int getDaysThisMonth();

// useful functions
extern int dayOfWeek(int dd, int mm, int yyyy);
extern int daysInMonth(int mm, int yy);
extern void addDay(int *day, int *month, int *year);
extern void subtractDay(int *day, int *month, int *year);

#ifdef __cplusplus
}
#endif
