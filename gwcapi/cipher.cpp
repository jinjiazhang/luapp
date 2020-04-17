#include "cipher.h"
#include <string>
#include <ctime>

extern "C" {
#include "aes.h"
}

cipher::cipher()
{
    memset(schedule_, 0, sizeof(schedule_));
    memset(key_, 0, sizeof(key_));
    memset(iv1_, 0, sizeof(iv1_));
    memset(iv2_, 0, sizeof(iv2_));
    iv1_off_ = 0;
    iv2_off_ = 0;
}

cipher::~cipher()
{

}

bool cipher::init()
{
    char key[AES_KEY_SIZE];
    std::srand((unsigned int)std::time(nullptr));
    for (int i = 0; i < AES_KEY_SIZE; i++)
    {
        key[i] = std::rand();
    }

    return init(key, sizeof(key));
}

bool cipher::init(const char* key, int size)
{
    if (size < sizeof(key_))
    {
        return false;
    }

    memcpy(key_, key, AES_KEY_SIZE);
    aes_key_setup((BYTE*)key_, (WORD*)schedule_, AES_KEY_BITS);

    for (int i = 0; i < AES_IV_SIZE; i++)
    {
        // fibonacci number
        iv1_[i] = i >= 2 ? iv1_[i - 2] + iv1_[i - 1] : 1;
        iv2_[i] = i >= 2 ? iv2_[i - 2] + iv2_[i - 1] : 1;
    }
    return true;
}

const char* cipher::get_key(int* keysize)
{
    if (keysize != nullptr)
    {
        *keysize = sizeof(key_);
    }

    return key_;
}

// aes-256-cfb
bool cipher::encrypt(const char* input, int inlen, char* output, int* outlen)
{
    if (*outlen < inlen)
        return false;

    char* iv = iv1_;
    int n = iv1_off_;
    int length = inlen;

    while (length--)
    {
        if (n == 0)
            aes_encrypt((BYTE*)iv, (BYTE*)iv, (WORD*)schedule_, AES_KEY_BITS);

        iv[n] = *output++ = iv[n] ^ *input++;
        n = (n + 1) % AES_BLOCK_SIZE;
    }

    iv1_off_ = n;
    *outlen = inlen;
    return true;
}

// aes-256-cfb
bool cipher::decrypt(const char* input, int inlen, char* output, int* outlen)
{
    if (*outlen < inlen)
        return false;

    char* iv = iv2_;
    int n = iv2_off_;
    int length = inlen;

    while (length--)
    {
        if (n == 0)
            aes_encrypt((BYTE*)iv, (BYTE*)iv, (WORD*)schedule_, AES_KEY_BITS);

        char c = *input++;
        *output++ = c ^ iv[n];
        iv[n] = c;
        n = (n + 1) % AES_BLOCK_SIZE;
    }

    iv2_off_ = n;
    *outlen = inlen;
    return true;
}