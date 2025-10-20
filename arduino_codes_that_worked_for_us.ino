// We used these coes in the pitching, they really worked

#define FLOW_SENSOR_PIN 2

volatile int pulseCount = 0;
float flowRate = 0.0;
unsigned long oldTime = 0;

String mac_address = "3A:9F:1B:77:2C:44";// Change this for second Arduino

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - oldTime >= 5000) {  // Every 5 seconds
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = pulseCount / 7.5;  // Convert pulses to L/min
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

    // Create JSON string
    String jsonData = "{\"mac_address\":\"" + mac_address +
                      "\",\"flow_rate_lph\":" + String(flowRate, 2) +
                      ",\"timestamp\":\"" + currentTimestamp() + "\"}";


    Serial.println(jsonData);
    oldTime = currentTime;
  }
}

// Function to generate timestamp
String currentTimestamp() {
  unsigned long now = millis() / 1000;
  unsigned long s = now % 60;
  unsigned long m = (now / 60) % 60;
  unsigned long h = (now / 3600) % 24;
  char buffer[20];
  sprintf(buffer, "2025-10-12 %02lu:%02lu:%02lu", h, m, s);
  return String(buffer);
}