#ifndef _PICTURE_INCLUDED
#define _PICTURE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "gif_lib.h"

#define PIC_NONE 0
#define PIC_JPEG 1
#define PIC_PNG 2
#define PIC_BMP 3
#define PIC_BIN 4
#define PIC_GIF 5
#define PIC_ANI 6

#define START_PROGRESS 0
#define FINISH_PROGRESS 1
#define SET_MAX 2
#define SET_CUR 3
#define GET_CUR 4

#define IMAGE_TYPE_UNKNOWN 0
#define IMAGE_TYPE_RAW 1
#define IMAGE_TYPE_BMP 2
#define IMAGE_TYPE_GIF 3
#define IMAGE_TYPE_JPG 4
#define IMAGE_TYPE_PNG 5

// stuff for gif

#ifdef SYSV
#define VoidPtr char *
#else
#define VoidPtr void *
#endif /* SYSV */
typedef int GifWord;
typedef char GifByteType;

/*
typedef struct GifColorType {
	uint16 color;
} GifColorType;

typedef struct ColorMapObject {
    int ColorCount;
    int BitsPerPixel;
    GifColorType *Colors;    // on malloc(3) heap
} ColorMapObject;

typedef struct GifImageDesc {
    GifWord Left, Top, Width, Height,   // Current image dimensions. 
      Interlace;                    // Sequential/Interlaced lines. 
    ColorMapObject *ColorMap;       // The local color map 
} GifImageDesc;

typedef struct GifFileType {
    GifWord SWidth, SHeight,        // Screen dimensions. 
      SColorResolution,         // How many colors can we generate? 
      SBackGroundColor;         // I hope you understand this one... 
    ColorMapObject *SColorMap;  // NULL if not exists. 
    int ImageCount;             // Number of current image 
    GifImageDesc Image;         // Block describing current image 
    struct SavedImage *SavedImages; // Use this to accumulate file state 
    VoidPtr UserData;           // hook to attach user data (TVT) 
    VoidPtr Private;            // Don't mess with this!
} GifFileType;
*/

typedef struct
{
	void *data;
}	PICTURE_ARRAY;

typedef struct
{	
	int max_X;
	int max_Y;
	int p_X;
	int p_Y;
	double ratX;
	double ratY;
	
	int curAni;
	int oldAni;
	
	int thumb_X;
	int thumb_Y;
	
	bool error;
	bool isLarge;
	bool usingFile;
	void *rawData;
	uint16 *picData;	
	int picType;
	int adjust;	
	int aniCount;
	int *aniDelays;
	int aniDelay;
	char transColor;
	int clrCount;
	PICTURE_ARRAY *aniPointers;	
	GifFileType *streamFile;
	char fileName[256];
	char binName[256];
	uint16 bgColor;
	int curPA;
	void (*resetFunction)();
} PICTURE_DATA;

void loadImage(char *fName, PICTURE_DATA *pd, int thumbWidth, int thumbHeight);
void freeImage(PICTURE_DATA *pd);
void openGifStreaming(char *fName, PICTURE_DATA *pd);
void closeGifStreaming(PICTURE_DATA *pd);
void loadNextImage(PICTURE_DATA *pd);
void restartGifFile(PICTURE_DATA *pd);
void restartAnimation(PICTURE_DATA *pd);
void copyFileData(PICTURE_DATA *pd);
bool getPictureSize(char *fName, int *max_X, int *max_Y);
void initImage(PICTURE_DATA *pd, int thumbWidth, int thumbHeight);
bool saveImageFromHandle(PICTURE_DATA *pd);
bool saveImage(char *fName, void *rawData, int max_X, int max_Y);
bool createImage(PICTURE_DATA *pd, int max_X, int max_Y, uint16 fillColor);
void setBGColor(uint16 color, PICTURE_DATA *pd);
void enableGIFFileStreaming();
void disableGIFFileStreaming();
void setLargeDimensions(int x, int y);
int getTypeFromMagicID(char *fName);

#ifdef __cplusplus
}
#endif

#endif
