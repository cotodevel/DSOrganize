#ifndef _PARSEVCARD_INCLUDED
#define _PARSEVCARD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define VCARD_UNKNOWN 0
#define VCARD_HOME 1
#define VCARD_WORK 2
#define VCARD_CELL 3
#define VCARD_INTERNET 4

#define ENCODING_QUOTED_PRINTABLE 100

#define TOKEN 0
#define ENCODING 1

typedef struct
{
	char addrLine[62];
	char city[32];
	char state[5];
	char zip[12];
} ADDRESS_LINE;

typedef struct
{
	char fileName[255];
	char firstName[32];
	char lastName[32];
	char nickName[82];
	char homePhone[16];
	char workPhone[16];
	char cellPhone[16];
	char email[62];
	char comment[102];
	
	ADDRESS_LINE homeAddr;
	bool homeAddrPresent;
	ADDRESS_LINE workAddr;
	bool workAddrPresent;
}	VCARD_FILE;

int isValidVCard(char *filename);
void loadVCard(char *filename, VCARD_FILE *vf);
void calculateListName(VCARD_FILE *vf, char *outStr);
void saveVCard(VCARD_FILE *vf, char *dir);
void clearVCard(VCARD_FILE *vf);
char charFromHex(char a);
void separateMultiples(char *dir);
bool isVCard(char *file);

#ifdef __cplusplus
}	  
#endif

#endif
