#include <ILI9341_t3.h>
#include <utility>
#include <I_no_can_speak_flex.h>
#include <cmath>
//basically just defining the pins that are used on the teensy
#define CS_PIN  8
#define TIRQ_PIN  2
#define TFT_DC      9
#define TFT_CS      10
#define TFT_RST    255  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

//no clue what this is for 
#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define LEFT_BUTTON 5
#define RIGHT_BUTTON 6

/*
 * 0 - None
 * 1 - Up
 * 2 - Down
 * 3 - Left / Back
 * 4 - Right / Forward
 */

////four example guages
static std::pair<int, int> g1(5 , 5);
static std::pair<int, int> g2(51 , 5);
static std::pair<int, int> g3(97 , 5);
static std::pair<int, int> g4(100 , 100);

//param for the size of the text
static int sizeoftext = 4;


//can line

I_no_can_speak_flex CAN(true);

//random ass func to chractarize static things
//basically this code is what draws the squares around the numbers and shii 
void setupSkeleton() {

  tft.fillScreen(ILI9341_BLACK);
  //tft.drawRect(0, 0, 36, 46, ILI9341_CYAN);
  //tft.drawRect(46, 0, 36, 46, ILI9341_GREEN);
  //tft.drawRect(92, 0, 36, 46,ILI9341_PURPLE);
  //tft.drawRect(95, 95, 100, 46,ILI9341_DARKGREEN);
  //tft.setTextColor(ILI9341_RED);
  //tft.setTextSize(3);
}

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(0xffff);
  setupSkeleton();
  CAN.begin();
}

//used to get the number of digits that need to be displayed 
unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}
/*
An example function to update the specific guage with params: i, x
param i: number to update to
param x: lcoation of the guage stored in an std::pair
*/
void printDig(int i, std::pair<int, int> x){
  unsigned n_digits = GetNumberOfDigits(i);
  tft.fillRect(x.first,x.second, n_digits * 6 * sizeoftext, 8 * sizeoftext , 0x0000);
  tft.setCursor(x.first,x.second);
  tft.setTextSize(sizeoftext);
  tft.println(i);
}
//same as above func but like 2 digits 
void printDoubleDig(int i, std::pair<int, int> x){
  tft.fillRect(x.first,x.second, 3 * 6 * sizeoftext,  8 * sizeoftext , 0x0000);
  tft.setCursor(x.first,x.second);
  tft.setTextSize(sizeoftext);
  tft.println(i);
}


int time = 0;
void loop() {
  CAN.readData();
  for(int i; i < 100; i++){
    printDoubleDig(i, g4);
    delay(1000);
  }
  /*
  if(millis() - time > 10){
    printDoubleDig(CAN.getThrottleIn(), g4);
    printDoubleDig(CAN.getVoltIn(), g1);
    Serial.println(CAN.getThrottleIn());
    time = millis();
  }
  */
  /*
  for(int i = 0; i< 10; i++){
    printSingleDig(i, g1);
    printSingleDig(i , g2);
    printSingleDig(i , g3);
    printDoubleDig(i * 2, g4);
    delay(200);
  }
  */
}