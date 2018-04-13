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
 
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libcommon.h>
#include <libdt.h>
#include "database.h"
#include "address.h"
#include "browser.h"
#include "webbrowser.h"
#include "../fatwrapper.h"
#include "../colors.h"
#include "../keyboard.h"
#include "../general.h"
#include "../font_arial_9.h"
#include "../font_gautami_10.h"
#include "../globals.h"
#include "../settings.h"
#include "../filerout.h"
#include "../wifi.h"
#include "../http.h"
#include "../html.h"
#include "../graphics.h"
#include "../resources.h"
#include "../language.h"
#include "../controls.h"
#include "../cookies.h"
#include "zlib.h"
#include "../errors.h"

extern bool reverseDate;
extern char defaultSavePath[256];
extern u32 maxSize;
extern u32 curSize;

static int listEntries = 0;
static int catEntries = 0;
static int pkgEntries = 0;
static int tmpPos = 0;
static int pos = 0;	
static int tCount = 0;
static int rCount = 0;
static int t_Sock = 0;
static int downLength = 0;
static u32 fileRecieved = 0;
static u32 fileLength = 0;
static bool gotHeader = false;
static int lastRecieved = 0;
static int lastSecond = 0;
static int speed = 0;
static char pkgName[64];
static int dbMode = DB_DISCONNECTED;
static char downStatus[256];
static int catSize = 0;
static int specialMode = 0;
static bool aPressed = false;
static bool hasStarted = false;
static int whichCharset = CHARSET_ASCII;
static int contentType = TYPE_HTML;
static char curCat[33];
static int whichCat = 0;
static bool currentDownload = false;
static bool hasInitialized = false;
static int tEncoding = ENCODING_RAW;
static z_stream strm;

static char tempURL[URL_SIZE];
static char downURL[URL_SIZE];
static char *referer = NULL;
static char *postData = NULL;
static char *tAgent = NULL;
static char *motd = NULL;
static char *list = NULL;
static char *pkgStatus = NULL;
static char *savedURL = NULL;
static INST_LIST *instList = NULL;
static HB_LIST *hbList = NULL;
static CAT_LIST *catList = NULL;

//for generic downloader
char *d_outStr = NULL;
void (*completeDownload)() = NULL;
void (*failedDownload)() = NULL;
int d_len = 0;

char downFile[256];
DRAGON_FILE *downFP = NULL;

int parseInstructions(char *inst);

bool isDownloading()
{
	return currentDownload;
}

void resetDBMode()
{
	dbMode = DB_DISCONNECTED;
	specialMode = 0;
}

void customDB(char *str)
{	
	specialMode = 1;
	strcpy(tempURL, str);
}

void freePackage()
{
	if(pkgStatus)
		trackFree(pkgStatus);
	
	pkgStatus = NULL;
}

void initPackage()
{
	freePackage();
	
	pkgStatus = (char *)trackMalloc(PACKAGE_SIZE, "package status");
	memset(pkgStatus, 0, PACKAGE_SIZE);
}

void resetRCount()
{
	rCount = 0;
}

void clearPostData()
{
	if(postData)
		trackFree(postData);
	postData = NULL;
}

void clearReferrer()
{
	if(referer)
		trackFree(referer);

	referer = NULL;
}

void setPostData(char *toPost)
{
	clearPostData();
	
	postData = (char *)trackMalloc(strlen(toPost)+1, "post temp");
	memset(postData, 0, strlen(toPost)+1);
	
	strcpy(postData, toPost);
}

void setReferrer(char *toRefer)
{
	clearReferrer();
	
	referer = (char *)trackMalloc(strlen(toRefer)+1, "refer temp");
	memset(referer, 0, strlen(toRefer)+1);
	
	strcpy(referer, toRefer);
}

void customPackage(char *str)
{
	specialMode = 2;
	
	list = (char *)trackMalloc(MAX_CUSTOM_LIST,"package list");	
	memcpy(list, str, strlen(str)+1);
	
	pkgEntries = parseInstructions(list);
	
	initPackage();
	
	strcpy(downStatus, "N/A");
	
	trackFree(list);
	list = NULL;
	tmpPos = 0;
	
	aPressed = false;
}

void initWifiMem()
{
	motd = (char *)trackMalloc(MOTD_SIZE,"motd string");
	memset(motd, 0, MOTD_SIZE);
}

int getHBDBEntries()
{
	return catEntries;
}

void freeWifiMem()
{
	if(motd != NULL)
		trackFree(motd);
	
	if(hbList != NULL)
		trackFree(hbList);
	
	if(catList != NULL)
		trackFree(catList);
	
	if(list != NULL)
		trackFree(list);
		
	if(pkgStatus)
		trackFree(pkgStatus);
	
	if(postData)
		trackFree(postData);

	postData = NULL;	
	list = NULL;
	catList = NULL;
	hbList = NULL;
	motd = NULL;
	pkgStatus = NULL;
}
void stripEnd(char *str)
{
	int i = strlen(str);
	
	if(i < 4)
		return;
	
	if(str[i-4] == '@' && str[i-3] == 'E' && str[i-2] == 'N' && str[i-1] == 'D')
	{
		str[strlen(str)-4] = 0;
	}
}

void insertCharDB(HB_LIST *hb, int state, int x, char c)
{
	switch(state)
	{
		case 0: // title
			if(x < 30)
				hb->name[x] = c;
			break;
		case 1: // desc
			if(x < 100)
				hb->description[x] = c;
			break;
		case 2: // url
			if(x < 64)
				hb->dataURL[x] = c;
			break;
		case 3: // date
			if(x < 10)
				hb->date[x] = c;
			break;
		case 4: // version
			if(x < 10)
				hb->version[x] = c;
			break;
		case 5: // size
			if(x < 10)
				hb->size[x] = c;
			break;
		case 6: // category
			if(x < 32)
				hb->category[x] = c;
			break;
	}
}

void addNA(HB_LIST *hb)
{
	if(strlen(hb->date) == 0)
		strcpy(hb->date,"N/A");
	if(strlen(hb->version) == 0)
		strcpy(hb->version,"N/A");
	if(strlen(hb->size) == 0)
		strcpy(hb->size,"N/A");
}

// this is an interesting function
// my server gives out the date in 'm/dd/yy' format
// but due to the fact people want a 'dd/mm/yy' format
// I have to make it parse and reassemble if needed
void craftDate(HB_LIST *hb)
{
	if(!reverseDate) // not needed
		return;
	
	if(strcmp(hb->date, "N/A") == 0) // date not returned
		return;
	
	// ok, separate into 3 variables
	
	int x = 0;
	int pos = 0;
	char str[3][10];	
	memset(str, 0, 30);
	
	while(hb->date[x] != 0)
	{
		if(hb->date[x] == '/')
		{
			pos++;
		}
		else
		{
			str[pos][strlen(str[pos])] = hb->date[x];
		}
		
		x++;
	}
	
	if(strlen(str[0]) == 1)
		sprintf(hb->date, "%s/0%s/%s", str[1], str[0], str[2]);
	else
		sprintf(hb->date, "%s/%s/%s", str[1], str[0], str[2]);		
}

