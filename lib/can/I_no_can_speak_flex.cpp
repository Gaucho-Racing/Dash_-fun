#include "I_no_can_speak_flex.h"
#include <FlexCAN_T4.h>
byte data[5][8]={{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //0 erpm,  Duty cycle,  Input voltage
                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //1 AC current,  DC current
                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //2 Controller temp,  Motor temp,  Fault codes
                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //3 FOC Id,  FOC Iq
                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; //4 Throttle input,  Brake input,  Digital in,  Digital out,  
                                                                    //Drive enable,  Cap temp limit,  DC current limit, Drive enabe limit,  
                                                                    //IGBT accleration temp limit,  IGBT temp limit,  Vin limit,  Motor accleration temp limit,
                                                                    //Motor temp limit,  RPM min limit,  RPM max limit,  Rower limit,  CAN version

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;


unsigned long lastRecieveTime=millis();
byte BMSInfo[24];
byte BMSIndex=0;
int BMSData[70];
unsigned long stopwatch=0;

bool canSend = false;



/*********************************************************************************************************
** Stuff
** 
*********************************************************************************************************/





I_no_can_speak_flex::I_no_can_speak_flex(bool s){canSend = s;}

bool I_no_can_speak_flex::begin(){   //Coordinate the magic CAN pixies to dance together 
    Serial.begin(115200);
    can1.begin();
    can1.setBaudRate(1000000);
    return true;
}




bool I_no_can_speak_flex::readData(){    //Read data from the inverter or the BMS
    lastRecieveTime = millis();
    if(!can1.read(msg)) // fix this line I beg you
        return 0;
    bool ext = msg.flags.extended;
    if(!ext)
        Serial.println("WHAT IS THIS SHIT???? I WANT AN EXTENDED FRAME!!!! COME ON DUDE");
    else if((int)(msg.id & 0xFF) == DTI_Id){
        if(msg.id >> 8 >= 0 && msg.id >> 8 < 5){
            for(int i = 0; i < 8; i++)
                data[msg.id >> 8][i] = msg.buf[i];//maybe add timestamp?
            return true;
        }
        Serial.println("ID not any information from inverter");
    }
    else if(msg.id == BMS_Id){
        //if(BMSSHIT()) //Fuck this

        Serial.print("BMS "); Serial.println(msg.id);
    }
    else if(msg.id == 105){
        Serial.println(micros()-stopwatch);
    }
    else{
        Serial.print("Unknown CAN Id "); Serial.println(msg.id);
    }
    return false;
}
bool I_no_can_speak_flex::readData(INT32U *tim){//Read function that gives the time since the last read
    *tim = millis() - lastRecieveTime;
    return readData();
}
void I_no_can_speak_flex::rawData(INT32U *id, INT8U *buffer){*id=msg.id; buffer=msg.buf;}  //Gives back the raw Id and data array


bool I_no_can_speak_flex::BMSSHIT(){       //Recieve stuff (the long stuff) from the BMS *NOT FINSIHED*
    if(msg.buf[0]==0x10){
        if(canSend){send(BMS_Id+0x8, 0x30, 1);} ///////////////////////MIGHT BE A PROBLEM
        for(int i=0; i<24; i++)
            BMSInfo[i]=0x0;
        BMSIndex = msg.buf[1]-6;
        for(int i=0; i<8; i++)
            BMSInfo[i]=msg.buf[i];
        return false;
    }
    else if(msg.buf[0]==0x20||msg.buf[0]==0x21||msg.buf[0]==0x22||msg.buf[0]==0x23){
        if(BMSIndex > 7){
            for(int i=0; i<7; i++)
                BMSInfo[i+BMSInfo[1]-BMSIndex]=msg.buf[i+1];
            BMSIndex-=7;
            return false;
        }
        else{
            for(int i=0; i<BMSIndex; i++)
                BMSInfo[i+BMSInfo[1]-BMSIndex]=msg.buf[i+1];
            BMSIndex=0;
            return true;
        }
    }
    else if(msg.buf[1]==0x62){// Data recieve thing
        //Have to use msg.buf[2/3] to sort the data into some array of integers?????
        if(msg.buf[2]==0xF0 && (msg.buf[3]<=0x49 && msg.buf[3]>=0x04)){
            if(msg.buf[0]==6)
                BMSData[msg.buf[3]-4] = msg.buf[4] << 8 + msg.buf[5];
            else if(msg.buf[0]==5)
                BMSData[msg.buf[3]-4] = msg.buf[4];
            else
                Serial.println("ERROR?? honestly i don't even know at this point. Give up?");
        }
        else if(msg.buf[2]==0xF0 && (msg.buf[3]<=0x49 && msg.buf[3]>=0x04)){
            //change for cell data ig.
        }
    }
    else if(msg.buf[1]==0x23){ // Fault recieve

        //other data idfk
        Serial.println("ill do this later");
    }
    //else
    return false;
}
void I_no_can_speak_flex::BMSrequest(int pid){
    byte stuff[8] = {pid & 0xFF00, pid & 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if(canSend){can1.read(msg);} //this line too
    else{Serial.println("Nope you can't request BMS data cuz ur a reciever. So sad. Go cry about it");}
}


void I_no_can_speak_flex::send(long OutId, long data, int dataLength){    //Sends 8 bytes with that Id and that data shifted left all the way
    byte stuff[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    for(int i=0; i<dataLength; i++)
        stuff[i] = (data >> (i*8));
    for (int i = 0; i < 8; i++)
        msg.buf[i] = stuff[i];
    msg.id = OutId;
    if(canSend){can1.write(msg);} //por favor mi amor
    else{Serial.println("YOU TRIED TO SEND????? - note this will prob appear when recieving from the bms actually nevermind ill fix that now");}
}
void I_no_can_speak_flex::sendToInv(int OutId, long data, int dataLength){     //Sends data to the inverter
    send(((long)OutId << 8) + DTI_Id, data, dataLength);
}



void I_no_can_speak_flex::ping(byte id){
    msg.id = id;
    stopwatch=micros();
    can1.write(msg);
}




































/*********************************************************************************************************
** Getter/Setter Stuff
** 
*********************************************************************************************************/










long I_no_can_speak_flex::getERPM() {return(((long)data[0][0] << 24) + ((long)data[0][1] << 16) + ((long)data[0][2] << 8) + data[0][3]);}
float I_no_can_speak_flex::getDuty() {return((((long)data[0][4] << 8) + data[0][5])/10);}
int I_no_can_speak_flex::getVoltIn() {return(((long)data[0][6] << 8) + data[0][7]);}
float I_no_can_speak_flex::getACCurrent() {return((((long)data[1][0] << 8) + data[1][1])/10);}
float I_no_can_speak_flex::getDCCurrent() {return(((long)(data[1][2] << 8) + data[1][3])/10);}
float I_no_can_speak_flex::getInvTemp() {return((((long)data[2][0] << 8) + data[2][1])/10);}
float I_no_can_speak_flex::getMotorTemp() {return((((long)data[2][2] << 8) + data[2][3])/10);}
byte I_no_can_speak_flex::getFaults() {return data[2][4];}
float I_no_can_speak_flex::getId() {return((((long)data[3][0] << 24) + ((long)data[3][1] << 16) + ((long)data[3][2] << 8) + data[3][3])/100);}
float I_no_can_speak_flex::getIq() {return((((long)data[3][4] << 24) + ((long)data[3][5] << 16) + ((long)data[3][6] << 8) + data[3][7])/100);}
byte I_no_can_speak_flex::getThrottleIn() {return data[4][0];}
byte I_no_can_speak_flex::getBrakeIn() {return data[4][1];}
bool I_no_can_speak_flex::getD1() {return ((data[4][2] & 0x80) == 0x80);}
bool I_no_can_speak_flex::getD2() {return ((data[4][2] & 0x40) == 0x40);}
bool I_no_can_speak_flex::getD3() {return ((data[4][2] & 0x20) == 0x20);}
bool I_no_can_speak_flex::getD4() {return ((data[4][2] & 0x10) == 0x10);}
bool I_no_can_speak_flex::getDO1() {return ((data[4][2] & 0x08) == 0x08);}
bool I_no_can_speak_flex::getDO2() {return ((data[4][2] & 0x04) == 0x04);}
bool I_no_can_speak_flex::getDO3() {return ((data[4][2] & 0x02) == 0x02);}
bool I_no_can_speak_flex::getDO4() {return ((data[4][2] & 0x01) == 0x01);}
bool I_no_can_speak_flex::getDriveEnable() {return ((data[4][3] & 0x01) == 0x01);}
bool I_no_can_speak_flex::getCapTempLim() {return ((data[4][4] & 0x80) == 0x80);}
bool I_no_can_speak_flex::getDCCurrentLim() {return ((data[4][4] & 0x40) == 0x40);}
bool I_no_can_speak_flex::getDriveEnableLim() {return ((data[4][4] & 0x20) == 0x20);}
bool I_no_can_speak_flex::getIgbtAccelTempLim() {return ((data[4][4] & 0x10) == 0x10);}
bool I_no_can_speak_flex::getIgbtTempLim() {return ((data[4][4] & 0x08) == 0x08);}
bool I_no_can_speak_flex::getVoltInLim() {return ((data[4][4] & 0x04) == 0x04);}
bool I_no_can_speak_flex::getMotorAccelTempLim() {return ((data[4][4] & 0x02) == 0x02);}
bool I_no_can_speak_flex::getMotorTempLim() {return ((data[4][4] & 0x01) == 0x01);}
bool I_no_can_speak_flex::getRPMMinLimit() {return ((data[4][5] & 0x80) == 0x80);}
bool I_no_can_speak_flex::getRPMMaxLimit() {return ((data[4][5] & 0x40) == 0x40);}
bool I_no_can_speak_flex::getPowerLimit() {return ((data[4][5] & 0x20) == 0x20);}

float I_no_can_speak_flex::getPackCurrent() {return (BMSData[8]);}
float I_no_can_speak_flex::getPackVoltage() {return (BMSData[9]);}
float I_no_can_speak_flex::getPackSOC() {return (BMSData[11]);}
int I_no_can_speak_flex::getPackHealth() {return (BMSData[15]);}
int I_no_can_speak_flex::getPackTemp() {return (BMSData[38]);}
float I_no_can_speak_flex::getCellVoltage() {return (BMSData[48]);}
float I_no_can_speak_flex::getOCellVoltage() {return (BMSData[51]);}

void I_no_can_speak_flex::setCurrent(float in){sendToInv(0x1A, (long)(in*10), 2);}
void I_no_can_speak_flex::setBrakeCurrent(float in){sendToInv(0x1B, (long)(in*10), 2);}
void I_no_can_speak_flex::setERPM(long in){sendToInv(0x1C, (long)in, 4);}
void I_no_can_speak_flex::setPosition(float in){sendToInv(0x1D, (long)in, 2);}
void I_no_can_speak_flex::setRCurrent(float in){sendToInv(0x1E, (long)(in*10), 2);}
void I_no_can_speak_flex::setRBrakeCurrent(float in){sendToInv(0x1F, (long)(in*10), 2);}
void I_no_can_speak_flex::setMaxCurrent(float in){sendToInv(0x20, (long)(in*10), 2);}
void I_no_can_speak_flex::setMaxBrakeCurrent(float in){sendToInv(0x21, (long)(in*10), 2);}
void I_no_can_speak_flex::setMaxDCCurrent(float in){sendToInv(0x22, (long)(in*10), 2);}
void I_no_can_speak_flex::setMaxDCBrakeCurrent(float in){sendToInv(0x23, (long)(in*10), 2);}
void I_no_can_speak_flex::setDriveEnable(byte in){sendToInv(0x24, (long)in, 1);}





bool I_no_can_speak_flex::getTrue(){return true;}         //Just returns true *For testing*
bool I_no_can_speak_flex::getFalse(){return false;}       //Just returns true *For testing*
void I_no_can_speak_flex::setDtiID(INT8U in){        //Sets the inverter ID
    #undef DTI_Id
    #define DTI_Id in
}
void I_no_can_speak_flex::setBmsId(INT8U in){        //Sets the BMS ID
    #undef BMS_Id
    #define BMS_Id in
}




/*********************************************************************************************************
** Datalogging/Debuging
** 
*********************************************************************************************************/
void I_no_can_speak_flex::printer(){
  for(int i=0; i<5; i++){
    for(int o=0; o<8; o++){
      Serial.print(data[i][o]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}
