#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include "fatwrapper.h"
#include "libpicture.h"
#include "jpegdecoder.h"
#include "png.h"
#include "gif_lib.h"

static int largeX = 256;
static int largeY = 192;
static int InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
static int InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
static PICTURE_DATA *tmpPTR;

bool allowFile = true; // change this to allow or disallow gif streaming

void sendResetFunction(PICTURE_DATA *pd)
{
	if(pd->resetFunction != NULL)
		pd->resetFunction();
}
void restartAnimation(PICTURE_DATA *pd)
{
	pd->curAni = 0;
	pd->oldAni = -1;
	
	sendResetFunction(pd);	
}

void enableGIFFileStreaming()
{
	allowFile = true;
}


void disableGIFFileStreaming()
{
	allowFile = false;
}

void setLargeDimensions(int x, int y)
{
	largeX = x;
	largeY = y;
}

void sepExtension(char *str, char *ext)
{
	int x = 0;
	
	for(int y=0;y<(int)strlen(str);y++)
	{
		if(str[y] == '.')
			x = y;
	}
	
	if(x > 0)
	{
		int y = 0;
		while(str[x] != 0)
		{
			ext[y] = str[x];
			str[x] = 0;
			x++;
			y++;
		}
		ext[y] = 0;
	}
	else
		ext[0] = 0;	
}

void setBGColor(uint16 color, PICTURE_DATA *pd)
{
	pd->bgColor = color | (1<<15);
}

void initImage(PICTURE_DATA *pd, int thumbWidth, int thumbHeight)
{	
	pd->picData = NULL;	
	pd->rawData = NULL;	
	pd->aniDelays = NULL;
	pd->aniPointers = NULL;
	
	pd->thumb_X = thumbWidth;
	pd->thumb_Y = thumbHeight;
	
	pd->bgColor = 0xFFFF;
}

void freeImage(PICTURE_DATA *pd)
{	
	if(pd->usingFile)
		closeGifStreaming(pd);
	
	if(pd->picData != NULL)
		free(pd->picData);
	
	pd->picData = NULL;	
	
	if(pd->rawData != NULL)
		free(pd->rawData);
	
	pd->rawData = NULL;	
	
	if(pd->aniDelays != NULL)
		free(pd->aniDelays);
	
	pd->aniDelays = NULL;
	
	if(pd->aniPointers != NULL)
	{
		for(int i=0;i<pd->clrCount;i++)
		{
			if(pd->aniPointers[i].data != NULL)
			{
				free(pd->aniPointers[i].data);
			}
			
			pd->aniPointers[i].data = NULL;
		}
		
		free(pd->aniPointers);
	}
	
	pd->aniPointers = NULL;
}

void copyDecodeJPG(int i, int chan, int bypp, char *scanline, uint16 *pData, PICTURE_DATA *pd)
{
	switch(chan) 
	{
		case 1:
			for (int j=0;j<pd->max_X - pd->adjust;++j) 
			{
				unsigned int g = scanline[bypp*j] >> 3;
				
				pData[j + (i*pd->max_X)] = RGB15(g,g,g);
			}
			break;
		case 3: 
			for (int j=0;j<pd->max_X - pd->adjust;++j) {
				unsigned int r, g, b;
				
				b = scanline[bypp*j + 2] >> 3;
				g = scanline[bypp*j + 1] >> 3;
				r = scanline[bypp*j + 0] >> 3;
				
				pData[j + (i*pd->max_X)] = RGB15(r,g,b);		
			}
			break;
	}
}

void copyDecodePNG(int i, int colortype, int bits, char *scanline, uint16 *pData, PICTURE_DATA *pd)
{
	switch(colortype)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			// Grab background color
			u32 tr, tg, tb;			
			tb = ((pd->bgColor >> 10) & 31) << 3;
			tg = ((pd->bgColor >> 5) & 31) << 3;
			tr = (pd->bgColor & 31) << 3;
			
			for (int j=0;j<pd->max_X - pd->adjust;++j) 
			{
				// Grab transparency
				u32 t = scanline[(j << 2) + 3];
				
				// Grab pixels, multiply by transparency
				u32 r, g, b;				
				b = scanline[(j << 2) + 2] * t;
				g = scanline[(j << 2) + 1] * t;
				r = scanline[(j << 2) + 0] * t;
				
				// Figure out reverse transparency
				u32 mt = 256 - t;
				
				// Multiply by background color
				u32 cr, cg, cb;
				cr = tr * mt;
				cg = tg * mt;
				cb = tb * mt;
				
				// Add to the current
				r += cr;
				g += cg;
				b += cb;
				
				// Divide by 256 and shift right 3
				pData[j + (i*pd->max_X)] = RGB15((r >> 11),(g >> 11),(b >> 11));
			}
			
			break;
		}
		case PNG_COLOR_TYPE_RGB:
			for (int j=0;j<pd->max_X - pd->adjust;++j) 
			{
				// Grab pixels, multiply by transparency
				u32 r, g, b;				
				b = scanline[(j * 3) + 2];
				g = scanline[(j * 3) + 1];
				r = scanline[(j * 3) + 0];
				
				// Shift right 3
				pData[j + (i*pd->max_X)] = RGB15((r >> 3),(g >> 3),(b >> 3));
			}
			
			break;
	}
	
}

void colorset(void *dst, uint16 src, u32 size)
{
	src |= (1<<15);
	
	uint16 *t = (uint16 *)dst;
	
	for(u32 i=0;i<(size>>1);i++)
		t[i] = src;
}

void colorsetRange(void *dst, uint16 src, int iWidth, int iHeight, int top, int left, int width, int height)
{
	src |= (1<<15);
	
	uint16 *t = (uint16 *)dst;
	
	for(int y=0;y<iHeight;y++)
	{
		for(int x=0;x<iWidth;x++)
		{
			if(x >= left && x <= (left + width) && y >= top && y <= (top + height))
			{
				t[(y * iWidth) + x] = src;
			}
		}
	}
}

uint16 swapRB(uint16 color)
{
	char r = (color >> 10) & 31;
	char g = (color >> 5) & 31;
	char b = color & 31;
	
	return RGB15(r,g,b);
}

// done

void copyBMP(DRAGON_FILE *fp, uint bpp, uint16 *pallate, uint16 *buffer, PICTURE_DATA *pd)
{
	uint width = (pd->max_X - pd->adjust);
	uint height = pd->max_Y;
	uint count = 0;
	
	void *fileRead = NULL;
	
	uint length = (width + pd->adjust) * height;
	uint curX = 0;
	
	switch(bpp)
	{
		case 24:
			fileRead = malloc(15);
			break;
		case 16:
			fileRead = malloc(10);
		case 8:
			fileRead = malloc(5);
			break;
		case 4:
		case 1:
			fileRead = malloc(1);
			break;
	}

	char *fRead = (char *)fileRead;
	uint16 *fRead16 = (uint16 *)fileRead;
	
	while(count < length)
	{
		switch(bpp)
		{
			case 24:		
				if(curX < width-5)
				{
					DRAGON_fread(fileRead, 1, 15, fp);
					
					for(int i=0;i<5;i++)
					{
						char b = fRead[i*3 + 0];
						char g = fRead[i*3 + 1];
						char r = fRead[i*3 + 2];
						
						buffer[count] = RGB15((r>>3),(g>>3),(b>>3));
						count++;
						curX++;
					}
				}
				else
				{
					DRAGON_fread(fileRead, 1, 3, fp);
					
					char b = fRead[0];
					char g = fRead[1];
					char r = fRead[2];
					
					buffer[count] = RGB15((r>>3),(g>>3),(b>>3));
					count++;
					curX++;
				}				
				break;
			case 16:
				if(curX < width-5)
				{
					DRAGON_fread(fileRead, 1, 10, fp);
					
					for(int i=0;i<5;i++)
					{						
						buffer[count] = swapRB(*(fRead16 + i));
						count++;
						curX++;
					}
				}
				else
				{
					DRAGON_fread(fileRead, 1, 2, fp);
					
					buffer[count] = swapRB(*fRead16);
					
					count++;
					curX++;
				}				
				break;			
			case 8:	
				if(curX < width-5)
				{
					DRAGON_fread(fileRead, 1, 5, fp);
					
					for(int i=0;i<5;i++)
					{
						buffer[count] = pallate[*(fRead + i)];
						count++;
						curX++;
					}
				}
				else
				{
					DRAGON_fread(fileRead, 1, 1, fp);
					
					buffer[count] = pallate[*fRead];
					
					count++;
					curX++;
				}				
				break;
			case 4:
				DRAGON_fread(fileRead, 1, 1, fp);
				
				buffer[count] = pallate[(*fRead) >> 4];
				count++;
				curX++;
				
				if(curX < width)
				{
					buffer[count] = pallate[(*fRead) & 15];
					count++;					
					curX++;
				}	
				break;
			case 1:
				DRAGON_fread(fileRead, 1, 1, fp);
				
				for(int i=7;i>=0;i--)
				{
					buffer[count] = pallate[((*fRead) >> i) & 1];
					count++;
					curX++;
					
					if(curX >= width)
						break;
				}
				break;
		}
		
		if(curX == width)
		{
			count += pd->adjust;
			curX = 0;
			
			while((DRAGON_ftell(fp) % 4) != 2 && !DRAGON_feof(fp))
				DRAGON_fgetc(fp);
			
			if(DRAGON_feof(fp))
			{
				if(fileRead != NULL)
					free(fileRead);
				return;
			}
		}
	}
	
	if(fileRead != NULL)
		free(fileRead);
}

