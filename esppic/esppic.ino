//
// esppic - An ESP8266-based Microchip PIC programmer
// Project located at github.com/SmallRoomLabs/esppic
// Copright (c) 2016 Mats Engstrom SmallRoomLabs
// Released under the MIT license
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char* ssid = "TANGMOHOME";
const char* password = "3334932015";

#define SWAP16(x) (((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))

#define FLASHSIZE     	8192
#define PICMAXWORDS   	8192
#define PICBLANKWORD  	0x3FFF

// Locations of the special Config & Id memories on the PIC
#define USERID  	0 
#define CONFIG1 	7
#define CONFIG2 	8
#define DEVID   	6
#define DEVREV  	5

#define DLY1  10      // 10 microseconds for toggling and stuff
#define DLY2  10000   // 10  millisecond for command delay

#define PIN_RESET D0
#define PIN_DAT   D1
#define PIN_CLK   D2

#define DAT_INPUT   pinMode(PIN_DAT, INPUT);
#define DAT_OUT     pinMode(PIN_DAT, OUTPUT);
#define DAT_LOW     digitalWrite(PIN_DAT,   LOW)
#define DAT_HIGH    digitalWrite(PIN_DAT,   HIGH)
#define DAT_GET     digitalRead(PIN_DAT)

#define CLK_LOW     digitalWrite(PIN_CLK,   LOW)
#define CLK_HIGH    digitalWrite(PIN_CLK,   HIGH)

#define RESET_LOW   digitalWrite(PIN_RESET, LOW)
#define RESET_HIGH  digitalWrite(PIN_RESET, HIGH)

#include "index_html.h"
#include "upload_html.h"
#include "favicon_ico.h"
#include "logo_png.h"

uint16_t currentAddress;
ESP8266WebServer server(80);



// Some hardcoded firmware to upload to the PIC for testing
char code[]={\
":08000000803102288731672FCF\n"\
":100ECE002000692FF0302100990023008C018E0143\n"\
":100EDE0021008C018E0122008C018E010530F20062\n"\
":100EEE000F30F100F430F000F00B7B2FF10B7B2F65\n"\
":100EFE00F20B7B2FFF3022008C00FF308E0029304A\n"\
":100F0E00F2009630F100A630F000F00B8C2FF10BB2\n"\
":100F1E008C2FF20B8C2F2930F2009630F100A63078\n"\
":100F2E00F000F00B982FF10B982FF20B982F293021\n"\
":100F3E00F2009630F100A630F000F00BA42FF10B6A\n"\
":100F4E00A42FF20BA42F2930F2009630F100A63018\n"\
":100F5E00F000F00BB02FF10BB02FF20BB02F2930A9\n"\
":100F6E00F2009630F100A630F000F00BBC2FF10B22\n"\
":100F7E00BC2FF20BBC2F2930F2009630F100A630B8\n"\
":100F8E00F000F00BC82FF10BC82FF20BC82F293031\n"\
":100F9E00F2009630F100A630F000F00BD42FF10BDA\n"\
":100FAE00D42FF20BD42F2930F2009630F100A63058\n"\
":100FBE00F000F00BE02FF10BE02FF20BE02F2930B9\n"\
":100FCE00F2009630F100A630F000F00BEC2FF10B92\n"\
":100FDE00EC2FF20BEC2F2530F2008730F100AE3003\n"\
":100FEE00F000F00BF82FF10BF82FF20BF82F003268\n"\
":020FFE00722F50\n"\
":020000040001F9\n"\
":08000000650003001800000078\n"\
":04000E00C4C9FFFF63\n"\
":00000001FF\n"\
};
char *codep=code;



//
// Get a line of HEX code from the hardcoded FW
//
char *getline(void) {
    static char tmps[80];
    uint8_t i=0;
    while (*codep!='\n') {
      tmps[i++]=*codep;
      codep++;
    }
  tmps[i]=0;
  codep++;        
  return tmps;
}


