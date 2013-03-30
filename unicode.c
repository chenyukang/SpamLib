
#include <stdio.h>

#define CheckSrcUTF8Length(start, offset, end)                          \
    do                                                                  \
    {                                                                   \
        if (start + offset >= end)                                      \
        {                                                               \
            fputs("Decoding UTF-8 to Unicode failed, Check the input using UTF-8 encoding!", \
                  stderr);                                              \
        }                                                               \
    }while (0)

#define CheckDescUTF8Length(start, offset, end)                         \
    do                                                                  \
    {                                                                   \
        if (start + offset >= end)                                      \
        {                                                               \
            fputs("Decoding Unicode to UTF-8 failed, Not enough UTF-8 buffer!", \
                  stderr);                                              \
        }                                                               \
    }while (0)

size_t UTF8toUnicode(char* utf8, size_t utf8length,
                           unsigned int* unicode, size_t unicodelength)
{
    size_t utf8_cursor = 0;
    size_t unicode_cursor = 0;
    size_t offset;
    unsigned int e;
    unsigned char* p;
    while ((utf8_cursor < utf8length) && (unicode_cursor < unicodelength))
    {
        p = (unsigned char*)&utf8[utf8_cursor];

        if (*p < 0xC0)
        {
            offset = 0;
            e = p[0];
        }
        else if (*p < 0xE0) 
        {
            /*2:<11000000>*/
            offset = 1;
            CheckSrcUTF8Length(utf8_cursor, offset, utf8length);

            e = (p[0] & 0x1f) << 6;
            e |= (p[1] & 0x3f);
        }
        else if (*p < 0xF0)
        {
            /*3:<11100000>*/
            offset = 2;
            CheckSrcUTF8Length(utf8_cursor, offset, utf8length);

            e = (p[0] & 0x0f) << 12;
            e |= (p[1] & 0x3f) << 6;
            e |= (p[2] & 0x3f);
        }
        else if (*p < 0xF8)
        {
            /*4:<11110000>*/
            offset = 3;
            CheckSrcUTF8Length(utf8_cursor, offset, utf8length);

            e = (p[0] & 0x07) << 18;
            e |= (p[1] & 0x3f) << 12;
            e |= (p[2] & 0x3f) << 6;
            e |= (p[3] & 0x3f);
        }
        else if (*p < 0xFC) 
        {
            /*5:<11111000>*/
            offset = 4;
            CheckSrcUTF8Length(utf8_cursor, offset, utf8length);

            e = (p[0] & 0x03) << 24;
            e |= (p[1] & 0x3f) << 18;
            e |= (p[2] & 0x3f) << 12;
            e |= (p[3] & 0x3f) << 6;
            e |= (p[4] & 0x3f);
        }
        else
        {
            /*6:<11111100>*/
            offset = 5;
            CheckSrcUTF8Length(utf8_cursor, offset, utf8length);

            e = (p[0] & 0x01) << 30;
            e |= (p[1] & 0x3f) << 24;
            e |= (p[2] & 0x3f) << 18;
            e |= (p[3] & 0x3f) << 12;
            e |= (p[4] & 0x3f) << 6;
            e |= (p[5] & 0x3f);
        }
        unicode[unicode_cursor] = e;

        unicode_cursor++;
        utf8_cursor = utf8_cursor + offset + 1;
    }

    return unicode_cursor;
}

size_t UnicodetoUTF8(unsigned int* unicode, size_t unicodelength,
                           char* utf8, size_t utf8length)
{
    size_t unicode_cursor = 0;
    size_t utf8_cursor = 0;
    size_t offset;
    size_t u;
    unsigned char* e;

    while ((unicode_cursor < unicodelength) && (utf8_cursor < utf8length))
    {
        u = unicode[unicode_cursor];
        e = (unsigned char*)&utf8[utf8_cursor];

        if (u < 0x80)
        {
            offset = 0;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = u;
        }
        else if (u < 0x800)
        {
            /*<11011111> < 000 0000 0000>*/
            offset = 1;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = ((u >> 6) & 0x1f)|0xc0;
            e[1] = (u & 0x3f)|0x80; 
        }
        else if(u < 0x10000)
        {
            /*<11101111> <0000 0000 0000 0000>*/
            offset = 2;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = ((u >> 12) & 0x0f)|0xe0; 
            e[1] = ((u >> 6) & 0x3f)|0x80;
            e[2] = (u & 0x3f)|0x80; 
        }
        else if(u < 0x200000)
        {
            /*<11110111> <0 0000 0000 0000 0000 0000>*/
            offset = 3;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = ((u >> 18) & 0x07)|0xf0; 
            e[1] = ((u >> 12) & 0x3f)|0x80;
            e[2] = ((u >> 6) & 0x3f)|0x80;
            e[3] = (u & 0x3f)|0x80; 
        }
        else if(u < 0x4000000)
        {
            /*<11111011> <00 0000 0000 0000 0000 0000 0000>*/
            offset = 4;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = ((u >> 24) & 0x03)|0xf8; 
            e[1] = ((u >> 18) & 0x3f)|0x80;
            e[2] = ((u >> 12) & 0x3f)|0x80;
            e[3] = ((u >> 6) & 0x3f)|0x80;
            e[4] = (u & 0x3f)|0x80; 
        }
        else
        {
            /*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/
            offset = 5;
            CheckDescUTF8Length(utf8_cursor, offset, utf8length);

            e[0] = ((u >> 30) & 0x01)|0xfc; 
            e[1] = ((u >> 24) & 0x3f)|0x80;
            e[2] = ((u >> 18) & 0x3f)|0x80;
            e[3] = ((u >> 12) & 0x3f)|0x80;
            e[4] = ((u >> 6) & 0x3f)|0x80;
            e[5] = (u & 0x3f)|0x80; 
        }
        
        unicode_cursor++;
        utf8_cursor = utf8_cursor + offset + 1;
    }

    return utf8_cursor;
}

