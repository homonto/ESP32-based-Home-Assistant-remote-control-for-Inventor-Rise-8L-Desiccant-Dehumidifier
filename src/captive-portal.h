#ifndef captive_portal_h
#define captive_portal_h

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <Preferences.h>
#include <nvs_flash.h>

#ifndef CP_TIMEOUT_S
    #define CP_TIMEOUT_S                    180             // Captive Portal will terminate after this time and ESP will restart
#endif
#ifndef WIF_STA_CONNECTION_TIMEOUT_S
    #define WIF_STA_CONNECTION_TIMEOUT_S    10              // WiFi STA will give up after this time and CP will start
#endif
#ifndef HOSTNAME
  #define HOSTNAME                          "ESP-CAPTIVE-PORTAL"
#endif

// heap variable - LED GPIO
int * heapLED_GPIO;             // it is NOT given to xtask - it is used as global variable - work on it to be a parameter!

Preferences preferences;
DNSServer dnsServer;
AsyncWebServer cp_server(80);

String ssid_str;
String password_str;
bool is_setup_done = false;
bool valid_ssid_received = false;
bool valid_password_received = false;
bool cp_wifi_timeout = false;

// timers
TimerHandle_t cp_timer_handle     = NULL;
int id                            = 1;

// tasks
TaskHandle_t led_blink_handle     = NULL;
BaseType_t xReturned_led_blink;

// declarations
void CaptivePortalServer();
void StartWiFiAP();
void StartWiFiSTA(String rec_ssid, String rec_password);
void StartAPandCaptivePortal();
bool connect_wifi(u_int8_t error_led);
void erase_nvs();
void cp_timer( TimerHandle_t cp_timer_handle );
void start_cp_timer();
void led_blink_cp(void *pvParams);
void read_wifi_credentials();
void write_wifi_credentials(bool wifi_ok_local, String ssid_str_local, String password_str_local);
void stop_cp_timer();

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Captive Portal Demo</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Captive Portal Demo</h3>
  <br><br>
  <form action="/get">
    <br>
    SSID: <input type="text" name="ssid">
    <br>
    Password: <input type="password" name="password">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html);
    }
};

void CaptivePortalServer()
{
  cp_server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
    Serial.printf("[%s]: Client Connected\n",__func__);
  });

  cp_server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("ssid")) {
      inputMessage = request->getParam("ssid")->value();
      inputParam = "ssid";
      ssid_str = inputMessage;
      Serial.println(inputMessage);
      valid_ssid_received = true;
    }

    if (request->hasParam("password")) {
      inputMessage = request->getParam("password")->value();
      inputParam = "password";
      password_str = inputMessage;
      Serial.println(inputMessage);
      valid_password_received = true;
    }
    request->send(200, "text/html", "The values entered by you have been successfully sent to the device. It will now attempt WiFi connection");
  });
}

void StartWiFiAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(HOSTNAME);
  Serial.printf("[%s]: SSID: ",__func__); Serial.println(HOSTNAME);
  Serial.printf("[%s]: AP IP address: ",__func__); Serial.println(WiFi.softAPIP());
    // blink LEDs in task, if not yet blinking
  if (led_blink_handle == NULL)
  {
    xReturned_led_blink = xTaskCreate(led_blink_cp, "led_blink_cp", 2000, NULL, 1, &led_blink_handle);
    if( xReturned_led_blink != pdPASS )
    {
      Serial.printf("[%s]: CANNOT create led_blink task\n",__func__);
    } else
    {
      // #ifdef DEBUG
        Serial.printf("[%s]: Task led_blink created, blinking LED GPIO=%d\n",__func__,*heapLED_GPIO);
      // #endif
    }
  } else
  // blink LEDs already blinking
  {
    Serial.printf("[%s]: Task led_blink ALREADY created\n",__func__);
  }
}

