import paho.mqtt.client as mqtt

# MQTT broker (same as in ESP code)
BROKER = "157.173.101.159"   # or your local broker IP
TOPIC = "home/valve"

def publish_message(message):
    client = mqtt.Client()
    client.connect(BROKER, 1883, 60)
    client.publish(TOPIC, message)
    client.disconnect()
    print(f"Sent '{message}' to topic {TOPIC}")

if __name__ == "__main__":
    while True:
        cmd = input("Type ON to open valve, OFF to close, EXIT to quit: ").strip().upper()
        if cmd in ["ON", "OFF"]:
            publish_message(cmd)
        elif cmd == "EXIT":
            break
        else:
            print("Invalid command. Use ON, OFF, or EXIT.")
