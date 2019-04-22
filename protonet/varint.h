#ifndef _JINJIAZHANG_VARINT_H_
#define _JINJIAZHANG_VARINT_H_

inline int length_varint(int value)
{
    if (value >= 0 && value < 0x80) {
        return 1;
    }

    unsigned int res = (unsigned int)value;
    int count = 0;
    while (res > 0x02) {
        res >>= 5;
        count++;
    }
    return count;
}

inline int encode_varint(char* data, int len, int value)
{
    if (len <= 0)
        return -1;

    if (value >= 0 && value < 0x80) {
        data[0] = (char)value;
        return 1;
    }

    unsigned int res = (unsigned int)value;
    int count = 0;
    while (res > 0x02) {
        res >>= 5;
        count++;
    }

    res = (unsigned int)value;
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

inline int decode_varint(int* value, const char* data, int len)
{
    if (len <= 0)
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