int parseList(char *list)
{	
	if(list[0] == 0)
		return 0;
	
	hbList = (HB_LIST *)trackMalloc(sizeof(HB_LIST),"homebrew list");
	memset(&hbList[0], 0, sizeof(HB_LIST));
	
	int lPos = 0;
	int pos = 0;
	int state = 0;
	int x = 0;
	
	while(list[lPos] != 0)
	{
		if(list[lPos] == '@') // end of line
		{
			addNA(&hbList[pos]);
			craftDate(&hbList[pos]);
			pos++;
			hbList = (HB_LIST *)trackRealloc(hbList, sizeof(HB_LIST) * (pos + 1));
			memset(&hbList[pos], 0, sizeof(HB_LIST));
			state = 0;
			x = 0;
		}
		else if(list[lPos] == '|') // end of data section
		{
			insertCharDB(&hbList[pos], state, x, 0);
			state++;
			x = 0;
		}
		else
		{
			insertCharDB(&hbList[pos], state, x, list[lPos]);
			x++;
		}
		
		lPos++;
	}
	
	return pos;
}

int compareHBCatName(const void *a, const void *b)
{
	CAT_LIST *v1 = (CAT_LIST*)a;
	CAT_LIST *v2 = (CAT_LIST*)b;
	
	char str1[33];
	char str2[33];
	
	memcpy(str1, v1->category, 33);
	memcpy(str2, v2->category, 33);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

void extractCategories()
{
	catList = (CAT_LIST *)trackMalloc(sizeof(CAT_LIST), "categories");
	memset(catList, 0, sizeof(CAT_LIST));
	catSize = 1;
	
	strcpy(catList[0].category, "All");
	
	for(int i=0;i<listEntries;++i)
	{
		bool pleaseAdd = true;
		
		for(int j=1;j<catSize;j++)
		{
			if(strcmp(catList[j].category, hbList[i].category) == 0)
			{
				pleaseAdd = false;
				break;
			}
		}
		
		if(pleaseAdd)
		{
			catSize++;
			catList = (CAT_LIST *)trackRealloc(catList, catSize * sizeof(CAT_LIST));
			memset(&catList[catSize-1], 0, sizeof(CAT_LIST));
			
			strcpy(catList[catSize-1].category, hbList[i].category);
		}
	}
	
	if(catSize > 2)
		qsort(&catList[1], catSize - 1, sizeof(CAT_LIST), compareHBCatName);
}

int compareHBListDate(const void *a, const void *b)
{
	HB_LIST *v1 = (HB_LIST*)a;
	HB_LIST *v2 = (HB_LIST*)b;
	
	char str1[256];
	char str2[256];
	
	if(strcmp(v1->date, "N/A") != 0 && strcmp(v2->date, "N/A") == 0) // dateless stuff always comes last
		return -1;
	
	if(strcmp(v1->date, "N/A") == 0 && strcmp(v2->date, "N/A") != 0) // dateless stuff always comes last
		return 1;
	
	if(strcmp(v1->date, v2->date) == 0) // equal, sort name!
	{
		strcpy(str1, v1->name);
		strcpy(str2, v2->name);
		
		strlwr(str1);
		strlwr(str2);
		
		return strcmp(str1, str2);	
	}
	
	// ok, separate into 3 variables
	
	int x = 0;
	int pos = 0;
	char date1[3][10];
	char date2[3][10];
	memset(date1, 0, 30);
	memset(date2, 0, 30);
	
	while(v1->date[x] != 0)
	{
		if(v1->date[x] == '/')
		{
			pos++;
		}
		else
		{
			date1[pos][strlen(date1[pos])] = v1->date[x];
		}
		
		x++;
	}
	
	x = 0;
	pos = 0;
	
	while(v2->date[x] != 0)
	{
		if(v2->date[x] == '/')
		{
			pos++;
		}
		else
		{
			date2[pos][strlen(date2[pos])] = v2->date[x];
		}
		
		x++;
	}
	
	int firstAdd = 0;
	int secondAdd = 1;
	
	if(reverseDate)
	{
		firstAdd = 1;
		secondAdd = 0;
	}
	
	strcpy(str1, date1[2]); // year
	if(strlen(date1[firstAdd]) == 1)
		strcat(str1, "0");
	strcat(str1, date1[firstAdd]); // month
	if(strlen(date1[secondAdd]) == 1)
		strcat(str1, "0");
	strcat(str1, date1[secondAdd]); // day
	
	strcpy(str2, date2[2]); // year
	if(strlen(date2[firstAdd]) == 1)
		strcat(str2, "0");
	strcat(str2, date2[firstAdd]); // month
	if(strlen(date2[secondAdd]) == 1)
		strcat(str2, "0");
	strcat(str2, date2[secondAdd]); // day
	
	return strcmp(str2, str1);	
}
	
int compareHBListName(const void *a, const void *b)
{
	HB_LIST *v1 = (HB_LIST*)a;
	HB_LIST *v2 = (HB_LIST*)b;
	
	char str1[256];
	char str2[256];
	
	strcpy(str1, v1->name);
	strcpy(str2, v2->name);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);
}

int parseInstructions(char *inst)
{
	if(list[0] == 0)
		return 0;
	
	if(instList)
		trackFree(instList);
	
	instList = (INST_LIST *)trackMalloc(sizeof(INST_LIST),"instructions parser");
	memset(&instList[0], 0, sizeof(INST_LIST));
	
	int lPos = 0;
	
	int pos = 0;
	int state = 0;
	int x = 0;
	
	while(1)
	{
		if(list[lPos] == '\r' || list[lPos] == '\n' || list[lPos] == '\t')
		{
			// skip
		} 
		else if(state == 0 && list[lPos] == ' ')
		{
			// skip trailing spaces
		}
		else if(list[lPos] == '@') // end of line
		{
			pos++;
			instList = (INST_LIST *)trackRealloc(instList, sizeof(INST_LIST) * (pos + 1));
			memset(&instList[pos], 0, sizeof(INST_LIST));
			state = 0;
			x = 0;
		}
		else if(list[lPos] == 0) // end of data
		{
			return pos;
		}		
		else
		{
			if(state == 0) // figure out command
			{
				char cmd[5];
				memset(cmd, 0, 5);
				
				for(int z=0;z<4;z++)
				{
					if(list[lPos] != '@')
					{
						cmd[z] = list[lPos];
						lPos++;
					}
					else
						break;
				}
				
				if(list[lPos] == '@')
					lPos--;
				
				strlwr(cmd);
				
				if(strcmp(cmd, "chdr") == 0)
					instList[pos].command = CHDR;
					
				if(strcmp(cmd, "mkdr") == 0)
					instList[pos].command = MKDR;
					
				if(strcmp(cmd, "down") == 0)
					instList[pos].command = DOWN;
					
				if(strcmp(cmd, "dele") == 0)
					instList[pos].command = DELE;
				
				if(strcmp(cmd, "cls") == 0)
					instList[pos].command = CLS;
				
				if(strcmp(cmd, "echo") == 0)
					instList[pos].command = ECHO;
				
				if(strcmp(cmd, "wait") == 0)
					instList[pos].command = WAIT;
				
				state = 1;
			}
			else
			{
				if(x < 256)
					instList[pos].instruction[x] = list[lPos];
				x++;
			}
		}
		
		lPos++;
	}
}

