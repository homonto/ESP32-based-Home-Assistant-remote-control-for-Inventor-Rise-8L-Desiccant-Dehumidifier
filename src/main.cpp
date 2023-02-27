#include "Arduino.h"

// chosing devices moved to platformio.ini

// #define DEBUG       // it sends also othe info to HA

#define ZH_VERSION "0.8.0"
#define OTA_ACTIVE
#define MQTT_DEVICE_IDENTIFIER String(WiFi.macAddress())

// libraries
// wifi
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>

// FW update
#include <HTTPClient.h>
#include <Update.h>

// OTA web
#ifdef OTA_ACTIVE
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <AsyncElegantOTA.h>
  AsyncWebServer server(8080);
  // #warning "OTA_ACTIVE defined"
#else
  #warning "OTA_ACTIVE NOT defined"
#endif
// mqtt
#include <PubSubClient.h>  
// JSON
#include <ArduinoJson.h>


// declarations
void do_update();
void updateFirmware(uint8_t *data, size_t len);
int update_firmware_prepare();

// various.h
void ConvertSectoDay(unsigned long n, char * pretty_ontime);
void uptime(char *uptime);
byte get_boot_reason();
void OnClicked_cb();
void OnDoubleClicked_cb();
void OnHolding_cb();


// mqtt_publish_data.h - UPDATE_INTERVAL
bool mqtt_publish_gw_status_config();
bool mqtt_publish_gw_status_values(const char* status);
void hearbeat();
bool mqtt_publish_sensors_config();
bool mqtt_publish_sensors_values();
bool mqtt_publish_config_all_buttons();

// measure volts and charing - getvolts_max17048.h
void measure_volts();
void charging_state();

// new functions for dehumidifier
void prepare_gpio();
u_int8_t check_led_status(u_int8_t gpio);
void check_modes(bool force);
void press_button(u_int8_t gpio);
bool mqtt_publish_button_config(const char* button);
bool mqtt_publish_text_sensor_config(const char* sensor);
bool mqtt_publish_text_sensor_values(const char* sensor, const char* value);

// custom config files - must be before functions
#include "passwords.h" 
#include "config.h"
#include "variables.h"


// custom functions files
#include "fw_update.h"
#include "mqtt.h"
#include "mqtt_publish_data.h"
#include "various.h"
#include "getvolts_max17048.h"
#include "captive-portal.h"



void prepare_gpio()
{
  pinMode(Q8_GPIO,INPUT);
  pinMode(Q9_GPIO,INPUT);
  pinMode(Q10_GPIO,INPUT);

  pinMode(ANODE_1_GPIO,INPUT);
  pinMode(ANODE_2_GPIO,INPUT);
  pinMode(ANODE_3_GPIO,INPUT);
  pinMode(ANODE_4_GPIO,INPUT);
  pinMode(ANODE_5_GPIO,INPUT);

  pinMode(BUTTON_POWER_GPIO,OUTPUT);
  pinMode(BUTTON_FAN_GPIO,OUTPUT);
  pinMode(BUTTON_MODE_GPIO,OUTPUT);
  pinMode(BUTTON_SWING_GPIO,OUTPUT);
  pinMode(BUTTON_TIMER_GPIO,OUTPUT);
  pinMode(BUTTON_ION_GPIO,OUTPUT);

  digitalWrite(BUTTON_POWER_GPIO,HIGH);
  digitalWrite(BUTTON_FAN_GPIO,HIGH);
  digitalWrite(BUTTON_MODE_GPIO,HIGH);
  digitalWrite(BUTTON_SWING_GPIO,HIGH);
  digitalWrite(BUTTON_TIMER_GPIO,HIGH);
  digitalWrite(BUTTON_ION_GPIO,HIGH);
}

