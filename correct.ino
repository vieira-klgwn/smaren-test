#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

const char* ssid = "Main Hall";
const char* password = "Meeting@2024";
const char* mqtt_server = "157.173.101.159";  // same broker

WiFiClient espClient;
PubSubClient client(espClient);

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600, 60000);  // offset +3h (change if needed)

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
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  timeClient.begin();   // start NTP
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
  timeClient.update();

  unsigned long currentTime = millis();
  if (currentTime - oldTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = (pulseCount / 7.5);  // L/min

    // Get UNIX time from NTP client
    time_t now = timeClient.getEpochTime();
    struct tm* timeinfo = localtime(&now);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Build JSON payload
    StaticJsonDocument<200> doc;
    doc["mac_address"] = mac_address;
    doc["flow_rate_lph"] = flowRate;
    doc["timestamp"] = timestamp;

    char buffer[200];
    serializeJson(doc, buffer);

    client.publish("smarten/water_reading", buffer);
    Serial.print("Published: ");
    Serial.println(buffer);

    pulseCount = 0;
    oldTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
  }
}
