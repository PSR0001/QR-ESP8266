
#ifndef _CONFIG_H
#define _CLIENT_H

#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#ifdef ESP8266
uint32_t chipID = ESP.getChipId();
#endif

#endif
