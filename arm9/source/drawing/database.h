/***************************************************************************
 *                                                                         *
 *  This file is part of DSOrganize.                                       *
 *                                                                         *
 *  DSOrganize is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  DSOrganize is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with DSOrganize.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _DATABASE_INCLUDED
#define _DATABASE_INCLUDED

#include "zlib.h"
#include "fatwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DB_DISCONNECTED 0
#define DB_CONNECTING 1
#define DB_CONNECTED 2
#define DB_FAILEDCONNECT 3
#define DB_RECIEVEDLIST 4
#define DB_RECONNECT 5
#define DB_GETLIST 6
#define DB_GETPACKAGE 7
#define DB_RECIEVEDPACKAGE 8
#define DB_GETTINGMOTD 9
#define DB_GETTINGLIST 10
#define DB_GETTINGPACKAGE 11
#define DB_FAILEDPACKAGE 12
#define DB_GETTINGFILE 13

#define MOTD_SIZE 2048
#define LIST_SIZE 100000
#define PACKAGE_SIZE 4096

#define URL_SIZE 512

#define TIMEOUT 180
#define RETRIES 3

#define CHDR 0
#define MKDR 1
#define DOWN 2
#define DELE 3
#define CLS  4
#define ECHO 5
#define WAIT 6

#define ENCODING_RAW 0
#define ENCODING_DEFLATE 1
#define ENCODING_GZIP 2

#define DECODE_CHUNK (32*1024)

#define DOWN_SIZE (1024*10) // 10 kb recieve

typedef struct
{
	char category[33];
} CAT_LIST;

typedef struct
{
	int command;
	char instruction[256];
} INST_LIST;

typedef struct
{
	char name[31];
	char description[101];
	char dataURL[65];
	char date[11];
	char version[11];
	char size[11];
	char category[33];
} HB_LIST;

void freeWifiMem();
void resetDBMode();
void drawTopDatabaseScreen();
void drawBottomDatabaseScreen();
void dbStartPressed();
void getHomebrew();
void refreshList();
void lButtonDatabase();
void rButtonDatabase();
int getHBDBEntries();
void customDB(char *str);
void customPackage(char *str);
void prevCategory();
void nextCategory();

// for webbrowser
void getFile(char *url, char *uAgent);
void checkFile();
void resetRCount();
void cancelDownload();
int getLastCharset();
int getLastContentType();
void setPostData(char *toPost);
void setReferrer(char *toRefer);
void setContentType(int ct);
bool isDownloading();
char *getDownStatus();
void runSpeed();

extern int parseInstructions(char *inst);
extern int listEntries;
extern int catEntries;
extern int pkgEntries;
extern int tmpPos;
extern int pos;	
extern int tCount;
extern int rCount;
extern int t_Sock;
extern int downLength;
extern u32 fileRecieved;
extern u32 fileLength;
extern bool gotHeader;
extern int lastRecieved;
extern int lastSecond;
extern int speed;
extern char pkgName[64];
extern int dbMode;
extern char downStatus[256];
extern int catSize;
extern int specialMode;
extern bool aPressed;
extern bool hasStarted;
extern int whichCharset;
extern int contentType;
extern char curCat[33];
extern int whichCat;
extern bool currentDownload;
extern bool hasInitialized ;
extern int tEncoding;
extern z_stream strm;

extern char tempURL[URL_SIZE];
extern char downURL[URL_SIZE];
extern char *referer;
extern char *postData;
extern char *tAgent;
extern char *motd;
extern char *list;
extern char *pkgStatus;
extern char *savedURL;
extern INST_LIST *instList;
extern HB_LIST *hbList;
extern CAT_LIST *catList;

//for generic downloader
extern char *d_outStr;
extern int d_len;

extern char downFile[256];
extern DRAGON_FILE *downFP;


#ifdef __cplusplus
}
#endif

#endif
