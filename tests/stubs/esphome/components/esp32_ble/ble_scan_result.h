#pragma once
#include <cstdint>

namespace esphome {
namespace esp32_ble {

struct BLEScanResult {
    int     search_evt{0};
    uint8_t bda[6]{};
    int     rssi{0};
    uint8_t ble_adv[62]{};
    uint8_t adv_data_len{0};
    uint8_t scan_rsp_len{0};
};

}  // namespace esp32_ble
}  // namespace esphome
