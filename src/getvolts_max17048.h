#pragma once
/*
MAX17048 functions
*/

void measure_volts()
{
  #if (USE_MAX17048 == 1)
    bool debug_mode = false;
    snprintf(bat_volts,sizeof(bat_volts),"%0.2f",lipo.getVoltage());
    snprintf(bat_percent,sizeof(bat_percent),"%0.2f",lipo.getSOC());
    snprintf(bat_chr,sizeof(bat_chr),"%0.2f",lipo.getChangeRate());
    if (max17ok)
    {
        if (debug_mode)
        {
            Serial.printf("[%s]: volt=%sV\n",__func__,bat_volts);
            Serial.printf("[%s]: perc=%s%%\n",__func__,bat_percent);
            Serial.printf("[%s]: chrg=%s%%\n",__func__,bat_chr);
        }
    } else 
    {
        Serial.printf("[%s]: MAX17048 NOT OK\n",__func__);
    }   
  #endif
}

// check charging
void charging_state()
{
  #if defined(CHARGING_GPIO) and defined(POWER_GPIO)
  /*
  POWER_GPIO      pin 6, green LED on charger TP4056, HIGH on charging, LOW on full
  CHARGING_GPIO   pin 7, red   LED on charger TP4056, HIGH on full, LOW on charging
  existing LEDs on TP4506 must be still connected to pins of TP4506 otherwise pins are floating, or rewire them to new LEDs on the box
  */
    if ((digitalRead(POWER_GPIO) == 0) and (digitalRead(CHARGING_GPIO) == 0)) {snprintf(charging,sizeof(CHARGING_NC), "%s", CHARGING_NC);}
    if ((digitalRead(POWER_GPIO) == 0) and (digitalRead(CHARGING_GPIO) == 1)) {snprintf(charging,sizeof(CHARGING_FULL), "%s", CHARGING_FULL);}
    if ((digitalRead(POWER_GPIO) == 1) and (digitalRead(CHARGING_GPIO) == 0)) {snprintf(charging,sizeof(CHARGING_ON), "%s", CHARGING_ON);}
    if ((digitalRead(POWER_GPIO) == 1) and (digitalRead(CHARGING_GPIO) == 1)) {snprintf(charging,sizeof(CHARGING_OFF), "%s", CHARGING_OFF);}
    #ifdef DEBUG
      Serial.printf("[%s]: charging=%s\n",__func__,charging);
    #endif
  #endif
}