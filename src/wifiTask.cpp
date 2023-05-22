/* Adapted from the Sockets Example for CITY1082
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "wifiTask.h"
#include "config.h"
//#include "mbed-mqtt/src/MQTTClientMbedOs.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"
#include <stdio.h>
#include <string.h>
#include "constants.h"
#ifndef MBED_CONF_MBED_MQTT_MAX_PACKET_SIZE
#define MBED_CONF_MBED_MQTT_MAX_PACKET_SIZE 1024
#define MBED_CONF_MBED_MQTT_MAX_CONNECTIONS 32
#endif
#define DEBUG
//#undef DEBUG
#include "mbed-mqtt/src/MQTTClientMbedOs.h"

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET
#define align8 8
const static uint16_t qLen = 32;
using pubPacket_t = struct {
  int topic;
  float value;
} __attribute__((aligned(align8)));
static auto qSize = 0;
static pubPacket_t myQueue[qLen];
static uint16_t stQueue = 0;
static uint16_t endQueue = 0;
static MemoryPool<pubPacket_t, 32> mpool1;
static Queue<pubPacket_t, 32> pqueue;
const char topicMap[NUM_TOPICS][TOPIC_LEN] = {
      "light", "lightState", "lightShield", "redled", "greenled",
      "blueled", "announce", "lightSet", "latitude", "longitude",
      "temperature", "tempSet", "rxCount", "txCount", "time", "statusled",
      "orangeled", "heaterState", "heaterSwitch", "humidity"};
extern things_t myData;
extern bool displayUp;
uint32_t rxCount = 0;
DigitalOut rxLed(RXLED);

void sendPub(int pTopic, float pValue) {
  if (qSize == qLen) {
    printf("Publish queue is full!\n");
  } else {
    myQueue[stQueue].topic = pTopic;
    myQueue[(stQueue++) % qLen].value = pValue;
    qSize++;
    if (stQueue >= qLen)
      stQueue = 0;
  }
}

class joinWifi {
    //static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    //static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    //static constexpr size_t REMOTE_PORT = 80; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    joinWifi() : _net(NetworkInterface::get_default_instance())
    {
    }

    ~joinWifi()
    {
        if (_net) {
            _net->disconnect();
        }
    }

    void run()
    {
        if (!_net) {
            printf("Error! No network interface found.\r\n");
            return;
        }

        /* if we're using a wifi interface run a quick scan */
        if (_net->wifiInterface()) {
            /* the scan is not required to connect and only serves to show visible access points */
            //wifi_scan(); */

            /* in this example we use credentials configured at compile time which are used by
             * NetworkInterface::connect() but it's possible to do this at runtime by using the
             * WiFiInterface::connect() which takes these parameters as arguments */
        }

        /* connect will perform the action appropriate to the interface type to connect to the network */

        printf("\033[3;1HConnecting to the network...\r\n");

        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }
        printf("\033[3;1HConnected to the network....\r\n");

        print_network_info();

        /* opening the socket only allocates resources */
        result = _socket.open(_net);
        if (result != 0) {
            printf("Error! _socket.open() returned: %d\r\n", result);
            return;
        }
        printf("Starting MQTT Reporting to %S\n", (wchar_t *)BROKER);
        char buffer[80];
        char topicBuffer[80];
        uint32_t rc;
        uint32_t failure = 0;
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.clientID.cstring = (char *)THING_NAME;
        data.keepAliveInterval = 20;
        data.cleansession = 1;
        data.username.cstring = (char *)"";
        data.password.cstring = (char *)"";
        char *host = (char *)BROKER;
        uint16_t port = PORT;
        TCPSocket socket;
        MQTTClient client(&socket);
        socket.open(_net);
        rc = socket.connect(host, port);
        if (rc == 0) {
            printf("Succesful connection of socket to Host %s port %d\n", host, port);
        } else {
            printf("Socket connection failed");
            while (socket.connect(host, port)) {
                printf(".");
            }
        }
        rc = client.connect(data);
        if (rc == 0) {
            printf("Succesful connection of %s to Broker\n", data.clientID.cstring);
        } else {
            printf("Client connection failed");
        }
        MQTT::Message message{};
        sprintf(buffer, "Hello World! from %s\r\n", THING_NAME);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void *)buffer;
        message.payloadlen = strlen(buffer) + 1;

        rc = client.publish(ANNOUNCE_TOPIC, message);
        if (rc == 0) {
            printf("publish announce worked\n");
            }
             
        else {
            printf("publish announce failed %d\n", rc);
        }

        rc = client.subscribe(LIGHT_SET_TOPIC, MQTT::QOS0,
                              messageLightSetArrived);
#ifdef DEBUG
        if (rc != 0)
            sprintf(buffer, "Subscription Error %d", rc);
        else
            sprintf(buffer, "Subscribed to %s", LIGHT_SET_TOPIC);
        printf("%s", buffer);
#endif
        rc = client.subscribe(TEMP_SET_TOPIC, MQTT::QOS0,
                              messageTempSetArrived);
#ifdef DEBUG
        if (rc != 0)
            sprintf(buffer, "Subscription Error %d", rc);
        else
            sprintf(buffer, "Subscribed to %s", TEMP_SET_TOPIC);
        printf("%s", buffer);
#endif
        rxLed = 1;

        while (true) {
            ThisThread::sleep_for(10);
            client.yield(10);
            if (qSize > 0) {
                sprintf(buffer, "%3.1f", myQueue[endQueue].value);
                sprintf(topicBuffer, "%s/%s", THING_NAME,
                        topicMap[myQueue[endQueue++].topic]);
                qSize--;
                if (endQueue >= qLen){
                    endQueue = 0;
                }

                message.payload = (void *)buffer;
                message.payloadlen = strlen(&buffer[0]) + 1;

                rc=client.publish(&topicBuffer[0], message);

            }
        }

    }
private:
    static void messageLightSetArrived(MQTT::MessageData &md) {
      MQTT::Message &message = md.message;
        uint32_t len = md.message.payloadlen;
        char rxed[len + 1];

        strncpy(&rxed[0], (char *)(&md.message.payload)[0], len);
        myData.setLightLevel = atoi(rxed);
        rxCount++;
        rxLed = !rxLed;
    }
private:
    static void messageTempSetArrived(MQTT::MessageData &md) {
      MQTT::Message &message = md.message;
        uint32_t len = md.message.payloadlen;
        char rxed[len + 1];

        strncpy(&rxed[0], (char *)(&md.message.payload)[0], len);
        myData.setTemp = atof(rxed);
        rxCount++;
        rxLed = !rxLed;
    }
private:
    void print_network_info()
    {
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("\033[5;1HIP address: %s", a.get_ip_address() ? a.get_ip_address() : "None");
    }

private:
    NetworkInterface *_net;

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

void runWifi() {
    printf("\033[2;1HStarting Wifi Connection...\n");

#ifdef MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif

    joinWifi *example = new joinWifi();
    MBED_ASSERT(example);
    example->run();

//    return 0;
}
