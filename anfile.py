import paho.mqtt.client as mqtt
import json

BROKER = "localhost"   # or your broker IP
PORT = 1883
TOPIC = "leakage/combined"

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        print("Received from leakage/combined:")
        print(json.dumps(data, indent=2))
    except Exception as e:
        print(f"Error decoding message: {e}")

client = mqtt.Client()
client.on_message = on_message
client.connect(BROKER, PORT, 60)

client.subscribe(TOPIC)

print(f"Listening for messages on {TOPIC}...")
client.loop_forever()