// done

void flipBMP(uint16 *buffer, uint width, uint height)
{
	uint numFlips = height / 2;
	uint16 *fBuffer = (uint16 *)malloc(width * 2);
	
	height--;
	
	for(uint x=0;x<numFlips;x++)
	{
		memcpy(fBuffer, buffer + (x * width), width * 2);
		memcpy(buffer + (x * width), buffer + ((height - x) * width), width * 2);
		memcpy(buffer + ((height - x) * width), fBuffer, width * 2);
	}	
	
	free(fBuffer);
}	

// done

void copyLineToRaw(uint16 *tempPtr, GifRowType ScreenBuffer, ColorMapObject *ColorMap, int tColor, PICTURE_DATA *pd)
{
	for(int i=0;i<pd->max_X - pd->adjust;i++)
	{
		if((int)ScreenBuffer[i] != tColor)
			tempPtr[i] = ColorMap->Colors[ScreenBuffer[i]].color;
	}
}

void createThumbnail(void *buffer, PICTURE_DATA *pd)
{
	if(buffer == NULL)
		return;
	
	int width = 0;
	int height = 0;
	
	int width2 = (int)((double)(pd->max_X * pd->thumb_Y) / (double)(pd->max_Y));
	int height2 = (int)((double)(pd->max_Y * pd->thumb_X) / (double)(pd->max_X));
	
	double curX = 0;
	double curY = 0;
		
	if(width2 > pd->thumb_X)
	{
		width = pd->thumb_X;
		height = height2;
	}
	else
	{
		width = width2;
		height = pd->thumb_Y;
	}
	
	// anything that tries to set a small enough thumbnail ends up getting 0 for one of these
	if(width == 0)
	{
		width++;
	}
	if(height == 0)
	{
		height++;
	}
	
	pd->ratX = (double)width / (double) pd->max_X;
	pd->ratY = (double)height / (double) pd->max_Y;
	
	pd->p_X = (largeX - width) / 2;
	pd->p_Y = (pd->thumb_Y - height) / 2;
	
	pd->picData = (uint16 *)malloc((width * height + 2) * 2);
	colorset(pd->picData, pd->bgColor, (width * height + 2) * 2);
	
	pd->picData[0] = width - 1;
	pd->picData[1] = height - 1;
	
	int lastX;
	int lastY = -1;
	
	for(int i=0;i<pd->max_Y;++i) 
	{
		uint16 *scanline = (uint16 *)buffer + (pd->max_X * i);
		
		if((int)curY == lastY)
			curY += pd->ratY;
		else
		{	
			lastY = (int)curY;
			
			curX = 0;
			lastX = -1;
			
			for (int j=0;j<pd->max_X - pd->adjust;++j) 
			{
				if((int)curX == lastX)
					curX += pd->ratX;
				else
				{
					lastX = (int)curX;
					
					// test just in case the gif is so small that calculations fail
					if(lastX < pd->thumb_X && lastY < pd->thumb_Y)
					{
						pd->picData[2 + lastX + (lastY * width)] = scanline[j];
					}
					
					curX += pd->ratX;	
				}
			}
			
			curY += pd->ratY;
		}
	}
}

// done

void openGifStreaming(PICTURE_DATA *pd)
{
	freeImage(pd);
	
	pd->usingFile = true;
	pd->error = false;
	
	pd->oldAni = -1;
	
	if((pd->streamFile = DGifOpenFileName(pd->fileName)) == NULL) 
	{
		pd->error = true;
		return;
	}	
	
	pd->max_X = pd->streamFile->SWidth;
	pd->max_Y = pd->streamFile->SHeight;
	
	pd->adjust = 0;
	if(pd->max_X & 1 == 1)
	{
		pd->max_X++;
		pd->adjust = 1;
	}
	
	pd->aniPointers = (PICTURE_ARRAY *)malloc((sizeof(pd->aniPointers)) * 3);	 // 3 rotating slots
	
	if(pd->aniPointers == NULL)
	{
		closeGifStreaming(pd);
		pd->error = true;
		return;
	}
	
	if(pd->max_X <= (largeX + pd->adjust) && pd->max_Y <= largeY)
	{
		pd->isLarge = false;		
		
		pd->aniPointers[0].data = malloc(pd->max_X*pd->max_Y*2 + 4);
		pd->aniPointers[1].data = malloc(pd->max_X*pd->max_Y*2 + 4);		
		pd->aniPointers[2].data = malloc(pd->max_X*pd->max_Y*2 + 4);
				
		if(pd->aniPointers[0].data == NULL || pd->aniPointers[1].data == NULL || pd->aniPointers[2].data == NULL)
		{
			closeGifStreaming(pd);
			pd->error = true;
			return;
		}
		
		colorset(pd->aniPointers[0].data, pd->bgColor, pd->max_X*pd->max_Y*2 + 4);
		colorset(pd->aniPointers[1].data, pd->bgColor, pd->max_X*pd->max_Y*2 + 4);
		colorset(pd->aniPointers[2].data, pd->bgColor, pd->max_X*pd->max_Y*2 + 4);
		
		((uint16 *)pd->aniPointers[0].data)[0] = pd->max_X - 1;
		((uint16 *)pd->aniPointers[0].data)[1] = pd->max_Y - 1;
		((uint16 *)pd->aniPointers[1].data)[0] = pd->max_X - 1;
		((uint16 *)pd->aniPointers[1].data)[1] = pd->max_Y - 1;
		((uint16 *)pd->aniPointers[2].data)[0] = pd->max_X - 1;
		((uint16 *)pd->aniPointers[2].data)[1] = pd->max_Y - 1;
	}
	else
	{
		pd->isLarge = true;
		pd->aniPointers[0].data	= malloc(pd->max_X*pd->max_Y*2);
		pd->aniPointers[1].data	= malloc(pd->max_X*pd->max_Y*2);
		pd->aniPointers[2].data	= malloc(pd->max_X*pd->max_Y*2);
		
		if(pd->aniPointers[0].data == NULL || pd->aniPointers[1].data == NULL || pd->aniPointers[2].data == NULL)
		{
			closeGifStreaming(pd);
			pd->error = true;
			return;
		}
		
		colorset(pd->aniPointers[0].data, pd->bgColor, pd->max_X*pd->max_Y*2);
		colorset(pd->aniPointers[1].data, pd->bgColor, pd->max_X*pd->max_Y*2);
		colorset(pd->aniPointers[2].data, pd->bgColor, pd->max_X*pd->max_Y*2);
	}		
	
	pd->picType = PIC_ANI;
	pd->curAni = 0;
	sendResetFunction(pd);
	pd->clrCount = 3;
	pd->curPA = -1;
	
	pd->rawData = NULL; // set this as such because there's already another pointer pointing to it
	
	loadNextImage(pd);
	
	if(pd->isLarge)
	{
		createThumbnail(pd->aniPointers[0].data, pd);
	}
	
	pd->oldAni = pd->curAni;
}

// done

void closeGifStreaming(PICTURE_DATA *pd)
{
	if(pd->streamFile != NULL)
		DGifCloseFile(pd->streamFile);
	
	pd->streamFile = NULL;
	pd->rawData = NULL;
	pd->usingFile = false;
}

