#pragma once
/*
mqtt functions
*/

// mqtt callback
void mqtt_callback(char* topic, byte* message, unsigned int length)
{
  long sm = micros();
  if (!wifi_connected)
  {
    Serial.printf("[%s]: WiFi not connected, leaving...\n",__func__);
    return;
  }

  if (!mqtt_connected)
  {
    Serial.printf("[%s]: MQTT not connected, leaving...\n",__func__);
    return;
  }

  bool publish_status = false;

  char restart_cmd_topic[30];
  snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);

  char update_cmd_topic[60];
  snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);

  char power_cmd_topic[30];
  snprintf(power_cmd_topic,sizeof(power_cmd_topic),"%s/cmd/power",HOSTNAME);

  char fan_cmd_topic[30];
  snprintf(fan_cmd_topic,sizeof(fan_cmd_topic),"%s/cmd/fan",HOSTNAME);

  char mode_cmd_topic[60];
  snprintf(mode_cmd_topic,sizeof(mode_cmd_topic),"%s/cmd/mode",HOSTNAME);

  char swing_cmd_topic[60];
  snprintf(swing_cmd_topic,sizeof(swing_cmd_topic),"%s/cmd/swing",HOSTNAME);

  char timer_cmd_topic[60];
  snprintf(timer_cmd_topic,sizeof(timer_cmd_topic),"%s/cmd/timer",HOSTNAME);

  char ion_cmd_topic[60];
  snprintf(ion_cmd_topic,sizeof(ion_cmd_topic),"%s/cmd/ion",HOSTNAME);


  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)toupper(message[i]);
  }
  Serial.printf("[%s]: MQTT message received:\n\t%s\n\tmessage:\t%s\n",__func__,topic,messageTemp);

  String topic_str = String(topic);

  //restart
  if (String(topic) == restart_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* restart_char="";  // clean restart command before actual restart to avoid the restart loop
      if (!mqttc.publish(restart_cmd_topic,(uint8_t*)restart_char,strlen(restart_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,restart_cmd_topic);
      }
      Serial.flush();
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: RESTARTING on MQTT command\n\n\n",__func__);
        mqtt_publish_gw_status_values("RESTARTING");
        sm2 = millis(); while(millis() < sm2 + 100) {};
        ESP.restart();
      }
    }
  }

  else
  //update
  if (String(topic) == update_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* update_char="";   //clean update topic before actual update to avoid the update loop
      if (!mqttc.publish(update_cmd_topic,(uint8_t*)update_char,strlen(update_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,update_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: UPDATING FIRMWARE on MQTT command\n",__func__);
        perform_update_firmware=true;
      }
    }
  }

  else
  //power
  if (String(topic) == power_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* power_char="";   //clean power topict o avoid the power loop
      if (!mqttc.publish(power_cmd_topic,(uint8_t*)power_char,strlen(power_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,power_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking POWER button on MQTT command\n",__func__);
        press_button(BUTTON_POWER_GPIO);
      }
    }
  }

  else
  //fan
  if (String(topic) == fan_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* fan_char="";   //clean fan topic to avoid the fan loop
      if (!mqttc.publish(fan_cmd_topic,(uint8_t*)fan_char,strlen(fan_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,fan_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking FAN button on MQTT command\n",__func__);
        press_button(BUTTON_FAN_GPIO);
      }
    }
  }

  else
  //mode
  if (String(topic) == mode_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* mode_char="";   //clean mode topic to avoid the mode loop
      if (!mqttc.publish(mode_cmd_topic,(uint8_t*)mode_char,strlen(mode_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,mode_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking MODE button on MQTT command\n",__func__);
        press_button(BUTTON_MODE_GPIO);
      }
    }
  }

  else
  //swing
  if (String(topic) == swing_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* swing_char="";   //clean swing topic to avoid the swing loop
      if (!mqttc.publish(swing_cmd_topic,(uint8_t*)swing_char,strlen(swing_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,swing_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking MODE button on MQTT command\n",__func__);
        press_button(BUTTON_SWING_GPIO);
      }
    }
  }

  else
  //timer
  if (String(topic) == timer_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* timer_char="";   //clean timer topic to avoid the timer loop
      if (!mqttc.publish(timer_cmd_topic,(uint8_t*)timer_char,strlen(timer_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,timer_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking MODE button on MQTT command\n",__func__);
        press_button(BUTTON_TIMER_GPIO);
      }
    }
  } 

  else
  //ion
  if (String(topic) == ion_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* ion_char="";   //clean ion topic to avoid the ion loop
      if (!mqttc.publish(ion_cmd_topic,(uint8_t*)ion_char,strlen(ion_char), true))
      {
        publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED",__func__);
      } else
      {
        publish_status = true;
        Serial.printf("[%s]: PUBLISHED %s\n",__func__,ion_cmd_topic);
      }
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: Clicking MODE button on MQTT command\n",__func__);
        press_button(BUTTON_ION_GPIO);
      }
    }
  }     
 
  long em = micros(); //END measurement time
  long mt=em-sm;
  Serial.printf("[%s]: MQTT CALLBACK TIME: %dus\n\n",__func__,mt);
}


// mqtt reconnect
void mqtt_reconnect()
{
  if (mqttc.connected())
  {
    mqtt_connected = true;
  } else
  {
    int state1 = mqttc.state();
    Serial.printf("[%s]: MQTT NOT connected (ERROR:%d)\n",__func__,state1);
    long sm = millis();
    int mqtt_error = 0;
    mqtt_connected=false;

    while (!mqttc.connected())
    {
      ++mqtt_error;
      Serial.printf("[%s]: Attempting MQTT connection for %d time\n",__func__,mqtt_error);
      long sm1=millis(); while(millis() < sm1 + 1000) {}

      if (mqttc.connect(HOSTNAME,HA_MQTT_USER, HA_MQTT_PASSWORD))
      {
        mqtt_connected = true;
        Serial.printf("[%s]: MQTT connected\n",__func__);
      } else
      {
        if (mqtt_error >= MAX_MQTT_ERROR)
        {
          int state2 = mqttc.state();
          Serial.printf("[%s]: FATAL MQTT ERROR !!!!!!!! (ERROR:%d)\n",__func__,state2);
          Serial.printf("[%s]: restarting ESP...\n\n",__func__);
          ESP.restart();
        }
      }
    }

    // restart
    char restart_cmd_topic[60];
    snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);
    mqttc.subscribe(restart_cmd_topic);

    // update
    char update_cmd_topic[60];
    snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);
    mqttc.subscribe(update_cmd_topic);

    // dehumidifier: power, fan, mode, swing, timer, ion
    // power
    char power_cmd_topic[60];
    snprintf(power_cmd_topic,sizeof(power_cmd_topic),"%s/cmd/power",HOSTNAME);
    mqttc.subscribe(power_cmd_topic);

    // fan
    char fan_cmd_topic[60];
    snprintf(fan_cmd_topic,sizeof(fan_cmd_topic),"%s/cmd/fan",HOSTNAME);
    mqttc.subscribe(fan_cmd_topic);

    // mode
    char mode_cmd_topic[60];
    snprintf(mode_cmd_topic,sizeof(mode_cmd_topic),"%s/cmd/mode",HOSTNAME);
    mqttc.subscribe(mode_cmd_topic);

    // swing
    char swing_cmd_topic[60];
    snprintf(swing_cmd_topic,sizeof(swing_cmd_topic),"%s/cmd/swing",HOSTNAME);
    mqttc.subscribe(swing_cmd_topic);

    // timer
    char timer_cmd_topic[60];
    snprintf(timer_cmd_topic,sizeof(timer_cmd_topic),"%s/cmd/timer",HOSTNAME);
    mqttc.subscribe(timer_cmd_topic);

    // ion
    char ion_cmd_topic[60];
    snprintf(ion_cmd_topic,sizeof(ion_cmd_topic),"%s/cmd/ion",HOSTNAME);
    mqttc.subscribe(ion_cmd_topic);

    Serial.printf("[%s]: Subscribed to:\n",__func__);
    {
      Serial.printf("\t%s\n",restart_cmd_topic);
      Serial.printf("\t%s\n",update_cmd_topic);
      Serial.printf("\t%s\n",power_cmd_topic);
      Serial.printf("\t%s\n",fan_cmd_topic);
      Serial.printf("\t%s\n",mode_cmd_topic);
      Serial.printf("\t%s\n",swing_cmd_topic);
      Serial.printf("\t%s\n",timer_cmd_topic);
      Serial.printf("\t%s\n",ion_cmd_topic);
    }
    

    long em = millis();
    long mt=em-sm;
    Serial.printf("[%s]: MQTT RECONNECT TIME: %dms\n",__func__,mt);
  }
}