void getGenericURL(char *url, char *outLoc, int len, void (*cD)(), void (*fD)())
{
	d_len = len;
	d_outStr = outLoc;

	savedURL = url;	
	memset(d_outStr, 0, d_len);
	
	completeDownload = cD;
	failedDownload = fD;
	
	t_Sock = sendHTTPRequest(url);
	
	fileRecieved = 0;
	fileLength = 0;
	gotHeader = false;
	hasStarted = false;
	pos = 0;	
	tCount = 0;
}

void finishGenericURL()
{	
	if(completeDownload)
		completeDownload();
}

void failedGenericURL()
{
	if(failedDownload)
		failedDownload();
}

void checkGenericURL()
{
	if(tCount > TIMEOUT)
	{			
		wifiClose(t_Sock);
		
		if(rCount > (RETRIES - 1))
		{
			failedGenericURL();
			return;
		}
		
		getGenericURL(savedURL, d_outStr, d_len, completeDownload, failedDownload);
		rCount++;
	}
	
	tCount++;
	
	if(!hasStarted)
	{
		curSize = tCount;
		maxSize = (TIMEOUT+1);
	}
	
	setGlobalError(ERROR_DB_TEMPSPACE_MOTD);
	char *tcpTemp = (char *)trackMalloc(DOWN_SIZE, "temp download space");	
	setGlobalError(ERROR_NONE);
	int i = 1;
	
	i = recvData(t_Sock,tcpTemp,2048);
	tcpTemp[i] = 0;
	
	if(!gotHeader)
	{
		int tNegCount = 0;
		int tLoop = 0;
		int where = i;
		
		if(where < 0)
		{
			where = 0;
			tNegCount = 1;
		}
		
		while(!strstr(tcpTemp, "\r\n\r\n") && tLoop < 4)
		{			
			i = recvData(t_Sock,tcpTemp + where,2048-where);
			
			if(i > 0)
				where += i;
			else
				tNegCount++;
			
			IRQVBlankWait();
			
			tLoop++;
		}
		
		if(tNegCount < tLoop + 1)
			i = where;
		else
			i = -1;
	}
	
	if(i == 0) 
	{	
		if(!hasStarted)
		{				
			// retry
			wifiClose(t_Sock);
			
			if(rCount > (RETRIES - 1))
			{			
				failedGenericURL();
				trackFree(tcpTemp);
				return;
			}					
			
			getGenericURL(savedURL, d_outStr, d_len, completeDownload, failedDownload);
			rCount++;
		}
		else
		{
			// finished
			wifiClose(t_Sock);
			finishGenericURL();
		}
		
		trackFree(tcpTemp);
		return;
	}
	
	if(i > 0)
	{		
		tCount = 0;
		hasStarted = true;
		// handle checking header, saving to file.
		
		if(!gotHeader) // haven't gotten the header yet, lets parse
		{
			char *tmp = strstr(tcpTemp, "\r\n\r\n");
			
			if(tmp == NULL)
			{
				// bad request
				wifiClose(t_Sock);
				
				if(rCount > (RETRIES - 1))
				{
					failedGenericURL();
					trackFree(tcpTemp);
					return;
				}								
				
				getGenericURL(savedURL, d_outStr, d_len, completeDownload, failedDownload);
				rCount++;
				
				trackFree(tcpTemp);
				return;
			}
			
			int lengthSubtract = (tmp - tcpTemp) + 4;
			
			if(i - lengthSubtract > 0)
			{		
				strncat(d_outStr, tmp + 4, i - lengthSubtract);
				fileRecieved += i - lengthSubtract;
			}
			
			tmp[0] = 0;
			int tmpReply = getHTTPReplyCode(tcpTemp);
			
			if(tmpReply < 0)
			{
				// bad request
				wifiClose(t_Sock);
				
				if(rCount > (RETRIES - 1))
				{
					failedGenericURL();
					trackFree(tcpTemp);
					return;
				}
				
				getGenericURL(savedURL, d_outStr, d_len, completeDownload, failedDownload);
				rCount++;
				
				trackFree(tcpTemp);
				return;
			}
			
			// status ok, or server error
			if((tmpReply >= 200 && tmpReply < 300) || (tmpReply >= 400)) 
			{
				tmp[0] = 0;			
				fileLength = getHTTPLength(tcpTemp);
				
				gotHeader = true;
				curSize = 0;
			}
			
			// redirect!
			if(tmpReply >= 300 && tmpReply < 400)
			{
				// don't support redirects in HBDB requests
				wifiClose(t_Sock);
				
				failedGenericURL();
				trackFree(tcpTemp);
				return;
			}
		}
		else
		{	
			strncat(d_outStr, tcpTemp, i);
			fileRecieved += i;
		}
	}
	
	trackFree(tcpTemp);
	
	if(fileRecieved >= fileLength && fileLength > 0) // greater than too, just in case...
	{
		// end of data stream
		
		wifiClose(t_Sock);
		finishGenericURL();
		return;
	}
}

void initData()
{
	strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
	
	switch(tEncoding)
	{
		case ENCODING_GZIP:
		case ENCODING_DEFLATE:
			// Initialize gzip/deflate
			inflateInit2(&strm, 15+32);
			
			// Set init flag
			hasInitialized = true;
			
			break;
	}
}

void writeData(void *data, u32 size, DRAGON_FILE *df)
{
	switch(tEncoding)
	{
		case ENCODING_RAW:
			DRAGON_fwrite(data, 1, size, df);
			break;
		case ENCODING_GZIP:
		case ENCODING_DEFLATE:
			strm.avail_in = size;
			strm.next_in = (Bytef *)data;
			
			void *tOut = malloc(DECODE_CHUNK);
			
			bool retryLoop;
			
			do 
			{
				retryLoop = false;
				
				strm.avail_out = DECODE_CHUNK;
				strm.next_out = (Bytef *)tOut;
				
				int ret = inflate(&strm, Z_NO_FLUSH);
				
				switch(ret) 
				{
					case Z_DATA_ERROR:
						if(hasInitialized)
						{
							// Close previous stream
							inflateEnd(&strm);
							
							// Initialize gzip/deflate
							inflateInit2(&strm, -15);
							
							// Ensure this doesn't happen again
							hasInitialized = false;
							
							// Re-init in
							strm.avail_in = size;
							strm.next_in = (Bytef *)data;
							
							// Set to retry loop
							retryLoop = true;
							
							// Skip the next two fallthroughs
							break;
						}
					case Z_NEED_DICT:
					case Z_MEM_ERROR:
						// Ensure we don't retry initializing
						hasInitialized = false;
						return;
				}
				
				if(!retryLoop)
				{
					int have = DECODE_CHUNK - strm.avail_out;
					
					DRAGON_fwrite(tOut, 1, have, df);
				}
			} while (strm.avail_out == 0 || retryLoop);
			
			// Remove flag so we don't try again on bad streams
			hasInitialized = false;
			
			free(tOut);
			
			break;
	}
}

void closeData()
{
	switch(tEncoding)
	{
		case ENCODING_GZIP:
		case ENCODING_DEFLATE:
			inflateEnd(&strm);
			break;
	}
}

void getFile(char *url, char *uAgent)
{
	savedURL = url;
	tAgent = uAgent;
	
	t_Sock = sendHTTPRequestRaw(url, uAgent, postData, referer, true);
	
	fileRecieved = 0;
	fileLength = 0;
	gotHeader = false;
	hasStarted = false;
	currentDownload = true;
	pos = 0;	
	tCount = 0;
	curSize = 0;
	maxSize = (TIMEOUT+1);
}

