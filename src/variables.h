// #pragma once
// /*
// variables
// */



// wifi
bool wifi_connected = false;
bool mqtt_connected = false;
TaskHandle_t h_setup_wifi = NULL;
BaseType_t xReturned_setup_wifi;

// OTA
#ifdef OTA_ACTIVE
  char ota_user[32];
  char ota_password[64];
#endif

// mqtt
WiFiClient espClient;
PubSubClient mqttc(espClient);

// global others
unsigned long em;
unsigned long aux_heartbeat_interval = 0;

// variables
// firmware update
HTTPClient firmware_update_client;
int fw_totalLength = 0;
int fw_currentLength = 0;
bool perform_update_firmware=false;
int update_progress=0;
int old_update_progress=0;
bool blink_led_status=false;

// MAX17048 - battery fuel gauge, I2C
#if (USE_MAX17048 == 1)
    #include <Wire.h>
    #include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
    SFE_MAX1704X lipo(MAX1704X_MAX17048);
    bool max17ok = true;
    #define MAX17048_DELAY_ON_RESET_MS  500 // in ms, as per datasheet: needed before next reading from MAX17048 after reset, only in use when reset/battery change
    // #define VOLTS_INTERVAL_S              3 // in seconds
    unsigned long aux_volts_interval = 0;
    char bat_volts[7];
    char bat_percent[7];
    char bat_chr[7];
#endif

// charging 
#define CHARGING_NC   "NC"
#define CHARGING_ON   "ON"
#define CHARGING_FULL "FULL"
#define CHARGING_OFF  "OFF"
char charging[5];

#ifdef PUSH_BUTTON_GPIO
  #include <SimpleButton.h>
  using namespace simplebutton;
  Button* b = NULL;
  unsigned long aux_pushbutton_interval;
  unsigned long press_time, released;
#endif