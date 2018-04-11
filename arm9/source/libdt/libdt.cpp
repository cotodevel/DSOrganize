//////////////////////////////////////////////////////////////////////
// LibDT aims to be a simple way to get date and time elements from
// the DS, as well as a few useful date and time operations.
//
// -- DragonMinded
//////////////////////////////////////////////////////////////////////

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "libdt.h"
#include "clockTGDS.h"

int month_no[12]={0,3,3,6,1,4,6,2,5,0,3,5};
int year_no[7]={0,5,3,1,6,4,2};

int check_leap(int corres_year) 
{
	int leap;
	if ((corres_year%100)!=0) {
		if ((corres_year%4)!=0) leap=0;
		else leap=1;
	}
	else {
		if ((corres_year%400)!=0) leap=0;
		else leap=1;
	}
	return leap;
}

int get_nearest_year(int corres_year) 
{
	int t = corres_year%4;
	return corres_year-t;
}

int get_year_no(int corres_year) 
{
	int i = ((get_nearest_year(corres_year)-1900)%28)/4;
	return year_no[i];
}

int dayOfWeek(int dd, int mm, int yyyy)
{
	int corres_year = yyyy;
	int p3 = 0;
	int p4 = 0;
	int p1 = dd%7;
	int p2 = (p1+month_no[mm-1])%7;
	int isleap = check_leap(corres_year);
	if (isleap==1) {
		if (mm!=1 && mm!=2) {
			p3=(p2+get_year_no(corres_year))%7;
		}
		else {
			p3=(p2+get_year_no(corres_year)-1)%7;
		}
	}
	if (isleap==0) {
		p3=(p2+get_year_no(corres_year)+(corres_year-get_nearest_year(corres_year)))%7;
	}
		
	if (yyyy>=1900) p4=p3;
	if (yyyy>=1800 && yyyy<1900) p4=(p3+2)%7;
	if (yyyy>=1700 && yyyy<1800) p4=(p3+4)%7;
	if (yyyy>=1600 && yyyy<1700) p4=(p3+6)%7;
	
	return p4;
}

int daysInMonth(int mm, int yy)
{
	switch(mm)
	{
		case 0:
		case 2:
		case 4:
		case 6:
		case 7:
		case 9:
		case 11:
			return 31;
			break;		
		case 3:
		case 5:
		case 8:
		case 10:
			return 30;
			break;
		case 1: // check for leapyear
			if(check_leap(yy) == 1)
				return 29;
			return 28;
	}

	return 0;		
}

int getDaysThisMonth()
{
	return daysInMonth(getMonth()-1,getYear());
}

int getDay()
{
	//return (*(TransferRegion3*)IPC).curtime[3];
	struct tm * tmInst = getTime();
	return (int)tmInst->tm_mday;
}

int getMonth()
{
	struct tm * tmInst = getTime();
	return (int)tmInst->tm_mon;
}

int getYear()
{
	struct tm * tmInst = getTime();
	return (int) (tmInst->tm_year + 2000) ;
	//return ((*(TransferRegion3*)IPC).curtime[1]) + 2000;
}

int getHour(bool military)
{
	struct tm * tmInst = getTime();
	int hrs = (int)tmInst->tm_hour;	//(*(TransferRegion3*)IPC).curtime[5];		
	int pm = 0;
	
	if(hrs >= 52)
	{
		hrs-=52;
		pm = 1;
	}
	
	if(military == true && pm == 1)
		hrs += 12;
	
	if(military == false && hrs == 0)
		hrs = 12;
		
	return hrs;
}

int getMinute()
{
	struct tm * tmInst = getTime();
	return (int)tmInst->tm_min;
	//return (*(TransferRegion3*)IPC).curtime[6];
}

int getSecond()
{
	struct tm * tmInst = getTime();
	return (int)tmInst->tm_sec;
	//return (*(TransferRegion3*)IPC).curtime[7];
}

int getDayOfWeek()
{
	return dayOfWeek(getDay(),getMonth(),getYear());
}

int isPM()
{	
	struct tm * tmInst = getTime();
	int hrs = (int)tmInst->tm_hour;
	if(hrs >= 52){	//if((*(TransferRegion3*)IPC).curtime[5] >= 52)
		return 1;
	}
	return 0;
}

void addDay(int *day, int *month, int *year)
{
	int d, m, y;
	
	d = *day;
	m = *month;
	y = *year;
	
	d++;
	
	if(d > daysInMonth(m,y))
	{
		d = 1;
		m++;
		if(m > 11)
		{
			m = 0;
			y++;
		}
	}
	
	*day = d;
	*month = m;
	*year = y;
}

void subtractDay(int *day, int *month, int *year)
{
	int d, m, y;
	
	d = *day;
	m = *month;
	y = *year;
	
	d--;
	
	if(d == 0)
	{
		m--;
		if(m < 0)
		{
			m = 11;
			y--;
		}
		
		d = daysInMonth(m,y);
	}
	
	*day = d;
	*month = m;
	*year = y;
}
