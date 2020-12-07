#ifndef STUDY_PACKET_H
#define STUDY_PACKET_H
extern "C" {
#include "sproto.h"
};
#include <cstdint>
#include <string>
using std::string;

enum class PktType {
    PING,
    PONG,
};

struct Ping {
    uint32_t tag;
    uint32_t nonce;
    string uuid;

    int estimate_size() const {
        return 64 + sizeof(tag) + sizeof(nonce) + uuid.size();
    }
    int decode(const struct sproto_arg *args);
    int encode(const struct sproto_arg *args) const;
};

struct Pong {
    uint32_t tag;
    uint32_t nonce;
    string r_addr;

    int estimate_size() const {
        return 64 + sizeof(tag) + sizeof(nonce) + r_addr.size();
    }
    int decode(const struct sproto_arg *args);
    int encode(const struct sproto_arg *args) const;
};

struct Tag {
    uint32_t tag;
    int decode(const struct sproto_arg *args);
};

struct Header {
    uint32_t len;
    uint32_t sum;
};

static uint32_t FNVHash1(const char *src, int len)
{
    auto data = (const unsigned char *)src;
    const uint32_t p = 16777619;
    uint32_t hash = 2166136261L;
    for (auto i = 0; i < len; i++)
        hash = (hash ^ data[i]) * p;
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}


template<typename T>
inline int sproto_encode_ex(const struct sproto_type *st, void * buffer, int size, const T &ud) {
    return sproto_encode(st, buffer, size, [](const struct sproto_arg *args) -> int {
        return ((T *)args->ud)->encode(args);
    }, (void *)&ud);
}

template<typename T>
inline int sproto_decode_ex(const struct sproto_type *st, const void *buffer, int size, T *ud) {
    return sproto_decode(st, buffer, size, [](const struct sproto_arg *args) -> int {
        return ((T *)args->ud)->decode(args);
    }, ud);
}

template<typename T>
int pack_with_header(const struct sproto_type *st, char *buffer, int size, const T &obj) {
    if (size > 1300)
        return -1;

    auto estimate_size = obj.estimate_size();
    if (estimate_size > size)
        size = estimate_size;

    char tmpbuf[size];
    auto len = sproto_encode_ex(st, tmpbuf, size, obj);
    if (len <= 0)
        return -2;

    auto data_buff = buffer + sizeof(Header);
    auto plen = sproto_pack(tmpbuf, len, data_buff,  size - sizeof(Header));
    if (plen <= 0)
        return -3;

    auto &header = *(Header *)buffer;
    header.len = htonl(plen);
    header.sum = htonl(FNVHash1(data_buff, plen));
    return plen + sizeof(Header);
}

#endif //STUDY_PACKET_H
