#include "packet.h"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include<tuple>
#include <functional>
#include "memory.h"

using namespace std;

int main() {
    string content;
    {
        std::stringstream buffer;
        auto path("/Users/sk/workspace/study2/sproto.spb");
        std::ifstream t(path);
        buffer << t.rdbuf();
        content = buffer.str();
    }

    auto proto = sproto_create(content.data(), content.size());
    auto tag_st = sproto_type(proto, "tag");
    auto ping_st = sproto_type(proto, "ping");
    if (!(tag_st && ping_st))
        std::terminate();

    Ping ping;
    ping.nonce = time(nullptr);
    ping.uuid = "1122334455667788";
    ping.tag = uint32_t(PktType::PING);

    char buffer[ping.estimate_size() + sizeof(Header)];
    auto len = pack_with_header(ping_st, buffer, sizeof(buffer), ping);
    if (len <= 0)
        std::terminate();
    SPDLOG_ERROR("{}", len);

    auto &header = *(Header *)buffer;
    auto dlen = ntohl(header.len);
     if (dlen != len - sizeof(header))
        std::terminate();

    auto rsum = FNVHash1(buffer + sizeof(header), dlen);
    if (htonl(rsum) != header.sum)
        std::terminate();

    char unpack_buffer[1400];
    auto ulen = sproto_unpack(buffer + sizeof(header), dlen, unpack_buffer, sizeof(unpack_buffer));
    if (ulen <= 0)
        std::terminate();

    Tag tag{};
    auto ret = sproto_decode_ex(tag_st, unpack_buffer, ulen, &tag);
    if (ret < 0)
        std::terminate();
    switch (PktType(tag.tag)) {
        case PktType::PING: {
            Ping pingx;
            ret = sproto_decode_ex(ping_st, unpack_buffer, ulen, &pingx);
            if (ret < 0)
                std::terminate();
            SPDLOG_INFO(" {} {} ", pingx.nonce == ping.nonce, pingx.uuid == ping.uuid);
            break;
        }
        default:
            std::terminate();
            break;
    }


    return 0;
}
