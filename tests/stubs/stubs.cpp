#include "esphome/core/application.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/components/esp32_ble/ble.h"
#include "esphome/components/json/json_util.h"

namespace esphome {

Application App;

namespace mqtt {
MQTTClientComponent *global_mqtt_client = nullptr;
}

namespace web_server_base {
WebServerBase *global_web_server_base = nullptr;
}

namespace esp32_ble {
BLEGlobal *global_ble = nullptr;
}

namespace json {

bool parse_json(const std::string &data, const json_parse_t &f) {
    JsonDocument doc;
    auto err = deserializeJson(doc, data);
    if (err) return false;
    return f(doc.as<JsonObject>());
}

std::string build_json(const json_build_t &f) {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    f(root);
    std::string out;
    serializeJson(doc, out);
    return out;
}

}  // namespace json
}  // namespace esphome
