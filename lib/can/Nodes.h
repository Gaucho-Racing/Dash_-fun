#include "arduino.h"
#include <FlexCAN_T4.h>
#include <SD.h>
#include <SPI.h>
/*

-------------------------------------------------------------------
TEMPLATE FOR NEW NODE

struct *NAME*
{
    unsigned long ID = 0;

    *NAME*(unsigned long id){ID = id;}

    void receive(unsigned long id, byte buf[]){
        -id will be the message ID from the node
        -buf is a byte array of the data from the CAN frame
        *CODE FOR SORTING YOUR DATA OR WHATEVER IDFK*
    }
};

--------------------------------------------------------------------
*/



struct Inverter //done?
{
    byte data[5][8]={{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //0 erpm,  Duty cycle,  Input voltage
                     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //1 AC current,  DC current
                     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //2 Controller temp,  Motor temp,  Fault codes
                     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //3 FOC Id,  FOC Iq
                     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; //4 Throttle input,  Brake input,  Digital in,  Digital out,  
                                                                    //Drive enable,  Cap temp limit,  DC current limit, Drive enabe limit,  
                                                                    //IGBT accleration temp limit,  IGBT temp limit,  Vin limit,  Motor accleration temp limit,
                                                                    //Motor temp limit,  RPM min limit,  RPM max limit,  Rower limit,  CAN version
    unsigned long ID = 0;
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can2;
    CAN_message_t msg2;
    bool canSend2 = true;
    Inverter(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){
        if(id >> 8 >= 0 && id >> 8 < 5){
            for(int i = 0; i < 8; i++)
                data[id >> 8][i] = buf[i];//maybe add                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            timestamp?
            return;
        }
        else{
            Serial.print(id, HEX);
            Serial.println(" is not data from inverter");
        }
    }
    void send(long OutId, long data, int dataLength){    //Sends 8 bytes with that Id and that data shifted left all the way
        byte stuff[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        for(int i=0; i<dataLength; i++)
            stuff[i] = (data >> (i*8));
        for (int i = 0; i < 8; i++)
            msg2.buf[i] = stuff[i];
        msg2.id = ((long)OutId << 8) + ID;
        if(canSend2){can2.write(msg2);} //por favor mi amor
        else{Serial.println("YOU TRIED TO SEND????? - note this will prob appear when recieving from the bms actually nevermind ill fix that now");}
    }
    unsigned long getID() {return ID;}
    long getERPM() {return(((long)data[0][0] << 24) + ((long)data[0][1] << 16) + ((long)data[0][2] << 8) + data[0][3]);}
    float getDuty() {return((((long)data[0][4] << 8) + data[0][5])/10);}
    int getVoltIn() {return(((long)data[0][6] << 8) + data[0][7]);}
    float getACCurrent() {return((((long)data[1][0] << 8) + data[1][1])/10);}
    float getDCCurrent() {return(((long)(data[1][2] << 8) + data[1][3])/10);}
    float getInvTemp() {return((((long)data[2][0] << 8) + data[2][1])/10);}
    float getMotorTemp() {return((((long)data[2][2] << 8) + data[2][3])/10);}
    byte getFaults() {return data[2][4];}
    float getCurrentD() {return((((long)data[3][0] << 24) + ((long)data[3][1] << 16) + ((long)data[3][2] << 8) + data[3][3])/100);}
    float getCurrentQ() {return((((long)data[3][4] << 24) + ((long)data[3][5] << 16) + ((long)data[3][6] << 8) + data[3][7])/100);}
    byte getThrottleIn() {return data[4][0];}
    byte getBrakeIn() {return data[4][1];}
    bool getD1() {return ((data[4][2] & 0x80) == 0x80);}
    bool getD2() {return ((data[4][2] & 0x40) == 0x40);}
    bool getD3() {return ((data[4][2] & 0x20) == 0x20);}
    bool getD4() {return ((data[4][2] & 0x10) == 0x10);}
    bool getDO1() {return ((data[4][2] & 0x08) == 0x08);}
    bool getDO2() {return ((data[4][2] & 0x04) == 0x04);}
    bool getDO3() {return ((data[4][2] & 0x02) == 0x02);}
    bool getDO4() {return ((data[4][2] & 0x01) == 0x01);}
    bool getDriveEnable() {return ((data[4][3] & 0x01) == 0x01);}
    bool getCapTempLim() {return ((data[4][4] & 0x80) == 0x80);}
    bool getDCCurrentLim() {return ((data[4][4] & 0x40) == 0x40);}
    bool getDriveEnableLim() {return ((data[4][4] & 0x20) == 0x20);}
    bool getIgbtAccelTempLim() {return ((data[4][4] & 0x10) == 0x10);}
    bool getIgbtTempLim() {return ((data[4][4] & 0x08) == 0x08);}
    bool getVoltInLim() {return ((data[4][4] & 0x04) == 0x04);}
    bool getMotorAccelTempLim() {return ((data[4][4] & 0x02) == 0x02);}
    bool getMotorTempLim() {return ((data[4][4] & 0x01) == 0x01);}
    bool getRPMMinLimit() {return ((data[4][5] & 0x80) == 0x80);}
    bool getRPMMaxLimit() {return ((data[4][5] & 0x40) == 0x40);}
    bool getPowerLimit() {return ((data[4][5] & 0x20) == 0x20);}

    void setCurrent(float in) {send(0x1A, (long)(in*10), 2);}
    void setBrakeCurrent(float in) {send(0x1B, (long)(in*10), 2);}
    void setERPM(long in) {send(0x1C, (long)in, 4);}
    void setPosition(float in) {send(0x1D, (long)in, 2);}
    void setRCurrent(float in) {send(0x1E, (long)(in*10), 2);}
    void setRBrakeCurrent(float in) {send(0x1F, (long)(in*10), 2);}
    void setMaxCurrent(float in) {send(0x20, (long)(in*10), 2);}
    void setMaxBrakeCurrent(float in) {send(0x21, (long)(in*10), 2);}
    void setMaxDCCurrent(float in) {send(0x22, (long)(in*10), 2);}
    void setMaxDCBrakeCurrent(float in) {send(0x23, (long)(in*10), 2);}
    void setDriveEnable(byte in) {send(0x24, (long)in, 1);}
};







struct Battery ///done for now
{
    byte BMSInfo[24];
    byte BMSIndex=0;
    int BMSData[70];
    unsigned long ID = 0;
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can2;
    CAN_message_t msg2;
    bool canSend2 = true;
    Battery(unsigned long id){ID = id;}
    unsigned long getID() {return ID;}
    
    void receive(unsigned long id, byte buf[]){
        if(msg2.buf[0]==0x10){
            if(canSend2){
                msg2.id = ID+0x8;
                byte stuff[8] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                for (int i = 0; i < 8; i++)
                    msg2.buf[i] = stuff[i];///////////////////////MIGHT BE A PROBLEM
                can2.write(msg2);
            }
            for(int i=0; i<24; i++)
                BMSInfo[i]=0x0;
            BMSIndex = msg2.buf[1]-6;
            for(int i=0; i<8; i++)
                BMSInfo[i]=msg2.buf[i];
            return;
        }
    else if(msg2.buf[0]==0x20||msg2.buf[0]==0x21||msg2.buf[0]==0x22||msg2.buf[0]==0x23){
        if(BMSIndex > 7){
            for(int i=0; i<7; i++)
                BMSInfo[i+BMSInfo[1]-BMSIndex]=msg2.buf[i+1];
            BMSIndex-=7;
            return;
        }
        else{
            for(int i=0; i<BMSIndex; i++)
                BMSInfo[i+BMSInfo[1]-BMSIndex]=msg2.buf[i+1];
            BMSIndex=0;
            return;
        }
    }
    else if(msg2.buf[1]==0x62){// Data receive thing
        //Have to use msg.buf[2/3] to sort the data into some array of integers?????
        if(msg2.buf[2]==0xF0 && (msg2.buf[3]<=0x49 && msg2.buf[3]>=0x04)){
            if(msg2.buf[0]==6)
                BMSData[msg2.buf[3]-4] = (msg2.buf[4] << 8) + msg2.buf[5];
            else if(msg2.buf[0]==5) 
                BMSData[msg2.buf[3]-4] = msg2.buf[4];
            else
                Serial.println("ERROR?? honestly i don't even know at this point. Give up?");
            }
            else if(msg2.buf[2]==0xF0 && (msg2.buf[3]<=0x49 && msg2.buf[3]>=0x04)){
                //change for cell data ig.
            }
        }
        else if(msg2.buf[1]==0x23){ // Fault receive

            //other data idfk
            Serial.println("ill do this later");
        }
        //else
        return;
    }


    float getCurrent() {return (BMSData[8]);}
    float getVoltage() {return (BMSData[9]);}
    float getChargeState() {return (BMSData[11]);}
    int getHealth() {return (BMSData[15]);}
    int getTemp() {return (BMSData[38]);}
    float getCellVoltage() {return (BMSData[48]);}
    float getOCellVoltage() {return (BMSData[51]);}

    void request(int pid){
        Serial.println("BMSREQUEST MIGHT BE WRONG");
        byte stuff[8] = {(byte)(pid & 0xFF00), (byte)(pid & 0xFF), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//MGITH BE WRONG
        if(canSend2){
            for (int i = 0; i < 8; i++)
                msg2.buf[i] = stuff[i];
            msg2.id = ID;
            can2.write(msg2);
        } //this line too
        else{Serial.println("Nope you can't request BMS data cuz ur a receiver. So sad. Go cry about it");}
    }
};





struct Isolation_Monitor // needs to be on a different bus
{
    byte data = 0;
    unsigned long ID = 0;
    Isolation_Monitor(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){
        if(buf[0] == 0xE0){
            data = buf[1];
        }
        else{
            Serial.print("Mux code: ");
            Serial.print(buf[0]);
            Serial.print(" is not what is expected. Full data is ");
            for(int i=0; i<8; i++){
                Serial.print(buf[i]);
                Serial.print(" ");
            }
            Serial.println();
        }
    }

    boolean getIsolation() {return(!((data & 0b10) + (data & 0b1)));} //true: Status ok, false: fail/warning
    byte getIsolationStates() {return(data & 0b11);} //00:Ok, 01: status unknown, 10: warning, 11: fault
    boolean getHardware_Error() {return(data & 0b10000000);}
    boolean getTouch_energy_fault() {return(data & 0b1000000);}
    boolean getHigh_Uncertainty() {return(data & 0b100000);}
    boolean getExc_off() {return(data & 0b10000);}
    boolean getHigh_Battery_Voltage() {return(data & 0b1000);}
    boolean getLow_Battery_Voltage() {return(data & 0b100);}
};




struct Charger
{
    unsigned long ID = 0;
    Charger(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){}
};



struct Sensors //TODO:   use the scaling thing
{
    byte data[12][8]={{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //0 GPSlatitude [0-3], GPSlatitudeHP [4]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //1 GPSlongitude [0-3], GPSlongitudeHP [4]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //2 GPSaccuracy [0-3]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //3 LinearAccel_X [0-3], LinearAccel_Y [4-7]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //4 LinearAccel_Z [0-3], AbsOrentation_X [4-7]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //5 AbsOrentation_Y [0-3], AbsOrentation_Z [4-7]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //6 AngVelocity_X [0-3], AngVelocity_Y [4-7]
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //7 AngVelocity_Z [0-3], ScalingFactor
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //8 FR {temp [0-1], temp [2-3], sus [4], speed [5-6], pressure [7]}
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //9 FL {temp [0-1], temp [2-3], sus [4], speed [5-6], pressure [7]}
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //10 RR {temp [0-1], temp [2-3], sus [4], speed [5-6], pressure [7]}
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; //11 RL {temp [0-1], temp [2-3], sus [4], speed [5-6], pressure [7]}
    unsigned long ID = 0;
    Sensors(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){
        if(id >> 8 >= 0 && id >> 8 < 12){
            for(int i = 0; i < 8; i++)
                data[id >> 8][i] = buf[i];//maybe add timestamp?
            return;
        }
        else{
            Serial.print(id, HEX);
            Serial.println(" is not data from sensors");
        }
    }
    float getLatitude() {return((((long)data[0][0] << 24) + ((long)data[0][1] << 16) + ((long)data[0][2] << 8) + data[0][3])/10000000);}
    float getLatitudeHP() {return(data[0][4]/1000000000);}
    float getLongitude() {return((((long)data[1][0] << 24) + ((long)data[1][1] << 16) + ((long)data[1][2] << 8) + data[1][3])/10000000);}
    float getLongitudeHP() {return(data[1][4]/1000000000);}
    float getGPSPrecision() {return(data[1][4]/10);} //Units: mm
    float getLinAccelX() {return(((long)data[3][0] << 24) + ((long)data[3][1] << 16) + ((long)data[3][2] << 8) + data[3][3]);}//change scaling
    float getLinAccelY() {return(((long)data[3][4] << 24) + ((long)data[3][5] << 16) + ((long)data[3][6] << 8) + data[3][7]);}//change scaling
    float getLinAccelZ() {return(((long)data[4][0] << 24) + ((long)data[4][1] << 16) + ((long)data[4][2] << 8) + data[4][3]);}//change scaling
    float getAbsOrenX() {return(((long)data[4][4] << 24) + ((long)data[4][5] << 16) + ((long)data[4][6] << 8) + data[4][7]);}//change scaling
    float getAbsOrenY() {return(((long)data[5][0] << 24) + ((long)data[5][1] << 16) + ((long)data[5][2] << 8) + data[5][3]);}//change scaling
    float getAbsOrenZ() {return(((long)data[5][4] << 24) + ((long)data[5][5] << 16) + ((long)data[5][6] << 8) + data[5][7]);}//change scaling
    float getAngVeloX() {return(((long)data[6][0] << 24) + ((long)data[6][1] << 16) + ((long)data[6][2] << 8) + data[6][3]);}//change scaling
    float getAngVeloY() {return(((long)data[6][4] << 24) + ((long)data[6][5] << 16) + ((long)data[6][6] << 8) + data[6][7]);}//change scaling
    float getAngVeloZ() {return(((long)data[7][0] << 24) + ((long)data[7][1] << 16) + ((long)data[7][2] << 8) + data[7][3]);}//change scaling
    float getFRtemp1() {return(((long)data[9][0] << 8) + data[9][1]);}//change scaling
    float getFRtemp2() {return(((long)data[9][2] << 8) + data[9][3]);}//change scaling
    float getFRtravel() {return(data[9][4]);}//change scaling
    float getFRspeed() {return(((long)data[9][5] << 8) + data[9][6]);}//change scaling
    float getFRpsi() {return(data[9][7]);}//change scaling
    float getFFtemp1() {return(((long)data[10][0] << 8) + data[10][1]);}//change scaling
    float getFFtemp2() {return(((long)data[10][2] << 8) + data[10][3]);}//change scaling
    float getFFtravel() {return(data[10][4]);}//change scaling
    float getFFspeed() {return(((long)data[10][5] << 8) + data[10][6]);}//change scaling
    float getFFpsi() {return(data[10][7]);}//change scaling
    float getRRtemp1() {return(((long)data[11][0] << 8) + data[11][1]);}//change scaling
    float getRRtemp2() {return(((long)data[11][2] << 8) + data[11][3]);}//change scaling
    float getRRtravel() {return(data[11][4]);}//change scaling
    float getRRspeed() {return(((long)data[11][5] << 8) + data[11][6]);}//change scaling
    float getRRpsi() {return(data[11][7]);}//change scaling
    float getRLtemp1() {return(((long)data[12][0] << 8) + data[12][1]);}//change scaling
    float getRLtemp2() {return(((long)data[12][2] << 8) + data[12][3]);}//change scaling
    float getRLtravel() {return(data[12][4]);}//change scaling
    float getRLspeed() {return(((long)data[12][5] << 8) + data[12][6]);}//change scaling
    float getRLpsi() {return(data[12][7]);}//change scaling
};




struct Sensors_Pedals
{
    byte data[1][8]={{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};  //0 AAPS [0-1], BrakeSW [2], BrakePressure_1 [3-4], BrakePressure_2 [5-6]
    unsigned long ID = 0;
    Sensors_Pedals(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){
        if(id >> 8 >= 0 && id >> 8 < 1){
            for(int i = 0; i < 8; i++)
                data[id >> 8][i] = buf[i];//maybe add timestamp?
            return;
        }
        else{
            Serial.print(id, HEX);
            Serial.println(" is not data from pedals");
        }
    }

    float getAPPS() {return((((long)data[0][0] << 8) + data[0][1])/10);}
    boolean getBrakeLimit() {return(data[0][2]);}
    int getBrakePressure1() {return((((long)data[0][3] << 8) + data[0][4]));}
    int getBrakePressure2() {return((((long)data[0][5] << 8) + data[0][6]));}
};



struct VCU
{
    unsigned long ID = 0;
    VCU(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){}
};


struct Calculated_Vals
{
    unsigned long ID = 0;
    Calculated_Vals(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){}
};


struct Debug
{
    File logFile;
    boolean logEnabled = false;
    bool hasSD2 = false;
    unsigned long ID = 0;
    int SD_writes = 0;
    String out = "";
    long filters[9] {-1,-1,-1,-1,-1,-1,-1,-1,-1};
    Debug(unsigned long id){ID = id;}
    void receive(unsigned long id, byte buf[]){}
    void CAN_CSV(CAN_message_t msg){
        if(!logEnabled || !filter(msg)){return;}
        if(!hasSD2){
            Serial.println("No SD card");
            return;
        }
        out = (String)micros()+",";
        out += (String)msg.timestamp+",";
        out += (String)msg.id+",";
        for(int i=0; i<8; i++){
            out+=((String)msg.buf[i]+",");
        }
        out += "\n";
        logFile.write(out.c_str(),sizeof(out.c_str()));
        Serial.print(out);
        SD_writes++;
        if(SD_writes > 100){
            logFile.flush();
            SD_writes = 0;
        }
    }
    void write(){
        logFile.flush();
    }
    bool filter(CAN_message_t msg){
        if(filters[0] != -1){
            if(msg.id != filters[0])
                return false;
        }
        for(int i=1; i<9; i++){
            if(filters[i] != -1){
                if(msg.buf[i-1] != filters[i])
                    return false;
            }
        }
        return true;
    }
    void serialRead(){
        String function = Serial.readStringUntil('\n');
        if(function.toLowerCase() == "filter"){
            Serial.println("Enter Filter ID");
            while(!Serial.available())
            filterWriter(Serial.readStringUntil('\n'), filters[0]);
            
        }

    }
    void filterWriter(String in, long out){
        out = in.toInt();
    }
};




