#pragma once
#include <cstdint>
#include <functional>
#include <string>

namespace esphome {

namespace setup_priority {
static constexpr float AFTER_BLUETOOTH = -100.f;
}

inline uint32_t &test_millis_ref() { static uint32_t t = 0; return t; }
inline void set_test_millis(uint32_t t) { test_millis_ref() = t; }
inline uint32_t millis() { return test_millis_ref(); }

class Component {
public:
    virtual void setup() {}
    virtual void loop() {}
    virtual void dump_config() {}
    virtual float get_setup_priority() const { return 0.f; }
    void set_interval(const std::string &, uint32_t, std::function<void()>) {}
    void set_timeout(const std::string &, uint32_t, std::function<void()>) {}
};

}  // namespace esphome
