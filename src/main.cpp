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

//top right
static std::pair<int, int> g1(15 , 30);
//top left
static std::pair<int, int> g2(269 , 5);
//bottom right
static std::pair<int, int> g3(130 , 200);
//bottom left
static std::pair<int, int> g4(130 , 220);
//middle
static std::pair<int, int> g5(120 , 30);

int curr_color = 0X0000;
int curr_display = 1;
int last_n_digitds = 0;

boolean debug_Display_hasRun = false;
boolean main_Display_hasRun = false;


//can line

I_no_can_speak_flex CAN(true);

//random ass func to chractarize static things
//basically this code is what draws the squares around the numbers and shii 
void setupSkeleton() {
  tft.fillScreen(ILI9341_BLACK);
}

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(curr_color);
  setupSkeleton();
  CAN.begin();
  Serial.begin(9600);
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
void printDig(int i, std::pair<int, int> x, int t_size, int color){
  unsigned n_digits = GetNumberOfDigits(i);
  tft.fillRect(x.first,x.second, (n_digits + 1) * 6 * t_size, 8 * t_size , color);
  tft.setCursor(x.first,x.second);
  tft.setTextSize(t_size);
  tft.println(i);
}

void printSpeedDig(int i, std::pair<int, int> x, int t_size, int color){
  int n_digits = GetNumberOfDigits(i);
  if(last_n_digitds < n_digits){
    last_n_digitds = n_digits;
  }
  if(last_n_digitds > n_digits){
    tft.fillRect(0,0,320,144,ILI9341_BLACK);
    last_n_digitds = n_digits;
  }
  int z = 170 - ((n_digits) * 6 * t_size)/2;
  tft.setCursor(z,x.second);
  tft.setTextSize(t_size);
  tft.println(i);
}

void main_Display_Setup(){
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(5,200);
  tft.setTextSize(2);
  tft.println("Batt Temp: ");
  tft.setCursor(5, 220);
  tft.println("Tire Temp: ");
}

void main_Display(){
  /*
  CAN.readData();
  replace all the i stuff with the can vars/funcs needed.
  */
  for(int i; i < 99; i++){
    printSpeedDig(i * 10, g5, 16, ILI9341_BLACK);
    printDig(i, g3, 2, 0x0000);
    printDig(i, g4, 2, 0x0000);
    delay(100);
  }
}
//dial positions for the debug display 
static std::pair<int, int> x;


void debug_Dsiplay_Setup(){
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.drawLine(0,40,200,40, ILI9341_WHITE);
  tft.drawLine(0,80,200,80, ILI9341_WHITE);
  tft.drawLine(0,120,200,120, ILI9341_WHITE);
  tft.drawLine(0,160,200,160, ILI9341_WHITE);
  tft.drawLine(0,200,200,200, ILI9341_WHITE);
  tft.drawLine(0,240,200,240, ILI9341_WHITE);
  tft.setCursor(5,10);
  tft.println("Batt Temp:  ");
  tft.setCursor(5,50);
  tft.println("Motor Temp: ");
  tft.setCursor(5,90);
  tft.println("Volatge:    ");
  tft.setCursor(5,130);
  tft.println("Status:     ");
  tft.setCursor(5,170);
  tft.println("CAN Speed:  ");
  tft.setCursor(5,210);
  tft.println("Death:      ");

  return;
}

void debug_Display(){
  //Serial.println("this shit");
  
  
}

void loop() {
  CAN.readData();
  switch(curr_display){
    case(0):
      if(main_Display_hasRun == false){
        main_Display_Setup();
        main_Display_hasRun = true;
        debug_Display_hasRun = false;
      }
      main_Display();
      return;
    case(1):
      if(debug_Display_hasRun == false){
        main_Display_Setup();
        debug_Dsiplay_Setup();
        debug_Display_hasRun = true;
        main_Display_hasRun = false;
      }
      debug_Display();
      return;
    case(2):
      return;
  }


    //next part is to get a button or smthn so I can somehow go throuhg the different pannels
    //will use a switch statement for that and jsut shove that in the loop :)
}