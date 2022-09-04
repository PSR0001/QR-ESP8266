#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <memory>

// Global variable
String clientId = "ESP" + String(ESP.getChipId());

const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{t}</title>";
const char HTTP_STYLE[] PROGMEM = "<style>body{text-align:center;font-family:verdana;}.a{max-width:350px;margin:auto}button{border:0;border-radius:5px;background-color:#0fe07b;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}input{padding:5px;font-size:1em;width:95%;}</style>";
const char HTTP_SCRIPT[] PROGMEM = "";
const char HTTP_HEADER_END[] PROGMEM = "</head><body><div class=\"a\">";
const char HTTP_H1[] PROGMEM = "<h2>{h}</h2>";
const char HTTP_MAIN_PAGE[] PROGMEM = "<hr><br><a href=\"/w\"><button>Connect to WiFi</button></a><br><br><a href=\"/q\"><button>Scan QR</button></a>";

const char HTTP_FORM_START[] PROGMEM = "<hr><form method='get'action='/s'><input id='s'name='s'length=32 placeholder='SSID'><br><br><input id='p'name='p'length=64 type='password'placeholder='Password'><br><br><button type='submit'>save</button></form>";
const char HTTP_QR_START[] PROGMEM = "";

const char HTTP_FORM_PARAM[] PROGMEM = "";
const char HTTP_FORM_END[] PROGMEM = "";
const char HTTP_END[] PROGMEM = "</div></body></html>";

// "/W" === WIFI SETUP
// "/q" === QR CODE


String ssid = "";
String password = "";

// ESP Server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Function Prototype
void handleRoot();
void handleWifi();
String allInfo();
void serverRoute();
void dnsStart();
void handleQr();
void handleWifiDetails();



void setup()
{
  delay(1000);
  Serial.begin(115200);
  dnsStart();
  serverRoute();
}

void connectWifi()
{
  Serial.println(F("Connecting as wifi client..."));
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  Serial.print("connRes: ");
  Serial.println(connRes);
}

void loop()
{

  // Do work:
  // DNS
  dnsServer.processNextRequest();
  // HTTP
  server.handleClient();
}

void dnsStart(){
    Serial.println(F("Configuring access point..."));

  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(clientId);
  delay(500); // !important

  Serial.print(F("AP IP address: "));
  Serial.println(WiFi.softAPIP());

  // SET TTL time 300 for accessing domain name
  dnsServer.setTTL(300);
  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "www.espqr.com", apIP);
  
}

void serverRoute()
{
  server.on("/", handleRoot);
  server.on("/w", handleWifi);
  server.on("/q", handleQr);
  server.on("/s", handleWifiDetails);  
  server.on("/fwlink", handleRoot);        
  server.onNotFound(handleRoot);
  server.begin(); // Web server start
  Serial.println(F("HTTP server started"));
}

void handleRoot()
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{t}", "WiFi | QR");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_H1);
  page.replace("{h}", "QR Code | WiFi");
  page += FPSTR(HTTP_MAIN_PAGE);
  page += FPSTR(HTTP_END);

  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);
}
void handleWifi()
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{t}", "WiFi");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_H1);
  page.replace("{h}", "WiFi 📶");
  page += FPSTR(HTTP_FORM_START);
  page += FPSTR(HTTP_END);

  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);
}
void handleQr()
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{t}", "QR-Code");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_H1);
  page.replace("{h}", "QR-Code");
  page += FPSTR(HTTP_QR_START);
  page += FPSTR(HTTP_END);

  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);
}

void handleWifiDetails(){


  //SAVE/connect here
  ssid = server.arg("s").c_str();
  password = server.arg("p").c_str();



Serial.println(ssid);
Serial.println(password);
  //parameters
  // for (int i = 0; i < _paramsCount; i++) {
  //   // if (_params[i] == NULL) {
  //   //   break;
  //   // }
  //   //read parameter
  //   String value = server.arg(_params[i]->getID()).c_str();
  //   //store it in array
  //   value.toCharArray(_params[i]->_value, _params[i]->_length + 1);
  //   Serial.println(F("Parameter"));
  //   Serial.print(_params[i]->getID());
  //   Serial.print(value);
  // }

  // if (server->arg("ip") != "") {
  //   DEBUG_WM(F("static ip"));
  //   DEBUG_WM(server->arg("ip"));
  //   //_sta_static_ip.fromString(server->arg("ip"));
  //   String ip = server->arg("ip");
  //   optionalIPFromString(&_sta_static_ip, ip.c_str());
  // }
  // if (server->arg("gw") != "") {
  //   DEBUG_WM(F("static gateway"));
  //   DEBUG_WM(server->arg("gw"));
  //   String gw = server->arg("gw");
  //   optionalIPFromString(&_sta_static_gw, gw.c_str());
  // }
  // if (server->arg("sn") != "") {
  //   DEBUG_WM(F("static netmask"));
  //   DEBUG_WM(server->arg("sn"));
  //   String sn = server->arg("sn");
  //   optionalIPFromString(&_sta_static_sn, sn.c_str());
  // }

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_END);

  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);

  Serial.print(F("Sent wifi save page"));

  //connect = true; //signal ready to connect/reset




}

/** Handle the info page */
String allInfo()
{
  String info = "";
  info += ESP.getChipId();
  info += "\n";
  info += ESP.getFlashChipId();
  info += "\n";
  info += ESP.getFlashChipSize();
  info += "\n";
  info += ESP.getFlashChipRealSize();
  info += "\n";
  info += WiFi.softAPIP().toString();
  info += "\n";
  info += WiFi.softAPmacAddress();
  info += "\n";
  info += WiFi.macAddress();
  return info;
}