// done

void restartGifFile(PICTURE_DATA *pd)
{
	if(pd->usingFile)
	{
		DGifCloseFile(pd->streamFile);
				
		pd->streamFile = NULL;
		
		if((pd->streamFile = DGifOpenFileName(pd->fileName)) == NULL) 
		{
			pd->error = true;
			return;
		}	
	}
}

// done

void loadNextImage(PICTURE_DATA *pd)
{		
	int copyOffset = 0;
	ColorMapObject *ColorMap = NULL;
	int ColorMapSize = 0;
	
	if(pd->isLarge)
		copyOffset = 0;
	else
		copyOffset = 2;
	
	GifRecordType RecordType;	
	uint offsetAmount = (pd->max_X*pd->max_Y) + copyOffset;
	int disposalMethod = 0;
	
	do {		
		GifByteType *Extension;			
		
		if(DGifGetRecordType(pd->streamFile, &RecordType) == GIF_ERROR) {
			closeGifStreaming(pd);
			pd->error = true;
			return;
		}
		
		switch (RecordType) {
			case IMAGE_DESC_RECORD_TYPE:
				if (DGifGetImageDesc(pd->streamFile) == GIF_ERROR) {
					closeGifStreaming(pd);
					pd->error = true;
					return;
				}
				
				ColorMap = (pd->streamFile->Image.ColorMap ? pd->streamFile->Image.ColorMap : pd->streamFile->SColorMap);
				ColorMapSize = ColorMap->ColorCount;
				
				pd->curPA++;
				
				if(pd->curPA == 3)
					pd->curPA = 0;
				
				pd->rawData = pd->aniPointers[pd->curPA].data;
				
				int Row, Col, w, h;
				
				Row = pd->streamFile->Image.Top;
				Col = pd->streamFile->Image.Left;
				w = pd->streamFile->Image.Width;
				h = pd->streamFile->Image.Height;
					
				switch(disposalMethod)
				{
					case 0: // do nothing
						colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
						break;
					case 1: // leave graphic on the screen
					case 2: // restore background color
					{
						int tmpPA = pd->curPA - 1;
						if(tmpPA < 0)
							tmpPA +=3;	
						
						memcpy(pd->rawData, pd->aniPointers[tmpPA].data, offsetAmount * 2);
						
						if(disposalMethod == 1)
						{
							break;
						}
						
						int tColor;
						
						if(pd->streamFile->SBackGroundColor != pd->transColor)
						{
							if(pd->streamFile->SBackGroundColor < 0 || pd->streamFile->SBackGroundColor > 255)
								tColor = pd->bgColor;
							else
								tColor = ColorMap->Colors[pd->streamFile->SBackGroundColor].color;
						}
						else
						{
							tColor = pd->bgColor;
						}
						
						colorsetRange((uint16 *)pd->rawData + copyOffset, tColor, pd->max_X, pd->max_Y, Row, Col, w, h); //colorset(pd->rawData, tColor, offsetAmount * 2);
						
						break;
					}
					case 3: // restore previous frame
					{
						int tmpPA2 = pd->curPA - 2;
						if(tmpPA2 < 0)
							tmpPA2 +=3;	
						
						memcpy(pd->rawData, pd->aniPointers[tmpPA2].data, offsetAmount * 2);
						break;
					}
					default: // undefined
						colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
						break;
				}	
				
				((uint16 *)pd->rawData)[0] = pd->max_X - 1;
				((uint16 *)pd->rawData)[1] = pd->max_Y - 1;
				
				if (pd->streamFile->Image.Left + pd->streamFile->Image.Width > pd->streamFile->SWidth || pd->streamFile->Image.Top + pd->streamFile->Image.Height > pd->streamFile->SHeight) 
				{
					closeGifStreaming(pd);
					pd->error = true;
					return;
				}
				
				GifRowType ScreenBuffer;
				
				ScreenBuffer = (GifRowType)malloc(pd->streamFile->SWidth * sizeof(GifPixelType));
				
				if (pd->streamFile->Image.Interlace) 
				{
					// Need to perform 4 passes on the images:
					for (int i = 0; i < 4; i++)
					{
						for (int j = Row + InterlacedOffset[i]; j < Row + h; j += InterlacedJumps[i]) 
						{
							memset(ScreenBuffer, pd->transColor, pd->streamFile->SWidth * sizeof(GifPixelType));
							if (DGifGetLine(pd->streamFile, ScreenBuffer, w) == GIF_ERROR) 
							{
								free(ScreenBuffer);
								closeGifStreaming(pd);
								pd->error = true;
								return;
							}
							else
							{
								uint16 *tmpBuffer = (uint16 *)pd->rawData;
								tmpBuffer += (j * (pd->max_X)) + copyOffset + Col;
								tmpBuffer += Row * pd->max_X;
								copyLineToRaw(tmpBuffer, ScreenBuffer, ColorMap, pd->transColor, pd);
							}
						}
					}
				}
				else 
				{
					for (int i = 0; i < h; i++) 
					{
						memset(ScreenBuffer, pd->transColor, pd->streamFile->SWidth * sizeof(GifPixelType));
						if(DGifGetLine(pd->streamFile, ScreenBuffer, w) == GIF_ERROR) 
						{
							free(ScreenBuffer);
							closeGifStreaming(pd);
							pd->error = true;
							return;
						}
						else
						{
							uint16 *tmpBuffer = (uint16 *)pd->rawData;
							tmpBuffer += (i * (pd->max_X)) + copyOffset + Col;
							tmpBuffer += Row * pd->max_X;
							copyLineToRaw(tmpBuffer, ScreenBuffer, ColorMap, pd->transColor, pd);
						}
					}
				}
				
				free(ScreenBuffer);
				
				return;
				
				break;
			case EXTENSION_RECORD_TYPE:
				int ExtCode;
				
				if(DGifGetExtension(pd->streamFile, &ExtCode, &Extension) == GIF_ERROR) 
				{
					closeGifStreaming(pd);
					pd->error = true;
					return;
				}
				while (Extension != NULL) 
				{
					if(ExtCode == GRAPHICS_EXT_FUNC_CODE)
					{
						disposalMethod = (Extension[1] >> 2) & 7;
						
						if(Extension[1] & 1 == 1) // transparency color exists
							pd->transColor = Extension[4];
						
						int tmpDelay = 0;
						double tmpNumber = 0;
						
						tmpDelay = Extension[2];
						tmpDelay += Extension[3] << 8;
						
						tmpNumber = (double)(tmpDelay) * (double)60;
						tmpNumber = tmpNumber / (double)100;
						
						tmpDelay = (int)tmpNumber;
						
						pd->aniDelay = tmpDelay;
					}
					if(DGifGetExtensionNext(pd->streamFile, &Extension) == GIF_ERROR) 
					{
						closeGifStreaming(pd);
						pd->error = true;
						return;
					}
				}
				break;
			case TERMINATE_RECORD_TYPE: // restart gif			
				restartGifFile(pd);
				
				break;
			default:
				break;
		}
	} while (1);
}

void copyFileData(PICTURE_DATA *pd)
{
	if(!pd->usingFile)
		return;
	
	if(pd->curAni != 1)
		return;
	
	pd->curAni = 0;
	
	loadNextImage(pd);
	pd->rawData = pd->aniPointers[pd->curPA].data;
}

