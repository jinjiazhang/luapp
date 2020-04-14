#include "cipher.h"
#include "aes.h"
#include <string>
#include <ctime>

cipher::cipher()
{
    memset(schedule_, 0, sizeof(schedule_));
    memset(key_, 0, sizeof(key_));
    memset(iv1_, 0, sizeof(iv1_));
    memset(iv2_, 0, sizeof(iv2_));
}

cipher::~cipher()
{

}

bool cipher::init(const char* key, int size)
{
    if (key != nullptr)
    {
        if (size < sizeof(key_))
        {
            return false;
        }
        memcpy(key_, key, AES_KEY_SIZE);
    }
    else
    {
        std::srand((unsigned int)std::time(nullptr));
        for (int i = 0; i < AES_KEY_SIZE; i++)
        {
            key_[i] = std::rand();
        }
    }
    
    aes_key_setup((const BYTE*)key_, (WORD*)schedule_, AES_KEY_BITS);
    for (int i = 0; i < AES_IV_SIZE; i++)
    {
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

bool cipher::encrypt(const char* input, int inlen, char* output, int* outlen)
{
    memcpy(output, input, inlen);
    *outlen = inlen;
    return true;
}

bool cipher::decrypt(const char* input, int inlen, char* output, int* outlen)
{
    memcpy(output, input, inlen);
    *outlen = inlen;
    return true;
}