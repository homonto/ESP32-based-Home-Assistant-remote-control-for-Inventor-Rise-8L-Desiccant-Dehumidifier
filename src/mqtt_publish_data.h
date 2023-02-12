#pragma once
/*
mqtt functions for device
*/

// this device (device) definition to be able to create devices in HA
//MQTT_DEVICE_IDENTIFIER is in setup after wifi as MAC is needed
#define CREATE_GW_MQTT_DEVICE \
dev = config.createNestedObject("device"); \
dev["ids"]=MQTT_DEVICE_IDENTIFIER;  \
dev["name"]=String(ROLE_NAME) + "_" + String(HOSTNAME); \
dev["mdl"]=MODEL; \
dev["mf"]="ZH"; \
dev["sw"]= String(ZH_VERSION)+ "," + String(__DATE__) + "_" + String(__TIME__);
// device device definition END

// device status config - sent by heartbeat() periodically to HA
bool mqtt_publish_gw_status_config()
{
  bool debug_mode = false;
  bool publish_status = false;      //changes locally in each config publish
  bool total_publish_status = true; //changes globally in each config publish when any fails

  // mac
  char mac_conf_topic[60];
  snprintf(mac_conf_topic,sizeof(mac_conf_topic),"homeassistant/sensor/%s/mac/config",HOSTNAME);
  if (debug_mode) Serial.println("mac_conf_topic="+String(mac_conf_topic));

  char mac_name[30];
  snprintf(mac_name,sizeof(mac_name),"%s_mac",HOSTNAME);
  if (debug_mode) Serial.println("mac_name="+String(mac_name));

  // status
  char status_conf_topic[60];
  snprintf(status_conf_topic,sizeof(status_conf_topic),"homeassistant/sensor/%s/status/config",HOSTNAME);
  if (debug_mode) Serial.println("status_conf_topic="+String(status_conf_topic));

  char status_name[30];
  snprintf(status_name,sizeof(status_name),"%s_mystatus",HOSTNAME);
  if (debug_mode) Serial.println("status_name="+String(status_name));

  // uptime
  char uptime_conf_topic[60];
  snprintf(uptime_conf_topic,sizeof(uptime_conf_topic),"homeassistant/sensor/%s/uptime/config",HOSTNAME);
  if (debug_mode) Serial.println("uptime_conf_topic="+String(uptime_conf_topic));

  char uptime_name[30];
  snprintf(uptime_name,sizeof(uptime_name),"%s_uptime",HOSTNAME);
  if (debug_mode) Serial.println("uptime_name="+String(uptime_name));

  // version
  char version_conf_topic[60];
  snprintf(version_conf_topic,sizeof(version_conf_topic),"homeassistant/sensor/%s/version/config",HOSTNAME);
  if (debug_mode) Serial.println("version_conf_topic="+String(version_conf_topic));

  char version_name[30];
  snprintf(version_name,sizeof(version_name),"%s_version",HOSTNAME);
  if (debug_mode) Serial.println("version_name="+String(version_name));

  // rssi of device<->AP
  char rssi_conf_topic[60];
  snprintf(rssi_conf_topic,sizeof(rssi_conf_topic),"homeassistant/sensor/%s/rssi/config",HOSTNAME);
  if (debug_mode) Serial.println("rssi_conf_topic="+String(rssi_conf_topic));

  char rssi_name[30];
  snprintf(rssi_name,sizeof(rssi_name),"%s_rssi",HOSTNAME);
  if (debug_mode) Serial.println("rssi_name="+String(rssi_name));

  // commont topic
  char status_state_topic[60];
  snprintf(status_state_topic,sizeof(status_state_topic),"%s/sensor/state",HOSTNAME);
  if (debug_mode) Serial.println("status_state_topic="+String(status_state_topic));

  StaticJsonDocument<JSON_CONFIG_SIZE> config;
  JsonObject dev;
  int size_c;
  char config_json[JSON_CONFIG_SIZE];

// gw mac config
  config.clear();
  config["name"] = mac_name;
  config["stat_t"] = status_state_topic;
  config["val_tpl"] = "{{value_json.mac}}";
  config["uniq_id"] = mac_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(mac_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,mac_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,mac_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG MAC ============");
    Serial.println("Size of mac config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n MAC CONFIG OK");
    } else
    {
      Serial.println("\n PRETTYONTIME CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG MAC END ========\n");
  }

// status config
  config.clear();
  config["name"] = status_name;
  config["stat_t"] = status_state_topic;
  config["qos"] = 2;
  config["uniq_id"] = status_name;
  config["val_tpl"] = "{{value_json.status}}";
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(status_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,status_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,status_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [STATUS] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [STATUS] CONFIG END ========\n");
  }

// uptime config
  config.clear();
  config["name"] = uptime_name;
  config["stat_t"] = status_state_topic;
  config["qos"] = 2;
  config["retain"] = "true";
  config["uniq_id"] = uptime_name;
  config["val_tpl"] = "{{value_json.uptime}}";
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(uptime_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,uptime_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,uptime_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [UPTIME] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [UPTIME] CONFIG END ========\n");
  }

// version config
  config.clear();
  config["name"] = version_name;
  config["stat_t"] = status_state_topic;
  config["qos"] = 2;
  config["retain"] = "true";
  config["uniq_id"] = version_name;
  config["val_tpl"] = "{{value_json.version}}";
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(version_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,version_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,version_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [VERSION] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [VERSION] CONFIG END ========\n");
  }

// rssi config
  config.clear();
  config["name"] = rssi_name;
  config["dev_cla"] = "signal_strength";
  config["stat_cla"] = "measurement";
  config["stat_t"] = status_state_topic;
  config["unit_of_meas"] = "dBm";
  config["val_tpl"] = "{{value_json.rssi}}";
  config["uniq_id"] = rssi_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(rssi_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,rssi_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,rssi_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [RSSI] CONFIG  ============");
    Serial.println("Size of config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [RSSI] CONFIG END ========\n");
  }

  return total_publish_status;
}


