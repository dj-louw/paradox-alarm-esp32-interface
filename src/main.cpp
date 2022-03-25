#include <WiFi.h>

#include "credentials.h"

//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 1
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

IPAddress staticIP(10,0,0,51);
IPAddress gateway(10,0,0,1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(1,1,1,1);

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

 #define RXD2 17
 #define TXD2 16

void setup() {



  Serial.begin(9600);
  Serial.println("\nConnecting");

  WiFi.mode(WIFI_STA);


  // Configures static IP address
  if (!WiFi.config(staticIP, gateway, subnet, dns, dns)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  // // Connect to Wi-Fi network with SSID and password
  // Serial.print("Connecting to ");
  // Serial.println(ssid);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print(WiFi.status());
  //   delay(500);
  //   Serial.print(".");
  // }



  Serial.println("Connecting Wifi ");
  for (int loops = 10; loops > 0; loops--) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("WiFi connected ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    else {
      Serial.println(loops);
      delay(1000);
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    delay(1000);
    ESP.restart();
  }

  //start UART and the server
  Serial2.begin(9600,SERIAL_8N1, RXD2, TXD2);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

}

void loop() {
  uint8_t i;
  if (WiFi.status() == WL_CONNECTED) {
    //check if there are any new clients
    if (server.hasClient()){
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          if (!serverClients[i]) Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].remoteIP());
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        server.available().stop();
      }
    }
    //check clients for data
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        if(serverClients[i].available()){
          //get data from the telnet client and push it to the UART
          while(serverClients[i].available()) Serial2.write(serverClients[i].read());
        }
      }
      else {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
      }
    }
    //check UART for data
    if(Serial2.available()){
      size_t len = Serial2.available();
      uint8_t sbuf[len];
      Serial2.readBytes(sbuf, len);
      Serial.write(sbuf,len);
      //push UART data to all connected telnet clients
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
          serverClients[i].write(sbuf, len);
          delay(1);
        }
      }
    }
  }
  else {
    Serial.println("WiFi not connected!");
    for(i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i]) serverClients[i].stop();
    }
    delay(1000);
    ESP.restart();
  }
}