void finishFile()
{
	currentDownload = false;
	closeData();
	DRAGON_fclose(downFP);
	dbMode = DB_RECIEVEDPACKAGE;
	setWebState(0);
	
	clearPostData();
}

void checkFile()
{
	if(tCount > TIMEOUT)
	{
		wifiClose(t_Sock);
		
		closeData();
		DRAGON_fclose(downFP);
		
		if(DRAGON_FileExists(downFile) != FE_NONE)
			DRAGON_remove(downFile);
		
		if(rCount > (RETRIES - 1))
		{
			currentDownload = false;
			dbMode = DB_FAILEDPACKAGE;
			setWebState(1);			
			
			clearPostData();
			return;
		}					
		
		downFP = DRAGON_fopen(downFile, "w");
		
		getFile(savedURL, tAgent);
		rCount++;
	}
	
	tCount ++;
	
	if(!hasStarted)
	{
		curSize = tCount;
		maxSize = (TIMEOUT+1);
	}
	
	checkJustKeys();
	setGlobalError(ERROR_DB_TEMPSPACE_FILE);
	char *tcpTemp = (char *)trackMalloc(DOWN_SIZE, "temp download space");
	setGlobalError(ERROR_NONE);
	int i = 1;
	
	i = recvData(t_Sock,tcpTemp,DOWN_SIZE);
	tcpTemp[i] = 0;
	
	checkJustKeys();
	
	if(!gotHeader)
	{	
		int tNegCount = 0;
		int tLoop = 0;
		int where = i;
		
		if(where < 0)
		{
			where = 0;
			tNegCount = 1;
		}
		
		while(!strstr(tcpTemp, "\r\n\r\n") && tLoop < 4)
		{
			i = recvData(t_Sock,tcpTemp + where,DOWN_SIZE-where);
			
			if(i > 0)
				where += i;
			else
				tNegCount++;
			
			IRQVBlankWait();
			
			tLoop++;
		}
		
		if(tNegCount < tLoop + 1)
			i = where;
		else
			i = -1;
	}
	
	if(i == 0) 
	{	
		if(!hasStarted)
		{				
			// retry
			wifiClose(t_Sock);
			
			closeData();
			DRAGON_fclose(downFP);
			
			if(DRAGON_FileExists(downFile) != FE_NONE)
				DRAGON_remove(downFile);
			
			if(rCount > (RETRIES - 1))
			{
				currentDownload = false;
				dbMode = DB_FAILEDPACKAGE;
				setWebState(1);			
				
				clearPostData();
				trackFree(tcpTemp);
				return;
			}					
			
			downFP = DRAGON_fopen(downFile, "w");
			
			getFile(savedURL, tAgent);
			rCount++;
		}
		else
		{
			// finished
			wifiClose(t_Sock);
			finishFile();
		}
		trackFree(tcpTemp);
		return;
	}
	
	if(i > 0)
	{		
		tCount = 0;
		hasStarted = true;
		// handle checking header, saving to file.
		
		if(!gotHeader) // haven't gotten the header yet, lets parse
		{
			char *tmp = strstr(tcpTemp, "\r\n\r\n");
			
			if(tmp == NULL)
			{
				// bad request
				wifiClose(t_Sock);
				
				closeData();
				DRAGON_fclose(downFP);
				
				if(DRAGON_FileExists(downFile) != FE_NONE)
					DRAGON_remove(downFile);
				
				if(rCount > (RETRIES - 1))
				{
					currentDownload = false;
					dbMode = DB_FAILEDPACKAGE;
					setWebState(1);
					clearPostData();
					trackFree(tcpTemp);
					return;
				}					
				
				downFP = DRAGON_fopen(downFile, "w");
				
				getFile(savedURL, tAgent);
				rCount++;
				
				trackFree(tcpTemp);
				return;
			}
			
			// to remember how much info to save
			int lengthSubtract = (tmp - tcpTemp) + 4;
			
			tmp[0] = 0;
			int tmpReply = getHTTPReplyCode(tcpTemp);
			
			if(tmpReply < 0)
			{
				// bad request
				wifiClose(t_Sock);
				
				closeData();
				DRAGON_fclose(downFP);
				
				if(DRAGON_FileExists(downFile) != FE_NONE)
					DRAGON_remove(downFile);
				
				if(rCount > (RETRIES - 1))
				{
					currentDownload = false;
					dbMode = DB_FAILEDPACKAGE;
					setWebState(1);			
					clearPostData();
					trackFree(tcpTemp);
					return;
				}					
				
				downFP = DRAGON_fopen(downFile, "w");
				
				getFile(savedURL, tAgent);
				rCount++;
				
				trackFree(tcpTemp);
				return;
			}
			
			// status ok, or server error
			if((tmpReply >= 200 && tmpReply < 300) || (tmpReply >= 400)) 
			{
				tmp[0] = 0;			
				fileLength = getHTTPLength(tcpTemp);
				
				addCookie(savedURL, tcpTemp);
				
				char tmpHeaderTag[URL_SIZE];
				
				// figure out the encoding type				
				tEncoding = ENCODING_RAW;
				
				memset(tmpHeaderTag, 0, URL_SIZE);					
				getHTTPHeaderValue(tcpTemp, "content-encoding", tmpHeaderTag);
				strlwr(tmpHeaderTag);
				
				if(strcmp(tmpHeaderTag, "deflate") == 0)
				{
					tEncoding = ENCODING_DEFLATE;
				}
				else if(strcmp(tmpHeaderTag, "gzip") == 0)
				{
					tEncoding = ENCODING_GZIP;
				}
				
				initData();
				
				// check to see if it's an attachment download
				memset(tmpHeaderTag, 0, URL_SIZE);					
				getHTTPHeaderValue(tcpTemp, "content-disposition", tmpHeaderTag);
				strlwr(tmpHeaderTag);
				
				if(strstr(tmpHeaderTag, "attachment;"))
				{
					// It's an attachment download
					
					char *tStr = (char *)safeMalloc(strlen(tmpHeaderTag)+2);
					
					// Find the filename
					if(strstr(tmpHeaderTag, "filename"))
					{
						char *tLoc = strstr(tmpHeaderTag, "filename") + 8;
						
						// Move to equals sign
						while(*tLoc != '=' && *tLoc != 0)
						{
							tLoc++;
						}
						
						if(*tLoc != 0)
						{
							// Move to place after equals
							tLoc++;
							
							// Move to first character
							while(*tLoc == ' ' && *tLoc != 0)
							{
								tLoc++;
							}
							
							if(*tLoc != 0)
							{
								// Found the filename
								bool isQuoted = false;
								
								if(*tLoc == '\"')
								{
									// Set flag that it's quoted and advance one
									isQuoted = true;
									tLoc++;
								}
								
								// Set initial string and position
								tStr[0] = '/';
								int tPos = 1;
								
								while(*tLoc != 0 && (!isQuoted || (isQuoted && *tLoc != '\"')))
								{
									// Assign char
									tStr[tPos] = *tLoc;
									
									// Advance pointers
									tLoc++;
									tPos++;
								}
								
								// Null terminate
								tStr[tPos] = 0;
								
								// Set filename
								setPageFile(tStr);
								
								// Set download flag
								setDownloadFlag();
								strcpy(downStatus, "");
							}
							
						}
					}
					
					free(tStr);
				}
				else
				{
					memset(tmpHeaderTag, 0, URL_SIZE);					
					getHTTPHeaderValue(tcpTemp, "content-type", tmpHeaderTag);
					strlwr(tmpHeaderTag);
					
					whichCharset = CHARSET_ASCII;
					contentType = TYPE_HTML;
					
					if(strstr(tmpHeaderTag, "charset="))
					{
						char *tFind = strstr(tmpHeaderTag, "charset=") + 8;
						
						char tTmp[64];
						
						strncpy(tTmp, tFind, 63);
						strlwr(tTmp);
						
						if(strstr(tTmp, ";"))
							strstr(tTmp, ";")[0] = 0;
						
						if(strcmp(tTmp, "utf-8") == 0)
							whichCharset = CHARSET_UTF8;
						if(strcmp(tTmp, "windows-1252") == 0)
							whichCharset = CHARSET_8859;
						if(strcmp(tTmp, " iso-8859-1") == 0)
							whichCharset = CHARSET_8859;
					}
					
					if(strstr(tmpHeaderTag, "text/html"))
						contentType = TYPE_HTML;
					else if(strstr(tmpHeaderTag, "text/css"))
						contentType = TYPE_PLAINTEXT;
					else if(strstr(tmpHeaderTag, "image/gif"))
						contentType = TYPE_IMAGE;
					else if(strstr(tmpHeaderTag, "image/jpeg"))
						contentType = TYPE_IMAGE;
					else if(strstr(tmpHeaderTag, "image/bmp"))
						contentType = TYPE_IMAGE;
					else if(strstr(tmpHeaderTag, "image/png"))
						contentType = TYPE_IMAGE;
					else
					{
						if(strstr(tmpHeaderTag, "text/plain"))
							contentType = TYPE_PLAINTEXT;
						
						// Should download here
						bool toDownload = true;
						char *tExt = getPageExtension();
						
						if(strlen(tExt) == 0)
							toDownload = false;
						else if(strcmp(tExt, "css") == 0)
							toDownload = false;
						else if(strcmp(tExt, "txt") == 0)
							toDownload = false;
						
						if(toDownload)
						{
							setDownloadFlag();
							strcpy(downStatus, "");
						}
					}
				}
				
				if(i - lengthSubtract > 0)
				{
					writeData(tmp + 4, i - lengthSubtract, downFP);
					fileRecieved += i - lengthSubtract;
				}
				
				gotHeader = true;
				curSize = 0;
			}
			
			// redirect!
			if(tmpReply >= 300 && tmpReply < 400)
			{
				if(tmpReply == 301 || tmpReply == 302)
				{
					// head to the new page
					wifiClose(t_Sock);
					
					closeData();
					DRAGON_fclose(downFP);
					
					if(DRAGON_FileExists(downFile) != FE_NONE)
						DRAGON_remove(downFile);
					
					downFP = DRAGON_fopen(downFile, "w");
					
					char tData[URL_SIZE];
					memset(tData, 0, URL_SIZE);					
					getHTTPHeaderValue(tcpTemp, "location", tData);
					
					setGlobalError(ERROR_DB_TEMPURL);
					char *tURL = (char *)safeMalloc(MAX_URL+1);
					setGlobalError(ERROR_NONE);
					memcpy(tURL, getCurrentURL(), MAX_URL+1);
					
					constructValidURL(tData, tURL);
					escapeIllegals(tURL);
					
					doRedirect(tURL);
					getFile(tURL, tAgent);
					
					free(tURL);
					
					rCount = 0; // since it's a new page
					trackFree(tcpTemp);
					
					return;
				}
			}
		}
		else
		{
			if(speed < 0)
				speed = 0;
			
			if(fileLength > 0)
				sprintf(downStatus, "%d of %d bytes (%d%c)\n%d KBps", fileRecieved, fileLength, (int)(((double)fileRecieved / (double)fileLength)  * 100), '%', speed >> 10);
			else
				sprintf(downStatus, "%d bytes\n%d KBps", fileRecieved, speed >> 10);
			
			writeData(tcpTemp, i, downFP);
			fileRecieved += i;
		}
	}
	
	if(hasStarted)
	{
		if(fileLength > 0)
		{
			curSize = fileRecieved + fileLength;
			maxSize = fileLength << 1;
			
			if(curSize > maxSize)
				curSize = maxSize;
		}
		else
		{
			curSize++;
			maxSize = 900;
			
			if(curSize > maxSize)
				curSize = maxSize;
		}
	}
	
	trackFree(tcpTemp);
	checkJustKeys();
	
	if(fileRecieved >= fileLength && fileLength > 0) // greater than too, just in case...
	{
		// end of data stream
		wifiClose(t_Sock);
		finishFile();
		return;
	}
}