// device status value
bool mqtt_publish_gw_status_values(const char* status)
{
  bool debug_mode = false;
  bool publish_status = false;

  char status_state_topic[60];
  snprintf(status_state_topic,sizeof(status_state_topic),"%s/sensor/state",HOSTNAME);
  if (debug_mode) Serial.println("status_state_topic="+String(status_state_topic));

  if (!mqtt_publish_gw_status_config()){
    Serial.println("\n GW STATUS CONFIG NOT published");
    return false;
  }

  StaticJsonDocument<JSON_PAYLOAD_SIZE> payload;
  payload["status"] = status;
  payload["rssi"] = WiFi.RSSI();
  payload["mac"]  = WiFi.macAddress();

  char ret_clk[60];
  uptime(ret_clk);
  payload["uptime"] = ret_clk;

  payload["version"] = ZH_VERSION;

  char payload_json[JSON_PAYLOAD_SIZE];
  int size_pl = serializeJson(payload, payload_json);

  if (!mqttc.publish(status_state_topic,(uint8_t*)payload_json,strlen(payload_json), true))
  {
    publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,status_state_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,status_state_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [STATUS] PAYLOAD ============");
    Serial.println("Size of payload="+String(size_pl)+" bytes");
    Serial.println("Serialised payload_json:");
    Serial.println(payload_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(payload, Serial);
    if (publish_status) {
      Serial.println("\n STATUS VALUES published OK");
    } else
    {
      Serial.println("\n STATUS VALUES NOT published");
    }
    Serial.println("============ DEBUG [STATUS] PAYLOAD END ========\n");
  }

  return publish_status;
}


// device FW update - button on HA
bool mqtt_publish_button_update_config()
{
  bool debug_mode = false;
  bool publish_status = false;      //changes locally in each config publish
  bool total_publish_status = true; //changes globally in each config publish when any fails

  char update_conf_topic[60];
  snprintf(update_conf_topic,sizeof(update_conf_topic),"homeassistant/button/%s/update/config",HOSTNAME);
  if (debug_mode) Serial.println("update_conf_topic="+String(update_conf_topic));

  char update_name[30];
  snprintf(update_name,sizeof(update_name),"%s_update",HOSTNAME);
  if (debug_mode) Serial.println("update_name="+String(update_name));

  char update_state_topic[60];
  snprintf(update_state_topic,sizeof(update_state_topic),"%s/button/update",HOSTNAME);
  if (debug_mode) Serial.println("update_state_topic="+String(update_state_topic));

  char update_cmd_topic[30];
  snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);
  if (debug_mode) Serial.println("update_cmd_topic="+String(update_cmd_topic));

  StaticJsonDocument<JSON_CONFIG_SIZE> config;
  JsonObject dev;
  int size_c;
  char config_json[JSON_CONFIG_SIZE];

// update config
  config.clear();
  config["name"] = update_name;
  config["command_topic"] = update_cmd_topic;
  config["payload_press"] = "ON";
  config["qos"] = "2";
  config["retain"] = "true";
  config["uniq_id"] = update_name;
  config["val_tpl"] = "{{value_json.update}}";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);


  if (!mqttc.publish(update_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,update_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,update_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [UPDATE] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [UPDATE] CONFIG END ========\n");
  }
  return total_publish_status;
}


