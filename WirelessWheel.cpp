#include "WirelessWheel.h"
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(2, 10);  // CE, CSN

WirelessWheel::WirelessWheel(){
    const byte address[6] = "VROOM";
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.startListening();
}

void WirelessWheel::read(){
    if (radio.available())
        radio.read(&data, 14);
}

byte WirelessWheel::getSpeed(){return data[0];}
byte WirelessWheel::getChargeState(){return data[1];}
byte WirelessWheel::getAAPS(){return data[2];}
byte WirelessWheel::getBrakePressure(){return data[3];}
int WirelessWheel::getPower(){return ((int)data[4] << 8) + data[5];}
int WirelessWheel::getVoltage(){return ((int)data[6] << 8) + data[7];}
int WirelessWheel::getCurrent(){return ((int)data[8] << 8) + data[9];}
byte WirelessWheel::getBatteryTemp(){return data[10];}
byte WirelessWheel::getMotorTemp(){return data[11];}
byte WirelessWheel::getInvertorTemp(){return data[12];}
boolean WirelessWheel::getSensor1_Status(){return data[13] & 0b10000000;}
boolean WirelessWheel::getSensor1_Status(){return data[13] & 0b01000000;}
boolean WirelessWheel::getBMS_Status(){return data[13] & 0b00100000;}
boolean WirelessWheel::getInvertor_Status(){return data[13] & 0b00010000;}
boolean WirelessWheel::getDatalogger_Status(){return data[13] & 0b00001000;}