void StartWiFiSTA(String rec_ssid, String rec_password)
{
  cp_wifi_timeout = false;
  WiFi.mode(WIFI_STA);
  char ssid_arr[20];
  char password_arr[20];
  rec_ssid.toCharArray(ssid_arr, rec_ssid.length() + 1);
  rec_password.toCharArray(password_arr, rec_password.length() + 1);
  WiFi.begin(ssid_arr, password_arr);

  uint32_t t1 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(20);
    Serial.print(".");
    if (millis() - t1 > (WIF_STA_CONNECTION_TIMEOUT_S * 1000))
    {
      Serial.printf("\n[%s]: Timeout connecting to WiFi. The SSID and Password seem incorrect\n",__func__);
      valid_ssid_received = false;
      valid_password_received = false;
      bool old_is_setup_done = is_setup_done;
      is_setup_done = false;
      if (is_setup_done != old_is_setup_done)
      {
        Serial.printf("[%s]: saving WiFi credentials (is_setup_done=%d): WiFi NOT ok\n",__func__,is_setup_done);
        preferences.begin("my-pref", false); //write
        preferences.putBool("is_setup_done", is_setup_done);
        preferences.end();
        delay(20);
      }
      StartAPandCaptivePortal();
      cp_wifi_timeout = true;
      break;
    }
  }
  if (!cp_wifi_timeout)
  {
    Serial.printf("\n[%s]: WiFi connected to: ",__func__); Serial.println(rec_ssid);
    Serial.printf  ("[%s]: IP address: ",__func__);  Serial.println(WiFi.localIP());
    bool old_is_setup_done = is_setup_done;
    is_setup_done = true;
    if (is_setup_done != old_is_setup_done)
    {
      Serial.printf("[%s]: WiFi OK\n",__func__);
      write_wifi_credentials(true,  rec_ssid, rec_password);
      // stop blinking
      if( led_blink_handle != NULL )
      {
        Serial.printf("[%s]: Disabling blinking LED\n",__func__);
        vTaskDelete( led_blink_handle );
        delay(5);
      } else
      {
        Serial.printf("[%s]: LED is still blinking or was never blinking\n",__func__);
      }
    }

    if(!MDNS.begin(HOSTNAME))
    {
      Serial.printf("[%s]: Error starting MDNS\n",__func__);
    }
  }
}

void StartAPandCaptivePortal() {
  Serial.printf("[%s]: Setting up AP Mode\n",__func__);
  StartWiFiAP();
  Serial.printf("[%s]: Setting up Async WebServer\n",__func__);
  CaptivePortalServer();
  Serial.printf("[%s]: Starting DNS Server\n",__func__);
  dnsServer.start(53, "*", WiFi.softAPIP());
  cp_server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  cp_server.begin();
  dnsServer.processNextRequest();
}

void erase_nvs()
{
  read_wifi_credentials();
  Serial.printf("[%s]: nvs_flash_erase...\n",__func__);
  if (nvs_flash_erase() != ESP_OK)
  {
    Serial.printf("[%s]: nvs_flash_erase FAILED\n",__func__);
  } else
  {
    Serial.printf("[%s]: nvs_flash_erase SUCCEESFUL\n",__func__);
  }
  delay(20);
  if (nvs_flash_init() != ESP_OK)
  {
      Serial.printf("[%s]: nvs_flash_init FAILED\n",__func__);
  } else
  {
    Serial.printf("[%s]: nvs_flash_init SUCCEESFUL\n",__func__);
  }
  write_wifi_credentials(is_setup_done, ssid_str, password_str);
}

void cp_timer( TimerHandle_t cp_timer_handle )
{
  Serial.printf("[%s]: Captive Portal timer expired\n",__func__);
  erase_nvs();
  Serial.printf("[%s]: RESTARTING...\n",__func__);
  ESP.restart();
}