// device restart - button on HA
bool mqtt_publish_button_restart_config()
{
  bool debug_mode = false;
  bool publish_status = false;      //changes locally in each config publish
  bool total_publish_status = true; //changes globally in each config publish when any fails

  char restart_conf_topic[60];
  snprintf(restart_conf_topic,sizeof(restart_conf_topic),"homeassistant/button/%s/restart/config",HOSTNAME);
  if (debug_mode) Serial.println("restart_conf_topic="+String(restart_conf_topic));

  char restart_name[30];
  snprintf(restart_name,sizeof(restart_name),"%s_restart",HOSTNAME);
  if (debug_mode) Serial.println("restart_name="+String(restart_name));

  char restart_state_topic[60];
  snprintf(restart_state_topic,sizeof(restart_state_topic),"%s/button/restart",HOSTNAME);
  if (debug_mode) Serial.println("restart_state_topic="+String(restart_state_topic));

  char restart_cmd_topic[30];
  snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);
  if (debug_mode) Serial.println("restart_cmd_topic="+String(restart_cmd_topic));

  StaticJsonDocument<JSON_CONFIG_SIZE> config;
  JsonObject dev;
  int size_c;
  char config_json[JSON_CONFIG_SIZE];

// restart config
  config.clear();
  config["name"] = restart_name;
  config["command_topic"] = restart_cmd_topic;
  config["payload_press"] = "ON";
  config["qos"] = "2";
  config["retain"] = "true";
  config["uniq_id"] = restart_name;
  config["val_tpl"] = "{{value_json.restart}}";
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(restart_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,restart_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,restart_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [RESTART] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CONFIG published OK");
    } else
    {
      Serial.println("\n CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [RESTART] CONFIG END ========\n");
  }
  return total_publish_status;
}


// device data
void heartbeat()
{
  bool debug_mode = false;
  bool publish_status = true;
  #ifdef STATUS_GW_LED_GPIO_RED
    digitalWrite(STATUS_GW_LED_GPIO_RED,HIGH);
  #endif
  char pretty_ontime[17]; // "999d 24h 60m 60s" = 16 characters
  ConvertSectoDay(millis()/1000,pretty_ontime);
  Serial.printf("[%s]: updating device status %s after boot...",__func__,pretty_ontime);

  publish_status = publish_status && mqtt_publish_gw_status_values("online");
  publish_status = publish_status && mqtt_publish_button_update_config();
  publish_status = publish_status && mqtt_publish_button_restart_config();
  publish_status = publish_status && mqtt_publish_config_all_buttons();

  if (publish_status)
  {
    Serial.printf("SUCCESSFULL\n");
  } else
  {
    Serial.printf("FAILED\n");
  }

  #ifdef STATUS_GW_LED_GPIO_RED
    digitalWrite(STATUS_GW_LED_GPIO_RED,LOW);
  #endif
}


