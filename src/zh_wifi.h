// #pragma once
// /*
// wifi functions
// */


// void wifi_watchdog(void*z)
// {
//   bool debug_mode = false;
//   Serial.printf("[%s]: WiFi watchdog started, active every %ds\n",__func__,WIFI_WATCHDOG_INTERVAL_S);
//   long ttc, sm1;
//   long wifi_start_time; // = millis();

//   while(1)
//     {
//       wifi_start_time = millis();
//       if (WiFi.status() == WL_CONNECTED)
//       {
//         wifi_connected =  true;
//         if (debug_mode) Serial.printf("[%s]: WiFi connected\n",__func__);
//       } else
//       {
//         Serial.printf("[%s]: WiFi NOT connected\n",__func__);
//         WiFi.disconnect();

//         wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
//         esp_wifi_init(&config);
//         esp_wifi_set_ps(WIFI_PS_NONE);
//         esp_wifi_set_mode(WIFI_MODE_STA);

//         byte mac[6];
//         WiFi.macAddress(mac);
//         char mac1[22];
//         snprintf(mac1, sizeof(mac1), "%02x:%02x:%02x:%02x:%02x:%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//         Serial.printf("[%s]: MAC: %s\n",__func__,mac1);

//         WiFi.begin(BT_SSID, BT_PASSWORD);
//         Serial.printf("[%s]: Connecting to %s ...\n",__func__,BT_SSID);

//         while (WiFi.status() != WL_CONNECTED)
//         {
//           ttc = millis() - wifi_start_time;
//           sm1=millis(); while(millis() < sm1 + 20)
//           {
//             vTaskDelay(pdMS_TO_TICKS(10));
//           }
//           if (ttc > (WAIT_FOR_WIFI_S * 1000)) {
//             Serial.printf("[%s]: NOT connected after %dms\n",__func__,ttc);
//             Serial.printf("[%s]: FATAL WiFi ERROR !!!!!!!!\n",__func__);
//             Serial.printf("[%s]: restarting ESP...\n\n",__func__);
//             ESP.restart();
//           }
//         }
//         Serial.printf("[%s]: CONNECTED after %dms\n",__func__,ttc);
//         Serial.printf("[%s]: Channel: %d\n",__func__,WiFi.channel());
//         IPAddress ip = WiFi.localIP();
//         char ip1[22];
//         snprintf(ip1, sizeof(ip1), "%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]);
//         Serial.printf("[%s]: IP: %s\n",__func__,ip1);

//         wifi_connected =  true;
//         Serial.printf("[%s]: SUCCESSFULL\n",__func__);

//         // get into esp32 by hostname.local
//         if(!MDNS.begin(HOSTNAME)) 
//         {
//           Serial.printf("[%s]: Error starting MDNS\n",__func__);
//         }
//       }
//       vTaskDelay(pdMS_TO_TICKS(WIFI_WATCHDOG_INTERVAL_S * 1000));
//     }
// }
