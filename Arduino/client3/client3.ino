#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h" 

#include <OneWire.h>          
#include <DallasTemperature.h> 
#include <SoftwareSerial.h>
#define ONE_WIRE_BUS 8 

OneWire oneWire(ONE_WIRE_BUS);      
DallasTemperature sensor(&oneWire); 

//pH
    const int ph_Pin = A0;
    float Po = 0;
    float PH_step;
    int nilai_analog_PH;
    double TeganganPh;
  //kalibrasi
    float PH4 = 3.45;
    float PH7 = 2.7;

float suhuDS18B20; 
float nilaiph1;
float nilaitemp1;
float nilaiph2;
float nilaitemp2;
int kondisi = 0;
int kirim;
int rssidbm;

LoRa_E220 e220ttl(4, 5, 3, 7, 6);
int turn = 1;

struct Message {
  float ph;
  float temperature;
} message;

struct Message1S {
  float ph1S;
  float temperature1S;
} message1S;

struct Message2 {
  float ph;
  float temperature;
} message2;

struct Message3 {
  float ph;
  float temperature;
} message3;

void setup()
{
  pinMode (ph_Pin, INPUT);
  Serial.begin(9600);
  delay(5000);
  e220ttl.begin();
  sensor.begin();

  ResponseStructContainer c;
 c = e220ttl.getConfiguration();
 Configuration configuration = *(Configuration*) c.data;
 configuration.ADDL = 0x07;
 configuration.ADDH = 0x00;
 
 configuration.CHAN = 23;
 
 configuration.SPED.uartBaudRate = UART_BPS_9600;
 configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
 configuration.SPED.uartParity = MODE_00_8N1;
 
 configuration.OPTION.subPacketSetting = SPS_200_00;
 configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_ENABLED;
 configuration.OPTION.transmissionPower = POWER_22;
 
 configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
 configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
 configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
 configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
 e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  Serial.println(c.status.getResponseDescription());
  Serial.println(c.status.code);

  printParameters(configuration);
  c.close();

  Serial.println("Hi, I'm going to receive and send message!");
}

void loop()
{
    //pH
    nilai_analog_PH = analogRead(ph_Pin);
    //Serial.print("Nilai ADC Ph: ");
    //Serial.println(nilai_analog_PH);
    TeganganPh = 5 /1024.0 * nilai_analog_PH;
    //Serial.print("TeganganPh: ");
    //Serial.println(TeganganPh, 3);

    PH_step = (PH4 - PH7) / 3;
    Po = 7.00 + ((PH7 - TeganganPh) / PH_step);
    //Serial.print("Nilai PH cairan: ");
    //Serial.println(Po, 2);
    
    //Suhu
    sensor.setResolution(12);
    sensor.requestTemperatures(); 
    suhuDS18B20 = sensor.getTempCByIndex(0);  //Membaca data suhu dari sensor #0 dan mengkonversikan ke nilai Celsius
    // suhuDS18B20 = (suhuDS18B20*9/5) + 32;
    // suhuDS18B20 = suhuDS18B20 = 273.15;
    //Serial.print("Temperature: ");
    //Serial.println(suhuDS18B20, 4);
  
      if (turn == 1) { //request data ke client 1
        delay(500);
        Serial.println("Send Request 1");
        SendRequest1(); 
        delay(1000);
        waitForAnswer();
        delay(500);
        SendRequest2(); 
        delay(1000);
        waitForAnswerb();
        turn = 2;
      }

      delay(1000);
      
      if (turn == 2) { //kirim data client node 2
        //Serial.println("Client 3 Waiting Message..");
        waitForAnswer2();   

                  if (kondisi == 1) {
                    SendRequest1(); 
                    delay(1000);
                    waitForAnswer();
                    delay(500);
                    SendRequest2(); 
                    delay(1000);
                    waitForAnswerb();
                    turn = 3; //refresh data baru
                  }
      } 

      if (turn == 3) { //kirim data client node 2
        //Serial.println("Client 3 Waiting Message Again..");
        waitForAnswer3();

                   if (kondisi == 0) {
                    turn = 1;
                     }
      } 
}


