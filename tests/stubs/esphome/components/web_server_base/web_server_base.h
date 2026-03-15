#pragma once
#include <string>

// Minimal ESP-IDF / ESPHome web server type stubs for host compilation
typedef int httpd_handle_t;

class AsyncWebServerRequest {
public:
    std::string arg(const char *) const { return ""; }
    bool hasArg(const char *) const { return false; }
    void send(int, const char *, const std::string &) {}
};

class AsyncWebHandler {
public:
    virtual ~AsyncWebHandler() = default;
    virtual bool canHandle(AsyncWebServerRequest *) const { return false; }
    virtual void handleRequest(AsyncWebServerRequest *) {}
};

namespace esphome {
namespace web_server_base {

class WebServerBase {
public:
    void add_handler(AsyncWebHandler *) {}
};

extern WebServerBase *global_web_server_base;

}  // namespace web_server_base
}  // namespace esphome
