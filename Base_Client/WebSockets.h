#pragma once
#include "HardwareSoftwareConfig.h"
#include "WebSocketsClient.h" // Install "WebSockets" by Markus Sattler from Library Manager
#include "WiFiConfig.h"

//Setup WebSocket Client
extern WebSocketsClient webSocket;

IPAddress local_IP(192, 168, 4, FAN_ID); // Static IP address for this ESP32
IPAddress gateway(192, 168, 4, 1); // Gateway IP address
IPAddress subnet(255, 255, 255, 0); // Subnet mask

void initWebSocketClient(); //Initialize WebSocket client
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length); //WebSocket Event Handler