File UploadFile;
String filename;
File fsUploadFile;
String webString;

//
//
//
void handleNotFound(){
  Serial.println("handlNotFound()");
}

//void handleFileUpload() {
//    server.onFileUpload([]() {
//    Serial.println("onFileUpload()");
//    HTTPUpload& upload = server.upload();
//    switch (upload.status) {
//      case UPLOAD_FILE_START:
//        Serial.println("UPLOAD_FILE_START");
//        filename = upload.filename;
//        Serial.print("Upload Name: "); 
//        Serial.println(filename);
//        UploadFile = SPIFFS.open("/" + filename, "w");
//        break;
//      case UPLOAD_FILE_WRITE:
//        Serial.println("UPLOAD_FILE_WRITE");
//          if (UploadFile) UploadFile.write(upload.buf, upload.currentSize);
//          break;
//      case UPLOAD_FILE_END:
//        Serial.println("UPLOAD_FILE_END");
//        if (UploadFile) UploadFile.close();
//        break;
//    } //switch
//  });
//
//}



void handleFileUpload() {
  Serial.println("handleFileUpload()"); 
  if(server.uri() != "/upload") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    filename = "/payloads/"+filename;
    Serial.print("Uploading file "); 
    Serial.print(filename+" ... ");
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  }
  else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
    fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
    fsUploadFile.close();
    Serial.println("Success");
  }
}



//
// Setup pins and start the serial comms for debugging
//
void setup() {
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DAT,   OUTPUT);
  pinMode(PIN_CLK,   OUTPUT);
  RESET_HIGH;
  DAT_LOW;
  CLK_LOW;

  Serial.begin(115200);
  Serial.println("\n\n\nESPPIC v0.2 starting\n");
  SPIFFS.begin();

  WiFi.mode(WIFI_AP_STA);
  Serial.printf("Connecting to wifi using %s/%s\n",ssid,password);
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected with IP ");
  Serial.println(WiFi.localIP());

  server.begin();

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
    char *html=(char *)malloc(10000);
    char tmps[1000];
    EnterLVPmode();
    CmdResetAddress();
    DumpConfig(tmps);
    RESET_HIGH;
    strcpy(html,"<h1>CONFIG AREAS</h1><tt>");
    strcat(html,tmps);
    strcat(html,"</tt>");
    server.send(200, "text/html", html);
    free(html);
  });

  server.on("/flash", HTTP_GET, []() {
    Serial.println("HTTP_GET /flash");
    char *html=(char *)malloc(10000);
    EnterLVPmode();
    CmdResetAddress();
    CmdLoadConfig(0x00);
    CmdBulkErase();
    Store(USERID+0,SWAP16(0x8031)); delay(5);
    Store(USERID+1,SWAP16(0x0228)); delay(5);
    Store(USERID+2,SWAP16(0x8731)); delay(5);
    Store(USERID+3,SWAP16(0xE12F)); delay(5);
    Store(CONFIG1, SWAP16(0xE4C9)); delay(5);
    Store(CONFIG2, SWAP16(0xFBFF)); delay(5);
    RESET_HIGH;
    strcpy(html,"<h1>FLASH DONE</h1><a href=\"/\">Back</a>");
    server.send(200, "text/html", html);
    free(html);
  });

  
  server.onNotFound(handleNotFound);
}


//
// Convert a one character hex string to an uint8
//
uint8_t HexDec1(uint8_t c1) {
  if (c1>='A') c1-=7;
  c1-='0';
  return c1;
}


//
// Convert a two character hex string to an uint8
//
uint8_t HexDec2(uint8_t c1, uint8_t c2) {
  return HexDec1(c1)*16 + HexDec1(c2);
}


//
// Convert a four character hex string to an uint16
//
uint16_t HexDec4(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4) {
  return HexDec2(c1,c2)*256 + HexDec2(c3,c4);
}

