#include "WebSockets.h"

void initWebSocketClient() {
  webSocket.begin(websocket_server, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  Serial.println("WebSocket client initialized");
}

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      setFanSpeed(0);  //Switch off fan immediately if disconnected!
      Client_Connected = false;
      break;

    case WStype_CONNECTED:
      Serial.printf("WebSocket Connected to: %s\n", payload);
      Client_Connected = true;
      // Send initial to register as client with a unique ID
      webSocket.sendTXT("REGISTER:" + String(FAN_ID));
      Serial.println("Sent registration message: " + String(FAN_ID));
      break;

    case WStype_TEXT:
      {
        Serial.printf("Received: %s\n", payload);

        // Process incoming data
        String msg = String((char*)payload);

        if (msg == "-1") {  //Shutdown command
          shutDownSystem();
        }
        if (msg.startsWith("MOTOR_SPEED:")) {
          String msgValue = msg.substring(strlen("MOTOR_SPEED:"));
          msgSpeed = msgValue.toInt();  //0 - 255
          setFanSpeed(msgSpeed);
        }
      }
      break;

    default:
      break;
  }
}