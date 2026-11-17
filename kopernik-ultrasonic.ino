/* kopernik-ultrasonic - a water level sensor on an ESP32-C3 SuperMini, or a C3 with a 72x40 OLED
 *
 * An SR04M-2 or AJ-SR04M trigger/echo module, powered from 3V3, looking down at the water and
 * reporting to the Frugal-IoT dashboard over MQTT. The library adds a captive portal to set WiFi
 * and the offset, over-the-air updates, and a sleep cycle.
 *
 * Everything you are likely to change is in the configuration block below. The module's own
 * settings - its range and trigger pulse - are in platform.h (Arduino IDE) and platformio.ini
 * (PlatformIO), because the library reads them too.
 *
 * Arduino IDE: Tools > Board is "Nologo ESP32C3 Super Mini" (also for the SuperMini Pro), or
 * "LOLIN C3 Mini" for the OLED board. Then Tools > Partition Scheme > "No OTA (2MB APP/2MB SPIFFS)" -
 * over-the-air updates still work, partitions.csv sets the real layout and the menu only raises
 * the size limit the IDE checks against. Keep every file in this folder together: platform.h holds
 * the settings the library itself needs, and partitions.csv makes the build fit in flash.
 * PlatformIO ignores those files and uses platformio.ini instead.
 */

// ===== Configuration ===========================================================================
// Before the include, so the library's own defaults give way to these.

// Default WiFi - more can be added through captive portal.
//#define SYSTEM_WIFI_SSID           "myssid"
//#define SYSTEM_WIFI_PASSWORD       "mypassword"

// The module's pins, per board. TRIG goes to the module, ECHO comes back from it. The module runs
// from 3V3, so its echo is 3.3V and goes straight to the pin - see README.md.
// The second name on each #if is for PlatformIO, which does not define the Arduino IDE's board macro
// for every board - see platformio.ini.
#if defined(ARDUINO_NOLOGO_ESP32C3_SUPER_MINI) || defined(SUPERMINI)
  #define SENSOR_ULTRASONIC_TRIG_PIN 21    // Bottom left corner, printed 21
  #define SENSOR_ULTRASONIC_ECHO_PIN 20    // Next one up
  #define SENSOR_BATTERY_PIN         4     // ADC1. Avoid 8 and 9: the LED and the BOOT button
  #define SENSOR_BATTERY_VOLTAGE_DIVIDER 2 // 100k + 100k: battery volts per volt at the pin
#elif defined(ARDUINO_LOLIN_C3_MINI) || defined(C3_OLED_72X40_NOBATT)
  #define SENSOR_ULTRASONIC_TRIG_PIN 21    // Printed TX
  #define SENSOR_ULTRASONIC_ECHO_PIN 20    // Printed RX
  // No battery on this board
#else
  #error "Under Tools > Board pick Nologo ESP32C3 Super Mini, or LOLIN C3 Mini for the C3 OLED 72x40 board"
#endif

// The cycle
#define SYSTEM_POWER_CYCLE_MS      15000   // 15 s between readings - for testing
//#define SYSTEM_POWER_CYCLE_MS    300000  // 5 min - for the field
#define SYSTEM_POWER_WAKE_MS       10000   // Awake this long each cycle, for the dashboard

// The Frugal-IoT dashboard. The node enrols itself with the secret - from the dashboard's gear
// icon -> Nodes -> Enrolment secret - and then has its own credential. With no secret it is refused
// and listed on that Nodes card for an administrator to approve, which also works.
#define SYSTEM_FRUGAL_ORG          "kopernik"
#define SYSTEM_FRUGAL_PROJECT      "developers"
#define SYSTEM_MQTT_HOST           "frugaliot.naturalinnovation.org"
#ifndef SYSTEM_MQTT_ENROL_SECRET   // PlatformIO: put it in a *-local.ini instead, see platformio.ini
  #define SYSTEM_MQTT_ENROL_SECRET ""
#endif
// ===============================================================================================

#include "Frugal-IoT.h"

System_Frugal frugal_iot(SYSTEM_FRUGAL_ORG, SYSTEM_FRUGAL_PROJECT, "Kopernik Level", "Ultrasonic water level");

void setup() {
  // Battery sensor has to come before pre_setup, all others should come after
  #ifdef SENSOR_BATTERY_PIN
    frugal_iot.configure_battery(SENSOR_BATTERY_PIN, SENSOR_BATTERY_VOLTAGE_DIVIDER);
  #endif
  // Awake for SYSTEM_POWER_WAKE_MS of every SYSTEM_POWER_CYCLE_MS, deep sleep for the rest.
  frugal_iot.configure_power(Power_Deep, SYSTEM_POWER_CYCLE_MS, SYSTEM_POWER_WAKE_MS);

  frugal_iot.pre_setup(); // Serial, file system, main config

  frugal_iot.configure_mqtt_enrolled(SYSTEM_MQTT_HOST, SYSTEM_MQTT_ENROL_SECRET);
  #ifdef SYSTEM_WIFI_SSID
    frugal_iot.wifi->addWiFi(F(SYSTEM_WIFI_SSID), F(SYSTEM_WIFI_PASSWORD)); // Others can be added on the captive portal
  #endif

  // The distance down to the water in mm, the median of several pings, assuming 20C
  Sensor_HCSR04* ultrasonic = new Sensor_HCSR04("hcsr04", "Distance",
    SENSOR_ULTRASONIC_TRIG_PIN, SENSOR_ULTRASONIC_ECHO_PIN);
  // Depth of water instead, for a module whose face is 2000mm above the bottom of the tank:
  //  new Sensor_HCSR04("hcsr04", "Depth", SENSOR_ULTRASONIC_TRIG_PIN, SENSOR_ULTRASONIC_ECHO_PIN,
  //    2000, DEFAULT_hcsr04_hcsr04_color, true, 2000.0, -1.0);
  frugal_iot.sensors->add(ultrasonic);

  // On the OLED board, show the reading on it - large, with its unit and a label
  #ifdef ACTUATOR_OLED_WANT
    frugal_iot.controls->add((new Control_Oled_Single("Control OLED", "mm", "Distance"))->wireFrom(ultrasonic));
  #endif

  frugal_iot.setup(); // After all the sensors, actuators and controls are added
  Serial.println(F("Kopernik Ultrasonic Starting Loop"));
}

void loop() {
  frugal_iot.loop();
}
