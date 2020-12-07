//#include "fmt/format.h"
#include "spdlog/spdlog.h"
//#include "asio.hpp"
#include <spdlog/sinks/sink.h>
#include <spdlog/details/log_msg.h>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
//#define SPDLOG_INFO
using namespace std;

class custom_sink : public spdlog::sinks::sink {
public:
    explicit custom_sink() {

    }
    ~custom_sink() override = default;

    virtual void log(const spdlog::details::log_msg &msg) final {
        fmt::print("log {}\n", msg.payload);
    }
    virtual void flush() final {
        fmt::print(stderr, "flush \n");
    }
    virtual void set_pattern(const std::string &pattern) final {
        fmt::print(stdout, "set_pattern {}\n", pattern);
    }
    virtual void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) final {
        fmt::print(stdout, "set_formatter\n");
    }

    custom_sink(const custom_sink &other) = delete;
    custom_sink &operator=(const custom_sink &other) = delete;
};

int main() {
    auto test_sink = std::make_shared<custom_sink>();
    size_t backtrace_size = 5;

    spdlog::logger logger("test-backtrace", test_sink);
    logger.set_pattern("%v");
    logger.enable_backtrace(backtrace_size);
    SPDLOG_INFO("xxx");
    logger.info("info message {} {}", fmt::to_string(123), "abc");
    return 0;
}
