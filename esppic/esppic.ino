//
// esppic - An ESP8266-based Microchip PIC programmer
//----------------------------------------------------
// More info at github.com/SmallRoomLabs/esppic
//
// Copyright (c) 2016 Mats Engstrom SmallRoomLabs
// Released under the MIT license
//
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>


#define SWAP16(x) (((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))

#include "index_html.h"
#include "upload_html.h"
#include "favicon_ico.h"
#include "logo_png.h"

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
String filename;
File fsUploadFile;
char resetflag=0;
uint8_t wsNum;

//
//
//
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  IPAddress ip;
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      wsNum=num;
      ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      webSocket.sendTXT(num, "Connected");
      webSocket.sendTXT(num, "f");
      webSocket.sendTXT(num, "s");
      webSocket.sendTXT(num, "p...");
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      if (payload[0]=='R') resetflag=payload[1];
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
      hexdump(payload, lenght);
      break;
  }
}



//
//
//
void handleNotFound(){
  Serial.println("handlNotFound()");
}


//
//
//
void handleFileUpload() {
  char tmps[30];
  static uint32 bytesSoFar=0;
  if(server.uri() != "/upload") return;
  Serial.println("handleFileUpload()"); 
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    bytesSoFar=0;
    filename = upload.filename;
    sprintf(tmps,"f%s",filename.c_str());
    webSocket.sendTXT(wsNum, tmps);
    sprintf(tmps,"s%d/%d bytes uploaded",bytesSoFar,upload.totalSize);
    webSocket.sendTXT(wsNum, tmps);
    webSocket.sendTXT(wsNum, "pUploading file");
    filename = "/payloads/"+filename;
    Serial.print("Uploading file "); 
    Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
  }
  else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
      bytesSoFar+=upload.currentSize;
      sprintf(tmps,"s%d/%d bytes uploaded",bytesSoFar,upload.totalSize);
      webSocket.sendTXT(wsNum, tmps);
    }
  }
  else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
    fsUploadFile.close();
    Serial.println("Success");
    webSocket.sendTXT(wsNum, "pFile uploaded" );
  }
}

void funcSendContent(String s) {
  server.sendContent(String(s));
}

 

//
// Setup pins and start the serial comms for debugging
//
void setup() {
  PicSetup();

  Serial.begin(115200);
  Serial.println("\n\n\nESPPIC v0.2 starting\n");
  SPIFFS.begin();
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.printf("%d of %d KB used on SPIFFS\n",fs_info.usedBytes/(1024),fs_info.totalBytes/(1024));
  ConnectToWifi();

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
    
  server.on("/", HTTP_GET, []() {
    Serial.println("HTTP_GET /");
    server.send_P(200,"text/html", index_html);
  });

  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br>Upload Successful!<br><br><a href=\"/listpayloads\">List Payloads</a>");
  },handleFileUpload);

  server.on("/favicon.ico", HTTP_GET, []() {
    Serial.println("HTTP_GET /favicon.ico");
    server.send_P(200, "image/x-icon", favicon_ico, sizeof(favicon_ico));
  });

  server.on("/logo.png", HTTP_GET, []() {
    Serial.println("HTTP_GET /logo.png");
    server.sendHeader("Cache-Control", "public,max-age:86400");
    server.send_P(200, "image/png", logo_png, sizeof(logo_png));
  });

  server.on("/readconfigs", HTTP_GET, []() {
    Serial.println("HTTP_GET /readconfig");

    char tmps[1000];

    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html","");
    server.sendContent(String("<h1>MEMORY DUMP</h1><samp>"));
    PicReadConfigs(funcSendContent);
    server.sendContent(String("</samp>"));
    server.client().stop();
  });

  server.on("/flash", HTTP_GET, []() {
    Serial.println("HTTP_GET /flash");
    PicFlash(filename);

    char *html=(char *)malloc(10000);
    strcpy(html,"<h1>FLASH DONE</h1><a href=\"/\">Back</a>");
    server.send(200, "text/html", html);
    free(html);
  });

  
  server.onNotFound(handleNotFound);
}



//
// Just loop the webserver & websocket handlers continously...
//
void loop(void) {
  server.handleClient();
  webSocket.loop();
  delay(10);
  if (resetflag) {
    if (resetflag=='H') PicReset('H');
    if (resetflag=='L') PicReset('L');
    if (resetflag=='P') PicReset('P');
    resetflag=0;
  }
}