// power config
bool mqtt_publish_sensors_config()
{
    bool debug_mode = false;

    bool publish_status = false;      //changes locally in each config publish
    bool total_publish_status = true; //changes globally in each config publish when any fails

    StaticJsonDocument<JSON_CONFIG_SIZE> config;
    JsonObject dev;
    int size_c;
    char config_json[JSON_CONFIG_SIZE];

    //topics
    char bat_conf_topic[60];
    snprintf(bat_conf_topic,sizeof(bat_conf_topic),"homeassistant/sensor/%s/battery/config",HOSTNAME);
    if (debug_mode) Serial.println("bat_conf_topic="+String(bat_conf_topic));

    char batpct_conf_topic[60];
    snprintf(batpct_conf_topic,sizeof(batpct_conf_topic),"homeassistant/sensor/%s/batterypercent/config",HOSTNAME);
    if (debug_mode) Serial.println("batpct_conf_topic="+String(batpct_conf_topic));

    char batchr_conf_topic[60];
    snprintf(batchr_conf_topic,sizeof(batchr_conf_topic),"homeassistant/sensor/%s/batchr/config",HOSTNAME);
    if (debug_mode) Serial.println("batchr_conf_topic="+String(batchr_conf_topic));

    char charging_conf_topic[60];
    snprintf(charging_conf_topic,sizeof(charging_conf_topic),"homeassistant/sensor/%s/charging/config",HOSTNAME);
    if (debug_mode) Serial.println("charging_conf_topic="+String(charging_conf_topic));
    
    // names
    char bat_name[30];
    snprintf(bat_name,sizeof(bat_name),"%s_battery",HOSTNAME);
    if (debug_mode) Serial.println("bat_name="+String(bat_name));

    char batpct_name[30];
    snprintf(batpct_name,sizeof(batpct_name),"%s_batterypercent",HOSTNAME);
    if (debug_mode) Serial.println("batpct_name="+String(batpct_name));

    char batchr_name[30];
    snprintf(batchr_name,sizeof(batchr_name),"%s_batchr",HOSTNAME);
    if (debug_mode) Serial.println("batchr_name="+String(batchr_name));

    char charging_name[30];
    snprintf(charging_name,sizeof(charging_name),"%s_charging",HOSTNAME);
    if (debug_mode) Serial.println("charging_name="+String(charging_name));

    // values/state topic
    char power_sensors_topic_state[60];
    snprintf(power_sensors_topic_state,sizeof(power_sensors_topic_state),"%s/sensor/power",HOSTNAME);
    if (debug_mode) Serial.println("power_sensors_topic_state="+String(power_sensors_topic_state));

    // battery volts config
    config.clear();
    config["name"] = bat_name;
    config["dev_cla"] = "voltage";
    config["stat_cla"] = "measurement";
    config["stat_t"] = power_sensors_topic_state;
    config["unit_of_meas"] = "V";
    config["val_tpl"] = "{{value_json.battery}}";
    config["uniq_id"] = bat_name;
    config["frc_upd"] = "true";
    config["entity_category"] = "diagnostic";
    // config["exp_aft"] = 900;

    CREATE_GW_MQTT_DEVICE

    size_c = serializeJson(config, config_json);

    if (!mqttc.publish(bat_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
    {
        publish_status = false; total_publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,bat_conf_topic);
    } else
    {
        publish_status = true;
        if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,bat_conf_topic);}
    }

    if (debug_mode) {
        Serial.println("\n============ DEBUG CONFIG BATTERY ============");
        Serial.println("Size of battery config="+String(size_c)+" bytes");
        Serial.println("Serialised config_json:");
        Serial.println(config_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(config, Serial);
        if (publish_status) {
        Serial.println("\n BATTERY CONFIG OK");
        } else
        {
        Serial.println("\n BATTERY CONFIG UNSUCCESSFULL");
        }
        Serial.println("============ DEBUG CONFIG BATTERY END ========\n");
    }

    // batterypercent config
    config.clear();
    config["name"] = batpct_name;
    config["dev_cla"] = "battery";
    config["stat_cla"] = "measurement";
    config["stat_t"] = power_sensors_topic_state;
    config["unit_of_meas"] = "%";
    config["val_tpl"] = "{{value_json.batterypercent}}";
    config["uniq_id"] = batpct_name;
    config["frc_upd"] = "true";
    config["entity_category"] = "diagnostic";
    // config["exp_aft"] = 900;

    CREATE_GW_MQTT_DEVICE

    size_c = serializeJson(config, config_json);

    if (!mqttc.publish(batpct_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
    {
        publish_status = false; total_publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,batpct_conf_topic);
    } else
    {
        publish_status = true;
        if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,batpct_conf_topic);}
    }

    if (debug_mode) {
        Serial.println("\n============ DEBUG CONFIG BATTERY PERCENT============");
        Serial.println("Size of battery percent config="+String(size_c)+" bytes");
        Serial.println("Serialised config_json:");
        Serial.println(config_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(config, Serial);
        if (publish_status) {
        Serial.println("\n BATTERY PERCENT CONFIG OK");
        } else
        {
        Serial.println("\n BATTERY PERCENT CONFIG UNSUCCESSFULL");
        }
        Serial.println("============ DEBUG CONFIG BATTERY PERCENT END ========\n");
    }

    // batchr config
    config.clear();
    config["name"] = batchr_name;
    config["stat_cla"] = "measurement";
    config["stat_t"] = power_sensors_topic_state;
    config["unit_of_meas"] = "%";
    config["val_tpl"] = "{{value_json.batchr}}";
    config["uniq_id"] = batchr_name;
    config["frc_upd"] = "true";
    config["entity_category"] = "diagnostic";
    // config["exp_aft"] = 900;

    CREATE_GW_MQTT_DEVICE

    size_c = serializeJson(config, config_json);

    if (!mqttc.publish(batchr_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
    {
        publish_status = false; total_publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,batchr_conf_topic);
    } else
    {
        publish_status = true;
        if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,batchr_conf_topic);}
    }

    if (debug_mode) {
        Serial.println("\n============ DEBUG CONFIG BATCHR PERCENT============");
        Serial.println("Size of batchr config="+String(size_c)+" bytes");
        Serial.println("Serialised config_json:");
        Serial.println(config_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(config, Serial);
        if (publish_status) {
        Serial.println("\n BATCHR CONFIG OK");
        } else
        {
        Serial.println("\n BATCHR CONFIG UNSUCCESSFULL");
        }
        Serial.println("============ DEBUG CONFIG BATCHR END ========\n");
    }

    // charging config
    config.clear();
    config["name"] = charging_name;
    config["stat_t"] = power_sensors_topic_state;
    config["val_tpl"] = "{{value_json.charging}}";
    config["uniq_id"] = charging_name;
    config["frc_upd"] = "true";
    config["entity_category"] = "diagnostic";
    // config["exp_aft"] = 900;

    CREATE_GW_MQTT_DEVICE

    size_c = serializeJson(config, config_json);

    if (!mqttc.publish(charging_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
    {
        publish_status = false; total_publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,charging_conf_topic);
    } else
    {
        publish_status = true;
        if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,charging_conf_topic);}
    }

    if (debug_mode) {
        Serial.println("\n============ DEBUG CONFIG CHARGING ============");
        Serial.println("Size of charging config="+String(size_c)+" bytes");
        Serial.println("Serialised config_json:");
        Serial.println(config_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(config, Serial);
        if (publish_status) {
        Serial.println("\n CHARGING CONFIG OK");
        } else
        {
        Serial.println("\n CHARGING CONFIG UNSUCCESSFULL");
        }
        Serial.println("============ DEBUG CONFIG CHARGING END ========\n");
    }

    return total_publish_status;
}


