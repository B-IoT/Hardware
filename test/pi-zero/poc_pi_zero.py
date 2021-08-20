import paho.mqtt.client as mqtt
import asyncio
import time
import json
from paho.mqtt.client import *
from bleak import BleakScanner

import ntplib
from time import ctime

class relay:

    def __init__(self):
        self.relayID = "relay_P1"
        self.company = "biot"
        self.mqttUsername = "testP1"
        self.mqttPassword = "testP1"

        self.latitude = 0.0
        self.longitude = 0.0
        self.floor = 0

        self.MAC_ADDRESS_LENGTH_BYTES = 6
        self.certificate_ca_path = "./isrgrootx1.pem"

        self.ntpClient = ntplib.NTPClient()

        self.mqttClient = None
        self.whiteList = []

        self.scanner = BleakScanner()
        self.beacons = {}

    # Parses the string whiteList passed and return a list of MAC formatted like aa:bb:cc:dd:ee:ff
    def _parse_whiteList(self, whiteListString):
        flag = True
        res = []

        while(flag):
            macAddr = ""
            for i in range(self.MAC_ADDRESS_LENGTH_BYTES):
                index = len(res) + 2*i 
                if index + 1 >= len(whiteListString):
                    flag = False
                    break
                macAddr += whiteListString[index] + whiteListString[index + 1]
                if i < 5:
                    macAddr += ":"
            if not flag:
                break
            res.append(macAddr.upper())
        return res

    def _send_beacons_on_mqtt(self):
        # Example message:
        #{"relayID":"relay_P1","beacons":[{"mac":"fc:02:a0:fa:33:19","rssi":-82,"battery":42,"temperature":24,"status":3}],"latitude":46.51746,"longitude":6.562729,"floor":0} from client relay_P1

        for addr, b in self.beacons.items():
            beaconDoc = b.copy()
            beaconDoc.pop("timeSinceLastMove")
            beaconDoc.pop("txPower")
            beaconDoc.pop("timeSinceLastClick")

            doc = {}
            doc["relayID"] = self.relayID
            doc["beacons"] = [ beaconDoc ]
            doc["latitude"] = self.latitude
            doc["longitude"] = self.longitude
            doc["floor"] = self.floor

            self.mqttClient.publish("incoming.update", payload = json.dumps(doc))
        
        beacons = []

    def _update_parameters_from_backend(self, msgJson):
        whiteListString = msgJson["whiteList"]

        self.whiteList = self._parse_whiteList(whiteListString)
        print(f"whiteList = {self.whiteList}")

        self.latitude = msgJson["latitude"]
        self.longitude = msgJson["longitude"]

    # def _parse_ble_payload(self, payload):
        


    def detection_callback_ble(self, device, advertisement_data):
        print(device.address, "RSSI:", device.rssi, advertisement_data)
        if device.address in self.whiteList:
            beacon = {}
            beacon["mac"] = device.address
            beacon["rssi"] = device.rssi
            
            payload = []
            for k, v in advertisement_data.service_data.items():
                payload = v
            print(f"payload utf-8 = {payload.decode('utf-8')}")
            beacon["temperature"] = 22 # TODO
            beacon["battery"] = 42 # TODO
            beacon["timeSinceLastMove"] = 42 # TODO
            beacon["txPower"] = 42 # TODO 
            beacon["timeSinceLastClick"] = 42 # TODO
            beacon["status"] = 0 # TODO

            self.beacons[beacon["mac"]] = beacon

    async def run_ble_scan_for_0_5_sec(self):
        self.scanner.register_detection_callback(self.detection_callback_ble)
        await self.scanner.start()
        await asyncio.sleep(0.5)
        await self.scanner.stop()


    async def loop(self):
        self.scanner.register_detection_callback(self.detection_callback_ble)
        while True:
            await self.run_ble_scan_for_0_5_sec()
            #time_response = ntpClient.request('europe.pool.ntp.org', version=3)
            #time_sec = time_response.tx_time
            time_sec = int(time.time())
            print(time_sec)
            while time_sec % 3 != 0 :
                time.sleep(0.01)
                time_sec = int(time.time())
                #time_response = ntpClient.request('europe.pool.ntp.org', version=3)
                #time_sec = time_response.tx_time
            # await self.scanner.stop()
            self._send_beacons_on_mqtt()

            


    # The callback for when the client receives a CONNACK response from the server.
    def on_connect_mqtt(self, client, userdata, flags, rc):
        print("Connected with result code "+str(rc))

        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe("update.parameters", 1)

    def on_disconnect(self, client, userdata, rc):
        client.reconnect()
    # The callback for when a PUBLISH message is received from the server.
    def on_message_mqtt(self, client, userdata, msg):
        print("topic= " + msg.topic+", message = "+str(msg.payload))
        if(msg.topic == "update.parameters"):
            msgJson = json.loads(msg.payload.decode("utf-8"))
            self._update_parameters_from_backend(msgJson)
    
    def connect_mqtt(self):
        # Connect the client to mqtt:
        self.mqttClient = mqtt.Client(client_id=self.relayID, clean_session=True, userdata=None, protocol=MQTTv311, transport="websockets")
        self.mqttClient.will_set("will", payload="{\"company\": \"" + self.company + "\"}", qos=0, retain=False)
        self.mqttClient.username_pw_set(self.mqttUsername, self.mqttPassword)
        # UNCOMMENT TO USE WSS
        #client.tls_set(ca_certs=certificate_ca_path) 
        self.mqttClient.on_connect = self.on_connect_mqtt
        self.mqttClient.on_message = self.on_message_mqtt

        self.mqttClient.connect("mqtt.b-iot.ch", port=443, keepalive=60)

        self.mqttClient.loop_start()

    # Launch BLE loop
    # event_loop = asyncio.get_event_loop()
    # event_loop.run_until_complete(loop())
    # event_loop.close()


async def main():
    relay_instance = relay()
    relay_instance.connect_mqtt()
    await relay_instance.loop()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())
    loop.close()