int getTypeFromMagicID(char *fName)
{
	char tmpName[256];
	char ext[10];
	
	strcpy(tmpName, fName);
	
	sepExtension(tmpName, ext);
	strlwr(ext);
	
	// we test for this first as this is a special format
	if(strcmp(ext,".bin") == 0 || strcmp(ext,".pal") == 0 || strcmp(ext,".raw") == 0)
		return IMAGE_TYPE_RAW;
	
	DRAGON_FILE *df = DRAGON_fopen(fName, "r");
	
	if(DRAGON_flength(df) == 0)
	{
		DRAGON_fclose(df);
		return IMAGE_TYPE_UNKNOWN;
	}
	
	char magicNumber[9];
	DRAGON_fread(magicNumber, 1, 8, df);
	magicNumber[8] = 0;
	DRAGON_fclose(df);
	
	// try to identify by magic number
	if(strncmp(magicNumber, "GIF87a", 6) == 0 || strncmp(magicNumber, "GIF89a", 6) == 0)
		return IMAGE_TYPE_GIF;	
	
	if(magicNumber[0] == 0x42 && magicNumber[1] == 0x4D)
		return IMAGE_TYPE_BMP;
	
	if( magicNumber[0] == 0x89 && magicNumber[1] == 0x50 && \
		magicNumber[2] == 0x4E && magicNumber[3] == 0x47 && \
		magicNumber[4] == 0x0D && magicNumber[5] == 0x0A && \
		magicNumber[6] == 0x1A && magicNumber[7] == 0x0A)
		return IMAGE_TYPE_PNG;
	
	if(magicNumber[0] == 0xFF && magicNumber[1] == 0xD8)
		return IMAGE_TYPE_JPG;
	
	// if we are here, it failed to find one of them, so we fall back on extensions
	
	if(strcmp(ext,".bmp") == 0)
		return IMAGE_TYPE_BMP;
	
	if(strcmp(ext,".gif") == 0)
		return IMAGE_TYPE_GIF;
	
	if(strcmp(ext,".png") == 0)
		return IMAGE_TYPE_PNG;
	
	if(strcmp(ext,".jpg") == 0 || strcmp(ext,".jpeg") == 0)
		return IMAGE_TYPE_JPG;
	
	return IMAGE_TYPE_UNKNOWN;
}