// power data
bool mqtt_publish_sensors_values()
{
    bool debug_mode = false;
    if (!mqtt_publish_sensors_config())
    {
        Serial.printf("[%s]: CONFIG NOT published, leaving\n",__func__);
        return false;
    } else
    {
        if (debug_mode) Serial.printf("[%s]: CONFIG published\n",__func__);
    }    

    // values topic
    char power_sensors_topic_state[60];
    snprintf(power_sensors_topic_state,sizeof(power_sensors_topic_state),"%s/sensor/power",HOSTNAME);
    if (debug_mode) Serial.println("power_sensors_topic_state="+String(power_sensors_topic_state));
    
    StaticJsonDocument<JSON_PAYLOAD_SIZE> payload;
    
    #if (USE_MAX17048 == 1)
      payload["battery"] = bat_volts;
      payload["batterypercent"] = bat_percent;
      payload["batchr"] = bat_chr;
    #else 
      payload["battery"] = "N/A";
      payload["batterypercent"] = "N/A";
      payload["batchr"] = "N/A";
    #endif

    #if defined(CHARGING_GPIO) and defined(POWER_GPIO)
      payload["charging"] = charging;
    #else 
      payload["charging"] = "N/A";
    #endif

    char payload_json[JSON_PAYLOAD_SIZE];
    int size_pl = serializeJson(payload, payload_json);

    if (debug_mode) {
        Serial.println("\n============ DEBUG PAYLOAD SENSORS============");
        Serial.println("Size of sensors payload="+String(size_pl)+" bytes");
        Serial.println("Serialised payload_json:");
        Serial.println(payload_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(payload, Serial);
        Serial.println("============ DEBUG PAYLOAD SENSORS END ========\n");
    }

    if (!mqttc.publish(power_sensors_topic_state,(uint8_t*)payload_json,strlen(payload_json), true))
    {
        Serial.printf("[%s]: -> sending data from \"%s\" to HA - FAILED, leaving\n",__func__,HOSTNAME);
        return false;
    }

    Serial.printf("[%s]: -> sending data from \"%s\" to HA - SUCCESSFULL\n",__func__,HOSTNAME);

    return true;
    
}



// universal button on HA
bool mqtt_publish_button_config(const char* button)
{
  bool debug_mode = false;
  bool publish_status = false;      //changes locally in each config publish
  bool total_publish_status = true; //changes globally in each config publish when any fails

  char button_conf_topic[100];
  snprintf(button_conf_topic,sizeof(button_conf_topic),"homeassistant/button/%s/%s/config",HOSTNAME,button);
  if (debug_mode) Serial.println("button_conf_topic="+String(button_conf_topic));

  char button_name[30];
  snprintf(button_name,sizeof(button_name),"%s_%s",HOSTNAME,button);
  if (debug_mode) Serial.println("button_name="+String(button_name));

  char button_state_topic[60];
  snprintf(button_state_topic,sizeof(button_state_topic),"%s/button/%s",HOSTNAME,button);
  if (debug_mode) Serial.println("button_state_topic="+String(button_state_topic));

  char button_cmd_topic[30];
  snprintf(button_cmd_topic,sizeof(button_cmd_topic),"%s/cmd/%s",HOSTNAME,button);
  if (debug_mode) Serial.println("button_cmd_topic="+String(button_cmd_topic));

  char button_val_tpl[30];
  snprintf(button_val_tpl,sizeof(button_val_tpl),"{{value_json.%s}}",button);
  if (debug_mode) Serial.println("button_val_tpl="+String(button_val_tpl));

  StaticJsonDocument<JSON_CONFIG_SIZE> config;
  JsonObject dev;
  int size_c;
  char config_json[JSON_CONFIG_SIZE];

// button config
  config.clear();
  config["name"] = button_name;
  config["command_topic"] = button_cmd_topic;
  config["payload_press"] = "ON";
  config["qos"] = "2";
  config["retain"] = "false";
  config["uniq_id"] = button_name;
  config["val_tpl"] = button_val_tpl;
  // config["exp_aft"] = 60;

  CREATE_GW_MQTT_DEVICE

  size_c = serializeJson(config, config_json);

  if (!mqttc.publish(button_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
  {
    publish_status = false; total_publish_status = false;
    Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,button_conf_topic);
  } else
  {
    publish_status = true;
    if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,button_conf_topic);}
  }

  if (debug_mode) {
    Serial.println("\n============ DEBUG [BUTTON] CONFIG:  ============");
    Serial.println("Size of config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n BUTTON CONFIG published OK");
    } else
    {
      Serial.println("\n BUTTON CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG [BUTTON] CONFIG END ========\n");
  }
  return total_publish_status;
}