u_int8_t check_led_status(u_int8_t gpio)
{
  // Serial.printf("[%s]:...\n",__func__);
  float voltage_thr;
  u_int8_t status = 0;
  u32_t read_digital=0; 
  float volts = 0.0f;
  for (u16_t i = 0; i < GPIO_VOLT_ITERATIONS; i++) {
      read_digital += analogRead(gpio);
  }
  read_digital /= GPIO_VOLT_ITERATIONS; 
  volts = ((3.3 * read_digital)/8191);
  if ((gpio == Q8_GPIO) or (gpio == Q9_GPIO) or (gpio == Q10_GPIO))
  {
    voltage_thr = Q_VOLTAGE_LEVEL;
    if (gpio == Q8_GPIO) q8_volt = volts;
    if (gpio == Q9_GPIO) q9_volt = volts;
    if (gpio == Q10_GPIO) q10_volt = volts;
  } else 
  {
    voltage_thr = ANODE_VOLTAGE_LEVEL;
    if (gpio == ANODE_1_GPIO) anode_1_volt = volts;
    if (gpio == ANODE_2_GPIO) anode_2_volt = volts;
    if (gpio == ANODE_3_GPIO) anode_3_volt = volts;
    if (gpio == ANODE_4_GPIO) anode_4_volt = volts;
    if (gpio == ANODE_5_GPIO) anode_5_volt = volts;
  }
  
  if (volts>voltage_thr) status=1; else status=0;

  Serial.printf("[%s]: GPIO=%d, volts=%0.2fV, voltage_thr=%0.2f, status=%d\n",__func__,gpio,volts,voltage_thr,status);
  return status;
}

// in force mode update even if status is the same as before
void check_modes(bool force)
{
  anodes_old = anodes;

  q8 = check_led_status(Q8_GPIO);
  q9 = check_led_status(Q9_GPIO);
  q10 = check_led_status(Q10_GPIO);

  anode_1 = check_led_status(ANODE_1_GPIO);
  anode_2 = check_led_status(ANODE_2_GPIO);
  anode_3 = check_led_status(ANODE_3_GPIO);
  anode_4 = check_led_status(ANODE_4_GPIO);
  anode_5 = check_led_status(ANODE_5_GPIO);

  anodes = anode_1 + (2 * anode_2) + (4 * anode_3) + (8 * anode_4) + (16 * anode_5); 

  if (force == false)
  {
    if (anodes == anodes_old) return;
  }

  
  Serial.printf("[%s]: Qs: q8=%d, q9=%d, q10=%d\n",__func__,q8,q9,q10);
  Serial.printf("[%s]: Anodes: a1=%d, a2=%d, a3=%d, a4=%d, a5=%d\n",__func__,anode_1,anode_2,anode_3,anode_4,anode_5);

  Serial.printf("[%s]: Anodes=%d\n",__func__,anodes);


  snprintf(work_mode,sizeof(work_mode),"%s","N/A");
  snprintf(fan_speed,sizeof(fan_speed),"%s","N/A"); 


  // MODES: 4
  if ((anodes == 6) or (anodes == 10) or (anodes == 18))  snprintf(work_mode,sizeof(work_mode),"%s","ECO");
  if ((anodes == 4) or (anodes == 12) or (anodes == 20))  snprintf(work_mode,sizeof(work_mode),"%s","NORMAL"); 
  if ((anodes == 8) or (anodes == 24))                    snprintf(work_mode,sizeof(work_mode),"%s","HIGH"); 
  if ((anodes == 5) or (anodes == 9) or (anodes == 17))   snprintf(work_mode,sizeof(work_mode),"%s","TURBO");

  if (anodes == 12)
  {
    if (anodes_old == 8)
    {
       snprintf(work_mode,sizeof(work_mode),"%s","NORMAL"); 
    } else 
    {
      snprintf(work_mode,sizeof(work_mode),"%s","HIGH"); 
    }
  }
  

  // FANS: 3
  if ((anodes == 6)  or (anodes == 4)  or (anodes == 5))  snprintf(fan_speed,sizeof(fan_speed),"%s","LOW"); 
  if ((anodes == 10) or (anodes == 8)  or (anodes == 9))  snprintf(fan_speed,sizeof(fan_speed),"%s","MEDIUM");  
  if ((anodes == 18) or (anodes == 20) or (anodes == 24) or (anodes == 17)) snprintf(fan_speed,sizeof(fan_speed),"%s","HIGH"); 

  if (anodes == 12)
  {
    if (anodes_old == 8)
    {
       snprintf(fan_speed,sizeof(fan_speed),"%s","MEDIUM"); 
    } else 
    {
      snprintf(fan_speed,sizeof(fan_speed),"%s","LOW"); 
    }
  }





  if (anodes == 0) 
  {
    snprintf(work_mode,sizeof(work_mode),"%s","OFF");
    snprintf(fan_speed,sizeof(fan_speed),"%s","OFF"); 
  }
  


  Serial.printf("[%s]: Mode=%s, FAN speed=%s\n",__func__,work_mode,fan_speed);

  mqtt_publish_text_sensor_values("work_mode", work_mode);
  mqtt_publish_text_sensor_values("fan_speed", fan_speed);


  #ifdef DEBUG

    snprintf(anodes_char,sizeof(anodes_char),"Anodes=%d: a1=%d, a2=%d, a3=%d, a4=%d, a5=%d",anodes,anode_1,anode_2,anode_3,anode_4,anode_5);
    snprintf(anodes_volts,sizeof(anodes_volts),"a1=%0.2fV, a2=%0.2fV, a3=%0.2fV, a4=%0.2fV, a5=%0.2fV",anode_1_volt,anode_2_volt,anode_3_volt,anode_4_volt,anode_5_volt);

    snprintf(q_char,sizeof(q_char),"Qs: q8=%d, q9=%d, q10=%d",q8,q9,q10);
    snprintf(q_volts,sizeof(q_volts),"q8=%0.2fV, q9=%0.2fV, q10=%0.2fV",q8_volt,q9_volt,q10_volt);
    

    mqtt_publish_text_sensor_values("a_state", anodes_char);
    mqtt_publish_text_sensor_values("a_volts", anodes_volts);

    mqtt_publish_text_sensor_values("q_state", q_char);
    mqtt_publish_text_sensor_values("q_volts", q_volts);
  #endif


  if (anodes > 0)
  {
    snprintf(dehum_power,sizeof(dehum_power),"%s","ON");
  } else 
  {
    snprintf(dehum_power,sizeof(dehum_power),"%s","OFF");
  }
  mqtt_publish_text_sensor_values("dehum_power", dehum_power);


  Serial.println("------------ E N D ----------\n");
}

