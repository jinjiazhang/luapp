#ifndef _JINJIAZHANG_VARINT_H_
#define _JINJIAZHANG_VARINT_H_

inline uint64_t zigzag_encode(int64_t n)
{
    return ((uint64_t)(n) << 1) ^ (uint64_t)(n >> 63);
}

inline int64_t zigzag_decode(uint64_t n)
{
    return (int64_t)((n >> 1) ^ (~(n & 1) + 1));
}

inline int encode_varint(char* data, int len, uint64_t value)
{
    char* pos = data;
    char* end = data + len;
    unsigned char code = 0;
    do {
        if (pos >= end)
            return -1;
        code = (unsigned char)(value & 0x7F);
        value >>= 7;
        *pos++ = code | (value > 0 ? 0x80 : 0);
    } while (value > 0);
    return pos - data;
}

inline int decode_varint(uint64_t* value, const char* data, int len)
{
    const char* pos = data;
    const char* end = data + len;
    uint64_t result = 0;
    uint64_t code = 0;
    int bits = 0;

    while (true) {
        if (pos >= end || bits > 63)
            return -1;
        code = *pos & 0x7F;
        result |= (code << bits);
        if ((*pos++ & 0x80) == 0)
            break;
        bits += 7;
    }
    *value = result;
    return pos - data;
}

#endif
