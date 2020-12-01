#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "asio.hpp"

#include <memory>
#include <string>
#include <vector>
//#define SPDLOG_INFO
using namespace std;

using asio::ip::tcp;

class shared_const_buffer {
public:
    explicit shared_const_buffer(const string &data):
    m_data(new vector<char>(data.begin(), data.end())),
    m_buffer(asio::buffer(*m_data)) {
        make_shared<vector<char>>();
    }
    typedef asio::const_buffer value_type;
    typedef const asio::const_buffer* const_iterator;
    const asio::const_buffer* begin() const { return &m_buffer; }
    const asio::const_buffer* end() const { return &m_buffer + 1; }
private:
    shared_ptr<vector<char>> m_data;
    asio::const_buffer m_buffer;
};

class session: public enable_shared_from_this<session> {
public:
    explicit session(asio::io_context &io_ctx): m_socket(io_ctx) {}
    tcp::socket& socket() { return m_socket; }
    void start() {
        auto now = time(nullptr);
        shared_const_buffer buffer(ctime(&now));
        auto self = shared_from_this();
        SPDLOG_INFO("==== {}", self.use_count());
        asio::async_write(m_socket, buffer, [this, self](auto &ec, size_t size) {
            SPDLOG_INFO("async_write {} {}", self.use_count(), size);
            this->handle_write(ec, size);
        });
    }
    void handle_write(const asio::error_code& ec, size_t size) {

    }
private:
    tcp::socket m_socket;
};

//using session_ptr = std::shared_ptr<session>;
typedef shared_ptr<session> session_ptr;

class server {
public:
    explicit server(asio::io_context &io_ctx, short port):
    m_io_ctx(io_ctx), m_acceptor(io_ctx, tcp::endpoint(tcp::v4(), port))
    {
//        auto new_session = std::make_shared<session>(io_ctx);
        session_ptr new_session(new session(m_io_ctx));
        SPDLOG_INFO("xxxxx {}", new_session.use_count());
        m_acceptor.async_accept(new_session->socket(), [this, new_session](auto &ec){
            SPDLOG_INFO("xxxxx {}", new_session.use_count());
            this->handle_accept(new_session, ec);
        });
    }
    void handle_accept(session_ptr new_session, const asio::error_code& ec) {
        if (!ec) {
            SPDLOG_INFO("handle_accept ok {}", new_session.use_count());
            new_session->start();
        } else {
            SPDLOG_INFO("handle_accept fail {}", ec.message());
        }

        auto &old_session = new_session;
        new_session = std::make_shared<session>(m_io_ctx);
        SPDLOG_INFO("reset ok {} {}", new_session.use_count(), old_session.use_count());

        m_acceptor.async_accept(new_session->socket(), [this, new_session](auto &ec){
            SPDLOG_INFO("async_accept {}", new_session.use_count());
            this->handle_accept(new_session, ec);
        });
    }
private:
    asio::io_context &m_io_ctx;
    tcp::acceptor  m_acceptor;
};

void test_reference_count() {
    asio::io_context io_ctx;
    server s(io_ctx, 1234);
    io_ctx.run();
}

int main() {
    SPDLOG_INFO("=====");
//    fmt::format("{}\n", 234);
    fmt::print("xxxx {}\n", 235);
    test_reference_count();
    return 0;
}
