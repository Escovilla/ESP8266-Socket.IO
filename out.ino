#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTTYPE    DHT11 
SocketIOclient socketIO;

const char* ssid = "pass";
const char* password = "ssid";
#define dht_dpin D6

DHT dht(dht_dpin, DHTTYPE);
float t = 0.0;
float h = 0.0;
#define USE_SERIAL Serial

long actor1;
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      USE_SERIAL.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      USE_SERIAL.printf("[IOc] get event: %s\n", payload);
      break;
    case sIOtype_ACK:
      USE_SERIAL.printf("[IOc] get ack: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      USE_SERIAL.printf("[IOc] get error: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      USE_SERIAL.printf("[IOc] get binary: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
      hexdump(payload, length);
      break;
  }
}
void setup() {
  USE_SERIAL.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  String ip = WiFi.localIP().toString();
  USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());
  socketIO.begin("testappnani.herokuapp.com", 80, "/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);
}
 
unsigned long messageTimestamp = 0;

void loop() {
  socketIO.loop();

  uint64_t now = millis();

  if (now - messageTimestamp > 100) {
    float newT = dht.readTemperature();
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    
    messageTimestamp = now;
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    array.add("event_name");
    JsonObject param1 = array.createNestedObject();
    JsonObject param2 = array.createNestedObject();

    actor1 = random(100, 142);
    param1["id"] = 20;
    param2["data"] = t;

    String output;
    serializeJson(doc, output);
    socketIO.sendEVENT(output);
    USE_SERIAL.println(output);
  }
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}