//
// prints 8-bit data in hex with leading zeroes
//
void PrintHex8(uint8_t *data, uint8_t length) {
   char tmp[5];
   byte first;
   byte second;
   for (uint8_t i=0; i<length; i++) {
       first = (data[i] >> 4) & 0x0f;
       second = data[i] & 0x0f;
       tmp[0] = '0'; // add leading 0
       tmp[1] = 'x'; // add leading x
       tmp[2] = first+48;
       if (first > 9) tmp[2] += 7;
       tmp[3] = second+48;
       if (second > 9) tmp[3] += 7;
       tmp[4] = ' '; // add trailing space
       tmp[5] = 0;
       Serial.print(&tmp[2]);
   }
}


//
// prints 16-bit data in hex with leading zeroes 
//
void PrintHex16(uint16_t *data, uint8_t length) {
   char tmp[7];
   uint8_t first;
   uint8_t second;
   uint8_t third;
   uint8_t fourth;
   for (uint8_t i=0; i<length; i++) {
       first = (data[i] >> 12) & 0x0f;
       second = (data[i] >> 8) & 0x0f;
       third = (data[i] >> 4) & 0x0f;
       fourth = data[i] & 0x0f;
       tmp[0] = '0'; // add leading 0
       tmp[1] = 'x'; // add leading x
       tmp[2] = first+48;  if (first > 9) tmp[2] += 7;
       tmp[3] = second+48; if (second > 9) tmp[3] += 7;
       tmp[4] = third+48;  if (third > 9) tmp[4] += 7;
       tmp[5] = fourth+48; if (fourth > 9) tmp[5] += 7;
       tmp[6] = ' '; // add trailing space
       tmp[7] = 0;
       Serial.print(&tmp[2]);
   }
}

//
//
//
void PrintBinary16(uint16_t data) {
  char tmp[19];
  tmp[0]=(data&0x8000?'1':'0');
  tmp[1]=(data&0x4000?'1':'0');
  tmp[2]=(data&0x2000?'1':'0');
  tmp[3]=(data&0x1000?'1':'0');
  tmp[4]=' ';
  tmp[5]=(data&0x800?'1':'0');
  tmp[6]=(data&0x400?'1':'0');
  tmp[7]=(data&0x200?'1':'0');
  tmp[8]=(data&0x100?'1':'0');
  tmp[9]=' ';
  tmp[10]=(data&0x80?'1':'0');
  tmp[11]=(data&0x40?'1':'0');
  tmp[12]=(data&0x20?'1':'0');
  tmp[13]=(data&0x10?'1':'0');
  tmp[14]=' ';
  tmp[15]=(data&0x8?'1':'0');
  tmp[16]=(data&0x4?'1':'0');
  tmp[17]=(data&0x2?'1':'0');
  tmp[18]=(data&0x1?'1':'0');
  tmp[19]=0;
  Serial.print(tmp);
}

//
//
//
void ToBinary16(char *tmp, uint16_t data) {
  tmp[0]=(data&0x8000?'1':'0');
  tmp[1]=(data&0x4000?'1':'0');
  tmp[2]=(data&0x2000?'1':'0');
  tmp[3]=(data&0x1000?'1':'0');
  tmp[4]=' ';
  tmp[5]=(data&0x800?'1':'0');
  tmp[6]=(data&0x400?'1':'0');
  tmp[7]=(data&0x200?'1':'0');
  tmp[8]=(data&0x100?'1':'0');
  tmp[9]=' ';
  tmp[10]=(data&0x80?'1':'0');
  tmp[11]=(data&0x40?'1':'0');
  tmp[12]=(data&0x20?'1':'0');
  tmp[13]=(data&0x10?'1':'0');
  tmp[14]=' ';
  tmp[15]=(data&0x8?'1':'0');
  tmp[16]=(data&0x4?'1':'0');
  tmp[17]=(data&0x2?'1':'0');
  tmp[18]=(data&0x1?'1':'0');
  tmp[19]=0;
}

