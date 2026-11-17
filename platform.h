/* Settings the whole build needs, not just the sketch, so they cannot go in kopernik-ultrasonic.ino -
 * the library's own files read them too. Arduino IDE only: PlatformIO ignores this file and uses
 * the same settings from platformio.ini.
 */
#ifndef PLATFORM_H
#define PLATFORM_H
#define CONFIG_ASYNC_TCP_STACK_SIZE 4096        // The captive portal's web server uses this
#define SYSTEM_OTA_PREFIX "kopernik-ultrasonic" // Which firmware over-the-air update looks for
#if defined(ARDUINO_NOLOGO_ESP32C3_SUPER_MINI)
  #define SYSTEM_OTA_SUFFIX "supermini"
#elif defined(ARDUINO_LOLIN_C3_MINI)            // The C3 OLED 72x40 board
  #define SYSTEM_OTA_SUFFIX "c3_oled_72x40_nobatt"
  #define ARDUINO_C3_OLED_72x40                 // Tells the library which display, and that there is no battery on 3
  #define ACTUATOR_OLED_WANT
#endif
#define SENSOR_HCSR04_MIN_MM 200                // SR04M-2 / AJ-SR04M: its blind zone is ~20cm. HC-SR04: delete these three
#define SENSOR_HCSR04_MAX_MM 4500
#define SENSOR_HCSR04_TRIGGER_US 100            // The module, run at 3V3, misses the default 10us trigger
#define SENSOR_HCSR04_DEBUG                     // Comment out to stop printing every ping
#define SYSTEM_WIFI_DEBUG                       // Comment out to stop debugging WIFI connection attempts
#define SYSTEM_MQTT_DEBUG                       // Comment out to stop debugging MQTT interactions
#endif // PLATFORM_H
