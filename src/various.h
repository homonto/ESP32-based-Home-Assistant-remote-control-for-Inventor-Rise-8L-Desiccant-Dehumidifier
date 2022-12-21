#pragma once
/*
various functions
*/

// used for gw
void ConvertSectoDay(unsigned long n, char *pretty_ontime)
{
  bool debug_mode = false;
  size_t nbytes;
  int day = n / (24 * 3600);
  n = n % (24 * 3600);
  int hour = n / 3600;
  n %= 3600;
  int minutes = n / 60 ;
  n %= 60;
  int seconds = n;
  if (day>0)
  {
    nbytes = snprintf(NULL,0,"%0dd %0dh %0dm %0ds",day,hour,minutes,seconds)+1;
    snprintf(pretty_ontime,nbytes,"%0dd %0dh %0dm %0ds",day,hour,minutes,seconds);
  } else
  {
    if (hour>0)
    {
      nbytes = snprintf(NULL,0,"%0dh %0dm %0ds",hour,minutes,seconds)+1;
      snprintf(pretty_ontime,nbytes,"%0dh %0dm %0ds",hour,minutes,seconds);
    } else
    {
      if (minutes>0)
      {
        nbytes = snprintf(NULL,0,"%0dm %0ds",minutes,seconds)+1;
        snprintf(pretty_ontime,nbytes,"%0dm %0ds",minutes,seconds);
      } else
      {
        nbytes = snprintf(NULL,0,"%0ds",seconds)+1;
        snprintf(pretty_ontime,nbytes,"%0ds",seconds);
      }
    }
  }
  if (debug_mode)
    Serial.printf("[%s]: pretty_ontime=%s\n",__func__,pretty_ontime);
}

// used for sensors
void uptime(char *uptime)
{
  bool debug_mode = false;
  byte days = 0;
  byte hours = 0;
  byte minutes = 0;
  byte seconds = 0;

  unsigned long sec2minutes = 60;
  unsigned long sec2hours = (sec2minutes * 60);
  unsigned long sec2days = (sec2hours * 24);

  unsigned long time_delta = (millis()) / 1000UL;

  days = (int)(time_delta / sec2days);
  hours = (int)((time_delta - days * sec2days) / sec2hours);
  minutes = (int)((time_delta - days * sec2days - hours * sec2hours) / sec2minutes);
  seconds = (int)(time_delta - days * sec2days - hours * sec2hours - minutes * sec2minutes);

  if ((minutes == 0) and (hours == 0) and (days == 0)){
    sprintf(uptime, "%01ds", seconds);
  }
  else {
    if ((hours == 0) and (days == 0))
    {
      sprintf(uptime, "%01dm %01ds", minutes, seconds);
    }
    else
    {
      if (days == 0)
      {
        sprintf(uptime, "%01dh %01dm", hours, minutes);
      } else
      {
          sprintf(uptime, "%01dd %01dh %01dm", days, hours, minutes);
      }
    }
  }
  if (debug_mode)
    Serial.printf("[%s]: uptime=%s\n",__func__,uptime);

}

byte get_boot_reason()
{
  byte boot_reason = esp_reset_reason();

  // #ifdef DEBUG
  Serial.printf("[%s]: Boot cause=%d - ",__func__,boot_reason);
  switch(boot_reason)
  {
    // 1 = reset/power on
    case ESP_RST_POWERON:
    {
      Serial.printf("power on or reset\n");
      break;
    }
    // 3 = Software reset via esp_restart
    case ESP_RST_SW:
    {
      Serial.printf("Software reset via esp_restart\n");
      break;
    }
    // 8 = deep sleep
    case ESP_RST_DEEPSLEEP:
    {
      Serial.printf("wake up from deep sleep \n");
      break;
    }
    default:
    {
      Serial.printf("other boot cause=%d\n");
      break;
    }
  }
  return boot_reason;
}

#ifdef PUSH_BUTTON_GPIO
// button functions
void OnClicked_cb()
{
  long sm2;
  uint32_t pushed_ms = b->getPushTime();
  char message[255];
  snprintf(message,sizeof(message),"%dms click",pushed_ms);
  if (pushed_ms < 3000) // < 3s
  {
    Serial.printf("[%s]: click (%ums)->doing nothing\n",__func__,pushed_ms);
    mqtt_publish_gw_status_values(message);
  } 
  else
  if (pushed_ms < 6000) // 3s < x < 6s
  {
    Serial.printf("[%s]: click (%ums)->RESTARTING\n",__func__,pushed_ms);
    mqtt_publish_gw_status_values(message);
    ESP.restart();
  } 
  else               
  if (pushed_ms < 9000) // 6s < x < 9s
  {
    Serial.printf("[%s]: click (%ums)->FW update\n",__func__,pushed_ms);
    mqtt_publish_gw_status_values(message);
    perform_update_firmware = true;
  } else                 // x > 9s
  {
    Serial.printf("[%s]: click (%ums)->doing nothing\n",__func__,pushed_ms);
    mqtt_publish_gw_status_values(message);
  }
  mqtt_publish_gw_status_values("online");
}


// void OnDoubleClicked_cb()
// {
//   // getPushTime for doubleclick is the time when double click happened - not timing of click
//   long sm2;
//   uint32_t pushed_ms = b->getPushTime();
//   Serial.printf("[%s]: button DoubleClicked at %ums\n",__func__,pushed_ms);
//   mqtt_publish_gw_status_values("DoubleClick");
//   sm2 = millis(); while(millis() < sm2 + 300) {};
//   mqtt_publish_gw_status_values("online");
// }


// void OnHolding_cb()
// {
//   // interval for onholding is called every interval time, for for 3s it calls every 3s
//   long sm2;
//   uint32_t pushed_ms = b->getPushTime();
//   Serial.printf("[%s]: button holding for %ums\n",__func__,pushed_ms);
//   mqtt_publish_gw_status_values("Held");
//   sm2 = millis(); while(millis() < sm2 + 100) {};

//   if ((pushed_ms > 1000) and (pushed_ms < 3000))  // < 3s
//   {
//     Serial.printf("[%s]: FW update\n",__func__,pushed_ms);
//     // perform_update_firmware = true;
//   } else 
//   if ((pushed_ms > 3000) and (pushed_ms < 10000) and (b->released())) //  3s < x < 10s
//   {
//     Serial.printf("[%s]: RESTARTING\n",__func__,pushed_ms);
//     mqtt_publish_gw_status_values("RESTARTING");
//     sm2 = millis(); while(millis() < sm2 + 500) {};
//     // ESP.restart();
//   } else                // x > 10s 
//   {
//     // something else here
//   }
// }

#endif