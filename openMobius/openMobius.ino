/*!
 * Discover and Control Mobius Devices
 *
 * Establishes a wifi and mqtt connection. 
 * Scans for Mobius devices and updates mqtt with a json for Home Assistant auto discovery.
 * The current scene for each device is found and an update sent to mqtt. 
 * Subscribes to a set scene mqtt topic that is set by Home Assistant.
 * Triggers a Mobius scene change on change in Home Assistant select scene control.
 * 
 * TODO:
 * + Doesn't always trigger loop() and have to press EN button on esp32 to start
 * + Seeme to disconnect from wifi after a fwe hours - memory leak?
 * 
 * This example code is released into the public domain.
 */

#include <esp_log.h>
#include <ESP32_MobiusBLE.h>
#include "ArduinoSerialDeviceEventListener.h"
#include "EspMQTTClient.h"
#include <string>
#include "secrets.h"

// Configuration for wifi and mqtt
EspMQTTClient client(
  mySSID,           // Your Wifi SSID
  myPassword,       // Your WiFi key
  mqttServer,       // MQTT Broker server ip
  mqttUser,         // mqtt username Can be omitted if not needed
  mqttPass,         // mqtt pass Can be omitted if not needed
  "Mobius",         // Client name that uniquely identify your device
  1883              // MQTT Broker server port
);

// Json mqtt template for scene status updates
// Need to insert the mac address and timestamp
//char *jsonTemplateScene = 
//"{                    \
//\"scene\": %d,       \
//\"timestamp\" : \"%s\"\
//}";

// Json mqtt template for home assistant auto discovery of mobius devices
char *jsonDiscoveryDevice = "{\"name\": \"%s\",\
  \"unique_id\": \"%s\",\
  \"icon\": \"mdi:pump\",\
  \"state_topic\": \"homeassistant/sensor/mobius/%s/scene/state\",\
  \"force_update\": \"true\",\
  \"device\" : {\
  \"identifiers\" : [  \"%s\" ],\
  \"name\": \"%s\",\
  \"model\": \"Mobius\",\
  \"manufacturer\": \"Ecotech\"}\
}";

// Json mqtt template for home assistant auto discovery of select scene widget 
char *jsonDiscoveryDeviceSelect = "{\"name\": \"Set Scene\",\
    \"unique_id\": \"%s_select\",\
    \"command_topic\": \"homeassistant/select/mobius/scene/%s\",\
    \"force_update\": \"true\",\
    \"options\": [\"0\", \"1\", \"Scene x\", \"Scene y\"],\
    \"device\" : {  \"identifiers\" : [  \"%s\" ],  \"name\": \"%s\", \"model\": \"Mobius\",  \"manufacturer\": \"Ecotech\"}\
}";


// Define a device buffer to hold found Mobius devices
MobiusDevice deviceBuffer[30];

// wifi and mqtt connection established
void onConnectionEstablished()
{
  Serial.println("Connected to MQTT Broker :)");

  // Set keepalive (default is 15sec)
  client.setKeepAlive(30);

  // Increase default message limit
  client.setMaxPacketSize(10000);

  // Set mqtt to be persistent
  client.enableMQTTPersistence();

  // Listen to wildcard topic for scene changes to HA
  client.subscribe("homeassistant/select/mobius/scene/#", onMessageReceived);
}

// Set scene from Home Assistant select scene 
// Comes from mqtt subscribe to /homeassistant/select/mobius/scene/<device>
void onMessageReceived(const String& topic, const String& message) {
  // Get serial number from the end of topic
  String serialNumberGiven = topic.substring(34);
  Serial.printf("INFO***: Serial Number: %s\n", serialNumberGiven.c_str());
  Serial.printf("INFO***: Set Scene is: %s\n", message);

  // Loop through devices till we match the serial number
  MobiusDevice device = deviceBuffer[0];
  int discovered = 0;
  while (discovered == 0){
    Serial.println("INFO****: Discovered NO");
    int count = 0;
    int scanDuration = 10; // in seconds
    while (!count) {
      count = MobiusDevice::scanForMobiusDevices(scanDuration, deviceBuffer);
    }
    Serial.printf("INFO*** Device count: %i\n", count);
    for (int i = 0; i < count; i++) {
      device = deviceBuffer[i];
      Serial.printf("INFO****: Check device: %i\n", count);
      // Get manufacturer info
      std::string manuData = device._device->getManufacturerData();

      // Don't connect unless we have a serial number
      if (manuData.length() > 1){
        // Connect, get serialNumber and current scene          
        Serial.printf("INFO****: Found manuData: %s\n", manuData.c_str());
        // serialNumber is from byte 11
        std::string serialNumberString = manuData.substr(11, manuData.length());
        char serialNumber[serialNumberString.length() + 1] = {};
        strcpy(serialNumber, serialNumberString.c_str());
        Serial.printf("INFO*****: Device serial number: %s\n", serialNumber);
        Serial.printf("INFO*****: Serial number given: %s\n", serialNumberGiven.c_str());
        if (strcmp(serialNumber, serialNumberGiven.c_str()) == 0){
          Serial.printf("INFO****: MATCH!!\n");

          // Connect device
          if (!device.connect()){
            Serial.println("ERROR****: Failed to connect to device");
          }
          else {
            Serial.println("INFO****: Connected to device");
          }

          // Set scene
          if (!device.setScene(message.toInt())){
            Serial.println("ERROR****: Failed to set device scene");
          }
          else {
            Serial.println("INFO****: Successfully set device scene");
          }

          // Disconnect
          device.disconnect();

          // Mark done
          discovered = 1;
        }
      }
    }
  }
}


