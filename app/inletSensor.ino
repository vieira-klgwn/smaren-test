#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

#define FLOW_SENSOR_PIN D2  // Inlet sensor pin

volatile int pulseCount = 0;
float flowRate = 0.0;
float totalLiters = 0.0;
unsigned long oldTime = 0;

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
  if (currentTime - oldTime >= 1000) { // every 1 second
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = (pulseCount / 7.5);  // L/min
    totalLiters += flowRate / 60.0;  // convert to L/sec

    client.publish("leakage/inletFlow", String(flowRate, 2).c_str());
    client.publish("leakage/inletVolume", String(totalLiters, 2).c_str());

    pulseCount = 0;
    oldTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
  }
}
