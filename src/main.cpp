/***
 * https://youtu.be/48RW4JHMXUA
 * Diese Version verwendet fauxmoESP v3.0.0. Mit dieser Version
 * lässt sich neben dem Device ein Parameter angeben (in %). Dieser
 * wird als Wert von 0 bis 255 übergeben.
**/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <RCSwitch.h>
#include "fauxmoESP.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include "secrets.h"

#define SERIAL_BAUDRATE 115200
#define LED 2

fauxmoESP fauxmo;

ESP8266WebServer server(80);

// Dip Switches Listening to: ID 1119583 / 1119519
// 1 2 3 4 5
// 1 0 1 0 1
RCSwitch mySwitch = RCSwitch();

bool run_switch = false;
unsigned char switch_device_id = 0;
bool switch_state = false;
char switch_value = 0;

// store switch states
bool switch_A_state = false;
bool switch_B_state = false;
bool switch_C_state = false;
bool switch_D_state = false;

// -----------------------------------------------------------------------------
// WLAN SETUP
// -----------------------------------------------------------------------------

void wifiSetup()
{
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("Verbindungs zu %s wird aufgebaut ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  // Connected
  Serial.println();
  Serial.printf("Verbunden! SSID: %s, IP Adresse: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

bool getDeviceState(unsigned char device_id)
{
  bool state = false;

  // Alle Lichter
  if (device_id == 0)
  {
    state = switch_A_state && switch_B_state && switch_C_state && switch_D_state;
  }

  // Wohnzimmerlicht
  else if (device_id == 1)
  {
    state = switch_A_state && switch_B_state;
  }

  // Sofalicht
  else if (device_id == 2)
  {
    state = switch_B_state;
  }

  // Lichterkette
  else if (device_id == 3)
  {
    state = switch_A_state;
  }

  // Türlicht
  else if (device_id == 4)
  {
    state = switch_C_state;
  }

  // Galerielicht
  else if (device_id == 5)
  {
    state = switch_D_state;
  }

  return state;
}

void sendDeviceState(unsigned char device_id, String device_name, String device_name_id, bool state)
{
  String state_str = state ? "on" : "off";
  char content_buffer[160];

  sprintf(content_buffer, "{ \"device_id\": \"%d\", \"device_name\": \"%s\", \"device_name_id\": \"%s\", \"state\": \"%s\" }", device_id, device_name.c_str(), device_name_id.c_str(), state_str.c_str());

  server.send(200, "application/json", content_buffer);
}

void callbackSetState(unsigned char device_id, String device_name, bool state, unsigned char value)
{
  Serial.print("Device: ");
  Serial.println(device_name);
  Serial.print("State: ");
  Serial.println(state);
  Serial.print("Value: ");
  Serial.println(value);

  run_switch = true;
  switch_device_id = device_id;
  switch_state = state;
  switch_value = value;
}

void handleDeviceAction(unsigned char device_id, String device_name, String device_name_id, bool state)
{
  callbackSetState(device_id, device_name, state, 0);
  sendDeviceState(device_id, device_name, device_name_id, state);
}

void handleWebRequests()
{
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);

  Serial.println(message);
}

void setupWebserver(void)
{
  if (MDNS.begin("Switch Station"))
  {
    Serial.println("MDNS responder started");
  }

  // Overview
  server.serveStatic("/", SPIFFS, "/www/index.html");
  server.serveStatic("/index.html", SPIFFS, "/www/index.html");
  server.serveStatic("/styles.css", SPIFFS, "/www/styles.css");
  server.serveStatic("/scripts.js", SPIFFS, "/www/scripts.js");

  // Switch devices
  server.on("/switch/alle-lichter/on", []() {
    handleDeviceAction(0, "Alle Lichter", "alle-lichter", true);
  });

  server.on("/switch/alle-lichter/off", []() {
    handleDeviceAction(0, "Alle Lichter", "alle-lichter", false);
  });

  server.on("/switch/wohnzimmerlicht/on", []() {
    handleDeviceAction(1, "Wohnzimmerlicht", "wohnzimmerlicht", true);
  });

  server.on("/switch/wohnzimmerlicht/off", []() {
    handleDeviceAction(1, "Wohnzimmerlicht", "wohnzimmerlicht", false);
  });

  server.on("/switch/sofalicht/on", []() {
    handleDeviceAction(2, "Sofalicht", "sofalicht", true);
  });

  server.on("/switch/sofalicht/off", []() {
    handleDeviceAction(2, "Sofa & Regal", "sofalicht", false);
  });

  server.on("/switch/lichterkette/on", []() {
    handleDeviceAction(3, "Lichterkette", "lichterkette", true);
  });

  server.on("/switch/lichterkette/off", []() {
    handleDeviceAction(3, "Lichterkette", "lichterkette", false);
  });

  server.on("/switch/tuerlicht/on", []() {
    handleDeviceAction(4, "Türlicht", "tuerlicht", true);
  });

  server.on("/switch/tuerlicht/off", []() {
    handleDeviceAction(4, "Türlicht", "tuerlicht", false);
  });

  server.on("/switch/galerielicht/on", []() {
    handleDeviceAction(5, "Galerielicht", "galerielicht", true);
  });

  server.on("/switch/galerielicht/off", []() {
    handleDeviceAction(5, "Galerielicht", "galerielicht", false);
  });

  // Get state
  server.on("/state/alle-lichter", []() {
    bool state = getDeviceState(0);

    sendDeviceState(0, "Alle Lichter", "alle-lichter", state);
  });

  server.on("/state/wohnzimmerlicht", []() {
    bool state = getDeviceState(1);

    sendDeviceState(1, "Wohnzimmerlicht", "wohnzimmerlicht", state);
  });

  server.on("/state/sofalicht", []() {
    bool state = getDeviceState(2);

    sendDeviceState(2, "Sofa & Regal", "sofalicht", state);
  });

  server.on("/state/lichterkette", []() {
    bool state = getDeviceState(3);

    sendDeviceState(3, "Lichterkette", "lichterkette", state);
  });

  server.on("/state/tuerlicht", []() {
    bool state = getDeviceState(4);

    sendDeviceState(4, "Türlicht", "tuerlicht", state);
  });

  server.on("/state/galerielicht", []() {
    bool state = getDeviceState(5);

    sendDeviceState(5, "Galerielicht", "galerielicht", state);
  });

  // 404
  server.onNotFound(handleWebRequests);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void setupDevices()
{
  fauxmo.addDevice("Alle Lichter");    //ID 0
  fauxmo.addDevice("Wohnzimmerlicht"); //ID 1
  fauxmo.addDevice("Sofalicht");       //ID 2
  fauxmo.addDevice("Lichterkette");    //ID 3
  fauxmo.addDevice("Türlicht");        //ID 4
  fauxmo.addDevice("Galerielicht");    //ID 5

  fauxmo.onSetState([](unsigned char device_id, String device_name, bool state, unsigned char value) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name.c_str(), state ? "ON" : "OFF", value);
    digitalWrite(LED, !state);
  });

  fauxmo.onSetState(callbackSetState);
}

void handleSwitchRequest(unsigned char device_id, bool state)
{
  if (run_switch == false)
  {
    return;
  }

  // do not run in next loop
  run_switch = false;

  // Enable
  if (state)
  {
    // Alle Lichter
    if (device_id == 0)
    {
      Serial.println("Alle Lichter AN");
      mySwitch.send(switch_send_A_on, 24);
      delay(700);
      switch_A_state = true;
      mySwitch.send(switch_send_B_on, 24);
      delay(700);
      switch_B_state = true;
      mySwitch.send(switch_send_C_on, 24);
      delay(700);
      switch_C_state = true;
      mySwitch.send(switch_send_D_on, 24);
      delay(700);
      switch_D_state = true;
    }

    // Wohnzimmerlicht
    else if (device_id == 1)
    {
      Serial.println("Wohnzimmerlicht AN");
      mySwitch.send(switch_send_A_on, 24);
      delay(700);
      switch_A_state = true;
      mySwitch.send(switch_send_B_on, 24);
      delay(700);
      switch_B_state = true;
    }

    // Sofalicht
    else if (device_id == 2)
    {
      Serial.println("Sofalicht AN");
      mySwitch.send(switch_send_B_on, 24);
      delay(700);
      switch_B_state = true;
    }

    // Lichterkette
    else if (device_id == 3)
    {
      Serial.println("Lichterkette AN");
      mySwitch.send(switch_send_A_on, 24);
      delay(700);
      switch_A_state = true;
    }

    // Eingangslicht
    else if (device_id == 4)
    {
      Serial.println("Türlicht AN");
      mySwitch.send(switch_send_C_on, 24);
      switch_C_state = true;
      delay(50);
    }

    // Galerielicht
    else if (device_id == 5)
    {
      Serial.println("Galerielicht AN");
      mySwitch.send(switch_send_D_on, 24);
      delay(700);
      switch_D_state = true;
    }
  }

  // Disable
  else
  {
    // Alle Lichter
    if (device_id == 0)
    {
      Serial.println("Alle Lichter AUS");
      mySwitch.send(switch_send_A_off, 24);
      delay(700);
      switch_A_state = false;
      mySwitch.send(switch_send_B_off, 24);
      delay(700);
      switch_B_state = false;
      mySwitch.send(switch_send_C_off, 24);
      delay(700);
      switch_C_state = false;
      mySwitch.send(switch_send_D_off, 24);
      delay(700);
      switch_D_state = false;
    }

    // Wohnzimmerlicht
    else if (device_id == 1)
    {
      Serial.println("Wohnzimmerlicht AUS");
      mySwitch.send(switch_send_A_off, 24);
      delay(700);
      switch_A_state = false;
      mySwitch.send(switch_send_B_off, 24);
      delay(700);
      switch_B_state = false;
    }

    // Sofalicht
    else if (device_id == 2)
    {
      Serial.println("Sofalicht AUS");
      mySwitch.send(switch_send_B_off, 24);
      delay(700);
      switch_B_state = false;
    }

    // Lichterkette
    else if (device_id == 3)
    {
      Serial.println("Lichterkette AUS");
      mySwitch.send(switch_send_A_off, 24);
      delay(700);
      switch_A_state = false;
    }

    // Türlicht
    else if (device_id == 4)
    {
      Serial.println("Türlicht AUS");
      mySwitch.send(switch_send_C_off, 24);
      delay(700);
      switch_C_state = false;
    }

    // Galerielicht
    else if (device_id == 5)
    {
      Serial.println("Galerielicht AUS");
      mySwitch.send(switch_send_D_off, 24);
      delay(700);
      switch_D_state = false;
    }
  }
}

bool loadFromSpiffs(String path)
{
  String dataType = "text/plain";

  if (path.endsWith("/"))
    path += "index.html";

  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";

  File dataFile = SPIFFS.open(path.c_str(), "r");

  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
  }

  dataFile.close();

  return true;
}

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Nach dem Verbinden, sage 'Echo, schalte <Gerät> an' oder 'aus'");

  // Send on GPIO2
  mySwitch.enableTransmit(2);

  // Wifi
  wifiSetup();

  // LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);
  fauxmo.enable(false);
  fauxmo.enable(true);

  setupDevices();
  SPIFFS.begin();
  setupWebserver();
}

void loop()
{
  // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
  // default, this means that it uses WiFiUdp class instead of AsyncUDP.
  // The later requires the Arduino Core for ESP8266 staging version
  // whilst the former works fine with current stable 2.3.0 version.
  // But, since it's not "async" anymore we have to manually poll for UDP
  // packets
  fauxmo.handle();
  server.handleClient();

  handleSwitchRequest(switch_device_id, switch_state);
}