void printParameters(struct Configuration configuration) {
  Serial.println("----------------------------------------");

  Serial.print(F("HEAD : "));  Serial.print(configuration.COMMAND, HEX);Serial.print(" ");Serial.print(configuration.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(configuration.LENGHT, HEX);
  Serial.println(F(" "));
  Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, HEX);
  Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, HEX);
  Serial.println(F(" "));
  Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
  Serial.println(F(" "));
  Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
  Serial.print(F("SpeedUARTDatte     : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRateDescription());
  Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRateDescription());
  Serial.println(F(" "));
  Serial.print(F("OptionSubPacketSett: "));  Serial.print(configuration.OPTION.subPacketSetting, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getSubPacketSetting());
  Serial.print(F("OptionTranPower    : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());
  Serial.print(F("OptionRSSIAmbientNo: "));  Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
  Serial.println(F(" "));
  Serial.print(F("TransModeWORPeriod : "));  Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
  Serial.print(F("TransModeEnableLBT : "));  Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
  Serial.print(F("TransModeEnableRSSI: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
  Serial.print(F("TransModeFixedTrans: "));  Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());


  Serial.println("----------------------------------------");
}

void waitForAnswer() { //fungsi tunggu jawaban dari node
  
 // If something available
  if (e220ttl.available()>=1) {
    // read the String message
//#ifdef ENABLE_RSSI
    ResponseStructContainer rsc = e220ttl.receiveMessageRSSI(sizeof(Message));
//#endif
  // Is something goes wrong print error
if (rsc.status.code!=1){
    Serial.println(rsc.status.getResponseDescription());
    
  }else{
    // Print the data received
    Serial.println(rsc.status.getResponseDescription());
    struct Message message = *(Message*) rsc.data;
    Serial.print("pH: ");
    Serial.println(message.ph);
    Serial.print("Temp: ");
    Serial.println(message.temperature);
    nilaiph1 = message.ph;
    nilaitemp1 = message.temperature;
    
#ifdef ENABLE_RSSI
  rssidbm = 256 - rsc.rssi;
  Serial.print("RSSI: -"); Serial.println(rssidbm);
  //Serial.print("RSSI: "); Serial.println(rsc.rssi, DEC);
#endif

    }
  }
}

void waitForAnswerb() { //fungsi tunggu jawaban dari node
  
 // If something available
  if (e220ttl.available()>=1) {
    // read the String message
//#ifdef ENABLE_RSSI
    ResponseStructContainer rsc = e220ttl.receiveMessageRSSI(sizeof(Message));
//#endif
  // Is something goes wrong print error
if (rsc.status.code!=1){
    Serial.println(rsc.status.getResponseDescription());
    
  }else{
    // Print the data received
    Serial.println(rsc.status.getResponseDescription());
    struct Message message = *(Message*) rsc.data;
    Serial.print("pH: ");
    Serial.println(message.ph);
    Serial.print("Temp: ");
    Serial.println(message.temperature);
    nilaiph2 = message.ph;
    nilaitemp2 = message.temperature;
    
#ifdef ENABLE_RSSI
  rssidbm = 256 - rsc.rssi;
  Serial.print("RSSI: -"); Serial.println(rssidbm);
  //Serial.print("RSSI: "); Serial.println(rsc.rssi, DEC);
#endif

    }
  }
}

void waitForAnswer2() { //fungsi tunggu jawaban dari node

  struct  Message1S message1S = {nilaiph1, nilaitemp1};
  struct  Message2 message2 = {Po, suhuDS18B20};
  struct  Message3 message3 = {nilaiph2, nilaitemp2};
  
  
 // If something available
   if (e220ttl.available()>=1) {
   Serial.println("Message received by Client 3!");
    // read the String message
    
#ifdef ENABLE_RSSI
  ResponseContainer rc = e220ttl.receiveMessageRSSI();
  rssidbm = 256 - rc.rssi;
  //rssidbm = 200;
  Serial.print("RSSI: -"); Serial.println(rssidbm);
  //Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif

if (rssidbm < 155){
  // Is something goes wrong print error
  if (rc.status.code!=1){
    Serial.println(rc.status.getResponseDescription());
  }else{
    // Print the data received
    //Serial.println(rc.status.getResponseDescription());
    Serial.println(rc.data);
    kirim = 1;

    }
      if (kirim == 1 && rc.data == "Gateway Request Data 1"){
    //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 1");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message1S, sizeof(Message1S));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
    kondisi = 1;
    }
    else if(kirim == 1 && rc.data == "Gateway Request Data 3"){
      //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 3");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message2, sizeof(Message2));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 1;
      }
    else if(kirim == 1 && rc.data == "Gateway Request Data 2"){
      //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 2");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message3, sizeof(Message3));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 1;
      }
    else if(kirim == 1 && rc.data == "Client 2 Request Data"){
      //Send Message
    #define DESTINATION_ADDL 5
    Serial.println("Sending 3");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message2, sizeof(Message2));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 1;
      }
      else{
      kondisi = 0;
       }
      }else{
        Serial.println("Distance too far");
        return;
      }
    }
  else {
    kirim = 0;
 }


}

void waitForAnswer3() { //fungsi tunggu jawaban dari node

  struct  Message1S message1S = {nilaiph1, nilaitemp1};
  struct  Message2 message2 = {Po, suhuDS18B20};
  struct  Message3 message3 = {nilaiph2, nilaitemp2};
  
  
 // If something available
   if (e220ttl.available()>=1) {
   Serial.println("Message received by Client 3 Again!");
    // read the String message
    
#ifdef ENABLE_RSSI
  ResponseContainer rc = e220ttl.receiveMessageRSSI();
  rssidbm = 256 - rc.rssi;
  //rssidbm = 200;
  Serial.print("RSSI: -"); Serial.println(rssidbm);
  //Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif

if (rssidbm < 155){
  // Is something goes wrong print error
  if (rc.status.code!=1){
    Serial.println(rc.status.getResponseDescription());
  }else{
    // Print the data received
    Serial.println(rc.status.getResponseDescription());
    Serial.println(rc.data);
    kirim = 3;

    }
    if (kirim == 3 && rc.data == "Gateway Request Data 3"){
    //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 3");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message2, sizeof(Message2));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
    kondisi = 0;
    }
    else if(kirim == 3 && rc.data == "Gateway Request Data 1"){
    //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 1");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message1S, sizeof(Message1S));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 0;
      }
    else if(kirim == 1 && rc.data == "Gateway Request Data 2"){
      //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 2");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message3, sizeof(Message3));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 1;
      }
    else if(kirim == 1 && rc.data == "Client 2 Request Data"){
      //Send Message
    #define DESTINATION_ADDL 5
    Serial.println("Sending 3");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message2, sizeof(Message2));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      kondisi = 1;
      }
      else{
        kondisi = 1;
       }
      }else{
        Serial.println("Distance too far");
        return;
        }
    }
  else {
    kirim = 0;
 }            
}

void SendRequest1() { //fungsi untuk Send request dengan variable parameter request
  // Send message
  #define DESTINATION_ADDL 3
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Client 3 Request Data");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());
}

void SendRequest2() { //fungsi untuk Send request dengan variable parameter request
  // Send message
  #define DESTINATION_ADDL 5
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Client 2 Request Data");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());
}