/*!
 * Main Setup method
 */
void setup() {
  // Connect the serial port for logs
  Serial.begin(115200);
  while (!Serial);

  // Connect to wifi and mqtt server
  while(!client.isConnected()){client.loop();};

  // Optional functionality for EspMQTTClient
  //client.setOnConnectionEstablishedCallback(onConnectionEstablished);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  //client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("Mobius/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  
  // Increase default packet size for HA mqtt json messages
  client.setMaxPacketSize(10000);

  // Initialize the library with a useful event listener
  MobiusDevice::init(new ArduinoSerialDeviceEventListener());

}

/*!
 * Main Loop method
 */
void loop() {

  // Wait for mqtt and wifi connection
  while(!client.isConnected()){client.loop();};

  // Loop mqtt
  client.loop();

  // Get number of mobius devices
  MobiusDevice device = deviceBuffer[0];
  int count = 0;
  //int scanDuration = 10; // in seconds
  int scanDuration = 2; // in seconds
  while (!count) {
    count = MobiusDevice::scanForMobiusDevices(scanDuration, deviceBuffer);
  }

  // Update mqtt with device count
  // Intended for debug purposes as it shows how reliable the BLE scans are. 
  // Look in graph in mqtt explorer.
  //char cstr[8];
  //sprintf(cstr, "%d", count);
  //if (!client.publish("homeassistant/sensor/mobius/discovered/count", cstr)) {
  //  Serial.println("ERROR: failed to publish device count");
  //}

  // Loop through each device found, autoconfigure home assistant with the device, and update the current scene of the device
  for (int i = 0; i < count; i++) {
        device = deviceBuffer[i];

        // Get manufacturer info
        std::string manuData = device._device->getManufacturerData();

        // Don't connect unless we have a serial number
        if (manuData.length() > 1){
          // Connect, get serialNumber and current scene
          Serial.printf("\nINFO: Connect to device number: %i\n", i);
          if(device.connect()) {
            Serial.printf("INFO: Connected to: %s\n", device._device->toString().c_str());

            // serialNumber is from byte 11
            std::string serialNumberString = manuData.substr(11, manuData.length());
            char serialNumber[serialNumberString.length() + 1] = {};
            strcpy(serialNumber, serialNumberString.c_str());
            Serial.printf("INFO: Device serial number: %s\n", serialNumber);
          
            // Get the devices mac address. Note that this changes every reboot so likely not useful
            std::string addressString = device._device->getAddress().toString();
            char deviceAddress[addressString.length() + 1] = {};
            strcpy(deviceAddress, addressString.c_str());
            Serial.printf("INFO: Device mac address is: %s\n", deviceAddress);

            // Get device name (not useful as always MOBIUS)
            //std::string deviceName = device._device->getName();
            //Serial.printf("INFO: Device Name: %s\n", deviceName.c_str());

            // Home Assistant autodiscovery
            // Substitute serialNumber into jsonDiscoveryDevice
            char json[512];
            sprintf(json, jsonDiscoveryDevice, serialNumber, serialNumber, serialNumber, deviceAddress, serialNumber);
            Serial.printf("INFO: Device discovery message:%s\n", json);
            char deviceDiscoveryTopic[400];
            sprintf(deviceDiscoveryTopic, "homeassistant/sensor/mobius/%s/config", serialNumber);
            Serial.printf("INFO: Device Discovery Topic: %s\n", deviceDiscoveryTopic);
            client.publish(deviceDiscoveryTopic, json);

            // Create scene select input
            char jsonSelect[512];
            sprintf(jsonSelect, jsonDiscoveryDeviceSelect, serialNumber, serialNumber, deviceAddress, serialNumber);
            Serial.printf("INFO: Device select discovery message:%s\n", jsonSelect);
            char deviceSelectDiscoveryTopic[400];
            sprintf(deviceSelectDiscoveryTopic, "homeassistant/select/mobius/%s/config", serialNumber);
            Serial.printf("INFO: Device Select Discovery Topic: %s\n", deviceSelectDiscoveryTopic);
            client.publish(deviceSelectDiscoveryTopic, jsonSelect);

            // Get current scene
            uint16_t sceneId = device.getCurrentScene();
            char sceneString[8];
            dtostrf(sceneId, 2, 0, sceneString);
            Serial.printf("INFO: Current scene string:%s\n", sceneString);
        
            // Set sensor scene
            char deviceTopic[400];
            sprintf(deviceTopic, "homeassistant/sensor/mobius/%s/scene/state", serialNumber);
            Serial.printf("INFO: Device Topic: %s\n", deviceTopic);
            client.publish(deviceTopic, sceneString);

            // Create scene state topic
            //char deviceSelectCommandDiscoveryTopic[400];
            //sprintf(deviceSelectCommandDiscoveryTopic, "homeassistant/select/mobius/%s/scene/state", serialNumber);
            //client.publish(deviceSelectCommandDiscoveryTopic, sceneString);

            
            //});

            // Create sensor state topic
            //char deviceSensorStateDiscoveryTopic[400];
            //sprintf(deviceSensorStateDiscoveryTopic, "homeassistant/sensor/mobius/%s/scene/state", serialNumber);
            //client.publish(deviceSensorStateDiscoveryTopic, sceneString);

            // Update device register with device info and available scenes
            // Store serialNumber, macAddress, name, and each discovered scene
            // We can then use the scenes to update the mqtt ha autodiscovery scene list
            // Or maybe we can do this from HA using a script / automation.
          //}
          // Disconnect
          device.disconnect();
        }
        else {
                Serial.println("ERROR: Failed to connect to device");
        }
      }
  }
}