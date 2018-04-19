#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <string.h>
#include <libdt.h>
#include "vcard.h"
#include "fatwrapper.h"
#include "extras.h"
#include "fsfatlayerTGDSLegacy.h"
#include "fsfatlayerTGDSNew.h"

extern char l_unknown[60];
extern bool firstLast;

int parseTokens(char *inStr, int type)
{
	strlwr(inStr);
	
	int x = 0;
	char tmpStr[100];
	
	memset(tmpStr, 0, 100);
		
	do
	{
		if(inStr[x] == ';' || inStr[x] == 0)
		{
			switch(type)
			{
				case TOKEN:				
					if(strcmp(tmpStr, "home") == 0)
						return VCARD_HOME;				
					if(strcmp(tmpStr, "type=home") == 0)
						return VCARD_HOME;
					
					if(strcmp(tmpStr, "work") == 0)
						return VCARD_WORK;		
					if(strcmp(tmpStr, "type=work") == 0)
						return VCARD_WORK;
						
					if(strcmp(tmpStr, "cell") == 0)
						return VCARD_CELL;		
					if(strcmp(tmpStr, "type=cell") == 0)
						return VCARD_CELL;
					
					if(strcmp(tmpStr, "internet") == 0)
						return VCARD_INTERNET;		
					if(strcmp(tmpStr, "type=internet") == 0)
						return VCARD_INTERNET;
						
					break;
				case ENCODING:
					if(strcmp(tmpStr, "encoding=quoted-printable") == 0)
						return ENCODING_QUOTED_PRINTABLE;
					
					break;
			}
			
			memset(tmpStr, 0, 100);			
		}
		else
		{
			tmpStr[strlen(tmpStr)] = inStr[x];
		}
		
		x++;
	}	
	while(inStr[x - 1] != 0);
	
	return VCARD_UNKNOWN;
}

int isValidVCard(char *filename)
{
	char str[255];
	
	DRAGON_FILE *fFile = DRAGON_fopen(filename, "r");
	DRAGON_detectUnicode(fFile);
	memset(str, 0, 255);
	DRAGON_fgets(str, 255, fFile);
	if(strncmp(str, "BEGIN:VCARD",11) != 0)		
	{
		DRAGON_fclose(fFile);
		return -1;
	}
	
	memset(str, 0, 255);
	DRAGON_fgets(str, 255, fFile);
	if(strncmp(str, "VERSION:2.1",11) != 0)
	{	
		if(strncmp(str, "VERSION:3.0",11) != 0)
		{
			DRAGON_fclose(fFile);
			return -1;
		}
	}
	
	DRAGON_fclose(fFile);	
	return 0;
}

void parseAddress(char *str, ADDRESS_LINE *al)
{
	uint16 x = 0;	
	
	while(str[x] != ';') // skip first line
	{
		x++;
	}
	
	x++;			
	while(str[x] != ';') // skip second line
	{
		x++;
	}
	
	x++;
	
	uint16 y = 0;
	while(str[x] != ';' && x < strlen(str))
	{
		if(y < 60)
		{
			al->addrLine[y] = str[x];
			y++;
		}
		x++;
	}			
	al->addrLine[y] = 0;
	
	x++;
	y = 0;			
	while(str[x] != ';' && x < strlen(str))
	{
		if(y < 30)
		{
			al->city[y] = str[x];
			y++;
		}
		x++;
	}
	al->city[y] = 0;
	
	x++;
	y = 0;			
	while(str[x] != ';' && x < strlen(str))
	{
		if(y < 2)
		{
			al->state[y] = str[x];
			y++;
		}
		x++;
	}			
	al->state[y] = 0;
	
	x++;
	y = 0;			
	while(str[x] != ';' && x < strlen(str))
	{
		if(y < 10)
		{
			al->zip[y] = str[x];
			y++;
		}
		x++;
	}
	
	al->zip[y] = 0;
}

char charFromHex(char a)
{
	switch(a)
	{
		case '0':
			return 0;
			break;
		case '1':
			return 1;
			break;
		case '2':
			return 2;
			break;
		case '3':
			return 3;
			break;
		case '4':
			return 4;
			break;
		case '5':
			return 5;
			break;
		case '6':
			return 6;
			break;
		case '7':
			return 7;
			break;
		case '8':
			return 8;
			break;
		case '9':
			return 9;
			break;
		case 'a':
		case 'A':
			return 10;
			break;
		case 'b':
		case 'B':
			return 11;
			break;
		case 'c':
		case 'C':
			return 12;
			break;
		case 'd':
		case 'D':
			return 13;
			break;
		case 'e':
		case 'E':
			return 14;
			break;
		case 'f':
		case 'F':
			return 15;
			break;
	}
	
	return 0;
}

