# python 3.6

import random
from re import sub
import time

from paho.mqtt import client as mqtt_client


broker = 'broker.emqx.io'
port = 1883
pub_topic_1 = "esp32/led_2"
sub_topic_1 = "esp32/button_0"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = 'emqx'
password = 'public'

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def publish(client, msg):
     # msg_count = 0
     #while True:
     # time.sleep(1)
     #    msg = f"messages: {msg_count}"
     result = client.publish(pub_topic_1, msg)
     # result: [0, 1]
     status = result[0]
     if status == 0:
          print(f"Send `{msg}` to topic `{pub_topic_1}`")
     else:
          print(f"Failed to send message to topic {pub_topic_1}")
     #    msg_count += 1

def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        if msg.topic == sub_topic_1:
          publish(client, msg.payload.decode())

    client.subscribe(sub_topic_1)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()
#     client.loop_start()
#     publish(client)


if __name__ == '__main__':
    run()
