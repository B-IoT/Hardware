import paho.mqtt.client as mqtt
from paho.mqtt.client import *


# The callback for when the client receives a CONNACK response from the server.
def on_connect_mqtt(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("$SYS/#")
    client.subscribe("update.parameters", 1)

# The callback for when a PUBLISH message is received from the server.
def on_message_mqtt(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client(client_id="relay_P1", clean_session=True, userdata=None, protocol=MQTTv311, transport="tcp")
client.will_set("will", payload="{\"company\": \"biot\"}", qos=0, retain=False)
client.username_pw_set("testP1", "testP1")
client.on_connect = on_connect_mqtt
client.on_message = on_message_mqtt

client.connect("mqtt.b-iot.ch", 1883, 60)



# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()