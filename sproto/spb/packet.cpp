#include "packet.h"
#include <string>
using namespace std;

static inline int encode_string(const struct sproto_arg *args, const string &str) {
    if (args->length < str.length())
        return SPROTO_CB_ERROR;
    memcpy(args->value, str.data(), str.size());
    return str.size();
}

static inline int encode_uint32(const struct sproto_arg *args, uint32_t n) {
    if (args->length < sizeof(uint32_t))
        return SPROTO_CB_ERROR;
    *(uint32_t*)args->value = n;
    return sizeof(uint32_t);
}

static inline bool default_true(uint32_t) {
    return true;
}

static inline bool default_str_true(const char *, int) {
    return true;
}

static inline int decode_uint32(const struct sproto_arg *args, uint32_t *out,
        const function<bool(uint32_t)>& pred = default_true) {
    if (args->length < sizeof(uint32_t))
        return SPROTO_CB_ERROR;
    auto n = *(uint32_t *)args->value;
    if (!pred(n))
        return SPROTO_CB_ERROR;
    *out = n;
    return 0;
}

static inline int decode_string(const struct sproto_arg *args, string *out,
        const function<bool(const char *, int)>& pred = default_str_true) {
    if (args->length == 0)
        return 0;
    auto value = (const char *)args->value;
    if (!pred(value, args->length))
        return SPROTO_CB_ERROR;
    out->assign(value, args->length);
    return 0;
}

struct check_range {
    uint32_t min, max;
    explicit check_range(uint32_t min, uint32_t max = UINT32_MAX): min(min), max(max) {}
    bool operator()(uint32_t n) const {
        return n >= min && n <= max;
    }
};

struct check_length {
    int min, max;
    explicit check_length(int min = 0, int max = 128): min(min), max(max) {}
    bool operator()(const char *data, int len) const {
        return len >= min && len <= max;
    }
};

/********************************* ping ************************************/
int Ping::decode(const struct sproto_arg *args) {
    if (!strcmp(args->tagname, "tag"))
        return decode_uint32(args, &tag, check_range(0));

    if (!strcmp(args->tagname, "nonce"))
        return decode_uint32(args, &nonce);

    if (!strcmp(args->tagname, "uuid"))
        return decode_string(args, &uuid, check_length(16, 16));

    return 0;
}

int Ping::encode(const struct sproto_arg *args) const {
    if (!strcmp(args->tagname, "tag"))
        return encode_uint32(args, tag);

    if (!strcmp(args->tagname, "nonce"))
        return encode_uint32(args, nonce);

    if (!strcmp(args->tagname, "uuid"))
        return encode_string(args, uuid);

    return 0;
}

/********************************* pong ************************************/
int Pong::decode(const struct sproto_arg *args) {
    if (!strcmp(args->tagname, "tag"))
        return decode_uint32(args, &tag, check_range(0));

    if (!strcmp(args->tagname, "nonce"))
        return decode_uint32(args, &nonce);

    if (!strcmp(args->tagname, "raddr"))
        return decode_string(args, &r_addr, check_length(0, 64));

    return 0;
}

int Pong::encode(const struct sproto_arg *args) const {
    if (!strcmp(args->tagname, "tag"))
        return encode_uint32(args, tag);

    if (!strcmp(args->tagname, "nonce"))
        return encode_uint32(args, nonce);

    if (!strcmp(args->tagname, "raddr"))
        return encode_string(args, r_addr);

    return 0;
}

/********************************* tag ************************************/
int Tag::decode(const struct sproto_arg *args) {
    if (!strcmp(args->tagname, "tag"))
        return decode_uint32(args, &tag, check_range(0));
    return 0;
}
