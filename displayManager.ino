#include <HT1632.h>

unsigned char c2h(int num){
  switch(num){
    case 0:
      return 0xBE;
    case 1:
      return 0x18;
    case 2:
      return 0x76;
    case 3:
      return 0x7c;
    case 4:
      return 0xD8;
    case 5:
      return 0xEC;
    case 6:
      return 0xEE;
    case 7:
      return 0x38;
    case 8:
      return 0xFE;
    case 9:
      return 0xFC;
    default:
      return 0x00;
  }
}

void clearDigit(int pos){
  for(int i=0; i<8; i++){
    HT1632.clearPixel(pos, i);
  }
}

void displayDigit(int pos, int digit){
  //clear the previous digit first
  clearDigit(pos);
  unsigned char d = c2h(digit);
  for(int i=7; i>=0; i--){
    if(d&0x01==1){
      HT1632.setPixel(pos, i);
    }
    d = d>>1;
  }
  HT1632.render();
}

void initDisplay(){
  HT1632.begin(CS, WR, DATA);
  HT1632.fillAll();
  HT1632.render();
  delay(2000);
  HT1632.clear();
}

void displayTest(){
}