char safeChar(char a)
{
	if(a == 0x0D)
		return ' ';
	if(a == 0x0A)
		return ' ';
	
	return a;
}

void stripEncoding(char *str)
{
	uint16 x = 0;	
	
	while(str[x] != ':') // skip the header part
	{
		x++;
	}
	
	x++;	
	uint16 y = x;
	while(str[x] != 0)
	{
		if(str[x] == '=')
		{
			char a = str[++x];
			char b = str[++x];
			
			str[y] = safeChar((charFromHex(a) << 4) | charFromHex(b));
		}
		else
			str[y] = str[x];	
		x++;
		y++;
	}
	
	str[y] = 0;
}

void calculateListName(VCARD_FILE *vf, char *outStr)
{
	if(strlen(vf->lastName) == 0 && strlen(vf->firstName) == 0)
		sprintf(outStr, "(%s)", l_unknown);
	if(strlen(vf->lastName) == 0 && strlen(vf->firstName) > 0)
		strcpy(outStr, vf->firstName);
	if(strlen(vf->lastName) > 0 && strlen(vf->firstName) == 0)
		strcpy(outStr, vf->lastName);
	if(strlen(vf->lastName) > 0 && strlen(vf->firstName) > 0)
	{
		if(firstLast)
		{
			strcpy(outStr, vf->firstName);
			strcat(outStr, " ");
			strcat(outStr, vf->lastName);
		}
		else
		{
			strcpy(outStr, vf->lastName);
			strcat(outStr, ", ");
			strcat(outStr, vf->firstName);
		}
	}
}

void separateHeader(char *header, char *dataChunk)
{
	int x = 0;
	int y = 0;
	int copyMode = 0;
	
	while(dataChunk[x] != 0)
	{
		if(copyMode == 0)
		{
			if(dataChunk[x] == ':')
			{
				header[x] = 0;
				copyMode = 1;
			}
			else
			{
				header[x] = dataChunk[x];
			}
		}
		else
		{
			dataChunk[y] = dataChunk[x];
			y++;
		}
		
		x++;
	}
	
	dataChunk[y] = 0;
	strlwr(header);
	
	if(strchr(header, '.') != NULL)
	{
		char tmpStr[100];
		
		separateExtension(header,tmpStr);
		strcpy(header, tmpStr + 1);
	}	
}

void clearVCard(VCARD_FILE *vf)
{
	memset(vf, 0, sizeof(VCARD_FILE));
	
	vf->homeAddrPresent = false;
	vf->workAddrPresent = false;
}

void loadVCard(char *filename, VCARD_FILE *vf)
{
	char *str = (char *)safeMalloc(4096);
	char str2[257];
	
	clearVCard(vf);
	
	strcpy(vf->fileName, filename);
	
	DRAGON_FILE *fFile = DRAGON_fopen(filename, "r");
	DRAGON_detectUnicode(fFile);
	
	while(!DRAGON_feof(fFile))
	{
		memset(str, 0, 257);
		DRAGON_fgets(str, 256, fFile);			
		
		while(str[strlen(str)-1] == '=')
		{
			str[strlen(str)-1] = 0;
			memset(str2, 0, 257);
			DRAGON_fgets(str2, 256, fFile);
			strcat(str, str2);
		}
		
		char header[128];
		
		memset(header, 0, 128);		
		separateHeader(header, str);
		
		//---------------------
		// first and last name!
		//---------------------
		if(strcmp(header, "n") == 0) 
		{
			uint16 x = 0;
			uint16 y = 0;
			
			while(str[x] != ';')
			{
				if(y < 30)
				{
					vf->lastName[y] = str[x];
					y++;
				}
				x++;
			}
			
			vf->lastName[y] = 0;
			
			x++;
			y = 0;			
			while(str[x] != ';' && str[x] != 0)
			{
				if(y < 30)
				{
					vf->firstName[y] = str[x];
					y++;
				}
				x++;
			}
			
			vf->firstName[y] = 0;
		}
		
		//----------
		// nickname!
		//----------
		if(strcmp(header, "nickname") == 0) 
		{
			strncpy(vf->nickName, str, 80);
		}
		
		//-------
		// phone!
		//-------
		if(strncmp(header, "tel", 3) == 0)
		{
			switch(parseTokens(header, TOKEN))
			{
				case VCARD_HOME:
					strncpy(vf->homePhone, str, 14);
					break;
				case VCARD_CELL:
					strncpy(vf->cellPhone, str, 14);
					break;
				case VCARD_WORK:
					strncpy(vf->workPhone, str, 14);
					break;
			}
		}
		
		if(strncmp(header, "email", 5) == 0)
		{
			switch(parseTokens(header, TOKEN))
			{
				case VCARD_INTERNET:
					strncpy(vf->email, str, 60);
					break;
			}
		}
		
		//------------------
		// comment (encoded)
		//------------------
		if(strncmp(header, "note", 4) == 0)
		{
			if(parseTokens(header, ENCODING) == ENCODING_QUOTED_PRINTABLE)
				stripEncoding(str);
			
			strncpy(vf->comment, str, 100);			
		}
		
		//---------
		// address!
		//---------
		if(strncmp(header, "adr", 3) == 0)
		{
			if(parseTokens(header, ENCODING) == ENCODING_QUOTED_PRINTABLE)
				stripEncoding(str);
			
			switch(parseTokens(header + 4, TOKEN))
			{
				case VCARD_HOME:
					parseAddress(str, &(vf->homeAddr));
					vf->homeAddrPresent = true;
					break;
				case VCARD_WORK:
					parseAddress(str, &(vf->workAddr));
					vf->workAddrPresent = true;
			}
		}
		
		//----
		// end
		//----
		if(strcmp(header,"end") == 0)
		{
			strlwr(str);
			
			if(strcmp(str,"vcard") == 0)
			{
				DRAGON_fclose(fFile);
				safeFree(str);
				return;
			}
		}
	}
	
	DRAGON_fclose(fFile);
	safeFree(str);
}

