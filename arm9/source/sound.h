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
 
#ifndef _SOUND_INCLUDED
#define _SOUND_INCLUDED

#include "fatwrapper.h"
#include "http.h"
#include "aacdec.h"
#include "api68.h"
#include "id3.h"
#include "mikmod_internals.h"
#include "mad.h"
#include "mp4ff.h"
#include "flac.h"

enum {
	SRC_NONE,
	SRC_WAV,
	SRC_MIKMOD,
	SRC_MP3,
	SRC_OGG,
	SRC_AAC,
	SRC_FLAC,
	SRC_SID,
	SRC_NSF,
	SRC_SPC,
	SRC_SNDH,
	SRC_STREAM_MP3,
	SRC_STREAM_OGG,
	SRC_STREAM_AAC
};

#define ARM9COPY 0
#define ARM7COPY 1

#define STATE_PLAYING 0
#define STATE_PAUSED 1
#define STATE_STOPPED 2
#define STATE_UNLOADED 3

// 2048 creates the crash bug in fat_fread
#define WAV_READ_SIZE 4096

#define MP3_READ_SIZE 8192
#define MP3_WRITE_SIZE 2048
#define STREAM_MP3_READ_SIZE 1024
#define STREAM_BUFFER_SIZE (1024*512) // 512 kb buffer
#define STREAM_WIFI_READ_SIZE (1024*20) // 20 kb recieve
#define STREAM_CACHE_SIZE (1024*64) // 64 kb cache size

#define OGG_READ_SIZE 1024

#define MIKMOD_FREQ 44100

#define AAC_READBUF_SIZE	(2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)
#define AAC_OUT_SIZE 32768
#ifdef AAC_ENABLE_SBR
  #define SBR_MUL		2
#else
  #define SBR_MUL		1
#endif

#define FLAC_OUT_SIZE 4096

#define SID_FREQ 48000
#define SID_OUT_SIZE 2048
#define SID_META_LOC 0x16

#define NSF_FREQ 48000
#define NSF_OUT_SIZE 2048

#define SPC_FREQ 32000
#define SPC_OUT_SIZE 2048

#define SNDH_OUT_SIZE 4096

// streaming stuff
#define STREAM_DISCONNECTED 0
#define STREAM_CONNECTING 1
#define STREAM_CONNECTED 2
#define STREAM_FAILEDCONNECT 3
#define STREAM_STREAMING 4
#define STREAM_STARTING 5
#define STREAM_BUFFERING 6
#define STREAM_TRYNEXT 7

#define ICY_HEADER_SIZE 2048

#define REG_SIWRAMCNT (*(vu8*)0x04000247)
#define SIWRAM0 ((s16 *)0x037F8000)
#define SIWRAM1 ((s16 *)0x037FC000)

// mikmod
#include "drv_nos.h"

typedef struct 
{
	char chunkID[4];
	long chunkSize;

	short wFormatTag;
	unsigned short wChannels;
	unsigned long dwSamplesPerSec;
	unsigned long dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
} wavFormatChunk;

typedef struct
{
	int sourceFmt;
	int bufLoc;
	int channels;
	DRAGON_FILE *filePointer;
	int bits;
	u32 len;
	u32 loc;
	u32 dataOffset;
	u32 dataLen;
	int mp3SampleRate;
} sndData;

#endif




#ifdef __cplusplus
extern "C" {
#endif

void initComplexSound();
void waitForInit();
void setSoundInterrupt();

// playback
bool loadSound(char *fName);
void updateStreamLoop();
void loadSoundGeneric(u32 wPlugin, int rate, int multiplicity, int sampleLength);
void pauseSound(bool pause);
void getSoundLoc(u32 *loc, u32 *max);
void setSoundLoc(u32 loc);
void closeSound();
void freeSound(); // should not be called unless absolutely necessary
int getState();
int getSoundLength();
int getSourceFmt();
void soundPrevTrack();
void soundNextTrack();
int getSNDHTrack();
int getSNDHTotalTracks();
void getSNDHMeta(api68_music_info_t * info);
void loadWavToMemory();

// wifi
void SendArm7Command(u32 command, u32 data);
int getCurrentStatus();
ICY_HEADER *getStreamData();

// for streaming record interrupt
void copyChunk();
void setSoundFrequency(u32 freq);
void setSoundLength(u32 len);

u32 getSoundChannels();
u8 getVolume();
void setVolume(u8 volume);
int getStreamLag();
int getStreamLead();
char *sidMeta(int which);

// for about screen to set or clear the ability to loop modules
void setLoop();
void clearLoop();

//extern bool canSend;
extern sndData soundData;
extern bool cutOff;
extern bool sndPaused;
extern bool playing;
extern bool updateRequested;
extern bool seekSpecial;
extern int sndLen;
extern int seekUpdate;
extern void updateStream();

// sound out
extern s16 *lBuffer;
extern s16 *rBuffer;

// wav
extern bool memoryLoad;
extern char *memoryContents;
extern u32 memoryPos;
extern u32 memorySize;
	
// mikmod
extern MODULE *module;
extern bool madFinished;
extern int sCursor;
extern bool allowEQ;

// mp3
extern struct mad_stream Stream;
extern struct mad_frame Frame;
extern struct mad_synth Synth;
extern mad_timer_t Timer;
extern unsigned char *mp3Buf;
extern int bufCursor;
extern int bytesLeft;
extern s16 *bytesLeftBuf;
extern int maxBytes;

// ogg
extern OggVorbis_File vf;
extern int current_section;

// streaming
extern URL_TYPE curSite;
extern char *tmpMeta;
extern int streamMode;
extern int s_socket;
extern int s_cursor;
extern int lagCursor;
extern char *s_buffer;
extern int s_retries;
extern int s_count;
extern int s_metaCount;
extern int icyCopy;
extern size_t oggStreamLoc;
extern ICY_HEADER curIcy;
extern bool streamOpened;
extern int tmpAmount;
extern int recAmount;

// aac
extern HAACDecoder *hAACDecoder;
extern unsigned char *aacReadBuf;
extern unsigned char *aacReadPtr;
extern s16 *aacOutBuf;
extern AACFrameInfo aacFrameInfo;
extern int aacBytesLeft, aacRead, aacErr, aacEofReached;
extern int aacLength;
extern bool isRawAAC;
extern mp4ff_t *mp4file;
extern mp4ff_callback_t mp4cb;
extern int mp4track;
extern int sampleId;

//flac
extern FLACContext fc;
extern uint8_t *flacInBuf;
extern int flacBLeft;
extern int32_t *decoded0;
extern int32_t *decoded1;
extern bool flacFinished;

//sid
extern char *sidfile;
extern u32 sidLength;
extern unsigned short sid_load_addr, sid_init_addr, sid_play_addr;
extern unsigned char sid_subSongsMax, sid_subSong, sid_song_speed;
extern int nSamplesRendered, nSamplesPerCall, nSamplesToRender;

//nsf
extern uint8_t *nsffile;
extern u32 nsfLength;
extern volatile bool inTrack;
extern volatile bool isSwitching;

//spc
extern uint8_t *spcfile;
extern u32 spcLength;

//sndh
extern uint8_t *sndhfile;
extern u32 sndhLength;
extern api68_init_t init68;
extern api68_t * sc68;
extern int sndhTracks;

// alternate safemalloc stuff
extern int m_SIWRAM;
extern int m_size;

#ifdef __cplusplus
}
#endif