char *getDownStatus()
{
	return downStatus;
}

int getLastCharset()
{
	return whichCharset;
}

int getLastContentType()
{
	return contentType;
}

void setContentType(int ct)
{
	contentType = ct;
}

void cancelDownload()
{
	wifiClose(t_Sock); // close current connection
	closeData();
	DRAGON_fclose(downFP);
	
	currentDownload = false;
	
	if(DRAGON_FileExists(downFile) != FE_NONE)
		DRAGON_remove(downFile);
	
	clearPostData();
}

void finishPackage()
{	
	stripEnd(list);
	
	pkgEntries = parseInstructions(list);
	
	initPackage();
	
	strcpy(downStatus, "N/A");
	
	trackFree(list);
	list = NULL;
	
	dbMode = DB_RECIEVEDPACKAGE;
	tmpPos = 0;
}

void failedPackage()
{
	if(list)
		trackFree(list);	
	
	list = NULL;
	
	dbMode = DB_FAILEDPACKAGE;
}

void getPackage(char *packageName)
{
	list = (char *)trackMalloc(MAX_CUSTOM_LIST,"package list");
	char pkg[URL_SIZE];
	
	if(specialMode == 1) // custom url
	{
		char tStr[URL_SIZE];
		strcpy(tStr, tempURL);
		strcat(tStr, "?req=PACKAGE&pkg=%s");
		
		sprintf(pkg, tStr, packageName);
	}
	else
		sprintf(pkg, res_PKG, packageName);
	
	rCount = 0;
	getGenericURL(pkg, list, MAX_CUSTOM_LIST, finishPackage, failedPackage);
}

void finishMOTD()
{	
	stripEnd(motd);
	dbMode = DB_GETLIST;
}

void failedMOTD()
{
	dbMode = DB_FAILEDCONNECT;
	
	if(catList)
		trackFree(catList);
	if(hbList)
		trackFree(hbList);
	if(list)
		trackFree(list);	
	
	list = NULL;	
	catList = NULL;
	hbList = NULL;
}

void getMOTD()
{	
	char tStr[URL_SIZE];
	
	if(specialMode == 1) // custom url
	{
		strcpy(tStr, tempURL);
		strcat(tStr, "?req=MOTD");
	}
	else
		strcpy(tStr, res_MOTD);
	
	rCount = 0;
	getGenericURL(tStr, motd, MOTD_SIZE, finishMOTD, failedMOTD);
}

