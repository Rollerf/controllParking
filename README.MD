# Base project for ESP32 projects
## Description
This is with ESP32.

## Configuration
You'll need your own variables for MQTT and WIFI connection. I use in this format:

WIFI connection file:
const char *ssid = "";
const char *password = "";

MQTT connection file:
const char *mqtt_broker = "";
const char *topicState = "";
const char *topicCommand = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = ;
const char *client_name = "";

## Connections
| Name                | Name in board | GPIO/IDE Arduino name |
|---------------------|---------------|-----------------------|
| Left light	      | IO16          | 16                    |
| Right light         | IO17          | 17                    |
| Radar 0516 sensor   | IO32          | 32                    |
| DTH11 sensor        | IO27          | 27                    |
