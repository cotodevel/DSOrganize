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
#include <dswifi9.h>
#include <stdlib.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#include "http.h"
#include "resources.h"
#include "general.h"
#include "wifi.h"
#include "cookies.h"

static bool proxyEnabled = false;
static URL_TYPE proxyServer;

bool isURL(char *url)
{
	char tmp[9];
	
	strncpy(tmp, url, 8);
	strlwr(tmp);
	
	if(strncmp(tmp,"http://", 7) == 0)
	{
		return true;
	}
	
	if(strncmp(tmp,"https://", 8) == 0)
	{
		return true;
	}
	
	return false;		
}

bool urlFromString(char *url, URL_TYPE *site)
{
	// reset the structure
	
	memset(site->username, 0, 30);
	memset(site->password, 0, 30);
	memset(site->server, 0, 60);
	destroyURL(site);
	
	site->serverIP = 0;
	site->serverPort = 0;
	
	if(!isURL(url)) // handle bad input
		return false;
	
	char *urlPointer = NULL;
	char tmpServer[256];
	char tmpProtocol[9];
	int i;
	
	memcpy(tmpProtocol, url, 8);
	tmpProtocol[8] = 0;
	strlwr(tmpProtocol);
	
	if(strncmp(tmpProtocol, "http://", 7) == 0)
	{
		urlPointer = url + 7; // skip 'http://'
		site->serverPort = 80; // default
		site->isSSL = false;
	}
	
	if(strncmp(tmpProtocol, "https://", 8) == 0)
	{
		urlPointer = url + 8; // skip 'https://'
		site->serverPort = 443; // default
		site->isSSL = true;
	}
	
	if(site->serverPort == 0) // unsupported protocol
		return false;
	
	char *tmp = strchr(urlPointer, '/');
	if(tmp != NULL)
	{
		i = tmp - urlPointer;
		
		strncpy(tmpServer, urlPointer, i);
		tmpServer[i] = 0;
		
		// allocate space and pop in the url
		site->remotePath = (char *)safeMalloc(strlen(tmp) + 1);
		site->remoteSentinal = site->remotePath;
		
		strcpy(site->remotePath, tmp);
	}
	else
	{
		// assume its in just name:port format		
		strcpy(tmpServer, urlPointer);
		
		site->remotePath = (char *)safeMalloc(2); // enough for the / and trailing null
		site->remoteSentinal = site->remotePath;
		
		strcpy(site->remotePath, "/");
	}	
	
	// find port
	
	for(i=strlen(tmpServer);i>0;i--)
	{
		if(tmpServer[i] == ':')
		{
			// we found our port
			
			site->serverPort = 0;
			
			for(int j=i+1;j<(int)strlen(tmpServer);j++)
			{
				site->serverPort *= 10;
				site->serverPort += (tmpServer[j] - '0');
			}
			
			tmpServer[i] = 0;
			break;
		}
	}	
	
	if(strchr(tmpServer, '@') != NULL)
	{
		// username and password
		
		strcpy(site->server, strchr(tmpServer, '@') + 1);
		
		(strchr(tmpServer, '@'))[0] = 0;
		
		if(strchr(tmpServer, ':') == NULL)
		{
			strcpy(site->username, tmpServer);
			return true;
		}
		else
		{
			strcpy(site->password, strchr(tmpServer, ':') + 1);
			(strchr(tmpServer, ':'))[0] = 0;
			strcpy(site->username, tmpServer);
			return true;
		}
	}
	else
	{
		// it's just the url left
		
		strcpy(site->server, tmpServer);
	}
	
	return true;
}

void destroyURL(URL_TYPE *site)
{
	// ensure we are freeing validly allocated data
	if(site->remotePath != 0 && site->remotePath == site->remoteSentinal)
	{
		free(site->remotePath);
	}
	
	site->remotePath = NULL;
	site->remoteSentinal = NULL;
}

void fillIPFromServer(URL_TYPE *site)
{
	struct hostent *tcp_he = gethostbyname(site->server);
	site->serverIP = *((unsigned long *)(tcp_he->h_addr_list[0]));
}