void finishList()
{	
	if(catList)
		trackFree(catList);
	if(hbList)
		trackFree(hbList);
	
	catList = NULL;
	hbList = NULL;
	
	stripEnd(list);
	
	listEntries = parseList(list);
	catEntries = listEntries;
	strcpy(curCat, "All");
	whichCat = 0;
	
	trackFree(list);
	list = NULL;
	
	if(listEntries > 1) // lets sort this fucker
		qsort(hbList, listEntries, sizeof(HB_LIST), compareHBListName);
	
	extractCategories();
	
	dbMode = DB_RECIEVEDLIST;	
}

void getList()
{
	if(list)
		trackFree(list);
	
	list = (char *)trackMalloc(LIST_SIZE,"hblist temp");
	
	char tStr[URL_SIZE];
	
	if(specialMode == 1) // custom url
	{
		strcpy(tStr, tempURL);
		strcat(tStr, "?req=HBLIST&ver=3");
	}
	else
	{
		char tCard[5];
		u32 iCard = DRAGON_DiscType();
		
		strcpy(tStr, res_CARD);
		
		memcpy(tCard, &iCard, 4);
		tCard[4] = 0;
		
		if(tCard[3] == ' ') // ds-x fix
			tCard[3] = 0;
		
		strcat(tStr, tCard);
	}
	
	rCount = 0;
	getGenericURL(tStr, list, LIST_SIZE, finishList, failedMOTD);
}

