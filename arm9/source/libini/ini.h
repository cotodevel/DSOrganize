#ifndef _PARSEINI_INCLUDED
#define _PARSEINI_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void setIniFile(char *path);
bool getSetting(char *section, char *name, char *buffer);
bool getBoolSetting(char *section, char *name);
uint16 getColorSetting(char *section, char *name);
int getNumberSetting(char *section, char *name);
int getNumber(char *number);

#ifdef __cplusplus
}	  
#endif

#endif
