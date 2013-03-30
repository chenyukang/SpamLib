
#ifndef KINGUNICODE_h_INCLUDED
#define KINGUNICODE_h_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif

#include <string.h>
    size_t UTF8toUnicode(char* utf8, size_t utf8length,
                               unsigned int* unicode, size_t unicodelength);

    size_t UnicodetoUTF8(unsigned int* unicode, size_t unicodelength,
                               char* utf8, size_t utf8length);

#ifdef __cplusplus
}
#endif

#endif
