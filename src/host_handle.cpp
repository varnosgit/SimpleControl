#include "host_handle.h"
#include "wireless.h"

extern uint8_t myMAC_Address[], Brodcast_Address[], Controller_Address[], TERMO_Address[];

hc_message hc_mesg;
uint8_t hc_sendFlag = 0, hc_recvFlag = 0;

void send_data_to_host(void)
{
    hc_mesg.begin_validator[0] = 'V';
    hc_mesg.begin_validator[1] = 'A';
    hc_mesg.begin_validator[2] = 'C';
    hc_mesg.command = 0x02;
    hc_mesg.data[0] = 1;
    hc_mesg.data[1] = 1;
    hc_mesg.data[2] = 1;
    hc_mesg.data[3] = 1;
    hc_mesg.end_validator = 'H';
    
    Serial2.write((const uint8_t *)&hc_mesg, sizeof(hc_mesg));  
}

bool receive_data_from_host(void)
{
    uint8_t rxdata[16];
    if (Serial2.available()) 
    {
      char* rxdata = (char*) &hc_mesg;
      Serial2.read(rxdata, 16);
      Serial.print((char *)rxdata);
      Serial.println(hc_mesg.data[0]);

      sendDataTo(Brodcast_Address, 0x07, hc_mesg.data);
      return true;
    }
     // delay(50);
    // byte n = Serial2.available();  //3:
    //  if(n != 0) //4:
    //  {           
    //      byte m = Serial.readBytesUntil('\n', rxdata, 5);  //5:
    //      rxdata[m] = '\0';  //6:
    //      Serial.print(String(rxdata)); Serial.println("rxdata");//7:
    //  }
    return false;
}

void handle_host_message(void)
{

}
