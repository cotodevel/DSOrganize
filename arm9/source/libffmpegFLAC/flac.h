#ifndef _FLAC_INCLUDED
#define _FLAC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "decoderflac.h"
#include "fatwrapper.h"

bool flac_init(DRAGON_FILE *df, FLACContext* fc);

#ifdef __cplusplus
}
#endif

#endif
