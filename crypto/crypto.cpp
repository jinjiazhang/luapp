#include "lua.hpp"

extern "C" {
#include "md5.h"
#include "sha256.h"
#include "hmac-sha256.h"
#include "base64.h"
}

bool tohex(const char* input, size_t inlen, char* output, size_t& outlen)
{
    if (outlen < inlen * 2)
        return false;

    unsigned char ch;
    unsigned char* oc = (unsigned char*)output;
    for (int i = 0; i < (int)inlen; i++)
    {
        ch = ((input[i] & 0xF0) >> 4);
        if (ch <= 0x09)
            *oc++ = '0' + ch;
        else
            *oc++ = 'a' + ch - 0x0a;

        ch = (input[i] & 0x0F);
        if (ch <= 0x09)
            *oc++ = '0' + ch;
        else
            *oc++ = 'a' + ch - 0x0a;
    }
    outlen = inlen * 2;
    return true;
}

int md5(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    size_t len = 0;
    const char* data = lua_tolstring(L, 1, &len);
    char block[MD5_BLOCK_SIZE];

    MD5_CTX ctx;
    md5_init(&ctx);
    md5_update(&ctx, (const BYTE*)data, len);
    md5_final(&ctx, (BYTE*)block);

    char result[MD5_BLOCK_SIZE * 2];
    size_t res_len = sizeof(result);
    tohex(block, MD5_BLOCK_SIZE, result, res_len);

    lua_pushlstring(L, result, res_len);
    return 1;
}

int sha256(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    size_t len = 0;
    const char* data = lua_tolstring(L, 1, &len);
    char block[SHA256_BLOCK_SIZE];

    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)data, len);
    sha256_final(&ctx, (BYTE*)block);

    char result[SHA256_BLOCK_SIZE * 2];
    size_t res_len = sizeof(result);
    tohex(block, SHA256_BLOCK_SIZE, result, res_len);

    lua_pushlstring(L, result, res_len);
    return 1;
}

int hmac_sha256(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    size_t data_len = 0;
    const char* data = lua_tolstring(L, 1, &data_len);
    size_t key_len = 0;
    const char* key = lua_tolstring(L, 2, &key_len);

    char block[HMAC_SHA256_BLOCK_SIZE];

    hmac_sha256((BYTE*)block, (const BYTE*)data, data_len, (const BYTE *)key, key_len);

    char result[HMAC_SHA256_BLOCK_SIZE * 2];
    size_t res_len = sizeof(result);
    tohex(block, HMAC_SHA256_BLOCK_SIZE, result, res_len);

    lua_pushlstring(L, result, res_len);
    return 1;
}

int base64_encode(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    size_t len = 0;
    const char* data = lua_tolstring(L, 1, &len);
    char block[1024];

    size_t res_len = base64_encode((BYTE*)data, (BYTE*)block, len, 0);
    lua_pushlstring(L, block, res_len);
    return 1;
}

int base64_decode(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    size_t len = 0;
    const char* data = lua_tolstring(L, 1, &len);
    char block[1024];

    size_t res_len = base64_decode((BYTE*)data, (BYTE*)block, len);
    lua_pushlstring(L, block, res_len);
    return 1;
}

int luaopen_crypto(lua_State* L)
{
    luaL_checkversion(L);
    static const struct luaL_Reg cryptoLibs[] = {
        { "md5", md5 },
        { "sha256", sha256 },
        { "hmac_sha256", hmac_sha256 },
        { "base64_encode", base64_encode },
        { "base64_decode", base64_decode },
        { NULL, NULL }
    };

    lua_newtable(L);
    luaL_setfuncs(L, cryptoLibs, 0);
    lua_setglobal(L, "crypto");

    return 0;
}