void press_button(u_int8_t gpio)
{
  Serial.printf("[%s]: Pressing GPIO=%d, ",__func__,gpio);
  digitalWrite(gpio,0);
  Serial.printf("delay for 100ms...");
  delay(BUTTON_PRESS_TIME_MS);
  Serial.printf("releasing GPIO=%d\n",gpio);
  digitalWrite(gpio,1);
}





void setup() 
{
  // pinMode(TEST_BUTTON,OUTPUT);
  // digitalWrite(TEST_BUTTON,1);

  prepare_gpio();


  #ifdef STATUS_GW_LED_GPIO_RED
    pinMode(STATUS_GW_LED_GPIO_RED, OUTPUT);
  #endif

  #ifdef POWER_ON_LED_GPIO_GREEN
    // PWM
    #if (POWER_ON_LED_USE_PWM == 1)
      ledcSetup(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_PWM_FREQ, POWER_ON_LED_PWM_RESOLUTION);
      ledcAttachPin(POWER_ON_LED_GPIO_GREEN, POWER_ON_LED_PWM_CHANNEL);
      // set brightness of GREEN LED (0-255)
      ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
    // or fixed
    #else
      pinMode(POWER_ON_LED_GPIO_GREEN, OUTPUT);
      digitalWrite(POWER_ON_LED_GPIO_GREEN, HIGH);
    #endif
  #endif

  bool debug_mode = false;
  // start
  Serial.begin(115200);
  unsigned long sm = millis(); while(millis() < sm + 100) {};

  Serial.printf("\n\n =============================================================\n");
  Serial.printf("[%s]: started, DEVICE_ID=%d, version=%s\n",__func__,DEVICE_ID,ZH_VERSION);

// power for sensors from GPIO - MUST be before any I2C sensor is in use obviously!
  #ifdef ENABLE_3V_GPIO
    Serial.printf("[%s]: enabling ENABLE_3V_GPIO\n",__func__);
    pinMode(ENABLE_3V_GPIO, OUTPUT);
    digitalWrite(ENABLE_3V_GPIO, HIGH);
    delay(10);
  #endif

  #ifdef PUSH_BUTTON_GPIO
    Serial.printf("[%s]: configuring button\n",__func__);
    // any GPIO - set in config.h, observe if active LOW or HIGH
    // GPIO_0 on S, S2 and S3 boards and GPIO_9 on C3 are active LOW (so true below)  
    if (PUSH_BUTTON_GPIO_ACT == HIGH)
    {
      Serial.printf("[%s]: GPIO=%d, active=%d so inverted=false, INPUT_PULLDOWN\n",__func__,PUSH_BUTTON_GPIO,PUSH_BUTTON_GPIO_ACT);
      b = new Button(PUSH_BUTTON_GPIO,false); // false = active HIGH, true = active LOW
      pinMode(PUSH_BUTTON_GPIO,INPUT_PULLDOWN);
    } else 
    {
      Serial.printf("[%s]: GPIO=%d, active=%d so inverted=true, INPUT_PULLUP\n",__func__,PUSH_BUTTON_GPIO,PUSH_BUTTON_GPIO_ACT);
      b = new Button(PUSH_BUTTON_GPIO,true); // false = active HIGH, true = active LOW
      pinMode(PUSH_BUTTON_GPIO,INPUT_PULLUP);
    }
    // pinMode(PUSH_BUTTON_GPIO,INPUT_PULLDOWN);

    // b->setDefaultMinPushTime(150);
    // b->setDefaultMinReleaseTime(150);
    // b->setDefaultTimeSpan(250);
    // b->setDefaultHoldTime(1000);

    // VERY IMPORTANT TIMING: minPushTime, timeSpan, minReleaseTime, interval
    
    // setOnClicked(ButtonEventFunction, uint32_t minPushTime, uint32_t minReleaseTime);
    b->setOnClicked(&OnClicked_cb,100,100); // single click as minimum 200ms otherwise double click is always second

    // setOnDoubleClicked(ButtonEventFunction, uint32_t minPushTime, uint32_t timeSpan);
    // b->setOnDoubleClicked(&OnDoubleClicked_cb,100,1000);
    
    // setOnHolding(ButtonEventFunction, uint32_t interval);
    // b->setOnHolding(&OnHolding_cb,500);
  #endif

  // MAX17048 - fuel gauge
  #if (USE_MAX17048 == 1)
    Wire.begin(); 
    delay(100);
    if (debug_mode)
    {
      lipo.enableDebugging();
      Serial.printf("[%s]: start USE_MAX17048\n",__func__);
    }
    if (! lipo.begin())
    {
      Serial.printf("[%s]: MAX17048 NOT detected ... Check your wiring or I2C ADDR!\n",__func__);
      max17ok = false;
    } else
    {
      Serial.printf("[%s]: start MAX17048 OK\n",__func__);
      if (get_boot_reason() == 1)
      {
        Serial.printf("[%s]: Resetting MAX17048 and applying delay for %dms\n",__func__,MAX17048_DELAY_ON_RESET_MS);
        lipo.reset();
        delay(MAX17048_DELAY_ON_RESET_MS);
        lipo.quickStart(); // required after power off/on for avoiding Wire.cpp errors (and increase of accuracy/guessing?)
      }
    }
  #else
      Serial.printf("[%s]: DONT USE_MAX17048\n",__func__);
  #endif

  // check if device is charging
  snprintf(charging,4,"%s","N/A");
  #if (defined(CHARGING_GPIO) and defined(POWER_GPIO))
    if (debug_mode)
      Serial.printf("[%s]: CHARGING_GPIO and POWER_GPIO enabled\n",__func__);
    pinMode(CHARGING_GPIO, INPUT_PULLDOWN);  //both down: NC initially, will be changed when checked
    pinMode(POWER_GPIO, INPUT_PULLDOWN);
    charging_state();
  #else
    if (debug_mode)
      Serial.printf("[%s]: checking CHARGING DISABLED\n",__func__);
  #endif

  wifi_connected = connect_wifi(STATUS_GW_LED_GPIO_RED);

  // don't start anything until wifi is not connected
  while (!wifi_connected)
  {
    Serial.printf("[%s]: WIFI NOT connected\n",__func__);
    sm = millis(); while(millis() < sm + 1000) {};
  }
  if (debug_mode) Serial.printf("[%s]: WIFI  connected\n",__func__);

  //OTA in Setup
  #ifdef OTA_ACTIVE
    strlcpy(ota_user, OTA_USER, sizeof(ota_user));
    strlcpy(ota_password, OTA_PASSWORD, sizeof(ota_password));
    if (debug_mode) Serial.printf("[%s]: Enabling OTA:...\n",__func__);
    if (debug_mode) Serial.printf("[%s]: user=%s, password=%s\n",__func__,ota_user,ota_password);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      String introtxt = "This is " + String(HOSTNAME)+ ", device ID="+String(DEVICE_ID) +", Version=" + String(ZH_VERSION) + ", MAC=" + String(WiFi.macAddress());
      request->send(200, "text/plain", String(introtxt));
    });
    AsyncElegantOTA.begin(&server, ota_user,ota_password);    // Start ElegantOTA
    server.begin();
  #else
    Serial.printf("[%s]: !!! OTA NOT ENABLED !!! \n",__func__);
  #endif
  //OTA in Setup END

  //start MQTT service
  mqttc.setServer(HA_MQTT_SERVER, 1883);
  mqttc.setBufferSize(MQTT_PAYLOAD_MAX_SIZE);
  mqttc.setCallback(mqtt_callback);
  mqtt_reconnect();
  while (!mqtt_connected)
  {
    Serial.printf("\n[%s]: MQTT NOT connected, waiting...\n",__func__);
    sm = millis(); while(millis() < sm + 1000) {};
  }
  if (debug_mode) Serial.printf("[%s]: MQTT connected\n",__func__);
  //start MQTT service END

  if (debug_mode) Serial.printf("[%s]: configuring DEVICE status in HA:...",__func__);
  if (mqtt_publish_gw_status_config())
  {
    if (debug_mode) Serial.printf("done\n");
  } else
  {
    if (debug_mode) Serial.printf("FAILED\n");
    Serial.printf("[%s]: configuring DEVICE FAILED, restarting in 3s\n",__func__);
    sm = millis(); while(millis() < sm + 3000) {};
    ESP.restart();
  }


  // initial status of DEVICE:
  if (mqtt_publish_gw_status_values("STARTING"))
  {
    sm = millis(); while(millis() < sm + 300) {};
    mqtt_publish_gw_status_values("STARTED");
    sm = millis(); while(millis() < sm + 300) {};
  } else
  {
    Serial.printf("[%s]: start FAILED, restarting in 3s\n",__func__);
    sm = millis(); while(millis() < sm + 3000) {};
    ESP.restart();
  }

  // initial status
  check_modes(true);  

  // initial heartbeat
  heartbeat();

  Serial.printf("[%s]: Setup finished\n",__func__);
  Serial.printf("=============================================================\n");
}