void loadImage(char *fName, PICTURE_DATA *pd, int thumbWidth, int thumbHeight)
{
	freeImage(pd);
	
	if(DRAGON_FileExists(fName) != FE_FILE)
	{
		pd->error = true;
		pd->picType = PIC_NONE;
		
		return;
	}
	
	int imageType = getTypeFromMagicID(fName);
	
	if(imageType == IMAGE_TYPE_UNKNOWN)
	{
		pd->error = true;
		pd->picType = PIC_NONE;
		
		return;
	}
	
	strcpy(pd->fileName, fName);
	
	pd->adjust = 0;
	pd->error = false;
	pd->picType = PIC_NONE;
	pd->isLarge = false;
	pd->thumb_X = thumbWidth;
	pd->thumb_Y = thumbHeight;
	pd->transColor = 0xFF;
	tmpPTR = NULL;
	
	if(imageType == IMAGE_TYPE_JPG)
	{
		//jpg file!
		
		Pjpeg_decoder_file_stream Pinput_stream;
		Pjpeg_decoder jpgDecoder;
		
		strcpy(pd->binName, "");
		
		Pinput_stream = new jpeg_decoder_file_stream();
		
		if(Pinput_stream->open(fName))
		{
			delete Pinput_stream;
			pd->error = true;
			return;
		}
		
		jpgDecoder = new jpeg_decoder(Pinput_stream, false);
		
		if(jpgDecoder->get_error_code() != 0)
		{
			delete jpgDecoder;
			delete Pinput_stream;
			pd->error = true;
			return;
		}
		
		pd->max_X = jpgDecoder->get_width();
		pd->max_Y = jpgDecoder->get_height();
		
		if(pd->max_X <= 2 || pd->max_Y <= 2) // no pictures with 2 or less pixels either way
		{
			delete jpgDecoder;
			delete Pinput_stream;
			pd->error = true;
			return;
		}
		
		if(pd->max_X <= largeX && pd->max_Y <= largeY)
		{		
			pd->isLarge = false;
			pd->picType = PIC_JPEG;
			
			if(pd->max_X & 1 == 1)
			{
				pd->max_X++;
				pd->adjust = 1;
			}				
			
			pd->picData = (uint16 *)malloc((pd->max_X * pd->max_Y + 2) * 2);
			
			if(pd->picData == NULL)
			{
				delete jpgDecoder;
				delete Pinput_stream;
				freeImage(pd);
				pd->error = true;
				return;
			}
			
			pd->picData[0] = (uint16)(pd->max_X - 1);
			pd->picData[1] = (uint16)(pd->max_Y - 1);
			
			if(jpgDecoder->begin())
			{
				delete jpgDecoder;
				delete Pinput_stream;
				freeImage(pd);
				pd->error = true;
				return;
			}
			
			int chan = jpgDecoder->get_num_components();
			int bypp = jpgDecoder->get_bytes_per_pixel();	
			
			pd->adjust = 0;
			
			for(int i = 0;i < pd->max_Y; ++i) 
			{
				void *scanline;
				uint scanlen;
				
				if(jpgDecoder->decode((void **)&scanline, &scanlen))
				{
					delete jpgDecoder;
					delete Pinput_stream;
					freeImage(pd);
					pd->error = true;
					return;
				}
				
				copyDecodeJPG(i, chan, bypp, (char *)scanline, pd->picData + 2, pd);
			}
			
			delete jpgDecoder;
			delete Pinput_stream;
		}
		else
		{
			pd->picType = PIC_JPEG;
			pd->isLarge = true;
			
			// calculate preview size
			
			int width = 0;
			int height = 0;
			
			int width2 = (int)((double)(pd->max_X * pd->thumb_Y) / (double)(pd->max_Y));
            int height2 = (int)((double)(pd->max_Y * pd->thumb_X) / (double)(pd->max_X));
			
			double curX = 0;
			double curY = 0;
                
			if(width2 > pd->thumb_X)
			{
				width = pd->thumb_X;
				height = height2;
			}
			else
			{
				width = width2;
				height = pd->thumb_Y;
			}
			
			pd->ratX = (double)width / (double) pd->max_X;
			pd->ratY = (double)height / (double) pd->max_Y;
			
			pd->p_X = (largeX - width) / 2;
			pd->p_Y = (pd->thumb_Y - height) / 2;
			
			pd->picData = (uint16 *)malloc((width * height + 2) * 2);
			
			if(pd->picData == NULL)
			{
				delete jpgDecoder;
				delete Pinput_stream;
				freeImage(pd);
				pd->error = true;
				
				return;
			}
			
			colorset(pd->picData, pd->bgColor, (width * height + 2) * 2);
			
			// anything that tries to set a small enough thumbnail ends up getting 0 for one of these
			if(width == 0)
			{
				width++;
			}
			if(height == 0)
			{
				height++;
			}
			
			pd->picData[0] = width - 1;
			pd->picData[1] = height - 1;
			
			pd->adjust = 0;
			if(pd->max_X & 1 == 1)
			{
				pd->max_X++;
				pd->adjust = 1;
			}
			
			pd->rawData = malloc(pd->max_X * pd->max_Y * 2);
			
			if(pd->rawData == NULL)
			{
				delete jpgDecoder;
				delete Pinput_stream;
				freeImage(pd);
				pd->error = true;
				
				return;
			}
			
			colorset(pd->rawData, pd->bgColor, pd->max_X * pd->max_Y * 2);
			
			if (jpgDecoder->begin())
			{
				delete jpgDecoder;
				delete Pinput_stream;
				freeImage(pd);
				pd->error = true;
				return;
			}
			
			int chan = jpgDecoder->get_num_components();
			int bypp = jpgDecoder->get_bytes_per_pixel();	
			int lastX;
			int lastY = -1;
			
			for (int i=0;i<pd->max_Y; ++i) 
			{
				void *scanline = NULL;
				uint scanlen;
				
				if((int)curY == lastY)
				{
					if(jpgDecoder->decode((void **)&scanline, &scanlen))
					{
						delete jpgDecoder;
						delete Pinput_stream;
						freeImage(pd);
						pd->error = true;
						
						return;
					}
					copyDecodeJPG(i, chan, bypp, (char *)scanline, (uint16 *)pd->rawData, pd);
					curY += pd->ratY;
				}
				else
				{	
					if(jpgDecoder->decode((void **)&scanline, &scanlen))
					{
						delete jpgDecoder;
						delete Pinput_stream;
						freeImage(pd);
						pd->error = true;
						return;
					}
					lastY = (int)curY;
					switch(chan) 
					{
						case 1:
							curX = 0;
							lastX = -1;
							
							for (int j=0;j<pd->max_X - pd->adjust;++j) 
							{							
								unsigned int g = ((char *)scanline)[bypp*j] >> 3;
								
								((uint16 *)pd->rawData)[j + (i*pd->max_X)] = RGB15(g,g,g);
								
								if((int)curX == lastX)
									curX += pd->ratX;
								else
								{
									lastX = (int)curX;
									
									if(lastX < pd->thumb_X && lastY < pd->thumb_Y)
									{
										pd->picData[2 + lastX + (lastY * width)] = RGB15(g,g,g);									
									}
									
									curX += pd->ratX;									
								}
							}
							break;
						// trichrome, should be 3bypp
						case 3: 
							curX = 0;
							lastX = -1;
							
							for (int j=0;j<pd->max_X - pd->adjust;++j) 
							{
								unsigned int r, g, b;
								
								b = ((char *)scanline)[bypp*j + 2] >> 3;
								g = ((char *)scanline)[bypp*j + 1] >> 3;
								r = ((char *)scanline)[bypp*j + 0] >> 3;	
								
								((uint16 *)pd->rawData)[j + (i*pd->max_X)] = RGB15(r,g,b);	
								
								if((int)curX == lastX)
									curX += pd->ratX;
								else
								{
									lastX = (int)curX;
									pd->picData[2 + lastX + (lastY * width)] = RGB15(r,g,b);
									curX += pd->ratX;	
								}
							}
							break;
					}
					curY += pd->ratY;
				}
			}
			
			delete jpgDecoder;
			delete Pinput_stream;
		}
		
		return;
	}
	
	if(imageType == IMAGE_TYPE_PNG)
	{
		strcpy(pd->binName, "");
		tmpPTR = pd;
		
		pd->error = false;		
		pd->isLarge = false;
		
		pd->max_X = 0;
		pd->max_Y = 0;
		
		png_byte header[8];
		
		DRAGON_FILE *fp = DRAGON_fopen(fName, "rb");
		DRAGON_fread(header, 1, 8, fp);
		int is_png = !png_sig_cmp(header, 0, 8);
		if (!is_png)
		{
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		{
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		
		pd->max_X = info_ptr->width;
		pd->max_Y = info_ptr->height;
		
		if(pd->max_X <= 2 || pd->max_Y <= 2) // no pictures with 2 or less pixels either way
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		int color_type = info_ptr->color_type;
		int bit_depth = info_ptr->bit_depth;
		
		if(color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);
		
		if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
			png_set_gray_1_2_4_to_8(png_ptr);
		
		if (bit_depth == 16)
			png_set_strip_16(png_ptr);
		
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) 
			png_set_tRNS_to_alpha(png_ptr);
		
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		
		png_set_interlace_handling(png_ptr);
		png_read_update_info(png_ptr, info_ptr);
		
		color_type = info_ptr->color_type;
		bit_depth = info_ptr->bit_depth;
		
		pd->picType = PIC_PNG;		
		pd->rawData = malloc(info_ptr->rowbytes * pd->max_Y);
		
		if(pd->rawData == NULL)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * pd->max_Y);
		
		if(row_pointers == NULL)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		for(int y=0;y<pd->max_Y;y++)
		{
			row_pointers[y] = (png_byte *)pd->rawData + (info_ptr->rowbytes * y);
		}
		
		png_read_image(png_ptr, row_pointers);
		
		pd->adjust = 0;
		if(pd->max_X & 1 == 1)
		{
			pd->max_X++;
			pd->adjust = 1;
		}	
		
		pd->picData = (uint16 *)malloc((pd->max_X * pd->max_Y + 2) * 2);
		
		if(pd->picData == NULL)
		{				
			free(row_pointers);
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		if(pd->max_X <= largeX && pd->max_Y <= largeY)
		{		
			pd->isLarge = false;
			
			pd->picData[0] = (uint16)(pd->max_X - 1);
			pd->picData[1] = (uint16)(pd->max_Y - 1);
			
			for(int y=0;y<pd->max_Y;y++) 
			{
				copyDecodePNG(y, color_type, bit_depth, (char *)row_pointers[y], pd->picData + 2, pd);
			}
		}
		else
		{
			pd->isLarge = true;
			
			// Swap the pointers so that we can write using rawdata
			pd->rawData = pd->picData;
			pd->picData = NULL;
			
			for(int i=0;i<pd->max_Y; ++i) 
			{
				copyDecodePNG(i, color_type, bit_depth, (char *)row_pointers[i], (uint16 *)pd->rawData, pd);
			}
			
			createThumbnail(pd->rawData, pd);
		}
		
		free(row_pointers);		
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		
		DRAGON_fclose(fp);
		
		return;		
	}
	
	if(imageType == IMAGE_TYPE_BMP)
	{
		//bmp file!
		
		strcpy(pd->binName, "");
		
		uint16 *pallate = NULL;
		
		DRAGON_FILE *fp = DRAGON_fopen(fName, "rb");
		
		char h1 = DRAGON_fgetc(fp);
		char h2 = DRAGON_fgetc(fp);
		
		if(h1 != 0x42 || h2 != 0x4D)
		{
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		DRAGON_fseek(fp, 10, 0);
		uint offset = 0;
		DRAGON_fread(&offset, 4, 1, fp);
		
		DRAGON_fseek(fp, 18, 0);
		DRAGON_fread(&pd->max_X, 4, 1, fp);
		DRAGON_fread(&pd->max_Y, 4, 1, fp);
		
		if(pd->max_X <= 2 || pd->max_Y <= 2) // no pictures with 2 or less pixels either way
		{
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		DRAGON_fseek(fp, 28, 0);
		uint16 bpp = 0;
		DRAGON_fread(&bpp, 2, 1, fp);
		
		uint compression = 0;
		DRAGON_fread(&compression, 4, 1, fp);
		
		if(compression != 0)
		{
			DRAGON_fclose(fp);
			pd->error = true;
			return;
		}
		
		DRAGON_fseek(fp, 46, 0);
		uint nColors = 0;
		DRAGON_fread(&nColors, 4, 1, fp);
		
		pd->picType = PIC_BMP;
		
		DRAGON_fseek(fp, 54, 0); // start of image, or pallate if bpp != 24
		
		if(nColors == 0)
		{
			// colors isn't set yet we need an amount
			switch(bpp)
			{
				case 8:
					nColors = 256;
					break;
				case 4:
					nColors = 16;
					break;
				case 1:
					nColors = 2;
					break;
			}
		}
		
		if(bpp <= 8)
		{
			pallate = (uint16 *)malloc(nColors * 2);
			
			if(pallate == NULL)
			{
				DRAGON_fclose(fp);
				pd->error = true;
				return;
			}
			
			for(uint i=0;i<nColors;i++)
			{
				char b = DRAGON_fgetc(fp);
				char g = DRAGON_fgetc(fp);
				char r = DRAGON_fgetc(fp);
				
				DRAGON_fgetc(fp);
				
				pallate[i] = RGB15((r>>3),(g>>3),(b>>3));
			}
		}
		
		DRAGON_fseek(fp, offset, 0);		
		
		if(pd->max_X <= largeX & pd->max_Y <= largeY)
		{
			pd->isLarge = false;
			
			pd->adjust = 0;
			if(pd->max_X & 1 == 1)
			{
				pd->max_X++;
				pd->adjust = 1;
			}	
			
			pd->picData = (uint16 *)malloc((pd->max_X * pd->max_Y + 2) * 2);
			
			if(pd->picData == NULL)
			{
				if(pallate != NULL)
				{
					free(pallate);
				}
				
				DRAGON_fclose(fp);
				pd->error = true;
				return;
			}
			
			colorset(pd->picData, pd->bgColor, (pd->max_X * pd->max_Y + 2) * 2);
			
			pd->picData[0] = (uint16)(pd->max_X - 1);
			pd->picData[1] = (uint16)(pd->max_Y - 1);
			
			copyBMP(fp, bpp, pallate, pd->picData + 2, pd);
			flipBMP(pd->picData + 2, pd->max_X, pd->max_Y);
		}
		else
		{
			pd->isLarge = true;
			
			// calculate preview size
			
			int width = 0;
			int height = 0;
			
			int width2 = (int)((double)(pd->max_X * pd->thumb_Y) / (double)(pd->max_Y));
            int height2 = (int)((double)(pd->max_Y * pd->thumb_X) / (double)(pd->max_X));
			
			double curX = 0;
			double curY = 0;
                
			if(width2 > pd->thumb_X)
			{
				width = pd->thumb_X;
				height = height2;
			}
			else
			{
				width = width2;
				height = pd->thumb_Y;
			}
			
			pd->ratX = (double)width / (double) pd->max_X;
			pd->ratY = (double)height / (double) pd->max_Y;
			
			pd->p_X = (largeX - width) / 2;
			pd->p_Y = (pd->thumb_Y - height) / 2;
			
			pd->picData = (uint16 *)malloc((width * height + 2) * 2);
			
			if(pd->picData == NULL)
			{
				if(pallate != NULL)
				{
					free(pallate);
				}
				
				DRAGON_fclose(fp);
				pd->error = true;
				return;
			}
			
			colorset(pd->picData, pd->bgColor, (width * height + 2) * 2);
			
			// anything that tries to set a small enough thumbnail ends up getting 0 for one of these
			if(width == 0)
			{
				width++;
			}
			if(height == 0)
			{
				height++;
			}
			
			pd->picData[0] = width - 1;
			pd->picData[1] = height - 1;
			
			pd->adjust = 0;
			if(pd->max_X & 1 == 1)
			{
				pd->max_X++;
				pd->adjust = 1;
			}
			
			pd->rawData = malloc(pd->max_X * pd->max_Y * 2);
			colorset(pd->rawData, pd->bgColor, pd->max_X * pd->max_Y * 2);
			
			if(pd->rawData == NULL)
			{
				DRAGON_fclose(fp);
				pd->error = true;
				
				if(pallate != NULL)
					free(pallate);
				
				if(pd->picData != NULL)
					free(pd->picData);
				
				return;
			}
			
			copyBMP(fp, bpp, pallate, (uint16 *)pd->rawData, pd);
			flipBMP((uint16 *)pd->rawData, pd->max_X, pd->max_Y);
			
			int lastX;
			int lastY = -1;
			
			// create thumbnail
			for(int i=0;i<pd->max_Y;++i) 
			{
				uint16 *scanline = (uint16 *)pd->rawData + (pd->max_X * i);
				
				if((int)curY == lastY)
					curY += pd->ratY;
				else
				{	
					lastY = (int)curY;
					
					curX = 0;
					lastX = -1;
					
					for (int j=0;j<pd->max_X - pd->adjust;++j) 
					{
						if((int)curX == lastX)
							curX += pd->ratX;
						else
						{
							lastX = (int)curX;
							
							if(lastX < pd->thumb_X && lastY < pd->thumb_Y)
							{
								pd->picData[2 + lastX + (lastY * width)] = scanline[j];
							}
							curX += pd->ratX;	
						}
					}
					
					curY += pd->ratY;
				}
			}
		}
		
		DRAGON_fclose(fp);		
		
		if(pallate != NULL)
			free(pallate);
		
		return;
	}
	
	if(imageType == IMAGE_TYPE_RAW)
	{
		//bin file!
		
		bool loaded = false;
		
		DRAGON_FILE *fp = DRAGON_fopen(fName, "rb");
		int tLength = DRAGON_flength(fp);
		
		if(tLength == 98304) // 256x192 raw
		{
			// raw bin
			pd->max_X = 256;
			pd->max_Y = 192;
			
			pd->rawData = malloc(256*192*2 + 4);
			DRAGON_fread((uint16 *)(pd->rawData) + 2, 1, 256*192*2, fp);
			
			((uint16 *)(pd->rawData))[0] = 255;
			((uint16 *)(pd->rawData))[1] = 191;
			
			pd->picType = PIC_BIN;
			
			loaded = true;
			
			strcpy(pd->binName, "");
		}
		else if(tLength == 131072) // 256x256 raw
		{
			// raw bin
			pd->max_X = 256;
			pd->max_Y = 256;
			
			pd->rawData = malloc(256*256*2);
			DRAGON_fread(pd->rawData, 1, 256*256*2, fp);
			
			loaded = true;
			
			pd->picType = PIC_BIN;
			
			strcpy(pd->binName, "");
		}
		else
		{
			if(strlen(pd->binName) == 0) // its irregular so we don't want it
				pd->error = true;
			else // this is pal data		
			{
				uint16 *pallate = (uint16 *)malloc(tLength);
				DRAGON_fread(pallate, 2, tLength / 2, fp);
				DRAGON_fclose(fp);		
				
				if(pd->max_X <= largeX && pd->max_Y <= largeY) // handle small images
					pd->rawData = malloc(pd->max_X*pd->max_Y*2 + 4);
				else
					pd->rawData = malloc(pd->max_X*pd->max_Y*2);
				
				char *tempData = (char *)malloc(pd->max_X*pd->max_Y);
				
				DRAGON_FILE *fp = DRAGON_fopen(pd->binName, "rb");
				DRAGON_fread(tempData, 1, pd->max_X*pd->max_Y, fp);
				
				uint16 *bgData = (uint16 *)pd->rawData;
				
				if(pd->max_X <= largeX && pd->max_Y <= largeY)
				{
					bgData[0] = pd->max_X - 1;
					bgData[1] = pd->max_Y - 1;
					
					bgData +=2;
				}
				
				for(int i=0;i<pd->max_X*pd->max_Y;i++)
					bgData[i] = pallate[tempData[i]];
					
				free(pallate);
				free(tempData);
				
				pd->picType = PIC_BIN;
				
				loaded = true;
			}
		}
		
		DRAGON_fclose(fp);		
		
		if(loaded == true)
		{
			if(pd->max_X <= largeX && pd->max_Y <= largeY)
			{
				pd->isLarge = false;
				pd->picData = (uint16 *)(pd->rawData);
				pd->rawData = NULL;
			}
			else
			{
				pd->isLarge = true;
				
				createThumbnail(pd->rawData, pd);
			}
		}		
		
		return;
	}
	
	if(imageType == IMAGE_TYPE_GIF)
	{
		//gif file!
		
		GifFileType *GifFile;
		
		jmp_buf env;
		pd->usingFile = false;
		pd->oldAni = -1;
		
		if(setjmp(env))
		{
			if(allowFile)
			{
				openGifStreaming(pd);
				return;
			}
			else
			{
				pd->error = true;
				return;
			}
		}
		
		int copyOffset = 0;
		ColorMapObject *ColorMap = NULL;
		int ColorMapSize = 0;
		
		if((GifFile = DGifOpenFileName(fName)) == NULL) 
		{
			pd->error = true;
			return;
		}	
		
		pd->max_X = GifFile->SWidth;
		pd->max_Y = GifFile->SHeight;
		
		if(pd->max_X <= 2 || pd->max_Y <= 2) // no pictures with 2 or less pixels either way
		{
			DGifCloseFile(GifFile);
			pd->error = true;
			return;
		}
		
		pd->adjust = 0;
		if(pd->max_X & 1 == 1)
		{
			pd->max_X++;
			pd->adjust = 1;
		}
		
		if(pd->max_X <= (largeX + pd->adjust) && pd->max_Y <= largeY)
		{
			pd->isLarge = false;			
			pd->rawData = malloc(pd->max_X*pd->max_Y*2 + 4);
			
			if(!pd->rawData)
			{
				DGifCloseFile(GifFile);
				pd->error = true;
				return;
			}
			
			colorset(pd->rawData, pd->bgColor, pd->max_X*pd->max_Y*2 + 4);
			copyOffset = 2;
			
			((uint16 *)pd->rawData)[0] = pd->max_X - 1;
			((uint16 *)pd->rawData)[1] = pd->max_Y - 1;
		}
		else
		{
			pd->isLarge = true;			
			pd->rawData = malloc(pd->max_X*pd->max_Y*2);
			
			if(!pd->rawData)
			{
				DGifCloseFile(GifFile);
				pd->error = true;
				return;
			}
			
			colorset(pd->rawData, pd->bgColor, pd->max_X*pd->max_Y*2);
			copyOffset = 0;
		}
		
		if(pd->rawData == NULL)
		{
			DGifCloseFile(GifFile);
			pd->error = true;
			return;
		}	
		
		GifRecordType RecordType;
		
		pd->aniDelays = (int *)malloc(sizeof(int));
		
		if(!pd->aniDelays)
		{
			free(pd->rawData);
			DGifCloseFile(GifFile);
			pd->error = true;
			return;
		}
		
		uint offsetAmount = (pd->max_X*pd->max_Y) + copyOffset;
		pd->aniCount = 0;
		int disposalMethod = 0;
		
		do {		
			GifByteType *Extension;			
			
			if(DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
				DGifCloseFile(GifFile);
				pd->error = true;
				return;
			}
			
			switch (RecordType) {
				case IMAGE_DESC_RECORD_TYPE:
					if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
						DGifCloseFile(GifFile);
						pd->error = true;
						return;
					}
					
					ColorMap = (GifFile->Image.ColorMap ? GifFile->Image.ColorMap : GifFile->SColorMap);
					ColorMapSize = ColorMap->ColorCount;
					
					int Row, Col, w, h;
					
					Row = GifFile->Image.Top;
					Col = GifFile->Image.Left;
					w = GifFile->Image.Width;
					h = GifFile->Image.Height;
					
					if(pd->aniCount > 0)
					{
						pd->aniPointers = (PICTURE_ARRAY *)realloc(pd->aniPointers, (sizeof(pd->aniPointers)) * (pd->aniCount + 1));							
						
						if(pd->aniPointers == NULL) // check here for if we need file buffering
						{								
							DGifCloseFile(GifFile);								
							longjmp(env,2);
						}
						
						if(pd->aniCount == 1)
							pd->aniPointers[pd->aniCount - 1].data = pd->rawData; // set up initial copy
						
						pd->rawData = NULL;
						pd->rawData = malloc(2 * offsetAmount); 
						
						pd->aniPointers[pd->aniCount].data = pd->rawData;
						
						if(pd->rawData == NULL) // check here for if we need file buffering
						{								
							DGifCloseFile(GifFile);								
							longjmp(env,2);
						}
						
						switch(disposalMethod)
						{
							case 0: // do nothing
								colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
								break;
							case 1: // leave graphic on the screen
							case 2: // restore background color
							{
								int tmpPA = pd->aniCount - 1;
								
								if(tmpPA < 0)
								{
									colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
								}
								else
								{								
									memcpy(pd->rawData, pd->aniPointers[tmpPA].data, offsetAmount * 2);
								}
								
								if(disposalMethod == 1)
								{
									break;
								}
								
								int tColor;
								
								if(pd->streamFile->SBackGroundColor != pd->transColor)
								{
									if(pd->streamFile->SBackGroundColor < 0 || pd->streamFile->SBackGroundColor > 255)
										tColor = pd->bgColor;
									else
										tColor = ColorMap->Colors[pd->streamFile->SBackGroundColor].color;
								}
								else
								{
									tColor = pd->bgColor;
								}
								
								colorsetRange((uint16 *)pd->rawData + copyOffset, tColor, pd->max_X, pd->max_Y, Row, Col, w, h); //colorset(pd->rawData, tColor, offsetAmount * 2);
								
								break;
							}
							case 3: // restore previous frame
								// check if we are actually at frame 2 or more so we can copy
								if(pd->aniCount >= 2)
									memcpy(pd->rawData, pd->aniPointers[pd->aniCount - 2].data, offsetAmount * 2);
								else
									colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
								
								break;
							default: // undefine
								colorset(pd->rawData, pd->bgColor, offsetAmount * 2);
								break;
						}				
						
						if(!pd->isLarge)
						{
							uint16 *tmpPointer = (uint16 *)pd->rawData;
							tmpPointer[0] = pd->max_X - 1;
							tmpPointer[1] = pd->max_Y - 1;
						}
					}
					
					pd->aniCount++;
					pd->clrCount = pd->aniCount;	
					
					if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth || GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) 
					{
						DGifCloseFile(GifFile);
						pd->error = true;
						return;
					}
					
					GifRowType ScreenBuffer;
					
					ScreenBuffer = (GifRowType)malloc(GifFile->SWidth * sizeof(GifPixelType));
					
					if(ScreenBuffer == NULL) // check here for if we need file buffering
					{								
						DGifCloseFile(GifFile);								
						longjmp(env,2);
					}
					
					if (GifFile->Image.Interlace) 
					{	
						// Need to perform 4 passes on the images:
						for (int i = 0; i < 4; i++)
						{
							for (int j = Row + InterlacedOffset[i]; j < Row + h; j += InterlacedJumps[i]) 
							{
								memset(ScreenBuffer, pd->transColor, GifFile->SWidth * sizeof(GifPixelType));
								if (DGifGetLine(GifFile, ScreenBuffer, w) == GIF_ERROR) 
								{
									free(ScreenBuffer);
									DGifCloseFile(GifFile);
									pd->error = true;
									return;
								}
								else
								{
									uint16 *tmpBuffer = (uint16 *)pd->rawData;
									tmpBuffer += (j * (pd->max_X)) + copyOffset + Col;
									tmpBuffer += Row * pd->max_X;
									copyLineToRaw(tmpBuffer, ScreenBuffer, ColorMap, pd->transColor, pd);
								}
							}
						}
					}
					else 
					{	
						for (int i = 0; i < h; i++) 
						{
							memset(ScreenBuffer, pd->transColor, GifFile->SWidth * sizeof(GifPixelType));
							if(DGifGetLine(GifFile, ScreenBuffer, w) == GIF_ERROR) 
							{
								free(ScreenBuffer);
								DGifCloseFile(GifFile);
								pd->error = true;
								return;
							}
							else
							{
								uint16 *tmpBuffer = (uint16 *)pd->rawData;
								tmpBuffer += (i * (pd->max_X)) + copyOffset + Col;
								tmpBuffer += Row * pd->max_X;
								copyLineToRaw(tmpBuffer, ScreenBuffer, ColorMap, pd->transColor, pd);
							}
						}
					}
					
					free(ScreenBuffer);
					
					if(pd->aniCount == 1 && pd->isLarge)
						createThumbnail(pd->rawData, pd);
					
					break;
				case EXTENSION_RECORD_TYPE:
					int ExtCode;
					
					if(DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) 
					{
						DGifCloseFile(GifFile);
						pd->error = true;
						return;
					}
					while (Extension != NULL) 
					{
						if(ExtCode == GRAPHICS_EXT_FUNC_CODE)
						{
							disposalMethod = (Extension[1] >> 2) & 7;
							
							if(Extension[1] & 1 == 1) // transparency color exists
							{
								pd->transColor = Extension[4];
							}
							
							int tmpDelay = 0;
							double tmpNumber = 0;
							
							tmpDelay = Extension[2];
							tmpDelay += Extension[3] << 8;
							
							tmpNumber = (double)(tmpDelay) * (double)60;
							tmpNumber = tmpNumber / (double)100;
							
							tmpDelay = (int)tmpNumber;
							
							pd->aniDelays[pd->aniCount] = tmpDelay;
							
							pd->aniDelays = (int *)realloc(pd->aniDelays, sizeof(int) * (pd->aniCount + 2));
							
							if(pd->aniDelays == NULL) // check here for if we need file buffering
							{								
								DGifCloseFile(GifFile);								
								longjmp(env,2);
							}
						}
						if(DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) 
						{
							DGifCloseFile(GifFile);
							pd->error = true;
							return;
						}
					}
					break;
				case TERMINATE_RECORD_TYPE:
					break;
				default:
					break;
			}
		} while (RecordType != TERMINATE_RECORD_TYPE);
		
		DGifCloseFile(GifFile);		
		
		if(pd->aniCount == 1)
		{
			pd->picType = PIC_GIF;
			
			if(!pd->isLarge)
			{	
				pd->picData = (uint16 *)(pd->rawData);
				pd->rawData = NULL;
			}
		}
		else
		{
			pd->picType = PIC_ANI;
			pd->curAni = 0;
			sendResetFunction(pd);
			pd->rawData = NULL; // set this as such because there's already another pointer pointing to it
		}
		
		return;
	}
}

bool getPictureSize(char *fName, int *max_X, int *max_Y)
{
	int imageType = getTypeFromMagicID(fName);
	
	if(imageType == IMAGE_TYPE_UNKNOWN)
		return false;
	
	if(imageType == IMAGE_TYPE_JPG)
	{
		//jpg file!
		
		Pjpeg_decoder_file_stream Pinput_stream;
		Pjpeg_decoder jpgDecoder;
		
		Pinput_stream = new jpeg_decoder_file_stream();
		
		if(Pinput_stream->open(fName))
		{
			delete Pinput_stream;
			return false;
		}
		
		jpgDecoder = new jpeg_decoder(Pinput_stream, false);		
		
		if(jpgDecoder->get_error_code() != 0)
		{
			delete jpgDecoder;
			delete Pinput_stream;
			return false;
		}
		
		*max_X = jpgDecoder->get_width();
		*max_Y = jpgDecoder->get_height();
		
		delete jpgDecoder;
		delete Pinput_stream;
		
		return true;
	}
	
	if(imageType == IMAGE_TYPE_PNG)
	{
		png_byte header[8];
		
		DRAGON_FILE *fp = DRAGON_fopen(fName, "rb");
		DRAGON_fread(header, 1, 8, fp);
		int is_png = !png_sig_cmp(header, 0, 8);
		if (!is_png)
		{
			DRAGON_fclose(fp);
			return false;
		}
		
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		{
			DRAGON_fclose(fp);
			return false;
		}
	
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			DRAGON_fclose(fp);
			return false;
		}
	
		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			DRAGON_fclose(fp);
			return false;
		}
		
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		
		*max_X = info_ptr->width;
		*max_Y = info_ptr->height;	
		
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		
		DRAGON_fclose(fp);
		
		return true;
	}
	
	if(imageType == IMAGE_TYPE_BMP)
	{
		//bmp file!
		
		DRAGON_FILE *fp = DRAGON_fopen(fName, "rb");
		
		char h1 = DRAGON_fgetc(fp);
		char h2 = DRAGON_fgetc(fp);
		
		if(h1 != 0x42 || h2 != 0x4D)
		{
			DRAGON_fclose(fp);
			return false;
		}
		
		DRAGON_fseek(fp, 18, 0);
		DRAGON_fread(*&max_X, 4, 1, fp);	//ori: &max_X
		DRAGON_fread(*&max_Y, 4, 1, fp);	//ori: &max_Y
		
		DRAGON_fclose(fp);		
		
		return true;
	}
	
	if(imageType == IMAGE_TYPE_RAW)
	{
		return false;
	}
	
	if(imageType == IMAGE_TYPE_GIF)
	{
		//gif file!
		
		GifFileType *GifFile;
		
		if((GifFile = DGifOpenFileName(fName)) == NULL) 
		{
			return false;
		}	
		
		*max_X = GifFile->SWidth;
		*max_Y = GifFile->SHeight;
		
		DGifCloseFile(GifFile);
		
		return true;
	}
	
	return false;
}