void stripHTTP(char *str)
{
	int x = 0;
	int y = 0;
	
	while(1)
	{
		if(str[x] == 0)
			return;
		
		if(str[x] == 13 && str[x+1] == 10 && str[x+2] == 13 && str[x+3] == 10)
		{
			y = x + 4;
			x = 0;
			break;
		}
		
		x++;
	}
	
	while(str[y] != 0)
	{
		str[x] = str[y];
		x++;
		y++;
	}
	
	str[x] = 0;
}

bool parseIcyHeader(char *header, ICY_HEADER *outFile)
{
	char icyHeader[18];
	
	memset(outFile, 0, sizeof(ICY_HEADER));
	
	strncpy(icyHeader, header, 12); // should be ICY 200 OK\r\n
	icyHeader[12] = 0;
	strlwr(icyHeader);
	
	if(strcmp(icyHeader, "icy 200 ok\r\n") != 0)
	{
		strncpy(icyHeader, header, 17); // should be HTTP/1.x 200 OK\r\n
		strlwr(icyHeader);
		icyHeader[17] = 0;
		icyHeader[7] = 'x';
		
		if(strcmp(icyHeader, "http/1.x 200 ok\r\n") != 0)
		{
			return false;
		}
	}
	
	int x = 12;
	int icyMode = 0;
	
	char icyTag[20];
	
	char *icyData = (char *)safeMalloc(ICY_PARSE_SIZE);
	
	memset(icyTag,0,20);
	
	while(true)
	{
		if(icyMode == 0) // searching for tag headers
		{
			if(header[x] == ':')
			{
				icyMode = 1;
				memset(icyData,0,ICY_PARSE_SIZE);
			}
			else
				icyTag[strlen(icyTag)] = header[x];
		}
		else
		{
			if(header[x] == '\r' || header[x] == 0)
			{
				// check for header and move the tag to the right location
				strlwr(icyTag);
				
				//if(strcmp(icyTag, "icy-notice1") == 0)
				//	strcpy(outFile->icyNotice1, icyData);
					
				//if(strcmp(icyTag, "icy-notice2") == 0)
				//	strcpy(outFile->icyNotice2, icyData);
					
				if(strcmp(icyTag, "icy-name") == 0)
					strcpy(outFile->icyName, icyData);
					
				if(strcmp(icyTag, "icy-genre") == 0)
					strcpy(outFile->icyGenre, icyData);
					
				if(strcmp(icyTag, "icy-url") == 0)
					strcpy(outFile->icyURL, icyData);
					
				if(strcmp(icyTag, "content-type") == 0)
				{
					strlwr(icyData);
					strcpy(outFile->icyMimeType, icyData);
				}
					
				if(strcmp(icyTag, "icy-pub") == 0)
				{
					if(icyData[0] == '1')
						outFile->icyPublic = true;
					else
						outFile->icyPublic = false;
				}
				
				if(strcmp(icyTag, "icy-br") == 0)
				{
					outFile->icyBitrate = 0;
					for(int i=0;i<(int)strlen(icyData);i++)
					{
						outFile->icyBitrate *= 10;
						outFile->icyBitrate += (icyData[i] - '0');
					}
				}
				
				if(strcmp(icyTag, "icy-metaint") == 0)
				{
					outFile->icyMetaInt = 0;
					for(int i=0;i<(int)strlen(icyData);i++)
					{
						outFile->icyMetaInt *= 10;
						outFile->icyMetaInt += (icyData[i] - '0');
					}
				}
			}
			else if(header[x] == '\n')
			{
				icyMode = 0;
				memset(icyTag,0,20);
			}
			else
			{
				if(icyMode == 1)
				{
					if(header[x] != 32)
						icyMode = 2;
				}
				
				if(icyMode != 1)
					icyData[strlen(icyData)] = header[x];
			}
		}
		
		if(header[x] == 0)
		{
			free(icyData);
			return true;
		}
		
		x++;
	}
	
	free(icyData);
	return true;
}

