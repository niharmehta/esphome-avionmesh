#pragma once

#include <cstdint>
#include <string>

namespace avionmesh {

class MqttDiscovery {
 public:
    void set_node_name(const std::string &name) { node_name_ = name; }
    void set_topic_prefix(const std::string &prefix) { topic_prefix_ = prefix; }
    void set_single_device(bool single_device) { single_device_ = single_device; }

    void publish_light(uint16_t avion_id, const std::string &name,
                       bool has_brightness, bool has_color_temp,
                       const std::string &product_name = "");

    void remove_light(uint16_t avion_id);

    void publish_state(uint16_t avion_id, bool on, uint8_t brightness,
                       bool has_color_temp, bool color_temp_known, uint16_t color_temp_mireds);

    std::string state_topic(uint16_t avion_id) const;
    std::string command_topic(uint16_t avion_id) const;
    std::string discovery_topic(uint16_t avion_id) const;
    std::string management_command_topic() const;
    std::string management_response_topic() const;

 protected:
    std::string node_name_;
    std::string topic_prefix_;
    bool single_device_{false};

    void publish_(const std::string &topic, const std::string &payload, bool retain = false);
};

}  // namespace avionmesh
