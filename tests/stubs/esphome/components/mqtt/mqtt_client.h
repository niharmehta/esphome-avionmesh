#pragma once
#include <functional>
#include <string>

namespace esphome {
namespace mqtt {

class MQTTClientComponent {
public:
    bool is_connected() { return false; }
    std::string get_topic_prefix() { return "avionmesh"; }
    void subscribe(const std::string &, std::function<void(const std::string &, const std::string &)>, int) {}
    void publish(const std::string &, const std::string &, int, bool) {}
};

extern MQTTClientComponent *global_mqtt_client;

}  // namespace mqtt
}  // namespace esphome
