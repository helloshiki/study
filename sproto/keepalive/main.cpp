#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "unistd.h"
#include <iostream>
#include "spdlog/spdlog.h"
using namespace std;

#define SERVER_PORT 5555
#define BUFF_LEN 512
#define SERVER_IP "127.0.0.1"

void udp_msg_sender(int fd, struct sockaddr* dst)
{
    socklen_t len;
    struct sockaddr_in src{};
    while(1)
    {
        char buf[BUFF_LEN] = "TEST UDP MSG!\n";
        len = sizeof(*dst);
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        memset(buf, 0, BUFF_LEN);
        auto ret = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        SPDLOG_INFO("{}", ret);
        sleep(1);  //一秒发送一次消息
    }
}

int run_keepalive(const string &server_addr, int server_port, int timeout_sec) {
    int client_fd;
    struct sockaddr_in ser_addr{};

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0) {
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(server_addr.c_str());
    ser_addr.sin_port = htons(server_port);

    struct timeval timeout = {timeout_sec,0};
    auto r = setsockopt(client_fd, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (r) {
        close(client_fd);
        return -1;
    }

    udp_msg_sender(client_fd, (struct sockaddr*)&ser_addr);

    close(client_fd);
    return 0;
}

int main(int argc, char* argv[]) {
    run_keepalive(SERVER_IP, SERVER_PORT, 5);

    return 0;
}