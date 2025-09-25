import paho.mqtt.client as mqtt

BROKER = "157.173.101.159"   # same broker you used in Arduino
PORT = 1883
TOPIC = "smarten/water_reading"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Connected to broker")
        client.subscribe(TOPIC)
        print(f"📡 Subscribed to {TOPIC}")
    else:
        print("❌ Connection failed, code:", rc)

def on_message(client, userdata, msg):
    print(f"[MESSAGE] {msg.topic} => {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("🔌 Connecting...")
client.connect(BROKER, PORT, 60)
client.loop_forever()
