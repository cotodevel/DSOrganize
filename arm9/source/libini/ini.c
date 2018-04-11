#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <string.h>
#include "ini.h"
#include "fatwrapper.h"

static char iniPath[256];

void setIniFile(char *path)
{
	strncpy(iniPath,path, 255);
	iniPath[255] = 0;
}

bool getSetting(char *section, char *name, char *buffer)
{
	char str[256];
	char curHeader[256];
	bool found = false;
	int x = 0;
	
	DRAGON_FILE *fFile = DRAGON_fopen(iniPath, "r");
	
	curHeader[0] = 0;
	str[0] = 0;
	
	while(!found && !DRAGON_feof(fFile))
	{
		memset(str, 0, 256);
		DRAGON_fgets(str, 255, fFile);
		
		switch(str[0])
		{
			case '[':
				//section header
				x = 0;
				while(str[x + 1] != ']')
				{
					curHeader[x] = str[x + 1];
					
					x++;
				}
				
				curHeader[x] = 0;
				
				break;
			case 13: // skip nothingness
			case 10:
			case 0:
				break;
			case ';':
				//comment
				
				//no need to handle anything here
				
				break;
			default:
				//setting name
				
				if(strcmp(curHeader,section) == 0) // the headers are the same
				{
					char data[256];
					memset(data, 0, 256);
					
					x = 0;
					while(str[x] != '=' && str[x] != 0)
						x++;
					
					if(str[x] == 0)
						break;
					
					str[x] = 0;
					
					x++;
					uint16 y = 0;
					while(str[x] != 0)
					{
						data[y] = str[x];
						
						x++;
						y++;
					}
					
					data[y] = 0;
					
					if(strcmp(str,name) == 0) // we have a match!
					{
						strcpy(buffer,data);
						DRAGON_fclose(fFile);
						return true;
					}
				}
				
				break;
		}				
	}
	
	DRAGON_fclose(fFile);
	strcpy(buffer,"");
	return false;
}

bool getBoolSetting(char *section, char *name)
{
	char str[512];
	
	memset(str, 0, 512);
	getSetting(section, name, str);
	strlwr(str);
	
	if(strcmp(str,"true") == 0)
		return true;

	return false;
}

uint16 getColorSetting(char *section, char *name)
{
	char str[512];
	int x;
	uint16 num = 0;
	
	memset(str, 0, 512);
	getSetting(section, name, str);
	strlwr(str);
	
	if(strlen(str) == 0)
		return 0;
	
	for(x=0;x<4;x++)
	{
		if(str[x] == 0)
			break;
		
		num <<= 4;
		
		if(str[x] >= '0' && str[x] <= '9')
			num |= (str[x] - '0') & 0xF;
			
		if(str[x] >= 'a' && str[x] <= 'f')
			num |= (str[x] - 'a' + 10) & 0xF;
	}
	
	return num | (1<<15);
}

int getNumber(char *number)
{
	int x = 0;
	int num = 0;
	
	if(strlen(number) == 0)
		return -1;	
	
	while(number[x] != 0)
	{
		num *= 10;
		
		if(number[x] >= '0' && number[x] <= '9')
			num += number[x] - '0';
		
		x++;
	}
	
	return num;
}

int getNumberSetting(char *section, char *name)
{
	char str[512];
	
	memset(str, 0, 512);
	getSetting(section, name, str);
	strlwr(str);
	
	return getNumber(str);
}
