#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
class WirelessWheel
{
public:
    byte data[14] = {0};

    WirelessWheel();
    void read();
    byte getSpeed();
    byte getChargeState();
    byte getAAPS();
    byte getBrakePressure();
    int getPower();
    int getVoltage();
    int getCurrent();
    byte getBatteryTemp();
    byte getMotorTemp();
    byte getInvertorTemp();
    boolean getSensor1_Status();
    boolean getSensor1_Status();
    boolean getBMS_Status();
    boolean getInvertor_Status();
    boolean getDatalogger_Status();

};