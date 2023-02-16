#include <ILI9341_t3.h>
#include <utility>
#include <I_no_can_speak_flex.h>
#include <cmath>
#include <string>
using namespace std;
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

//pot pin
static int pot_pin = A0;
static int pot_max = 1000;

//display config
int curr_display = 2;
int max_display = 2;

//for the main speed 
int last_n_digitds;
int last_n_digits_small;

//button vars to cheeck when the button is pressed
int buttonstate; 
int val;

//booleans for the different displays 
boolean debug_Display_hasRun = false;
boolean main_Display_hasRun = false;
boolean display_2_hasRun = false;


//can line
I_no_can_speak_flex CAN(true);


//tft skeleton (what runs on statrup)
//setup function for specifically the tft display 
void setupSkeleton() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
}


//setup func
void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT);
  buttonstate = digitalRead(7);
  CAN.begin();
  setupSkeleton();
}

//used to get the number of digits that need to be displayed 
unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

//shows the digits with a set precision of five leading zeros 
void printDig(int i, std::pair<int, int> x, int t_size){
  int n_digits = GetNumberOfDigits(i);
  tft.setCursor(x.first, x.second);
  tft.setTextSize(t_size);
  tft.printf("%03d", i);
}

//function used for printing and aligning the main speed dial on the main display
void printSpeedDig(int i, std::pair<int, int> x, int t_size){
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


//main display
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
  CAN.<node name>.get<>;
  replace all the i stuff with the can vars/funcs needed.
  */
  int i = rand() % 100;
  printSpeedDig(i * 10, g5, 16);
  printDig(i, g3, 2);
  printDig(i, g4, 2);
  //might require a delay so im keeping it
  delay(100);
}


//secondary display
void debug_Dsiplay_Setup(){
  //this is legit a setup that only runs once so that there is no flicker when it is contantly being reset 
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
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

void display_2_setup(){
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.drawLine(40,0, 40, 240, ILI9341_WHITE);
  tft.drawLine(0,25, 320, 25, ILI9341_WHITE);
  tft.setCursor(45, 0);
  tft.print("mtr");
  tft.setCursor(90, 0);
  tft.print("invrt");
  tft.setCursor(165, 0);
  tft.print("bat");
  tft.setCursor(0, 30);
  tft.print("C" + char(247));
  tft.setCursor(0, 60);
  tft.print("V");
  tft.setCursor(0, 90);
  tft.print("A");
}
void display_2(){
  printDig(CAN.DTI.getMotorTemp(), std::pair<int, int> (40,30), 2);
  printDig(CAN.DTI.getVoltIn(), std::pair<int, int> (40, 60), 2);
  printDig(CAN.DTI.getACCurrent(), std::pair<int, int> (40,90), 2);
}

void debug_Display(){
  CAN.readData();
  //Serial.println("this shit");
  //tft.setCursor(160, 10);
  printDig(CAN.DTI.getThrottleIn(), std::pair<int, int> (160,10), 2);
  //Serial.println(CAN.readData());
  
  
  
}

void loop() {
  
  //button press

  val = digitalRead(7);
  if(val != buttonstate){
    if(val == LOW){
      if(curr_display == max_display){
        curr_display = 0;
      }else{
        curr_display ++;
      }
      //requires a delay so that the counter does not go batshit crazy
      delay(250);
    }
    Serial.println(buttonstate);
    Serial.println(val);
    Serial.println(max_display);
  }
  buttonstate = val;
  //filler cod for when I get pot
  int pot_val = analogRead(pot_pin);
  if(pot_max/3 > pot_val > 0 ){
    curr_display = 0;
  }else if(pot_max/3 * 2 > pot_val > pot_max/3){
    curr_display = 1;
  }else{
    curr_display = 2;
  }

  

  CAN.readData();//I dont think I need this atm.
  //checks what display you are on and runs specifically that display 
  switch(curr_display){
    case(0):
      if(main_Display_hasRun == false){
        main_Display_Setup();
        main_Display_hasRun = true;
        debug_Display_hasRun = false;
        display_2_hasRun = false;
      }
      main_Display();
      return;
    case(1):
      if(debug_Display_hasRun == false){
        //main_Display_Setup();
        debug_Dsiplay_Setup();
        debug_Display_hasRun = true;
        main_Display_hasRun = false;
        display_2_hasRun = false;
      }
      debug_Display();
      return;
    case(2):
      if(display_2_hasRun == false){
        display_2_setup();
        debug_Display_hasRun = false;
        main_Display_hasRun = false;
        display_2_hasRun = true;
      }
  }
}