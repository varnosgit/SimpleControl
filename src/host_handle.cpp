#include "host_handle.h"
#include "wireless_ESPNOW.h"

extern bool host_available_flag;
extern uint8_t myMAC_Address[], Brodcast_Address[];
extern hc_message ESPNOW_mesg;
hc_message hc_mesg;

void send_data_to_host(void)
{  
  ESPNOW_mesg.begin_validator[0] = 'V';
  ESPNOW_mesg.begin_validator[1] = 'A';
  ESPNOW_mesg.begin_validator[2] = 'C';
  ESPNOW_mesg.end_validator = 'H';
  Serial2.write((const uint8_t *)&ESPNOW_mesg, sizeof(ESPNOW_mesg));   
  Serial.println("send data to host");
}

bool receive_data_from_host(void)
{
    if (Serial2.available() > 0) 
    {
      delay(10);
      char* rxdata = (char*) &hc_mesg;
      Serial2.read(rxdata, sizeof(hc_mesg));
      return true;
    }
    return false;
}

void handle_host_message(void)
{  
  if ((hc_mesg.begin_validator[0] == 'V') & (hc_mesg.begin_validator[1] == 'A') & (hc_mesg.begin_validator[2] == 'C') &
     (hc_mesg.end_validator == 'H'))
  {
    if (hc_mesg.__hcdata == 2) // 2 = introduce host to controller rec mac = host ip
    {
      Serial.println("Host Found successfully.");
      host_available_flag = true;
      Serial.printf("\rHost IP: %03d:%03d:%02d:%02d\n",
        hc_mesg.reciever_MAC_addr[0], hc_mesg.reciever_MAC_addr[1], hc_mesg.reciever_MAC_addr[2],
          hc_mesg.reciever_MAC_addr[3]);
      
    }
    else if (hc_mesg.__hcdata == 3) // 3 = host: directly send this message to a device with reciever MAC Add
    {
      Serial.println("sending data to a device...");
      memcpy(hc_mesg.sender_MAC_addr, myMAC_Address, 6);
      if (esp_now_is_peer_exist(hc_mesg.reciever_MAC_addr) == false){
        pairNew_device(hc_mesg.reciever_MAC_addr);
      }
      if (esp_now_send(hc_mesg.reciever_MAC_addr, (uint8_t *)&hc_mesg, sizeof(hc_mesg)) == ESP_OK){
        Serial.println("data has been sent with success.");
      }
      else {
        Serial.println("can not send data to device.");
        ESPNOW_mesg.__hcdata = 6; // 6 = cont.: delivery to device failed (dev mac = reciver mac address)
        send_data_to_host();
      }
    }
    else if (hc_mesg.__hcdata == 4) // 4 = host: pair a device with sender MAC Add
    {
      Serial.println("Add new pair...");
      pairNew_device(hc_mesg.sender_MAC_addr);
      hc_mesg._command = 0x01; // register
      hc_mesg._sender = 0x01;  // controller
      for (int i=0;i<6;i++){
        hc_mesg.reciever_MAC_addr[i] = hc_mesg.sender_MAC_addr[i];
        hc_mesg.sender_MAC_addr[i] = myMAC_Address[i];
      }
      esp_now_send(hc_mesg.reciever_MAC_addr, (uint8_t *)&hc_mesg, sizeof(hc_mesg));
    }
    else if (hc_mesg.__hcdata == 5) // 5 = host: unpair this device with reciever MAC Add if any
    {
      Serial.println("Unpair a device...");
      hc_mesg._command = 0x04; // unregister
      hc_mesg._sender = 0x01;  // controller
      for (int i=0;i<6;i++) hc_mesg.sender_MAC_addr[i] = myMAC_Address[i];
      esp_now_send(hc_mesg.reciever_MAC_addr, (uint8_t *)&hc_mesg, sizeof(hc_mesg));

      esp_now_del_peer(hc_mesg.reciever_MAC_addr);
    }
    else
    {
      Serial.println("UnknowHost command!");
      Serial2.flush();
    }
  }
  else
  {
      Serial.println("Unknow data from Host!!! WTF");
      Serial2.read();
  }
}
