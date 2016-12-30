#define FLASHSIZE       8192
#define PICMAXWORDS     8192
#define PICBLANKWORD    0x3FFF

// Locations of the special Config & Id memories on the PIC
#define USERID    0 
#define CONFIG1   7
#define CONFIG2   8
#define DEVID     6
#define DEVREV    5

#define DLY1  10      // 10 microseconds for toggling and stuff
#define DLY2  10000   // 10  millisecond for command delay

#define PIN_RESET D0
#define PIN_DAT   D1
#define PIN_CLK   D2

#define DAT_INPUT   pinMode(PIN_DAT, INPUT);
#define DAT_OUTPUT  pinMode(PIN_DAT, OUTPUT);
#define DAT_LOW     digitalWrite(PIN_DAT,   LOW)
#define DAT_HIGH    digitalWrite(PIN_DAT,   HIGH)
#define DAT_GET     digitalRead(PIN_DAT)

#define CLK_INPUT   pinMode(PIN_CLK, INPUT);
#define CLK_OUTPUT  pinMode(PIN_CLK, OUTPUT);
#define CLK_LOW     digitalWrite(PIN_CLK,   LOW)
#define CLK_HIGH    digitalWrite(PIN_CLK,   HIGH)

#define RESET_INPUT  pinMode(PIN_RESET, INPUT);
#define RESET_OUTPUT pinMode(PIN_RESET, OUTPUT);
void RESET_LOW(){   digitalWrite(PIN_RESET, LOW);}
void RESET_HIGH(){  digitalWrite(PIN_RESET, HIGH);}


uint16_t currentAddress;


//
//
//
void PicSetup() {
  RESET_OUTPUT;
  DAT_INPUT;
  CLK_INPUT;
  // pinMode(PIN_RESET, OUTPUT);
  // pinMode(PIN_DAT,   OUTPUT);
  // pinMode(PIN_CLK,   OUTPUT);
  RESET_HIGH();
  // DAT_LOW;
  // CLK_LOW;
}


//
// Send the magic 32 bits plus one extra clock to get into the LVP programming 
// mode as long as the RESET line is kept low.
//
void EnterLVPmode() {
  DAT_OUTPUT;
  CLK_OUTPUT;
  CLK_LOW;
  DAT_LOW;
  delayMicroseconds(500);
  RESET_LOW();
  delayMicroseconds(500);
  Send(0b01010000,8); // P
  Send(0b01001000,8); // H
  Send(0b01000011,8); // C
  Send(0b01001101,8); // M
  Send(0,1);          // and one single final bit
  delayMicroseconds(DLY1);
 }


//
// Clock out data to the PIC. The "bits" argument
// specifies how many bits to be sent.
//
void Send(uint16_t data, uint8_t bits) {
  DAT_OUTPUT;
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
  CmdLoadConfig(0);
  Send(0x09,6);
  delay(10);
}



//
// Dump the contents of the PIC flash memory onto the...
//
void DumpMemory(void (*outFunc)(const String)) {
  char buf[120];
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
      sprintf(buf,"%04X: %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X<br />",
        address,
        data[0],data[1],data[2],data[3],
        data[4],data[5],data[6],data[7],
        data[8],data[9],data[10],data[11],
        data[12],data[13],data[14],data[15]
      );
      outFunc(String(buf));
    }
    yield();
  }  
}




//
// Dump the CONFIG memory parts of the PIC
//
void DumpConfig(void (*f)(const String)) {
  uint16_t data[16];
  char bin1[20];
  char bin2[20];
  char buf[1000];

  CmdLoadConfig(0x00);
  CmdReadData(data,16);

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
  f(String(buf));
}


//
// Flash one word of data into the specified location on the PIC
//
void Store(uint32_t address, uint16_t data) {
  Serial.printf("Store(address:0x%08X, data:0x%04X\n",address,data);
  if (address<currentAddress) CmdResetAddress();
  while (address>currentAddress) CmdIncAddress();
  Send(0x02,6); 
  Send(data<<1,16); 
  CmdBeginProgramI();
  delay(3);
}




//
//
//
void PicFlash(String filename) {
    int num=0;
    int cnt=0;
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

    // CODE
    Serial.print("Trying to open ");
    Serial.println(filename);
    File f = SPIFFS.open(filename, "r");
    if (!f) {
      webSocket.sendTXT(num, "pFile error");
      Serial.print("Couldn't open ");
      Serial.println(filename);
      return;
    }
    webSocket.sendTXT(num, "pFlashing...");
    uint16_t offset;
    while(f.available()) {
      uint8_t d_len;
      uint16_t d_addr;
      uint8_t d_typ;
      String s = f.readStringUntil('\n');
      Serial.println(s);
      d_len=HexDec2(s[1],s[2]);
      d_addr=HexDec4(s[3],s[4],s[5],s[6]);
      d_typ=HexDec2(s[7],s[8]);
      Serial.printf("len=%02x addres=%04x type=%02x\n",d_len,d_addr,d_typ);
      if (d_typ==0x00) {
        for (uint8_t i=0; i<d_len*2; i+=4) {
          uint32_t address=d_addr/2+i/4;
          uint16_t data=HexDec4(s[11+i],s[12+i],s[9+i],s[10+i]);
          if (offset==0) {
            cnt++;
            Store(address,data);
          } else {

          }
        }
      }
      if (d_typ==0x04) {
        offset=HexDec4(s[11],s[12],s[9],s[10]);
        Serial.printf("Offset=%04x\n",offset);
      }
    }
    f.close();
    char tmps[20];
    sprintf(tmps,"pTotal %d bytes flashed",cnt);
    webSocket.sendTXT(num, tmps);

    RESET_HIGH();
    DAT_INPUT;
    CLK_INPUT;
}


//
//
//
void PicReadConfigs(void (*f)(const String)) {
    EnterLVPmode();
    CmdResetAddress();
    DumpConfig(f);
    f("<br />");
    DumpMemory(f);
    RESET_HIGH();
    DAT_INPUT;
    CLK_INPUT;
}


//
//
//
void PicReset(char type) {
  if (type=='H') RESET_HIGH();
  if (type=='L') RESET_LOW();
  if (type=='P') {
      RESET_LOW();
      delay(250);
      RESET_HIGH();
  }
}