void saveVCard(VCARD_FILE *vf, char *dir)
{
	char filename[MAX_TGDSFILENAME_LENGTH+1] = {0};
	char listName[MAX_TGDSFILENAME_LENGTH+1] = {0};
	
	DRAGON_chdir("/");
	calculateListName(vf, listName);
	DRAGON_chdir(dir);
	
	DRAGON_FILE *fFile = NULL;
	
	if(strlen(vf->fileName) == 0)
	{	// new vcard						
		
		sprintf(filename, "%s.vcf", listName);
		if(DRAGON_FileExists(filename) != FT_NONE)
			sprintf(filename, "%s%d%d%d%d%d.vcf", listName, getDay(), getMonth(), getYear(), getHour(true), getMinute());
		
		safeFileName(filename);
	}
	else
		strcpy(filename, vf->fileName);
		
	fFile = DRAGON_fopen(filename, "w");
	DRAGON_fputs("BEGIN:VCARD", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("VERSION:2.1", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	
	if(strlen(vf->firstName) > 0 || strlen(vf->lastName) > 0)
	{
		DRAGON_fputs("N:", fFile);
		DRAGON_fputs(vf->lastName, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->firstName, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->nickName) > 0)
	{
		DRAGON_fputs("NICKNAME:", fFile);
		DRAGON_fputs(vf->nickName, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->homePhone) > 0)
	{
		DRAGON_fputs("TEL;HOME;VOICE:", fFile);
		DRAGON_fputs(vf->homePhone, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->cellPhone) > 0)
	{
		DRAGON_fputs("TEL;CELL;VOICE:", fFile);
		DRAGON_fputs(vf->cellPhone, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->workPhone) > 0)
	{
		DRAGON_fputs("TEL;WORK;VOICE:", fFile);
		DRAGON_fputs(vf->workPhone, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->email) > 0)
	{
		DRAGON_fputs("EMAIL;PREF;INTERNET:", fFile);
		DRAGON_fputs(vf->email, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->comment) > 0)
	{
		DRAGON_fputs("NOTE:", fFile);
		DRAGON_fputs(vf->comment, fFile);			
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->homeAddr.addrLine) > 0 || strlen(vf->homeAddr.city) > 0 || strlen(vf->homeAddr.state) > 0 || strlen(vf->homeAddr.zip) > 0)
	{
		DRAGON_fputs("ADR;HOME:", fFile);		
		DRAGON_fputc(';', fFile);		
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->homeAddr.addrLine, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->homeAddr.city, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->homeAddr.state, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->homeAddr.zip, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	if(strlen(vf->workAddr.addrLine) > 0 || strlen(vf->workAddr.city) > 0 || strlen(vf->workAddr.state) > 0 || strlen(vf->workAddr.zip) > 0)
	{
		DRAGON_fputs("ADR;HOME:", fFile);		
		DRAGON_fputc(';', fFile);		
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->workAddr.addrLine, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->workAddr.city, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->workAddr.state, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputs(vf->workAddr.zip, fFile);			
		DRAGON_fputc(';', fFile);
		DRAGON_fputc(0x0D, fFile);
		DRAGON_fputc(0x0A, fFile);
	}
	
	char tStr[256];
	
	sprintf(tStr, "REV:%04d%02d%02dT%02d%02d%02dZ", getYear(), getMonth(), getDay(), getHour(true), getMinute(), getSecond());
	DRAGON_fputs(tStr, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	
	DRAGON_fputs("END:VCARD", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fclose(fFile);
	
	DRAGON_chdir("/");
	
	strcpy(vf->fileName, filename); // write back filename
	
	/*
		return;
	}
	
	// this code is fail :(
	
	// we can't just create a new card, or we'll erase all the data that DSOrganize doesn't support
	// what we have to do is read in the file, selectively replace with our data, and then write
	// back, thus preserving all the shit they had there before.
	
	// ... but we got to for now, uncommenting this causes a crash on the first read for some reason...
	
	char str[4096];
	char str2[257];
	
	char finalStr[10240];
	bool found = false;
	
	memset(finalStr, 0, 10240);	
	
	fFile = DRAGON_fopen(vf->fileName, "r");
	
	while(!DRAGON_feof(fFile))
	{
		DRAGON_fgets(str, 256, fFile);			
		
		while(str[strlen(str)-1] == '=')
		{
			DRAGON_fgets(str2, 256, fFile);		
			sprintf(str,"%s%c%c%s", str, 0x0D, 0x0A, str2);
		}
		
		found = false;
		
		if(strncmp(str, "N:", 2) == 0) // first and last name!
		{
			if(strlen(vf->firstName) > 0 || strlen(vf->lastName) > 0)
				sprintf(finalStr, "%sN:%s;%s%c%c", finalStr, vf->lastName, vf->firstName, 0x0D, 0X0A);
			found = true;
		}
		
		if(strncmp(str, "NICKNAME:", 9) == 0) // nickname!
		{
			if(strlen(vf->nickName) > 0)
				sprintf(finalStr, "%sNICKNAME:%s%c%c", finalStr, vf->nickName, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "TEL;HOME;VOICE:", 15) == 0) // homephone!
		{
			if(strlen(vf->homePhone) > 0)
				sprintf(finalStr, "%sTEL;HOME;VOICE:%s%c%c", finalStr, vf->homePhone, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "TEL;CELL;VOICE:", 15) == 0) // cellphone!
		{
			if(strlen(vf->cellPhone) > 0)
				sprintf(finalStr, "%sTEL;CELL;VOICE:%s%c%c", finalStr, vf->cellPhone, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "EMAIL;PREF;INTERNET:", 20) == 0) // email!
		{
			if(strlen(vf->email) > 0)
				sprintf(finalStr, "%sEMAIL;PREF;INTERNET:%s%c%c", finalStr, vf->email, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "NOTE;ENCODING=QUOTED-PRINTABLE:", 31) == 0) // comment!
		{
			if(strlen(vf->comment) > 0)
				sprintf(finalStr, "%sNOTE:%s%c%c", finalStr, vf->comment, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "NOTE:", 5) == 0) // comment!
		{
			if(strlen(vf->comment) > 0)
				sprintf(finalStr, "%sNOTE:%s%c%c", finalStr, vf->comment, 0x0D, 0X0A);	
			found = true;
		}
		
		if(strncmp(str, "ADR;HOME:", 9) == 0) // address!
		{
			if(strlen(vf->homeAddrLine) > 0 || strlen(vf->city) > 0 || strlen(vf->state) > 0 || strlen(vf->zip) > 0)
				sprintf(finalStr, "%sADR;HOME:;;%s;%s;%s;%s;%c%c", finalStr, vf->homeAddrLine, vf->city, vf->state, vf->zip, 0x0D, 0x0A);		
			found = true;
		}
		
		if(strncmp(str, "ADR;HOME;ENCODING=QUOTED-PRINTABLE:", 35) == 0) // address!
		{
			if(strlen(vf->homeAddrLine) > 0 || strlen(vf->city) > 0 || strlen(vf->state) > 0 || strlen(vf->zip) > 0)
				sprintf(finalStr, "%sADR;HOME:;;%s;%s;%s;%s;%c%c", finalStr, vf->homeAddrLine, vf->city, vf->state, vf->zip, 0x0D, 0x0A);		
			found = true;
		}
		
		if(strncmp(str, "REV:", 4) == 0) // revision date!
		{
			sprintf(finalStr, "%sREV:%04d%02d%02dT%02d%02d%02dZ", finalStr, getYear(), getMonth(), getDay(), getHour(true), getMinute(), getSecond());
			found = true;
		}
		
		if(found == false)
			sprintf(finalStr, "%s%s%c%c", finalStr, str, 0x0D, 0x0A);
	}
	
	DRAGON_fclose(fFile);
	
	fFile = DRAGON_fopen(vf->fileName, "w");
	DRAGON_fputs(finalStr, fFile);	
	DRAGON_fclose(fFile);		
	
	DRAGON_chdir("/");
	*/
}

bool checkMultiples(char *tmpFile)
{
	char *str = (char *)safeMalloc(4096);
	char str2[257];
	char ext[10];
	
	DRAGON_FILE *fFile = DRAGON_fopen(tmpFile, "r");
	DRAGON_detectUnicode(fFile);
	
	while(!DRAGON_feof(fFile))
	{
		memset(str, 0, 257);
		
		if(DRAGON_fgets(str, 256, fFile) != NULL)
		{		
			while(str[strlen(str)-1] == '=')
			{
				str[strlen(str)-1] = 0;
				memset(str2, 0, 257);
				DRAGON_fgets(str2, 256, fFile);
				strcat(str, str2);
			}
			
			if(strcmp(str,"END:VCARD") == 0)
			{
				if(DRAGON_feof(fFile)) // the file only has one entry in it
				{
					DRAGON_fclose(fFile);
					safeFree(str);
					return false;
				}
				
				u32 originalPos = DRAGON_ftell(fFile);
				
				strcpy(str, tmpFile);
				separateExtension(str, ext);
				
				if(strchr(str, '_') != NULL)
					strchr(str, '_')[0] = 0;
				
				int fNum = 2;
				bool found = false;
				
				while(!found)
				{
					sprintf(str2, "%s_%d%s", str, fNum, ext);  //in the format of originalfile_2.vcf, well keep trying until we find a good file
					if(DRAGON_FileExists(str2) == FT_NONE)
						found = true;
					else
						fNum++;
				}
				
				// copy over the rest of the contents, doesn't matter if it contains multiples because it'll be recursive
				
				DRAGON_FILE *newFile = DRAGON_fopen(str2, "w");
				char *contents = (char *)trackMalloc(100, "copy vcard stuff");
				memset(contents, 0, 100);
				
				while(1)
				{
					u32 readAmount = DRAGON_fread(contents, 1, 100, fFile);
					DRAGON_fwrite(contents, 1, readAmount, newFile);
					
					if(readAmount < 100)
						break;
				}
				trackFree(contents);
				DRAGON_fclose(newFile);
				
				// now copy out the stuff we want to save, delete and reopen the file, and write
				
				DRAGON_fseek(fFile, 0, 0);
				contents = (char *)trackMalloc(originalPos, "copy vcard stuff");
				DRAGON_fread(contents, 1, originalPos, fFile);
				DRAGON_fclose(fFile);
				DRAGON_remove(tmpFile);
				
				fFile = DRAGON_fopen(tmpFile, "w");
				DRAGON_fwrite(contents, 1, originalPos, fFile);
				DRAGON_fclose(fFile);
				
				safeFree(str);
				return true;
			}
		}
	}
	
	DRAGON_fclose(fFile);
	safeFree(str);
	return false;
}

bool isVCard(char *file)
{
	uint16 x = strlen(file);
	
	if(x < 3)
		return false;
		
	x--;
	
	if((file[x-3] == '.') && (file[x-2] == 'v' || file[x-2] == 'V') && (file[x-1] == 'c' || file[x-1] == 'C') && (file[x] == 'f' || file[x] == 'F'))
	{
		if(isValidVCard(file) == 0)
			return true;
	}
	
	return false;	
}

void separateMultiples(char *dir)
{
	char tmpFile[MAX_TGDSFILENAME_LENGTH+1] = {0};
	bool repeat = true;
	int fType;
	
	while(repeat)
	{
		repeat = false;
		DRAGON_chdir(dir);
		fType = DRAGON_FindFirstFile(tmpFile);
		while(fType != FT_NONE)
		{
			if(fType == FT_FILE)
			{
				DRAGON_preserveVars();
				
				if(isVCard(tmpFile))
					repeat |= checkMultiples(tmpFile);
				
				DRAGON_restoreVars();
			}
			
			fType = DRAGON_FindNextFile(tmpFile);		
		}
		
		DRAGON_closeFind();		
		DRAGON_chdir("/");
	}
}
