

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
// prints 16-bit data in hex with leading zeroes 
//
char *ToHex16(uint16_t *data, uint8_t length) {
  static char buf[(4+1)*16+1];
  char *p;
  char tmp[7];
  uint8_t first,second,third,fourth;
  
  p=buf;
  for (uint8_t i=0; i<length; i++) {
    first = (data[i] >> 12) & 0x0f;
    second = (data[i] >> 8) & 0x0f;
    third = (data[i] >> 4) & 0x0f;
    fourth = data[i] & 0x0f;
    *p = first+48;  if (first > 9) *p += 7;
    p++;
    *p = second+48; if (second > 9) *p += 7;
    p++;
    *p = third+48;  if (third > 9) *p += 7;
    p++;
    *p = fourth+48; if (fourth > 9) *p += 7;
    p++;
    *p++ = ' '; // add trailing space
  }
  *p=0;
  return buf;
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