void drawTopDatabaseScreen()
{	
	// actions
	
	switch(dbMode)
	{
		case DB_DISCONNECTED: // start connecting
			aPressed = false;
			connectWifi(); // request connect
			dbMode = DB_CONNECTING;
			break;
		case DB_CONNECTING: // test status
		{
			int i = Wifi_AssocStatus(); // check status
			if(i==ASSOCSTATUS_ASSOCIATED)
				dbMode = DB_CONNECTED;
				
			if(i==ASSOCSTATUS_CANNOTCONNECT) 
			{
				dbMode = DB_FAILEDCONNECT;
				listEntries = 0;
				catEntries = 0;
			}
			
			break;
		}
		case DB_RECONNECT:
			dbMode = DB_CONNECTED;
			break;
		case DB_CONNECTED:
			switch(specialMode)
			{
				case 0: // default
				case 1: // custom connect url
					resetCursor();
					initWifiMem();
					
					rCount = 0;
					getMOTD();
					dbMode = DB_GETTINGMOTD;
					break;
				case 2: // custom package
					dbMode = DB_RECIEVEDPACKAGE;
					break;
			}
			
			break;
		case DB_GETTINGMOTD:
			checkGenericURL();
			break;
		case DB_GETLIST:
			rCount = 0;	
			getList();
			dbMode = DB_GETTINGLIST;
			
			break;	
		case DB_GETTINGLIST:
			checkGenericURL();
			break;	
		case DB_FAILEDCONNECT:
			// do retry stuff here
			break;
		case DB_FAILEDPACKAGE:
			// do retry stuff here
			break;
		case DB_GETPACKAGE:
			rCount = 0;
			
			getPackage(pkgName);
			dbMode = DB_GETTINGPACKAGE;
			
			break;
		case DB_GETTINGPACKAGE:
			strcpy(downStatus, "N/A");
			checkGenericURL();
			break;
		case DB_RECIEVEDPACKAGE:
			// scripting stuff here
			
			if(tmpPos > pkgEntries)
				break;
				
			if(tmpPos == pkgEntries)
			{
				strcat(pkgStatus, l_done);
				strcat(pkgStatus, "\n");
				strcpy(downStatus, l_done);
				tmpPos++;
				
				break;
			}
			
			switch(instList[tmpPos].command)
			{
				case CHDR:
					// directories are always given relative to root
					DRAGON_chdir("/");
					char tmpDisp[512];
					
					if(strncmp(instList[tmpPos].instruction, "/%root%/", 8) == 0)
					{
						strcpy(tmpDisp, defaultSavePath);
						if(strlen(instList[tmpPos].instruction) > 8)
							strcat(tmpDisp, instList[tmpPos].instruction + 8);
					}
					else if(strncmp(instList[tmpPos].instruction, "/%data%/", 8) == 0) // it's dso data dir
					{
						strcpy(tmpDisp, d_base);
						if(strlen(instList[tmpPos].instruction) > 8)
							strcat(tmpDisp, instList[tmpPos].instruction + 8);
					}
					else
						strcpy(tmpDisp, instList[tmpPos].instruction);
					
					DRAGON_chdir(tmpDisp);
					
					strcat(pkgStatus, l_changedto);
					strcat(pkgStatus, " '");
					strcat(pkgStatus, tmpDisp);	
					strcat(pkgStatus, "'.\n");
					
					strcpy(downStatus, "N/A");
					
					break;
				case MKDR:
					// mkdir is relative to current path
					if(DRAGON_FileExists(instList[tmpPos].instruction) == FE_DIR)
					{
						// already exists
						
						strcat(pkgStatus, l_directory);
						strcat(pkgStatus, " '");
						strcat(pkgStatus, instList[tmpPos].instruction);
						strcat(pkgStatus, "' ");
						strcat(pkgStatus, l_alreadyexists);
						strcat(pkgStatus, ".\n");
						
						strcpy(downStatus, "N/A");
						
						break;
					}
					
					if(DRAGON_FileExists(instList[tmpPos].instruction) == FE_FILE)
					{
						// it's a file, lets delete it first
						DRAGON_remove(instList[tmpPos].instruction);
					}
					
					DRAGON_mkdir(instList[tmpPos].instruction);
					
					strcat(pkgStatus, l_createddirectory);
					strcat(pkgStatus, " '");
					strcat(pkgStatus, instList[tmpPos].instruction);
					strcat(pkgStatus, "'.\n");
					
					strcpy(downStatus, "N/A");
					
					break;
				case DOWN:
				{
					// download is relative to current path
					
					strcat(pkgStatus, l_starteddownload);
					strcat(pkgStatus, " '");
					strcat(pkgStatus, instList[tmpPos].instruction);
					strcat(pkgStatus, "'.\n");
					
					strcpy(downStatus, l_connectingtohost);
					
					downLength = 0;
					strcpy(downURL, instList[tmpPos].instruction);
					
					char *tmpStr = downURL;
					int x = 0;
					
					while(downURL[x] != 0)
					{
						if(downURL[x] == '/')
							tmpStr = &downURL[x+1];
						
						x++;
					}
					
					fileRecieved = 0;
					lastRecieved = 0;
					speed = 0;					
					
					strcpy(downFile, tmpStr);
					
					if(DRAGON_FileExists(downFile) != FE_NONE)
						DRAGON_remove(downFile);
					
					downFP = DRAGON_fopen(downFile, "w");
					
					rCount = 0;
					getFile(downURL, res_uAgent);
					dbMode = DB_GETTINGFILE;
					
					break;
				}
				case DELE:
					// delete a file relative to current path
					DRAGON_remove(instList[tmpPos].instruction);
					strcat(pkgStatus, l_deleted);
					strcat(pkgStatus, " '");
					strcat(pkgStatus, instList[tmpPos].instruction);
					strcat(pkgStatus, "'.\n");
					
					strcpy(downStatus, "N/A");
					
					break;
				case CLS:
					memset(pkgStatus, 0, PACKAGE_SIZE);
					strcpy(downStatus, "N/A");
					break;
				case ECHO:
					strcat(pkgStatus, instList[tmpPos].instruction);
					strcat(pkgStatus, "\n");
					
					strcpy(downStatus, "N/A");
					break;
				case WAIT:					
					strcpy(downStatus, l_waitforinput);
					strcat(downStatus, ".\n");
					
					drawButtonTexts(l_continue, NULL);
					
					if(aPressed)
						aPressed = false;
					else						
						tmpPos--;
					
					break;
			}		
			
			tmpPos++;
			
			break;
		case DB_GETTINGFILE:
			checkFile();
			break;
	}
	
	// display
	
	switch(dbMode)
	{
		case DB_DISCONNECTED:
		case DB_CONNECTING:
		case DB_CONNECTED:
		case DB_GETTINGMOTD:
		case DB_GETLIST:
		case DB_GETTINGLIST:
		{
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_motd);
			fb_dispString(3, 96, l_description);
			fb_dispString(3, 165, l_date);	
			fb_dispString(123, 165, l_version);
			fb_dispString(203 - 15, 165, l_size);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 90);
			fb_drawFilledRect(3, 12, 252, 93, widgetBorderColor, textEntryFillColor);			
			fb_dispString(0,0, l_retrieving);
			
			fb_setClipping(5, 108, 250, 158);
			fb_drawFilledRect(3, 105, 252, 161, widgetBorderColor, textEntryFillColor);
			fb_dispString(0, 0, l_retrieving);
			
			fb_setClipping(5, 177, 115, 185);
			fb_drawFilledRect(3, 174, 117, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			fb_setClipping(125, 177, 195 - 15, 185);
			fb_drawFilledRect(123, 174, 197 - 15, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			fb_setClipping(205 - 15, 177, 250, 185);
			fb_drawFilledRect(203 - 15, 174, 252, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			break;
		}
		case DB_FAILEDCONNECT:
		{
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_motd);
			fb_dispString(3, 96, l_description);
			fb_dispString(3, 165, l_date);	
			fb_dispString(123, 165, l_version);
			fb_dispString(203 - 15, 165, l_size);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 90);
			fb_drawFilledRect(3, 12, 252, 93, widgetBorderColor, textEntryFillColor);		
			fb_dispString(0,0, l_errorretrieving);
			
			fb_setClipping(5, 108, 250, 158);
			fb_drawFilledRect(3, 105, 252, 161, widgetBorderColor, textEntryFillColor);
			fb_dispString(0, 0, l_errorretrieving);			
			
			fb_setClipping(5, 177, 115, 185);
			fb_drawFilledRect(3, 174, 117, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			fb_setClipping(125, 177, 195 - 15, 185);
			fb_drawFilledRect(123, 174, 197 - 15, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			fb_setClipping(205 - 15, 177, 250, 185);
			fb_drawFilledRect(203 - 15, 174, 252, 188, widgetBorderColor, textEntryFillColor);
			fb_dispString(0,1, "N/A");
			
			break;
		}
		case DB_RECIEVEDLIST:
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_motd);
			fb_dispString(3, 96, l_description);
			fb_dispString(3, 165, l_date);	
			fb_dispString(123, 165, l_version);
			fb_dispString(203 - 15, 165, l_size);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 90);
			fb_drawFilledRect(3, 12, 252, 93, widgetBorderColor, textEntryFillColor);			
			fb_dispString(0,0,motd);
			
			fb_drawFilledRect(3, 105, 252, 161, widgetBorderColor, textEntryFillColor);
			fb_drawFilledRect(3, 174, 117, 188, widgetBorderColor, textEntryFillColor);
			fb_drawFilledRect(123, 174, 197 - 15, 188, widgetBorderColor, textEntryFillColor);
			fb_drawFilledRect(203 - 15, 174, 252, 188, widgetBorderColor, textEntryFillColor);
			
			if(catEntries > 0)
			{
				int x = -1;
				
				for(uint16 lx=0;lx<listEntries;++lx)
				{
					bool drawList = false;
					
					if(strcmp(curCat, "All") == 0 || strcmp(hbList[lx].category, curCat) == 0)
					{
						drawList = true;
						x++;
					}
					
					if(drawList && x == getCursor())
					{
						fb_setClipping(5, 108, 250, 158);
						fb_dispString(0, 0, hbList[lx].description);
						
						fb_setClipping(5, 177, 115, 185);
						fb_dispString(0,1, hbList[lx].date);
						
						fb_setClipping(125, 177, 195 - 15, 185);
						fb_dispString(0,1, hbList[lx].version);
						
						fb_setClipping(205 - 15, 177, 250, 185);
						fb_dispString(0,1, hbList[lx].size);
					}
				}
			}
			
			break;
		case DB_GETPACKAGE:
		case DB_GETTINGPACKAGE:
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_status);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 185);
			fb_drawFilledRect(3, 12, 252, 188, widgetBorderColor, textEntryFillColor);		
			fb_dispString(0,0, l_gettingpackage);
			break;
		case DB_RECIEVEDPACKAGE:
		case DB_GETTINGFILE:
		{
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_status);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 185);
			fb_drawFilledRect(3, 12, 252, 188, widgetBorderColor, textEntryFillColor);		
			
			// calculate how far down to display to ensure last line always visible, like a console.
			
			if(pkgStatus)
			{
				int addAmount = 0;
				
				int *pts = NULL;
				int numPts = getWrapPoints(0, 0, pkgStatus, 5, 15, 250, 185, &pts, (uint16 **)font_gautami_10);
				
				if(numPts > 15)
					addAmount = pts[(numPts - 15)];
				
				free(pts);
				
				fb_dispString(0,0,pkgStatus + addAmount);
			}
			
			break;
		}
		case DB_FAILEDPACKAGE:
		{
			setColor(genericTextColor);
			setFont((uint16 **)font_gautami_10);
			
			fb_dispString(3, 3, l_status);
			
			setColor(textEntryTextColor);
			
			fb_setClipping(5, 15, 250, 185);
			fb_drawFilledRect(3, 12, 252, 188, widgetBorderColor, textEntryFillColor);		
			fb_dispString(0,0, l_failedpackage);
			break;
		}
	}
}

void databaseListCallback(int pos, int x, int y)
{
	char str[256];
	int lx = -1;
	int cx = 0;
	
	// locate true element
	for(int findX = 0;findX < listEntries;findX++)
	{
		if(strcmp(curCat, "All") == 0 || strcmp(hbList[findX].category, curCat) == 0)
		{
			if(pos == cx)
			{
				lx = findX;
				break;
			}
			
			cx++;
		}
	}
	
	// display it if it exists
	if(lx != -1)
	{	
		strcpy(str, hbList[lx].name);
		abbreviateString(str, list_right - (list_left + 17), (uint16 **)font_arial_9);		
		
		bg_dispString(15, 0, str);
		bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
	}
}

void runSpeed()
{	
	if(getSecond() != lastSecond)
	{
		speed = fileRecieved - lastRecieved; // in bytes
		
		lastRecieved = fileRecieved; // set to zero again
		lastSecond = getSecond(); // reset seconds
	}
}