void loop()
{
  wifi_connected = connect_wifi(STATUS_GW_LED_GPIO_RED);
  unsigned long sm = 0;
  if (!mqttc.connected()) {
    mqtt_reconnect();
  }
  mqttc.loop();

  // button
  #ifdef PUSH_BUTTON_GPIO
    b->update();
    if (b->getState() == 1)
    {
      unsigned long now = millis();
      press_time = now - released;
      // setting LEDs as per ESPnow devices (red on green off until 3s, then change, then again at 6s and 9s)
      { 
        if (press_time < 3000)  // RED on, GREEN off
        {
          #ifdef POWER_ON_LED_GPIO_GREEN
            ledcWrite(POWER_ON_LED_PWM_CHANNEL, 0);
          #endif
          #ifdef STATUS_GW_LED_GPIO_RED
            digitalWrite(STATUS_GW_LED_GPIO_RED, HIGH);
          #endif
        } else
        if (press_time < 6000)  // RED off, GREEN on
        {
          #ifdef POWER_ON_LED_GPIO_GREEN
            ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
          #endif
          #ifdef STATUS_GW_LED_GPIO_RED
            digitalWrite(STATUS_GW_LED_GPIO_RED, LOW);
          #endif
        } else
        if (press_time < 9000)  // RED on, GREEN off
        {
          #ifdef POWER_ON_LED_GPIO_GREEN
            ledcWrite(POWER_ON_LED_PWM_CHANNEL, 0);
          #endif
          #ifdef STATUS_GW_LED_GPIO_RED
            digitalWrite(STATUS_GW_LED_GPIO_RED, HIGH);
          #endif
        }
        else  
        if (press_time > 9000)  // RED off, GREEN on
        {
          #ifdef POWER_ON_LED_GPIO_GREEN
            ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
          #endif
          #ifdef STATUS_GW_LED_GPIO_RED
            digitalWrite(STATUS_GW_LED_GPIO_RED, LOW);
          #endif
        }
      }

      // updating HA and Serial
      if (millis() >= aux_pushbutton_interval + PUSHBUTTON_UPDATE_INTERVAL_MS)
      {
        if (press_time < 3000)  // RED on, GREEN off
        {
          Serial.printf("[%s]: click (%ums)->doing nothing?\n",__func__,press_time);
          mqtt_publish_gw_status_values("nothing?");
        } else
        if (press_time < 6000)  // RED off, GREEN on
        {
          Serial.printf("[%s]: click (%ums)->RESTART?\n",__func__,press_time);
          mqtt_publish_gw_status_values("Restart?");
        } else
        if (press_time < 9000)  // RED on, GREEN off
        {
          Serial.printf("[%s]: click (%ums)->FW update?\n",__func__,press_time);
          mqtt_publish_gw_status_values("FW update?");
        }
        else  
        if (press_time > 9000)  // RED off, GREEN on
        {
          Serial.printf("[%s]: click (%ums)->doing nothing again?\n",__func__,press_time);
          mqtt_publish_gw_status_values("too long");
        }
        aux_pushbutton_interval = millis();
      }      
    } else 
    {
      released = millis();
      #ifdef POWER_ON_LED_GPIO_GREEN
        ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
      #endif
      #ifdef STATUS_GW_LED_GPIO_RED
        digitalWrite(STATUS_GW_LED_GPIO_RED, LOW);
      #endif
    }
  #endif

  if (millis() >= aux_heartbeat_interval + (HEARTBEAT_INTERVAL_S * 1000))
  {
    #ifdef POWER_ON_LED_GPIO_GREEN
      ledcWrite(POWER_ON_LED_PWM_CHANNEL, 0);
    #endif
    #ifdef STATUS_GW_LED_GPIO_RED
      digitalWrite(STATUS_GW_LED_GPIO_RED, HIGH);
    #endif
    heartbeat();
    
    #ifdef STATUS_GW_LED_GPIO_RED
      digitalWrite(STATUS_GW_LED_GPIO_RED, LOW);
    #endif
    #ifdef POWER_ON_LED_GPIO_GREEN
      ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
    #endif

    check_modes(true);  
    aux_heartbeat_interval = millis();
  }

  #if (USE_MAX17048 == 1)
    if (millis() >= aux_volts_interval + (VOLTS_INTERVAL_S * 1000))
    {
      #ifdef POWER_ON_LED_GPIO_GREEN
        ledcWrite(POWER_ON_LED_PWM_CHANNEL, 0);
      #endif
      #ifdef STATUS_GW_LED_GPIO_RED
        digitalWrite(STATUS_GW_LED_GPIO_RED, HIGH);
      #endif
      #if (defined(CHARGING_GPIO) and defined(POWER_GPIO))
        charging_state();
      #endif
      measure_volts();
      mqtt_publish_sensors_values();
      aux_volts_interval = millis();
      #ifdef STATUS_GW_LED_GPIO_RED
        digitalWrite(STATUS_GW_LED_GPIO_RED, LOW);
      #endif
      #ifdef POWER_ON_LED_GPIO_GREEN
        ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
      #endif
    }
  #endif
  do_update();

  check_modes(false);  


}