#ifndef _JINJIAZHANG_CIPHER_H_
#define _JINJIAZHANG_CIPHER_H_

#define AES_SCHEDULE_SIZE   60
#define AES_KEY_BITS        256
#define AES_KEY_SIZE        32
#define AES_IV_SIZE         16

class cipher
{
public:
    cipher();
    ~cipher();

    bool init();
    bool init(const char* key, int size);
    bool encrypt(const char* input, int inlen, char* output, int* outlen);
    bool decrypt(const char* input, int inlen, char* output, int* outlen);
    const char* get_key(int* outlen = nullptr);
private:
    int schedule_[AES_SCHEDULE_SIZE];
    char key_[AES_KEY_SIZE];
    char iv1_[AES_IV_SIZE];
    char iv2_[AES_IV_SIZE];
    int  iv1_off_;
    int  iv2_off_;
};

#endif