//
// Clock out data to the PIC. The "bits" argument
// specifies how many bits to be sent.
//
void Send(uint16_t data, uint8_t bits) {
  DAT_OUT;
  delayMicroseconds(DLY1);
  for (uint8_t i=0; i<bits; i++) {
    if (data&0x01) {
      DAT_HIGH;
    } else {
      DAT_LOW;
    }
    delayMicroseconds(DLY1);
    CLK_HIGH;
    delayMicroseconds(DLY1);
    CLK_LOW;
    delayMicroseconds(DLY1);
    data = data >> 1;
  }
  DAT_LOW;
}


//
// Clock in 16 bits of data from the PIC
//
uint16_t Read16(void){
  uint16_t data=0;
  
  DAT_INPUT;
  delayMicroseconds(DLY1);
  for (uint8_t i=0; i<16; i++) {
    CLK_HIGH;
    delayMicroseconds(DLY1);
    CLK_LOW;
    delayMicroseconds(DLY1);
    data=data >> 1;
    if (DAT_GET) data = data | 0x8000;
  }
  return data;
}


//
// Send the RESET ADDRESS command to the PIC
//
void CmdResetAddress(void) {
  currentAddress=0;
  Send(0x16,6);
  delayMicroseconds(DLY2);
}

//
// Send the INCREMENT ADDRESS command to the PIC
//
void CmdIncAddress(void) {
  currentAddress++;
  Send(0x06,6);
  delayMicroseconds(DLY1);
}

//
// Send the BEGIN PROGRAMMING INTERNAL TIMED command to the PIC
//
void CmdBeginProgramI(void) {
  Send(0x08,6);
  delayMicroseconds(DLY2);
}

//
// Send the LOAD CONFIG command to the PIC
//
void CmdLoadConfig(uint16_t data) {
  currentAddress=0;
  Send(0x00,6);
  Send(data,16);
  delayMicroseconds(DLY2);
}


//
// Send the READ DATA command to the PIC
// TODO: This command should not increment the address here
//
void CmdReadData(uint16_t *data, uint8_t cnt) {
for (uint8_t i=0; i<cnt; i++) {
  Send(0x04, 6);
  delayMicroseconds(DLY1);
  data[i] = (Read16() & 0x7FFE) >> 1;
  CmdIncAddress();
  }
}


//
// Send the BULK ERASE command to the PIC
//
void CmdBulkErase(void) {
  Serial.println("BulkErase");
  CmdLoadConfig(0);
  Send(0x09,6);
  delay(10);
}

//
// Send the magic 32 bits plus one extra clock 
// to get into the LVP programming mode
//
void EnterLVPmode() {
  CLK_LOW;
  DAT_LOW;
  delayMicroseconds(500);
  RESET_LOW;
  delayMicroseconds(500);
  Send(0b01010000,8); //P
  Send(0b01001000,8); //H
  Send(0b01000011,8); //C
  Send(0b01001101,8); //M
  Send(0,1);
  delayMicroseconds(DLY1);
 }



//
// Dump the contents of the PIC flash memory onto the
// serial line for debugging
//
void DumpMemory(void) {
  bool isBlank;
  uint16_t data[16];
  
  CmdResetAddress();
  for (uint16_t rows=0; rows<FLASHSIZE/16; rows++) {
    CmdReadData(data,16);
    isBlank=true;
    for (uint8_t i=0; i<16; i++) {
      if (data[i]!=0x3FFF) isBlank=false; 
    }
    if (!isBlank) {
      uint16_t address=rows*16;
      PrintHex16(&address,1); 
      Serial.print(":");
      PrintHex16(&data[0],16); 
      Serial.println();
    }
    yield();
  }  
}




