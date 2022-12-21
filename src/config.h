#pragma once
/*
config
*/
// =======================================================================================================

// DEVICE CUSTOM SETTINGS 
// ---------------------------------------------------------------------------------------------------

#if DEVICE_ID == 40
  #define BOARD_TYPE                2
  #define HOSTNAME                  "esp32040"
  #define ROLE_NAME                 "Dehumidifier"
  
  #define USE_MAX17048              1
  #define PUBLISH_BATPERCENT        0
  // #define CHARGING_GPIO            39  // comment out if not in use - don't use "0" here
  // #define POWER_GPIO               38  // comment out if not in use - don't use "0" here
  #define STATUS_GW_LED_GPIO_RED    5  // on event - check which one is better
  #define POWER_ON_LED_GPIO_GREEN   6  // always ON, with PWM if needed
  #define POWER_ON_LED_USE_PWM      1  // better with PWM if too bright
  #define POWER_ON_LED_DC           20
  #define PUSH_BUTTON_GPIO          0  // to control ESP
  #define PUSH_BUTTON_GPIO_ACT    LOW  // HIGH or LOW, HIGH = 1, LOW = 0, 0 if not defined (so default)

  #define BUTTON_POWER_GPIO         21
  #define BUTTON_FAN_GPIO           26
  #define BUTTON_MODE_GPIO          33
  #define BUTTON_SWING_GPIO         34
  #define BUTTON_TIMER_GPIO         35
  #define BUTTON_ION_GPIO           36

  
  #define GPIO_VOLT_ITERATIONS      1000
  #define BUTTON_PRESS_TIME_MS      100

  #define Q_VOLTAGE_LEVEL           2.0f
  #define ANODE_VOLTAGE_LEVEL       1.3f

  #define Q8_GPIO                   11
  #define Q9_GPIO                   12
  #define Q10_GPIO                  13
  
  #define ANODE_1_GPIO              1
  #define ANODE_2_GPIO              2
  #define ANODE_3_GPIO              4
  #define ANODE_4_GPIO              7
  #define ANODE_5_GPIO              10


  #pragma message "compilation for: ESPnow_esp32093-ups-test"
// ---------------------------------------------------------------------------------------------------


#else
  #error "Wrong DEVICE_ID chosen"
#endif
// DEVICE CUSTOM SETTINGS END

// for PubSubClient:
// it is used in: mqttc.setBufferSize(MQTT_PAYLOAD_MAX_SIZE); to increase max packet size
//default is 256 but it is not enough in this program - check debug in case of issue
#define MQTT_PAYLOAD_MAX_SIZE       1024

// for ArduinoJson
#define JSON_CONFIG_SIZE            1024  // config is bigger than payload due to device information
#define JSON_PAYLOAD_SIZE           512

// mqtt - how many attempts to connect to MQTT broker before restarting
#define MAX_MQTT_ERROR              15  // in seconds / times

// how often to update HA on GW status
#define HEARTBEAT_INTERVAL_S        30    //10 in seconds

// how often to update HA on GW battery
#define VOLTS_INTERVAL_S            3 // HEARTBEAT_INTERVAL_S // in seconds

// push button
#define PUSHBUTTON_UPDATE_INTERVAL_MS     100    // in ms

// LED PWM settings
#ifdef POWER_ON_LED_GPIO_GREEN
  #define POWER_ON_LED_PWM_CHANNEL    1
  #define POWER_ON_LED_PWM_RESOLUTION 8
  #define POWER_ON_LED_PWM_FREQ       5000
  #define POWER_ON_LED_MIN_DC         2
  #define POWER_ON_LED_MAX_DC         255
#endif

// assigning MODEL, FW file name and checking if proper board is selected (only Arduino)
#define PRINT_COMPILER_MESSAGES // comment out to disable messages in precompiler
#ifndef BOARD_TYPE
  #error BOARD_TYPE not defined
#else
  #if (BOARD_TYPE == 1) and (!defined(CONFIG_IDF_TARGET_ESP32))
    #error wrong board selected in Arduino - choose ESP32DEV
  #endif
  #if (BOARD_TYPE == 2) and (!defined(CONFIG_IDF_TARGET_ESP32S2))
    #error wrong board selected in Arduino - choose S2
  #endif
  #if (BOARD_TYPE == 3) and (!defined(CONFIG_IDF_TARGET_ESP32S3))
    #error wrong board selected in Arduino - choose S3
  #endif
  #if (BOARD_TYPE == 4) and (!defined(CONFIG_IDF_TARGET_ESP32C3))
    #error wrong board selected in Arduino - choose C3
  #endif

  #if (BOARD_TYPE == 1)
    #define MODEL "ESP32"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32"
    #endif
    // #define FW_BIN_FILE "ups-battery-sensor.ino.esp32.bin"
  #elif (BOARD_TYPE == 2)
    #define MODEL "ESP32S2"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S2"
    #endif
    // #define FW_BIN_FILE "ups-battery-sensor.ino.esp32s2.bin"
  #elif (BOARD_TYPE == 3)
    #define MODEL "ESP32S3"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S3"
    #endif
    // #define FW_BIN_FILE "ups-battery-sensor.ino.esp32s3.bin"
  #elif (BOARD_TYPE == 4)
    #define MODEL "ESP32C3"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32C3"
    #endif
    // #define FW_BIN_FILE "ups-battery-sensor.ino.esp32c3.bin"
  #else
    #error BOARD_TYPE not defined
  #endif


#endif
// assigning MODEL, FW file name and checking if proper board is selected END