// universal TEXT sensor (no unit)
bool mqtt_publish_text_sensor_config(const char* sensor)
{
    bool debug_mode = false;

    bool publish_status = false;      //changes locally in each config publish
    bool total_publish_status = true; //changes globally in each config publish when any fails

    StaticJsonDocument<JSON_CONFIG_SIZE> config;
    JsonObject dev;
    int size_c;
    char config_json[JSON_CONFIG_SIZE];

    //topics
    char sensor_conf_topic[100];
    snprintf(sensor_conf_topic,sizeof(sensor_conf_topic),"homeassistant/sensor/%s/%s/config",HOSTNAME,sensor);
    if (debug_mode) Serial.println("sensor_conf_topic="+String(sensor_conf_topic));

    
    // names
    char sensor_name[30];
    snprintf(sensor_name,sizeof(sensor_name),"%s_%s",HOSTNAME,sensor);
    if (debug_mode) Serial.println("sensor_name="+String(sensor_name));


    // values/state topic
    char sensor_topic_state[60];
    snprintf(sensor_topic_state,sizeof(sensor_topic_state),"%s/sensor/%s",HOSTNAME,sensor);
    if (debug_mode) Serial.println("sensor_topic_state="+String(sensor_topic_state));

    // val_tpl 
    char sensor_val_tpl[30];
    snprintf(sensor_val_tpl,sizeof(sensor_val_tpl),"{{value_json.%s}}",sensor);
    if (debug_mode) Serial.println("sensor_val_tpl="+String(sensor_val_tpl));

    // battery volts config
    config.clear();
    config["name"] = sensor_name;
    // config["dev_cla"] = "voltage";
    // config["stat_cla"] = "measurement";
    config["stat_t"] = sensor_topic_state;
    // config["unit_of_meas"] = "V";
    config["val_tpl"] = sensor_val_tpl;
    config["uniq_id"] = sensor_name;
    config["frc_upd"] = "true";
    // config["entity_category"] = "diagnostic";
    // config["exp_aft"] = 900;

    CREATE_GW_MQTT_DEVICE

    size_c = serializeJson(config, config_json);

    if (!mqttc.publish(sensor_conf_topic,(uint8_t*)config_json,strlen(config_json), true))
    {
        publish_status = false; total_publish_status = false;
        Serial.printf("[%s]: PUBLISH FAILED for %s\n",__func__,sensor_conf_topic);
    } else
    {
        publish_status = true;
        if (debug_mode) {Serial.printf("[%s]: PUBLISH SUCCESSFULL for %s\n",__func__,sensor_conf_topic);}
    }

    if (debug_mode) {
        Serial.println("\n============ DEBUG CONFIG SENSOR ============");
        Serial.println("Size of battery config="+String(size_c)+" bytes");
        Serial.println("Serialised config_json:");
        Serial.println(config_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(config, Serial);
        if (publish_status) {
        Serial.println("\n SENSOR CONFIG OK");
        } else
        {
        Serial.println("\n SENSOR CONFIG UNSUCCESSFULL");
        }
        Serial.println("============ DEBUG CONFIG SENSOR END ========\n");
    }

    return total_publish_status;
}

// universal TEXT sensor data
bool mqtt_publish_text_sensor_values(const char* sensor, const char* value)
{
    bool debug_mode = false;
    if (!mqtt_publish_text_sensor_config(sensor))
    {
        Serial.printf("[%s]: CONFIG NOT published, leaving\n",__func__);
        return false;
    } else
    {
        if (debug_mode) Serial.printf("[%s]: CONFIG published\n",__func__);
    }    

    // values topic
    char text_sensor_topic_state[60];
    snprintf(text_sensor_topic_state,sizeof(text_sensor_topic_state),"%s/sensor/%s",HOSTNAME,sensor);
    if (debug_mode) Serial.println("text_sensor_topic_state="+String(text_sensor_topic_state));
    
    StaticJsonDocument<JSON_PAYLOAD_SIZE> payload;
    
    payload[sensor] = value;


    char payload_json[JSON_PAYLOAD_SIZE];
    int size_pl = serializeJson(payload, payload_json);

    if (debug_mode) {
        Serial.println("\n============ DEBUG PAYLOAD SENSORS============");
        Serial.println("Size of sensors payload="+String(size_pl)+" bytes");
        Serial.println("Serialised payload_json:");
        Serial.println(payload_json);
        Serial.println("serializeJsonPretty");
        serializeJsonPretty(payload, Serial);
        Serial.println("============ DEBUG PAYLOAD SENSORS END ========\n");
    }

    if (!mqttc.publish(text_sensor_topic_state,(uint8_t*)payload_json,strlen(payload_json), true))
    {
        Serial.printf("[%s]: -> sending data from \"%s\" to HA - FAILED, leaving\n",__func__,HOSTNAME);
        return false;
    }

    Serial.printf("[%s]: -> sending data from \"%s\" to HA - SUCCESSFULL\n",__func__,HOSTNAME);

    return true;
    
}

bool mqtt_publish_config_all_buttons()
{
  bool total_publish_status = true;
  // publish power button config
  Serial.printf("[%s]: mqtt_publish_button_config (power) in HA:...",__func__);
  if (mqtt_publish_button_config("power"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish fan button config
  Serial.printf("[%s]: mqtt_publish_button_config (fan) in HA:...",__func__);
  if (mqtt_publish_button_config("fan"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish mode button config
  Serial.printf("[%s]: mqtt_publish_button_config (mode) in HA:...",__func__);
  if (mqtt_publish_button_config("mode"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish swing button config
  Serial.printf("[%s]: mqtt_publish_button_config (swing) in HA:...",__func__);
  if (mqtt_publish_button_config("swing"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish timer button config
  Serial.printf("[%s]: mqtt_publish_button_config (timer) in HA:...",__func__);
  if (mqtt_publish_button_config("timer"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish ion button config
  Serial.printf("[%s]: mqtt_publish_button_config (ion) in HA:...",__func__);
  if (mqtt_publish_button_config("ion"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish work_mode config
  Serial.printf("[%s]: mqtt_publish_text_sensor_config (work_mode) in HA:...",__func__);
  if (mqtt_publish_text_sensor_config("work_mode"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  // publish fan_speed config
  Serial.printf("[%s]: mqtt_publish_text_sensor_config (fan_speed) in HA:...",__func__);
  if (mqtt_publish_text_sensor_config("fan_speed"))
  {
      Serial.printf("done\n");
  } else
  {
      Serial.printf("FAILED\n");
      total_publish_status = false;
  }

  return total_publish_status;
}