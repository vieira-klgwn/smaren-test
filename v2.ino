#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "klgwn";
const char* password = "e11iesag";
const char* mqtt_server = "157.173.101.159";  // same broker

WiFiClient espClient;
PubSubClient client(espClient);

#define FLOW_SENSOR_PIN D2  // Inlet sensor pin

volatile int pulseCount = 0;
float flowRate = 0.0;
unsigned long oldTime = 0;

String mac_address = "BC:DD:C2:79:B9:F9";  // inlet MAC

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP_Inlet")) break;
    delay(5000);
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long currentTime = millis();
  if (currentTime - oldTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = (pulseCount / 7.5);  // L/min

    // Build JSON payload
    StaticJsonDocument<128> doc;
    doc["mac_address"] = mac_address;
    doc["flow_rate_lph"] = flowRate;   // just flow, no volume

    char buffer[128];
    serializeJson(doc, buffer);

    client.publish("smarten/water_reading", buffer);
    Serial.print("Published: ");
    Serial.println(buffer);

    pulseCount = 0;
    oldTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
  }
}
