#include "FlexCAN_T4.h"
#include "Nodes.h"


#define INT8U byte
#define INT32U unsigned long

#ifndef I_no_can_speak_flex_
#define I_no_can_speak_flex_

#define Sender true
#define Reciever false


class I_no_can_speak_flex
{
public:
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
    CAN_message_t msg;
    bool canSend = false;
    Inverter DTI = Inverter(22);
    Battery BMS = Battery(0x7EB);
    Isolation_Monitor IMD = Isolation_Monitor(69420);//////IDK
    Charger charger = Charger(1278123890);//////IDK
    Sensors sensors = Sensors(5738);
    Sensors_Pedals pedals = Sensors_Pedals(7900);
    VCU vcu = VCU(2021);
    Debug stuffz = Debug(0);




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



    void send(long, long, int);
    void send(long , byte[]);
    void sendToInv(int, long, int);


    void ping(byte);



    void BMSrequest(int);
    void log();
    void printer();
};
#endif
