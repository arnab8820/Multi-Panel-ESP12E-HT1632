/***************************************************
 * Pixel to segment mapping for 7 segment displays
 * 
 *    Segment   -->   Pixel
 *    A         -->   2
 *    B         -->   3
 *    C         -->   4
 *    D         -->   5
 *    E         -->   6
 *    F         -->   0
 *    G         -->   1
 *    DP        -->   7
 *    
 ****************************************************/

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

void clearDisplay(){
  HT1632.clear();
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

//display OTA message and progress
void otaDisplay(int progress){
  //character O
  displayDigit(0, 0);
  //character T
  HT1632.setPixel(1, 5);
  HT1632.setPixel(1, 6);
  HT1632.setPixel(1, 0);
  HT1632.setPixel(1, 1);
  //character A
  HT1632.setPixel(2, 2);
  HT1632.setPixel(2, 3);
  HT1632.setPixel(2, 4);
  HT1632.setPixel(2, 6);
  HT1632.setPixel(2, 0);
  HT1632.setPixel(2, 1);
  
  if(progress>=0){
    int pos = 8;
    while(progress!=0){
      displayDigit(pos, progress%10);
      progress=progress/10;
      pos--;
    }
  }
  HT1632.render();
}
