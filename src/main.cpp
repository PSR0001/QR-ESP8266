#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <memory>
#include <qrcode.h>
// Global variable
String clientId = "ESP" + String(ESP.getChipId());

const char HTTP_HEADER[] PROGMEM                = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{t}</title>";
const char HTTP_STYLE[] PROGMEM                 = "<style>#qrcode{width:160px;height:160px;margin-top:15px;}body{text-align:center;font-family:verdana;}.a{max-width:350px;margin:auto}button{border:0;border-radius:5px;background-color:#0fe07b;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}input{padding:5px;font-size:1em;width:95%;}</style>";
const char HTTP_SCRIPT_START[] PROGMEM          = "<script>let qrcode = new QRCode(\"qrcode\"); qrcode.makeCode(\"";
const char HTTP_SCRIPT_END[] PROGMEM            = "\");</script>";
const char HTTP_HEADER_END[] PROGMEM            = "</head><body><div class=\"a\">";
const char HTTP_H1[] PROGMEM                    = "<h2>{h}</h2>";
const char HTTP_MAIN_PAGE[] PROGMEM             = "<hr><br><a href=\"/w\"><button>Connect to WiFi</button></a><br><br><a href=\"/q\"><button>Scan QR</button></a>";
const char HTTP_SAVED[] PROGMEM                 = "Credentials Saved";
const char HTTP_FORM_START[] PROGMEM            = "<hr><form method='get'action='/s'><input id='s'name='s'length=32 placeholder='SSID'><br><br><input id='p'name='p'length=64 type='password'placeholder='Password'><br><br><button type='submit'>save</button></form>";
// const char HTTP_QR_START[] PROGMEM              = "<hr>Scan the QR code <img src='https://api.qrserver.com/v1/create-qr-code/?size=200x200&data=";
// const char HTTP_QR_END[] PROGMEM                = "'alt='QR'/>";
const char HTTP_QR_LIB[] PROGMEM                = "<div id=\"qrcode\"></div>";
const char HTTP_END[] PROGMEM                   = "</div></body></html>";

// "/W" === WIFI SETUP
// "/q" === QR CODE

String ssid;
String password;
bool connect=0;
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

byte connectWifi()
{
  Serial.println(F("Connecting as wifi client..."));
  // WiFi.disconnect();

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)
    return true;
  else
    return false;
}

void loop()
{
  // DNS
  dnsServer.processNextRequest();
  // HTTP
  server.handleClient();
}

void dnsStart()
{
  // Serial.println(F("Configuring access point..."));
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(clientId);
  delay(500); // !important

  Serial.print(F("AP IP address: "));
  Serial.println(WiFi.softAPIP());

  // SET TTL time 300 for accessing domain name
  dnsServer.setTTL(300);
  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  // dnsServer.start(DNS_PORT, "www.espqrwifi.com", apIP);
  dnsServer.start(DNS_PORT, "*", apIP);
}

void serverRoute()
{
  server.on("/", handleRoot);
  server.on("/w", handleWifi);
  server.on("/q", handleQr);
  server.on("/s", handleWifiDetails);
  //server.on("/f", handleRoot);
  server.onNotFound(handleRoot);
  server.begin(); // Web server start
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
  String A = allInfo();
  String page = FPSTR(HTTP_HEADER);
  page.replace("{t}", "QR-Code");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_H1);
  page.replace("{h}", "QR-Code");
  // page += FPSTR(HTTP_QR_START);
  // page += FPSTR(HTTP_QR_END);
  page += FPSTR(HTTP_QR_LIB);
  page +=FPSTR(QR_CODE);
  page+=FPSTR(HTTP_SCRIPT_START);
  page+=A;
  page+=FPSTR(HTTP_SCRIPT_END);
  page += FPSTR(HTTP_END);

  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);
}


void handleWifiDetails()
{
  // SAVE/connect here
  ssid = server.arg("s").c_str();
  password = server.arg("p").c_str();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_SAVED);
  page += FPSTR(HTTP_END);
  server.sendHeader("Content-Length", String(page.length()));
  server.send(200, "text/html", page);
  if (connectWifi())
  {
    connect = true;
  }
}

/** Handle the info page */
String allInfo()
{
  String info = "";
  info += String(ESP.getChipId()).c_str();
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
