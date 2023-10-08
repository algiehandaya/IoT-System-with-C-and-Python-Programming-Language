#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h"

#include <OneWire.h>          
#include <DallasTemperature.h> 
#include <SoftwareSerial.h>
#define ONE_WIRE_BUS 8 

OneWire oneWire(ONE_WIRE_BUS);      
DallasTemperature sensor(&oneWire); 

//---------------------------------------
 //pH
    const int ph_Pin = A0;
    float Po = 0;
    float PH_step;
    int nilai_analog_PH;
    double TeganganPh;
  //kalibrasi
    float PH4 = 3.45;
    float PH7 = 2.55;
//---------------------------------------

float suhuDS18B20; 
int rssidbm;

struct Message {
  float ph;
  float temperature;
} message;

LoRa_E220 e220ttl(4, 5, 3, 7, 6);

void setup()
{
  pinMode (ph_Pin, INPUT);
  Serial.begin(9600);
  delay(100);
  e220ttl.begin();
  sensor.begin();

  ResponseStructContainer c;
 c = e220ttl.getConfiguration();
 Configuration configuration = *(Configuration*) c.data;
 configuration.ADDL = 0x03;
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

  Serial.println("Client 1 send message mode");
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

    waitForAnswers();
    

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

void waitForAnswers(){
  struct  Message message = {Po, suhuDS18B20};
  
    if (e220ttl.available()>=1) {
   Serial.println("Message received by Client 1!");
    
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

    if (rc.data == "Client 2 Request Data"){
    //Send Message
    #define DESTINATION_ADDL 5
    Serial.println("Sending 2");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message, sizeof(Message));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
    }
    else if(rc.data == "Client 3 Request Data"){
    //Send Message
    #define DESTINATION_ADDL 7
    Serial.println("Sending 3");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message, sizeof(Message));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      }
    else if(rc.data == "Gateway Request Data 1"){
    //Send Message
    #define DESTINATION_ADDL 8
    Serial.println("Sending 1");
    ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message, sizeof(Message));
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());
      }
    }
} else{
  Serial.println("Distance too far");
  return;
}
  }
  //Serial.println("Loopin Again");
  //Serial.println("Checking Message..");
  delay(500);
}
