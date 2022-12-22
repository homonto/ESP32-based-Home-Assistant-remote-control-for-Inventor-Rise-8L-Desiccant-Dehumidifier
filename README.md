Modified "Inventor Rise 8L Desiccant Dehumidifier with Ioniser (WEE/MM0449AA)" to have remote control from Home Assistant.
This device is without any remote control so we decided to implement some and to make it fully controllable from HA.

In current version working are:
- turn ON/OFF
- switch Modes (ECO, NORMAL, HIGH, TURBO)
- switch Fan Speeds (LOW, MEDIUM, HIGH)

MQTT with discovery so there is no need to configure anything on Home Assistant.
WiFi credentials to be set up on first run via Captive Portal.

Still beta version as not everything is yet implemented - TODO:
- swing
- ion
- timer
<img width="891" alt="Screenshot 2022-12-21 at 22 00 47" src="https://user-images.githubusercontent.com/46562447/209010618-0617cb95-4162-4fe3-8d19-2527e822cb96.png">

<img width="885" alt="Screenshot 2022-12-21 at 22 00 38" src="https://user-images.githubusercontent.com/46562447/209010598-ca298738-348a-4421-b76e-c58ac97754cb.png">


<img width="886" alt="Screenshot 2022-12-21 at 22 01 06" src="https://user-images.githubusercontent.com/46562447/209010634-f0927e58-ec1d-4e47-93c9-38c0e499df45.png">

