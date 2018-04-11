void *trackMalloc(u32 length, char *desc);
void trackFree(void *tmp);
void *trackRealloc(void *tmp, u32 length);

void *safeMalloc(size_t size);
void safeFree(void *p);

void separateExtension(char *str, char *ext);
void safeFileName();
