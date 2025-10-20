# We used these coes in the pitching, they really worked

import serial
import time
import paho.mqtt.client as mqtt

# MQTT broker info
BROKER = "test.mosquitto.org"
TOPIC = "smarten/water_reading"

# Serial port for your Arduino
SERIAL_PORT = "/dev/ttyACM0"  # adjust this (use `ls /dev/tty*` to find it)
BAUD_RATE = 9600

# Initialize MQTT client
client = mqtt.Client()
client.connect(BROKER, 1883, 60)

# Connect to Arduino
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
print(f"Connected to Arduino on {SERIAL_PORT}")

while True:
    line = ser.readline().decode().strip()
    if line:
        try:
            client.publish(TOPIC, line)
            print(f"Published: {line}")
        except Exception as e:
            print("Error:", e)
    time.sleep(0.1)