//
// Dump the CONFIG memory parts of the PIC onto the 
// serial line for debugging 
//
void DumpConfig(char *buf) {
  uint16_t data[16];
  char bin1[20];
  char bin2[20];

  CmdLoadConfig(0x00);
  CmdReadData(data,16);
  Serial.print("DEV ID : "); PrintHex16(&data[DEVID],1); Serial.println();
  Serial.print("DEV REV: "); PrintHex16(&data[DEVREV],1); Serial.println();
  Serial.print("CONFIG1: "); PrintHex16(&data[CONFIG1],1); Serial.print(" ("); PrintBinary16(data[7]); Serial.println(")");
  Serial.print("CONFIG2: "); PrintHex16(&data[CONFIG2],1); Serial.print(" ("); PrintBinary16(data[8]); Serial.println(")");
  Serial.print("USER ID: "); PrintHex16(&data[USERID],4); Serial.println();
  
  ToBinary16(bin1,data[CONFIG1]);
  ToBinary16(bin2,data[CONFIG2]);
  sprintf(buf,
    "DEV ID : %04x<br/>"
    "DEV REV: %04x<br/>"
    "CONFIG1: %04x (%s)<br/>"
    "CONFIG2: %04x (%s)<br/>"
    "USER ID: %04x %04x %04x %04x<br/>",
    data[DEVID],
    data[DEVREV],
    data[CONFIG1], bin1,
    data[CONFIG2], bin2,
    data[USERID+0],data[USERID+1],data[USERID+2],data[USERID+3]
    );
    
    

}


//
// Flash one word of data into the specified location on the PIC
//
void Store(uint16_t address, uint16_t data) {
  Serial.printf("0x%04X:0x%04X\n",address,data);
  if (address<currentAddress) CmdResetAddress();
  while (address>currentAddress) CmdIncAddress();
  Send(0x02,6); 
  Send(data<<1,16); 
  CmdBeginProgramI();
  delay(3);
}



//
// The main code that does the stuff ^__^
//
void flash_gordon() {
  uint16_t data[32];
  uint16_t a;
  uint8_t d_len;
  uint16_t d_addr;
  uint8_t d_typ;
  
  EnterLVPmode();

  // CONFIG words
  CmdResetAddress();
  CmdLoadConfig(0x00);
  CmdBulkErase();
  Store(CONFIG1, SWAP16(0xE4C9));  delay(5);
  Store(CONFIG2, SWAP16(0xFBFF));  delay(5);

  CmdResetAddress();

  // USER ID's in the CONFIG area
  Store(USERID+0,SWAP16(0x8031));	// Just put some hardcoded
  Store(USERID+1,SWAP16(0x0228));	// data here for fun while
  Store(USERID+2,SWAP16(0x8731));	// debugging
  Store(USERID+3,SWAP16(0xE12F));

  // CODE
  char *s;
  do {
    s=getline();
    Serial.println(s);
    d_len=HexDec2(s[1],s[2]);
    d_addr=HexDec4(s[3],s[4],s[5],s[6]);
    d_typ=HexDec2(s[7],s[8]);
    Serial.printf("len=%02x addres=%04x type=%02x\n",d_len,d_addr,d_typ);
    if (d_typ==0x00) {
      for (uint8_t i=0; i<d_len*2; i+=4) {              
        Store(d_addr/2+i/4,HexDec4(s[11+i],s[12+i],s[9+i],s[10+i]));
      }
    }
  } while (d_typ!=1);

  // Dump the CONFIGs and the CODE areas onto serial
  // for debugging
  CmdResetAddress();
//  DumpConfig();
  CmdResetAddress();
  DumpMemory();


  // De-assert the PIC reset line so the new code just uploaded
  // can start running on it. Then just hang the ESP in an forever loop
  RESET_HIGH;
  for (;;) delay(1);
}



//
// Run the webserver handler in a continous loop
//
void loop(void) {
  server.handleClient();
  delay(10);
}

