import paho.mqtt.client as mqtt
import json
from datetime import datetime

# Assign MAC addresses to each sensor
SENSOR_MACS = {
    "leakage/inletFlow": "bc:dd:c2:79:b9:f9".upper(),
    "leakage/inletVolume": "bc:dd:c2:79:b9:f9".upper(),
    "leakage/outletFlow": "30:83:98:a5:7b:97".upper(),
    "leakage/outletVolume": "30:83:98:a5:7b:97".upper(),
}

# MQTT setup
BROKER = "157.173.101.159"  # same as frontend/ESP
PORT = 1883
INPUT_TOPICS = [
    "leakage/inletFlow",
    "leakage/outletFlow",
    "leakage/inletVolume",
    "leakage/outletVolume",
]
OUTPUT_TOPIC = "smarten/water_reading"

client = mqtt.Client()

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()

        if msg.topic in INPUT_TOPICS:
            # Try to parse value
            try:
                value = float(payload)
            except ValueError:
                value = None

            mac = SENSOR_MACS.get(msg.topic, "unknown")

            # Format timestamp as "YYYY-MM-DD HH:MM:SS"
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            # Choose field name based on topic
            if "Flow" in msg.topic:
                data = {
                    "mac_address": mac,
                    "flow_rate_lph": value,
                    "timestamp": timestamp,
                }
            elif "Volume" in msg.topic:
                data = {
                    "mac_address": mac,
                    "volume_liters": value,
                    "timestamp": timestamp,
                }
            else:
                data = {
                    "mac_address": mac,
                    "value": value,
                    "timestamp": timestamp,
                }

            # Publish cleaned format
            client.publish(OUTPUT_TOPIC, json.dumps(data))
            print(f"Published: {data}")

        elif msg.topic == OUTPUT_TOPIC:
            print(f"[COMBINED] {msg.payload.decode()}")

    except Exception as e:
        print(f"Error processing message on {msg.topic}: {e}")

client.on_message = on_message
client.connect(BROKER, PORT, 60)

# Subscribe to inlet/outlet topics and the combined one
for t in INPUT_TOPICS:
    client.subscribe(t)
client.subscribe(OUTPUT_TOPIC)

print("Listening for inlet/outlet flow + volume data...")
print(f"Forwarding all to {OUTPUT_TOPIC} in formatted style")

client.loop_forever()