void drawBottomDatabaseScreen()
{
    char statusStr[64];
	
	sprintf(statusStr, "%s:", l_status);
	
	switch(dbMode)
	{	
		case DB_DISCONNECTED:
		case DB_CONNECTING:
			setFont((uint16 **)font_arial_9);
			setColor(genericTextColor);
			
			bg_dispString(10,20, statusStr);
			setBold(true);			
			bg_dispString(50,20,l_connecting);
			setBold(false);
			
			drawButtonTexts(NULL, l_cancel);
			
			break;
		case DB_CONNECTED:
		case DB_GETTINGMOTD:
			setFont((uint16 **)font_arial_9);
			setColor(genericTextColor);
			
			bg_dispString(10,20, statusStr);
			setBold(true);			
			bg_dispString(50,20,l_getmotd);
			setBold(false);		
			
			drawButtonTexts(NULL, l_cancel);
			
			break;
		case DB_GETLIST:
		case DB_GETTINGLIST:
			setFont((uint16 **)font_arial_9);
			setColor(genericTextColor);
			
			bg_dispString(10,20, statusStr);
			setBold(true);			
			bg_dispString(50,20,l_getlist);
			setBold(false);		
			
			drawButtonTexts(NULL, l_cancel);			
			
			break;
		case DB_FAILEDCONNECT:
			listEntries = 0;		
			catEntries = 0;
		case DB_RECIEVEDLIST:
		{
			drawHome();
			
			if(getHBDBEntries() == 0)
				drawButtonTexts(NULL, l_refresh);
			else
				drawButtonTexts(l_download, l_refresh);
			
			drawListBox(list_left, LIST_TOP, list_right, LIST_BOTTOM - 15, getCursor(), getHBDBEntries(), l_nodbentries, databaseListCallback);
			drawScrollBar(getCursor(), getHBDBEntries() - 1, default_scroll_left, DEFAULT_SCROLL_TOP, DEFAULT_SCROLL_WIDTH, DEFAULT_SCROLL_HEIGHT - 15);
			
			if(dbMode == DB_RECIEVEDLIST)
			{
				// sort buttons
				
				drawLR(l_namesort, l_datesort);
				
				bg_drawFilledRect(13, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 13, 242, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) + 2, widgetBorderColor, textEntryFillColor);
				bg_dispCustomSprite(5, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 10, spr_left, 31775, configurationArrowColor);
				bg_dispCustomSprite(245, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 10, spr_right, 31775, configurationArrowColor);
				
				setColor(textEntryTextColor);
				bg_setClipping(13, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) - 13, 242, (DEFAULT_SCROLL_TOP + DEFAULT_SCROLL_HEIGHT) + 5);
				bg_dispString(centerOnPt(112, curCat, (uint16 **)font_gautami_10), 4, curCat);
			}
			
			break;	
		}
		case DB_GETPACKAGE:
		case DB_GETTINGPACKAGE:
		case DB_RECIEVEDPACKAGE:
		case DB_GETTINGFILE:			
			if(tmpPos < pkgEntries || dbMode == DB_GETPACKAGE || dbMode == DB_GETTINGPACKAGE || dbMode == DB_GETTINGFILE)
				drawButtonTexts(NULL, l_cancel);
			else
				drawButtonTexts(NULL, l_done);
			
			setFont((uint16 **)font_arial_9);
			setColor(genericTextColor);
			bg_dispString(10,20, statusStr);
			setBold(true);			
			bg_dispString(50,20,downStatus);
			setBold(false);			
			
			runSpeed();
			
			break;
		case DB_FAILEDPACKAGE:
			drawButtonTexts(NULL, l_done);
			break;	
	}
}

void dbStartPressed()
{
	switch(dbMode)
	{
		case DB_RECIEVEDLIST:
		case DB_FAILEDCONNECT:
			returnHome();
			freeWifiMem();
			disconnectWifi();
			break;
	}
}

void getHomebrew()
{
	if(dbMode == DB_RECIEVEDLIST)
	{
		if(catEntries == 0)
			return;
		
		int x = -1;
		
		for(uint16 lx=0;lx<listEntries;++lx)
		{
			bool drawList = false;
			
			if(strcmp(curCat, "All") == 0 || strcmp(hbList[lx].category, curCat) == 0)
			{
				drawList = true;
				x++;
			}
			
			if(drawList && x == getCursor())
			{
				strcpy(pkgName, hbList[lx].dataURL);
				break;
			}
		}
		
		dbMode = DB_GETPACKAGE;
	}
	else if(dbMode == DB_RECIEVEDPACKAGE)
		aPressed = true;
}

void refreshList()
{	
	switch(dbMode)
	{
		case DB_DISCONNECTED:
		case DB_CONNECTING:
		case DB_CONNECTED:
		case DB_GETTINGMOTD:
		case DB_GETLIST:
		case DB_GETTINGLIST:
			dbMode = DB_FAILEDCONNECT;
			listEntries = 0;
			catEntries = 0;
			break;
		case DB_RECIEVEDLIST:
		case DB_FAILEDCONNECT:
			freeWifiMem();
			initWifiMem();
			dbMode = DB_RECONNECT;
			break;
		case DB_GETPACKAGE:
		case DB_GETTINGPACKAGE:
		case DB_RECIEVEDPACKAGE:
		case DB_FAILEDPACKAGE:
			freePackage();
			
			if(specialMode == 2) // custom package
			{
				popCursor();
				setMode(BROWSER);
				freeWifiMem();
				initWifiMem();
				disconnectWifi();
			}
			else
				dbMode = DB_RECIEVEDLIST;
			
			break;				
		case DB_GETTINGFILE:
			cancelDownload();
			
			if(specialMode == 2) // custom package
			{
				popCursor();
				setMode(BROWSER);
				freeWifiMem();
				initWifiMem();
				disconnectWifi();
			}
			else
				dbMode = DB_RECIEVEDLIST;
			
			break;			
			
	}
}

void lButtonDatabase()
{
	if(dbMode != DB_RECIEVEDLIST)
		return;
		
	if(listEntries > 1) // lets sort this fucker
		qsort(hbList, listEntries, sizeof(HB_LIST), compareHBListName);
}

void rButtonDatabase()
{
	if(dbMode != DB_RECIEVEDLIST)
		return;
	
	if(listEntries > 1) // lets sort this fucker
		qsort(hbList, listEntries, sizeof(HB_LIST), compareHBListDate);
}

void prevCategory()
{
	if(catSize < 2)
	{
		strcpy(curCat, "All");
		whichCat = 0;
		catEntries = listEntries;
		
		return;
	}
	
	whichCat--;
	
	if(whichCat < 0)
		whichCat = catSize-1;
	
	strcpy(curCat, catList[whichCat].category);
	catEntries = 0;
	resetCursor();
	
	for(uint16 lx=0;lx<listEntries;++lx)
	{
		if(strcmp(curCat, "All") == 0 || strcmp(hbList[lx].category, curCat) == 0)
			catEntries++;
	}
}

void nextCategory()
{
	if(catSize < 2)
	{
		strcpy(curCat, "All");
		whichCat = 0;
		catEntries = listEntries;
		
		return;
	}

	whichCat++;
	
	if(whichCat > catSize-1)
		whichCat = 0;
	
	strcpy(curCat, catList[whichCat].category);
	catEntries = 0;
	resetCursor();
	
	for(uint16 lx=0;lx<listEntries;++lx)
	{
		if(strcmp(curCat, "All") == 0 || strcmp(hbList[lx].category, curCat) == 0)
			catEntries++;
	}
}

