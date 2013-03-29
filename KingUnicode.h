
#ifndef KINGUNICODE_h_INCLUDED
#define KINGUNICODE_h_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif

    unsigned int UTF8toUnicode(char* utf8, unsigned int utf8length,
                               unsigned int* unicode, unsigned int unicodelength);

    unsigned int UnicodetoUTF8(unsigned int* unicode, unsigned int unicodelength,
                               char* utf8, unsigned int utf8length);

#ifdef __cplusplus
}
#endif

#endif
