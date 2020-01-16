#ifndef _JINJIAZHANG_VARINT_H_
#define _JINJIAZHANG_VARINT_H_

// https://en.wikipedia.org/wiki/UTF-8

inline int count_varint(int value)
{
    if (value < 0)
        return 0;
    else if (value <= 0x0000007F)
        return 1;
    else if (value <= 0x000007FF)
        return 2;
    else if (value <= 0x0000FFFF)
        return 3;
    else if (value <= 0x001FFFFF)
        return 4;
    else if (value <= 0x03FFFFFF)
        return 5;
    else if (value <= 0x7FFFFFFF)
        return 6;
    else
        return -1;
}

inline int encode_varint(char* data, int len, int value)
{
    int count = count_varint(value);
    if (count <= 0 || len < count)
        return -1;

    if (count == 1) {
        data[0] = (char)value;
        return count;
    }

    unsigned char c = 0x80;
    for (int idx = 0; idx < count; idx++) {
        data[count - idx - 1] = (value & 0x3F) | 0x80;
        c |= (c >> 1);
        value >>= 6;
    }
    data[0] |= (c << 1);
    return count;
}

inline int decode_varint(int* value, const char* data, int len)
{
    if (data == NULL || len <= 0)
        return -1;

    const unsigned char *s = (unsigned char *)data;
    unsigned int c = s[0];
    if (c < 0x80) {
        if (value) *value = c;
        return 1;
    }

    unsigned int res = 0;
    int count = 0;
    while (c & 0x40) {
        if (count + 1 >= len)
            return 0;
        int cc = s[++count];
        if ((cc & 0xC0) != 0x80)
            return -1;
        res = (res << 6) | (cc & 0x3F);
        c <<= 1;
    }

    res |= ((c & 0x7F) << (count * 5));
    if (value) *value = res;
    return count + 1;
}

#endif