void setProxy(bool enabled, char *address)
{
	proxyEnabled = enabled;
	
	if(enabled)
	{
		char proxyAddr[256];
		
		strcpy(proxyAddr, "http://");
		strcat(proxyAddr, address);
		
		urlFromString(proxyAddr, &proxyServer);
		destroyURL(&proxyServer);
	}
}

int sendHTTPRequest(char *req)
{
	return sendHTTPRequestRaw(req, res_uAgent, NULL, NULL, false);
}

int sendHTTPRequestRaw(char *req, char *uAgent, char *postData, char *referData, bool allowCompression)
{
	int sock = 0;
	char *str = (char *)safeMalloc(strlen(req) + HTTP_HEADER_EXTRA);
	char *tType = NULL;
	URL_TYPE site;
	
	if(proxyEnabled)
	{
		if(proxyServer.serverIP == 0)
		{
			fillIPFromServer(&proxyServer);
		}
		
		urlFromString(req, &site);
		
		while(sock == 0)
		{
			sock = wifiConnectIP(proxyServer.serverIP, proxyServer.serverPort, site.isSSL);
		}
		
		if(postData)
			tType = "POST";
		else
			tType = "GET";
		
		// First populate and send the always required header stuff.
		sprintf(str, "%s %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n", tType, req, site.server, uAgent);
		sendData(sock, str, strlen(str));
		
		if(allowCompression)
		{
			// Signify that we allow compression
			strcpy(str, "Accept-Encoding: deflate, gzip\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		if(referData)
		{
			// Send the referrer if given.
			strcpy(str, "Referer: ");
			strcat(str, referData);
			strcat(str, "\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		// Send any cookies along.
		sendCookies(sock, site.server, req);
		
		if(postData)
		{
			// Send post data if given.
			char tSize[32];			
			sprintf(tSize, "%d", (int)strlen(postData));
			
			strcpy(str, "Content-Length: ");
			strcat(str, tSize);
			strcat(str, "\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");
			strcat(str, postData);
			strcat(str, "\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		// Send final CRLF
		strcpy(str, "\r\n");
		sendData(sock, str, strlen(str));
	}
	else
	{	
		urlFromString(req, &site);
		fillIPFromServer(&site);
		
		while(sock == 0)
		{
			sock = wifiConnectIP(site.serverIP, site.serverPort, site.isSSL);
		}
		
		if(postData)
			tType = "POST";
		else
			tType = "GET";
		
		// First populate and send the always required header stuff.
		sprintf(str, "%s %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n", tType, site.remotePath, site.server, uAgent);
		sendData(sock, str, strlen(str));
		
		if(allowCompression)
		{
			// Signify that we allow compression
			strcpy(str, "Accept-Encoding: deflate, gzip\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		if(referData)
		{
			// Send the referrer if given.
			strcpy(str, "Referer: ");
			strcat(str, referData);
			strcat(str, "\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		// Send any cookies along.
		sendCookies(sock, site.server, req);
		
		if(postData)
		{
			// Send post data if given.
			char tSize[32];			
			sprintf(tSize, "%d", (int)strlen(postData));
			
			strcpy(str, "Content-Length: ");
			strcat(str, tSize);
			strcat(str, "\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");
			strcat(str, postData);
			strcat(str, "\r\n");
			
			sendData(sock, str, strlen(str));
		}
		
		// Send final CRLF
		strcpy(str, "\r\n");
		sendData(sock, str, strlen(str));
	}
	
	free(str);
	destroyURL(&site);
	
	return sock;
}

int sendStreamRequest(u32 ip, unsigned short port, char *remotePath, bool useMeta)
{
	int s_socket = 0;
	char metaStr[64];
	
	if(useMeta)
		strcpy(metaStr, "Icy-MetaData:1\r\n");
	else
		strcpy(metaStr, "");
	
	if(proxyEnabled)
	{
		if(proxyServer.serverIP == 0)
			fillIPFromServer(&proxyServer);
		
		s_socket = 0;
		while(s_socket == 0)
		{
			s_socket = wifiConnectIP(proxyServer.serverIP, proxyServer.serverPort, false);
		}
		
		char connectStr[512];	
		sprintf(connectStr, "GET http://%d.%d.%d.%d:%d%s HTTP/1.0\r\nHost: %d.%d.%d.%d\r\nAccept: */*\r\nUser-Agent: %s\r\n%sConnection: close\r\n\r\n", (int)(ip&255), (int)((ip>>8)&255), (int)((ip>>16)&255), (int)((ip>>24)&255), port, remotePath, (int)(ip&255), (int)((ip>>8)&255), (int)((ip>>16)&255), (int)((ip>>24)&255), res_sAgent, metaStr);
		sendData(s_socket, connectStr, strlen(connectStr));
	}
	else
	{	
		s_socket = 0;
		while(s_socket == 0)
			s_socket = wifiConnectIP(ip, port, false);
		
		char connectStr[512];	
		sprintf(connectStr, "GET %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\n%sConnection: close\r\n\r\n", remotePath, res_sAgent, metaStr);
		sendData(s_socket, connectStr, strlen(connectStr));
	}
	
	return s_socket;
}

int getHTTPReplyCode(char *header)
{
	char httpHeader[13];
	
	strncpy(httpHeader, header, 12); // should be 'HTTP/1.x xxx'
	httpHeader[7] = 'x';
	httpHeader[12] = 0;
	strlwr(httpHeader);
	
	if(strncmp(httpHeader, "http/1.x ", 9) != 0)
		return -1;
	if(!isNumerical(httpHeader[9]) || !isNumerical(httpHeader[10]) || !isNumerical(httpHeader[11]))
		return -1;
	
	return ((httpHeader[9] - '0') * 100) + ((httpHeader[10] - '0') * 10) + (httpHeader[11] - '0');
}

bool getHTTPHeaderValue(char *header, char *search, char *out)
{
	char httpHeader[13];
	
	strncpy(httpHeader, header, 12); // should be 'HTTP/1.x xxx'
	httpHeader[7] = 'x';
	httpHeader[12] = 0;
	strlwr(httpHeader);
	
	if(strncmp(httpHeader, "http/1.x ", 9) != 0)
		return false;
	
	int x = 0;
	while(header[x] != '\r')
		x++;
	x+=2;

	int httpMode = 0;
	char httpTag[1024];
	char httpData[1024];
	memset(httpTag,0,20);
	memset(httpData,0,1024);
	strlwr(search);
	
	while(true)
	{
		if(httpMode == 0) // searching for tag headers
		{
			if(header[x] == ':')
			{
				httpMode = 1;
				memset(httpData,0,1024);
			}
			else
			{
				httpTag[strlen(httpTag)] = header[x];
			}
		}
		else
		{
			if(header[x] == '\r' || header[x] == 0)
			{
				// check for header and move the tag to the right location
				strlwr(httpTag);
				
				if(strcmp(httpTag, search) == 0)
				{					
					int tX = 0;
					
					while(httpData[tX] == ' ')
						tX++;
					
					strcpy(out, httpData + tX);
					return true;
				}				
			}
			else if(header[x] == '\n')
			{
				httpMode = 0;
				memset(httpTag,0,1024);
			}
			else
			{
				if(httpMode == 1)
				{
					if(header[x] != 32)
						httpMode = 2;
				}
				
				if(httpMode != 1)
					httpData[strlen(httpData)] = header[x];
			}
		}
		
		if(header[x] == 0)
			return false;
		
		x++;
	}

	return false;	
}

u32 getHTTPLength(char *header) // returns the length of the file, given the header
{
	char httpData[1024];
	memset(httpData, 0, 1024);
	
	if(getHTTPHeaderValue(header, "content-length", httpData))
	{
		u32 fileSize = 0;
		
		for(int i=0;i<(int)strlen(httpData);i++)
		{
			fileSize *= 10;
			fileSize += (httpData[i] - '0');
		}
		
		return fileSize;
	}
	
	return 0;
}