void start_cp_timer()
{
      // create CP timer if not yet created
  if (cp_timer_handle  == NULL)
  {
    cp_timer_handle = xTimerCreate("MyTimer", pdMS_TO_TICKS(CP_TIMEOUT_S * 1000), pdTRUE, ( void * )id, &cp_timer);
    if( xTimerStart(cp_timer_handle, 10 ) != pdPASS )
    {
      Serial.printf("[%s]: CP timer start error\n",__func__);
    } else
    {
    //   #ifdef DEBUG
        Serial.printf("[%s]: CP timer STARTED\n",__func__);
    //   #endif
    }
  } else
  // CP timer created so restart it
  {
    if( xTimerReset( cp_timer_handle, 0 ) != pdPASS )
    {
      Serial.printf("[%s]: CP timer was not reset\n",__func__);
    } else
    {
      Serial.printf("[%s]: CP timer RE-STARTED\n",__func__);
    }
  }
}

void stop_cp_timer()
{
      if( xTimerStop( cp_timer_handle, 0 ) != pdPASS )
    {
      Serial.printf("[%s]: CP timer was NOT stopped\n",__func__);
    } else
    {
      Serial.printf("[%s]: CP timer stopped\n",__func__);
    }
}

void led_blink_cp(void *pvParams)
{
  int delay_ms = 50;
  pinMode(*heapLED_GPIO,OUTPUT);
    while(1)
    {
      //  low
      digitalWrite(*heapLED_GPIO,LOW);

      // delay
      vTaskDelay(delay_ms/portTICK_RATE_MS);

      // red high
      digitalWrite(*heapLED_GPIO,HIGH);

      // delay
      vTaskDelay(delay_ms/portTICK_RATE_MS);
    }
}

void read_wifi_credentials()
{
  Serial.printf("[%s]: reading WiFi credentials...\n",__func__);
  preferences.begin("my-pref", true); //read only
  is_setup_done = preferences.getBool("is_setup_done", false);
  ssid_str = preferences.getString("rec_ssid", "Sample_SSID");
  password_str = preferences.getString("rec_password", "abcdefgh");
  preferences.end();
}

void write_wifi_credentials(bool wifi_ok_local, String ssid_str_local, String password_str_local)
{
  Serial.printf("[%s]: saving WiFi credentials...\n",__func__);
  preferences.begin("my-pref", false); //write
  preferences.putBool("is_setup_done", wifi_ok_local);
  preferences.putString("rec_ssid", ssid_str_local);
  preferences.putString("rec_password", password_str_local);
  preferences.end();
  delay(20);
}


bool connect_wifi(u_int8_t error_led)
{
  if (WiFi.status() == WL_CONNECTED)
  {
     return true;
  }
  heapLED_GPIO = (int *) malloc(sizeof(int));
  *heapLED_GPIO = error_led;

  // start timer
  start_cp_timer();

  read_wifi_credentials();
  Serial.printf("[%s]: Saved SSID is:......... ",__func__);Serial.println(ssid_str);
  Serial.printf("[%s]: Saved Password is:..... ",__func__);Serial.println(password_str);
  Serial.printf("[%s]: Saved wifi status is:.. ",__func__);Serial.println(is_setup_done);

  // lets try first connection anyway
  StartWiFiSTA(ssid_str, password_str);

  if (!is_setup_done)
  {
    StartAPandCaptivePortal();
  }
  else
  {
    Serial.printf("[%s]: Connecting to WiFi using saved credentials...\n",__func__);
    StartWiFiSTA(ssid_str, password_str);
  }

  while (!is_setup_done)
  {
    dnsServer.processNextRequest();
    delay(10);
    if (valid_ssid_received && valid_password_received)
    {
      Serial.printf("[%s]: Attempting WiFi Connection...\n",__func__);
      StartWiFiSTA(ssid_str, password_str);
    }
  }
  free (heapLED_GPIO);
  stop_cp_timer();
  #define MQTT_DEVICE_IDENTIFIER String(WiFi.macAddress())
/* testing only
  // print your subnet mask:
  // IPAddress ip;
  // IPAddress subnet;
  // IPAddress gateway;
  // subnet = WiFi.subnetMask();
  // Serial.print("NETMASK: ");
  // Serial.println(subnet);

  //   // print your gateway address:
  // gateway = WiFi.gatewayIP();
  // Serial.print("GATEWAY: ");
  // Serial.println(gateway);
*/

  return true;
}

#endif