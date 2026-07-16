#include <Arduino.h>
#include <SPI.h>
#include <Ra01S.h>

//#define RF_FREQUENCY                                433000000 // Hz  center frequency
//#define RF_FREQUENCY                                866000000 // Hz  center frequency
// ============合法频段===============
#define RF_FREQUENCY                                486000000 // Hz  center frequency
#define TX_OUTPUT_POWER                             22        // dBm tx output power
#define LORA_BANDWIDTH                              4         // bandwidth
                                                             // 2: 31.25Khz
                                                             // 3: 62.5Khz
                                                             // 4: 125Khz
                                                             // 5: 250KHZ
                                                             // 6: 500Khz 
#define LORA_SPREADING_FACTOR                       7         // spreading factor [SF5..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_PAYLOADLENGTH                          0         // 0: Variable length packet (explicit header)
                                                              // 1..255  Fixed length packet (implicit header)



                                                              
                                                              
//==========引脚定义（SuperMini nRF52840）===========                                                           


#define PIN_NSS  32   //该板子20引脚无法驱动
#define PIN_RST  9    // 9引脚为复位引脚
#define PIN_BUSY 24    // 24引脚为Busy引脚


SX126x  lora(PIN_NSS,               //Port-Pin Output: SPI select
             PIN_RST,               //Port-Pin Output: Reset 
             PIN_BUSY                //Port-Pin Input:  Busy
             );

void setup() 
{
  delay(1000);
  Serial.begin(115200);
  while (!Serial) delay(20); // 等待串口初始化完成

  //调试：测试SPI基础通信
  Serial.println("=== SPI Debug ===");
  SPI.begin();

  Serial.println("SPI.begin() done");

  // 初始化LoRa模块
  Serial.println("Initializing LoRa...");
  int16_t ret = lora.begin(RF_FREQUENCY, TX_OUTPUT_POWER);
  if (ret != ERR_NONE) {
    Serial.println("LoRa.begin() failed");
    Serial.print("Error code: ");
    Serial.println(ret);
    while(1) {delay(1);}
  }
   
 //配置LoRa参数
  lora.LoRaConfig(LORA_SPREADING_FACTOR, 
                  LORA_BANDWIDTH, 
                  LORA_CODINGRATE, 
                  LORA_PREAMBLE_LENGTH, 
                  LORA_PAYLOADLENGTH,
                  true,               // CRC开启 
                  false);             // 不反转中断
  
  Serial.println("LoRa.rady, receiving...");

}

void loop() 
{
  uint8_t rxData[255];
  uint8_t rxLen = lora.Receive(rxData, 255);
  if ( rxLen > 0 )
  { 
    Serial.print("Receive rxLen:");
    Serial.println(rxLen);
    for(int i=0;i< rxLen;i++) {
      Serial.print(rxData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    Serial.print("Receive data: ");
    for(int i=0;i< rxLen;i++) {
      if (rxData[i] > 0x19 && rxData[i] < 0x7F) {
        char myChar = rxData[i];
        Serial.print(myChar);
      } else {
        Serial.print("?");
      }
    }
    Serial.println();

    int8_t rssi, snr;
    lora.GetPacketStatus(&rssi, &snr);
    Serial.print("rssi: ");
    Serial.print(rssi, DEC);
    Serial.println(" dBm");
    Serial.print("snr: ");
    Serial.print(snr, DEC);
    Serial.println(" dB");
  }
  delay(1);
}