bool createImage(PICTURE_DATA *pd, int max_X, int max_Y, uint16 fillColor)
{
	freeImage(pd);
	
	pd->max_X = max_X;
	pd->max_Y = max_Y;
	
	if(pd->max_X <= largeX && pd->max_Y <= largeY)
	{
		pd->adjust = 0;
		if(pd->max_X & 1 == 1)
		{
			pd->max_X++;
			pd->adjust = 1;
		}
			
		pd->picData = (uint16 *)malloc((pd->max_X * pd->max_Y + 2) * 2);
		if(pd->picData == NULL)
			return false;
		
		colorset(pd->picData, fillColor, (pd->max_X * pd->max_Y + 2) * 2);
		pd->picData[0] = max_X - 1;
		pd->picData[1] = max_Y - 1;
		
		pd->isLarge = false;
	}
	else
	{
		pd->adjust = 0;
		if(pd->max_X & 1 == 1)
		{
			pd->max_X++;
			pd->adjust = 1;
		}	
			
		pd->rawData = malloc(pd->max_X * pd->max_Y * 2);
		if(pd->rawData == NULL)
			return false;
		
		colorset(pd->rawData, fillColor, pd->max_X * pd->max_Y * 2);
		
		pd->isLarge = true;
	}
	
	return true;
}

