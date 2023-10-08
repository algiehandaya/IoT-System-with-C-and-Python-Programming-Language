#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h"  

LoRa_E220 e220ttl(4, 5, 3, 7, 6);
int turn = 1;
int next;
int rssidbm;

struct Message1S { //Client Node 3
  float ph1S;
  float temperature1S;
} message1S;

struct Message2 { // Client Node 1
  float ph;
  float temperature;
} message2;

struct Message3 { //Client Node 2
  float ph;
  float temperature;
} message3;

void setup()
{
  Serial.begin(9600);
  delay(500);
  e220ttl.begin();

  ResponseStructContainer c;
 c = e220ttl.getConfiguration();
 Configuration configuration = *(Configuration*) c.data;
 configuration.ADDL = 0x08;
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
  //Serial.println(c.status.getResponseDescription());
  //Serial.println(c.status.code);

  //printParameters(configuration);
  c.close();

  //Serial.println("Hi, I'm going to receive!");
  
}

void loop()
{
  
      if (turn == 1) { //req client 2
        //Serial.println("Send Request 1");
        SendRequest1a(); 
        delay(4000);
        waitForAnswer();

        if (next == 1){
        SendRequest1(); 
        delay(4000);
        waitForAnswer();
        delay(5000);
        SendRequest1b(); 
        delay(4000);
        waitForAnswer();
          turn = 2;
        }
        
      }

      delay(5000);
      
      if (turn == 2) {
        //Serial.println("Send Request 2"); //req client 3
        SendRequest2(); 
        delay(4000);
        waitForAnswer2();
        
        if (next == 0){
        SendRequest2a(); 
        delay(4000);
        waitForAnswer2();
        delay(5000);
        SendRequest2b(); 
        delay(4000);
        waitForAnswer2();
          turn = 3;
        }
      }

      delay(5000);

      if (turn == 3) { //req client 2 again
        //Serial.println("Send Request 3");
        SendRequest3(); 
        delay(4000);
        waitForAnswer3();

        if (next == 1){
        SendRequest3a(); 
        delay(4000);
        waitForAnswer3();
        delay(5000);
        SendRequest3b(); 
        delay(4000);
        waitForAnswer3();
          turn = 1;
        }
      }
      
      delay(7000);
      
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
    //Serial.println("Message received by Gateway!");
#ifdef ENABLE_RSSI
    ResponseStructContainer rsc = e220ttl.receiveMessageRSSI(sizeof(Message1S));
    rssidbm = 256 - rsc.rssi;
    Serial.print("RSSI: -"); Serial.println(rssidbm);
    //Serial.print("RSSI: "); Serial.println(rsc.rssi, DEC);
#endif
if (rssidbm < 155){
  // Is something goes wrong print error
if (rsc.status.code!=1){
    //Serial.println(rsc.status.getResponseDescription());
  }else{
    // Print the data received
    //Serial.println(rsc.status.getResponseDescription());
    struct Message1S message1S = *(Message1S*) rsc.data;
    Serial.print("A2");
    Serial.print(",");
    Serial.print(message1S.ph1S);
    Serial.print(",");
    Serial.print(message1S.temperature1S);
    Serial.print(",");
    Serial.println("Bandeng");
    next = 1;

    }
  }else{
    return;
  }
  }
  else{
    next = 1;
  }
}

void waitForAnswer2() { //fungsi tunggu jawaban dari node
 // If something available
  if (e220ttl.available()>=1) {
    // read the String message
    //Serial.println("Message received by Gateway again!");
#ifdef ENABLE_RSSI
    ResponseStructContainer rsc = e220ttl.receiveMessageRSSI(sizeof(Message3));
    rssidbm = 256 - rsc.rssi;
    Serial.print("RSSI: -"); Serial.println(rssidbm);
    //Serial.print("RSSI: "); Serial.println(rsc.rssi, DEC);
#endif
if (rssidbm < 155){
  // Is something goes wrong print error
if (rsc.status.code!=1){
    //Serial.println(rsc.status.getResponseDescription());
  }else{
    // Print the data received
    //Serial.println(rsc.status.getResponseDescription());
    struct Message3 message3 = *(Message3*) rsc.data;
    Serial.print("A3");
    Serial.print(",");
    Serial.print(message3.ph);
    Serial.print(",");
    Serial.print(message3.temperature);
    Serial.print(",");
    Serial.println("Windu");
    next = 0;

    }
  }else{
    return;
  }
  }
  else{
    next = 0;
  }
}

void waitForAnswer3() { //fungsi tunggu jawaban dari node
 // If something available
  if (e220ttl.available()>=1) {
    // read the String message
    //Serial.println("Last Message received by Gateway!");
#ifdef ENABLE_RSSI
    ResponseStructContainer rsc = e220ttl.receiveMessageRSSI(sizeof(Message2));
    rssidbm = 256 - rsc.rssi;
    Serial.print("RSSI: -"); Serial.println(rssidbm);
    //Serial.print("RSSI: "); Serial.println(rsc.rssi, DEC);
#endif
if (rssidbm < 155){
  // Is something goes wrong print error
if (rsc.status.code!=1){
    //Serial.println(rsc.status.getResponseDescription());
  }else{
    // Print the data received
    //Serial.println(rsc.status.getResponseDescription());
    struct Message2 message2 = *(Message2*) rsc.data;
    Serial.print("A1");
    Serial.print(",");
    Serial.print(message2.ph);
    Serial.print(",");
    Serial.print(message2.temperature);
    Serial.print(",");
    Serial.println("Vaname");
    next = 1;


    }
}else{
  return;
}
  }else{
    next = 1;
  }
}


void SendRequest1() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 5
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 2");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest1a() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 7
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 2");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest1b() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 3
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 2");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest2() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 5
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 3");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest2a() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 7
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 3");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest2b() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 3
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 3");
  //Serial.println(rs.getResponseDescription());
}


void SendRequest3() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 5
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 1");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest3a() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 7
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 1");
  //Serial.println(rs.getResponseDescription());
}

void SendRequest3b() { //fungsi untuk Send request dengan variable parameter request
  #define DESTINATION_ADDL 3
  ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Gateway Request Data 1");
  //Serial.println(rs.getResponseDescription());
}
