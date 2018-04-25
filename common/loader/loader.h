#ifndef loader_chishms
#define loader_chishms

typedef signed int addr_t;
typedef unsigned char data_t;

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void NdsLoaderCheck(void);
extern void boot_gba(void);
extern bool dldiPatchLoader(data_t *binData, u32 binSize, bool clearBSS);
extern bool bootndschishms(char* filename, bool dldipatch);
extern void bootndsCheck(void);

#ifdef __cplusplus
}
#endif
