#ifndef _JINJIAZHANG_VARINT_H_
#define _JINJIAZHANG_VARINT_H_

inline int encode_varint(char* data, int len, int var)
{
    if (len <= 0)
        return -1;

    if (var >= 0 && var < 0x80) {
        data[0] = (char)var;
        return 1;
    }

    unsigned int res = (unsigned int)var;
    int count = 0;
    while (res > 0x02) {
        res >>= 5;
        count++;
    }

    res = (unsigned int)var;
    unsigned char *s = (unsigned char *)data;
    unsigned char c = 0x80;
    for (int idx = 0; idx < count; idx++) {
        s[count - idx] = ((res & 0x3F) | 0x80);
        c |= (c >> 1);
        res >>= 6;
    }
    s[0] = c | res;
    return count + 1;
}

inline int decode_varint(int* var, char* data, int len)
{
    if (len <= 0)
        return -1;

    const unsigned char *s = (unsigned char *)data;
    unsigned int c = s[0];
    if (c < 0x80) {
        if (var) *var = c;
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
    if (var) *var = res;
    return count + 1;
}

#endif
