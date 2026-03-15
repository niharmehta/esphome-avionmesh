// Tests: MQTT command topics → BLE mesh send + MQTT state echo.
// LampDimmer (type=90) has dimming and color_temp.

#include "mock_hub.h"
#include "esphome/core/component.h"
#include <gtest/gtest.h>

using namespace avionmesh;

static constexpr uint16_t DEV = 32900;
static const std::string PREFIX = "avionmesh";

class MqttCommandTest : public ::testing::Test {
protected:
    TestHub hub;

    void SetUp() override {
        hub.db().add_device(DEV, 93 /* RecessedDL — has both dimming and color_temp */, "Test Light");
        hub.db().find_device(DEV)->mqtt_exposed = true;
        hub.test_setup();
        esphome::set_test_millis(1000);  // non-zero so rapid-dim window doesn't trigger
    }
};

// --- Brightness command ---

TEST_F(MqttCommandTest, BrightnessSet_SendsToBle) {
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "128");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    auto &cmd = hub.mesh_sends[0];
    EXPECT_EQ(cmd.dest_id, DEV);
    // payload[1] = Noun::Dimming (0x0A), payload[5] = brightness
    EXPECT_EQ(cmd.payload[1], 0x0A);
    EXPECT_EQ(cmd.payload[5], 128);
}

TEST_F(MqttCommandTest, BrightnessSet_EchoesStateToMqtt) {
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "200");

    bool found = false;
    for (auto &[topic, payload, retain] : hub.mqtt_publishes) {
        if (topic == PREFIX + "/light/" + std::to_string(DEV) + "/brightness/state") {
            found = true;
            EXPECT_EQ(payload, "200");
        }
    }
    EXPECT_TRUE(found) << "expected brightness state echo on MQTT";
}

// --- ON command ---

TEST_F(MqttCommandTest, OnCommand_NoPriorState_Sends255) {
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/set", "ON");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    EXPECT_EQ(hub.mesh_sends[0].payload[5], 255);
}

TEST_F(MqttCommandTest, OnCommand_WithPriorBrightness_RestoresBrightness) {
    // Pre-set brightness state (e.g. from a prior BLE status)
    hub.states()[DEV] = {128, 0, true, false};

    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/set", "ON");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    EXPECT_EQ(hub.mesh_sends[0].payload[5], 128)
        << "ON should restore last known brightness, not send 255";
}

TEST_F(MqttCommandTest, OnCommand_PriorBrightnessIsZero_Sends255) {
    hub.states()[DEV] = {0, 0, true, false};

    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/set", "ON");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    EXPECT_EQ(hub.mesh_sends[0].payload[5], 255)
        << "ON with prior brightness=0 should fall back to 255";
}

// --- OFF command ---

TEST_F(MqttCommandTest, OffCommand_SendsBrightness0) {
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/set", "OFF");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    EXPECT_EQ(hub.mesh_sends[0].payload[5], 0);
}

TEST_F(MqttCommandTest, OffCommand_EchoesOffStateToMqtt) {
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/set", "OFF");

    bool found_off = false;
    for (auto &[topic, payload, retain] : hub.mqtt_publishes)
        if (topic == PREFIX + "/light/" + std::to_string(DEV) + "/state" && payload == "OFF")
            found_off = true;

    EXPECT_TRUE(found_off) << "expected ON/OFF state echo = OFF";
}

// --- Color-temp command ---

TEST_F(MqttCommandTest, ColorTempSet_SendsCorrectKelvin) {
    // 370 mireds → 1000000/370 = 2702 kelvin
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/color_temp/set", "370");

    ASSERT_EQ(hub.mesh_sends.size(), 1u);
    auto &cmd = hub.mesh_sends[0];
    EXPECT_EQ(cmd.payload[1], 0x1D);  // Noun::Color
    uint16_t kelvin = (static_cast<uint16_t>(cmd.payload[6]) << 8) | cmd.payload[7];
    EXPECT_EQ(kelvin, 1000000u / 370u);
}

// --- Rapid-dim detection ---

TEST_F(MqttCommandTest, RapidDim_SecondCallWithinWindow_SkipsMeshSend) {
    esphome::set_test_millis(1000);
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "100");
    ASSERT_EQ(hub.mesh_sends.size(), 1u);

    // Still within 750 ms window
    esphome::set_test_millis(1100);
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "80");

    // Second command should NOT produce an extra mesh send
    EXPECT_EQ(hub.mesh_sends.size(), 1u) << "rapid dim should suppress second mesh send";
}

TEST_F(MqttCommandTest, RapidDim_SecondCallOutsideWindow_SendsToBle) {
    esphome::set_test_millis(1000);
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "100");

    esphome::set_test_millis(2000);  // 1 s later — outside 750 ms window
    hub.inject_mqtt(PREFIX + "/light/" + std::to_string(DEV) + "/brightness/set", "80");

    EXPECT_EQ(hub.mesh_sends.size(), 2u) << "non-rapid dim should send both commands";
}
