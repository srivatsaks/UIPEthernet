/*
 * UIPEthernet UdpServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This UdpServer example sets up a udp-server at 192.168.0.6 on port 5000.
 * send packet via upd to test
 *
 * Copyright (C) 2013 by Norbert Truchsess (norbert.truchsess@t-online.de)
 * 
 */

#define MACADDRESS 0x00,0x01,0x02,0x03,0x04,0x05
#define MYIPADDR 192,168,1,6
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1

#if defined(__MBED__)
  #include <mbed.h>
  #include "mbed/millis.h"
  #define delay(x) wait_ms(x)
  #define PROGMEM
  #include "mbed/Print.h"
#endif

#include <UIPEthernet_edtd.h>
#include "utility/logging.h"

EthernetUDP udp;

#if defined(ARDUINO)
void setup() {
#endif  
#if defined(__MBED__)
int main() {
#endif
  #if ACTLOGLEVEL>LOG_NONE
    #if defined(ARDUINO)
      LogObject.begin(9600);
    #endif
    #if defined(__MBED__)
      Serial LogObject(SERIAL_TX,SERIAL_RX);
    #endif
  #endif

  
  uint8_t mac[6] = {MACADDRESS};
  uint8_t myIP[4] = {MYIPADDR};
  uint8_t myMASK[4] = {MYIPMASK};
  uint8_t myDNS[4] = {MYDNS};
  uint8_t myGW[4] = {MYGW};

  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  Serial.begin(115200); // for debugging
  digitalWrite(5, HIGH);
  Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);

  int success = udp.begin(5000);
  Serial.print("server is at: ");
  Serial.println(Ethernet.localIP());
  #if ACTLOGLEVEL>=LOG_INFO
    LogObject.uart_send_str(F("initialize: "));
    LogObject.uart_send_strln(success ? "success" : "failed");
  #endif
#if defined(ARDUINO)
}

void loop() {
#endif  

#if defined(__MBED__)
while(true) {
#endif
  //check for new udp-packet:
  int size = udp.parsePacket();
  if (size > 0) {
    do
      {
        char* msg = (char*)malloc(size+1);
        int len = udp.read(msg,size+1);
        msg[len]=0;
        #if ACTLOGLEVEL>=LOG_INFO
          LogObject.uart_send_str(F("received: '"));
          LogObject.uart_send_str(msg);
        #endif
        free(msg);
      }
    while ((size = udp.available())>0);
    //finish reading this packet:
    udp.flush();
    #if ACTLOGLEVEL>=LOG_INFO
      LogObject.uart_send_strln(F("'"));
    #endif
    int success;
    do
      {
      #if ACTLOGLEVEL>=LOG_INFO
        LogObject.uart_send_str(F("remote ip: "));
        #if defined(ARDUINO)
          LogObject.println(udp.remoteIP());
        #endif
        #if defined(__MBED__)
          LogObject.printf("%d.%d.%d.%d",udp.remoteIP()[0],udp.remoteIP()[1],udp.remoteIP()[2],udp.remoteIP()[3]);
        #endif
        LogObject.uart_send_str(F("remote port: "));
        LogObject.uart_send_decln(udp.remotePort());
      #endif
        //send new packet back to ip/port of client. This also
        //configures the current connection to ignore packets from
        //other clients!
        success = udp.beginPacket(udp.remoteIP(),udp.remotePort());
      #if ACTLOGLEVEL>=LOG_INFO
        LogObject.uart_send_str(F("beginPacket: "));
        LogObject.uart_send_strln(success ? "success" : "failed");
      #endif
    //beginPacket fails if remote ethaddr is unknown. In this case an
    //arp-request is send out first and beginPacket succeeds as soon
    //the arp-response is received.
      }
    while (!success);

    success = udp.println(F("hello world from arduino"));

    #if ACTLOGLEVEL>=LOG_INFO
      LogObject.uart_send_str(F("bytes written: "));
      LogObject.uart_send_decln(success);
    #endif

    success = udp.endPacket();

    #if ACTLOGLEVEL>=LOG_INFO
      LogObject.uart_send_str(F("endPacket: "));
      LogObject.uart_send_strln(success ? "success" : "failed");
    #endif

    udp.stop();
    //restart with new connection to receive packets from other clients
    #if ACTLOGLEVEL>=LOG_INFO
      LogObject.uart_send_str(F("restart connection: "));
      LogObject.uart_send_strln(udp.begin(5000) ? "success" : "failed");
    #endif
  }
}
#if defined(__MBED__)
}
#endif
