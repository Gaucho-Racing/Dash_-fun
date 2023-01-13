#include "FlexCAN_T4-master/FlexCAN_T4.h"


#define INT8U byte
#define INT32U unsigned long


#define CAN_INT 2
#define CAN_CS 10
#define DTI_Id 22
#define BMS_Id 0x7EB

#ifndef I_no_can_speak_flex_
#define I_no_can_speak_flex_

#define Sender true
#define Reciever false


class I_no_can_speak_flex
{
public:
    I_no_can_speak_flex(bool);
    bool readData();
    bool readData(INT32U*);
    void getData();
    bool begin();
    bool getTrue();
    bool getFalse();
    void rawData(INT32U*, INT8U*);
    void setDtiID(INT8U);
    void setBmsId(INT8U);


    bool BMSSHIT();


////////////////////////something to return id and data or somehitng
    long getERPM();
    float getDuty();
    int getVoltIn();
    float getACCurrent();
    float getDCCurrent();
    float getInvTemp();
    float getMotorTemp();
    byte getFaults();
    float getId();
    float getIq();
    byte getThrottleIn();
    byte getBrakeIn();
    bool getD1();
    bool getD2();
    bool getD3();
    bool getD4();
    bool getDO1();
    bool getDO2();
    bool getDO3();
    bool getDO4();
    bool getDriveEnable();
    bool getCapTempLim();
    bool getDCCurrentLim();
    bool getDriveEnableLim();
    bool getIgbtAccelTempLim();
    bool getIgbtTempLim();
    bool getVoltInLim();
    bool getMotorAccelTempLim();
    bool getMotorTempLim();
    bool getRPMMinLimit();
    bool getRPMMaxLimit();
    bool getPowerLimit();

    float getPackCurrent();
    float getPackVoltage();
    float getPackSOC();
    int getPackHealth();
    int getPackTemp();
    float getCellVoltage();
    float getOCellVoltage();

    void setCurrent(float);
    void setBrakeCurrent(float);
    void setERPM(long);
    void setPosition(float);
    void setRCurrent(float);
    void setRBrakeCurrent(float);
    void setMaxCurrent(float);
    void setMaxBrakeCurrent(float);
    void setMaxDCCurrent(float);
    void setMaxDCBrakeCurrent(float);
    void setDriveEnable(byte);
    void send(long, long, int);
    void sendToInv(int, long, int);


    void ping(byte);



    void BMSrequest(int);
    void printer();
};
#endif