bool saveImageFromHandle(PICTURE_DATA *pd)
{
	if(pd->picType == PIC_ANI)
		return false;
	
	if(pd->isLarge)
		return saveImage(pd->fileName, pd->rawData, pd->max_X, pd->max_Y);
	else
		return saveImage(pd->fileName, pd->picData + 2, pd->max_X, pd->max_Y);
}

bool saveImage(char *fName, void *rawData, int max_X, int max_Y)
{
	char tmpName[256];
	char ext[10];
	
	strcpy(tmpName, fName);
	
	sepExtension(tmpName, ext);
	strlwr(ext);
	
	if(strcmp(ext,".png") == 0)
	{
		DRAGON_FILE *fp;
		png_structp png_ptr;
		png_infop info_ptr;
		
		fp = DRAGON_fopen(fName, "w");
		if (fp == NULL)
			return false;
		
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		
		if (png_ptr == NULL)
		{
			DRAGON_fclose(fp);
			return false;
		}
		
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL)
		{
			DRAGON_fclose(fp);
			png_destroy_write_struct(&png_ptr, png_infopp_NULL);
			return false;
		}
		
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			DRAGON_fclose(fp);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return false;
		}
		
		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, max_X, max_Y, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_write_info(png_ptr, info_ptr);
		png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * max_Y);
		
		if(row_pointers == NULL)
		{
			png_destroy_write_struct(&png_ptr, &info_ptr);
			DRAGON_fclose(fp);
			return false;
		}
		
		void *rawSave = malloc(max_X * max_Y * 3);
		
		if(rawSave == NULL)
		{
			png_destroy_write_struct(&png_ptr, &info_ptr);
			DRAGON_fclose(fp);
			return false;
		}
		
		char *tmpSave = (char *)rawSave;
		
		for(int x=0;x<max_X*max_Y;x++)
		{
			uint16 color = ((uint16 *)rawData)[x];
			
			char r = color & 31;
			char g = (color >> 5) & 31;
			char b = (color >> 10) & 31;
			
			tmpSave[x*3] = r << 3;
			tmpSave[x*3 + 1] = g << 3;
			tmpSave[x*3 + 2] = b << 3;
		}
		
		for(int y=0;y<max_Y;y++)
			row_pointers[y] = (png_byte *)rawSave + (max_X * 3 * y);
			
		png_write_image(png_ptr, row_pointers);
		
		free(row_pointers);
		free(rawSave);
		
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		DRAGON_fclose(fp);
		
		return true;
	}	
	
	if(strcmp(ext,".bmp") == 0)
	{
		if(DRAGON_FileExists(fName) == FE_FILE)
			DRAGON_remove(fName);
		
		uint tData = 0;
		uint16 tData16 = 0;
		DRAGON_FILE *fp = DRAGON_fopen(fName, "w");
		
		// signature
		DRAGON_fputc(0x42, fp);
		DRAGON_fputc(0x4D, fp);
		
		// size of bmp file
		tData = (max_X * max_Y * 2) + 54;
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		// reserved
		DRAGON_fputc(0, fp);
		DRAGON_fputc(0, fp);
		DRAGON_fputc(0, fp);
		DRAGON_fputc(0, fp);
		
		// offset
		tData = 54;
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		// size of BITMAPINFOHEADER
		tData = 40;
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		// dimensions
		DRAGON_fwrite(&max_X, 4, 1, fp);
		DRAGON_fwrite(&max_Y, 4, 1, fp);
		
		// planes in bmp
		tData16 = 1;
		DRAGON_fwrite(&tData16, 2, 1, fp);
		
		// bit depth, always 16 in this case
		tData16 = 16;
		DRAGON_fwrite(&tData16, 2, 1, fp);
		
		// compression
		tData = 0;
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		// size of data in bytes
		tData = max_X * max_Y * 2;
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		// unimportant data
		tData = 0;
		DRAGON_fwrite(&tData, 4, 1, fp);
		DRAGON_fwrite(&tData, 4, 1, fp);
		DRAGON_fwrite(&tData, 4, 1, fp);
		DRAGON_fwrite(&tData, 4, 1, fp);
		
		for(int i=0;i<max_X*max_Y;i++)
		{
			((uint16 *)rawData)[i] = swapRB(((uint16 *)rawData)[i]);
		}	
		
		flipBMP((uint16 *)rawData, max_X, max_Y);
		
		for(int i=0;i<max_Y;i++)
		{
			DRAGON_fwrite(((uint16 *)rawData) + (max_X * i), 2, max_X, fp); // write line
			
			// check for padding
			
			switch((max_X << 1) & 3) // check for padding to 4, each pixel is two bytes wide
			{
				case 0:
					// multiple of 4, we are good
					break;
				case 2:
					// we need 2 more bytes
					DRAGON_fputc(0, fp);
					DRAGON_fputc(0, fp);
					break;					
			}
		}	
		
		DRAGON_fclose(fp);
		
		return true;
	}
	
	return false;
}
