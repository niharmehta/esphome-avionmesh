#pragma once
#include <functional>
#include <string>

#define ARDUINOJSON_ENABLE_STD_STRING 1
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

namespace esphome {
namespace json {

using json_parse_t = std::function<bool(JsonObject)>;
using json_build_t = std::function<void(JsonObject)>;

bool parse_json(const std::string &data, const json_parse_t &f);
std::string build_json(const json_build_t &f);

}  // namespace json
}  // namespace esphome
