#include <Arduino.h>
#include <SPI.h>
#include <Ra01S.h>

//test
// ========== 频率（中国合法频段） ==========
#define RF_FREQUENCY                486000000   // 486 MHz (470-510 MHz 合法)
#define TX_OUTPUT_POWER             22          // dBm
#define LORA_BANDWIDTH              4           // 125 kHz
#define LORA_SPREADING_FACTOR       7           // SF7
#define LORA_CODINGRATE             1           // 4/5
#define LORA_PREAMBLE_LENGTH        8
#define LORA_PAYLOADLENGTH          0           // 可变长度

// ========== 引脚定义（SuperMini nRF52840） ==========
// 请根据实际接线修改以下引脚
#define PIN_NSS    32   // 该板子20引脚无法驱动
#define PIN_RST    9    // 
#define PIN_BUSY   24    // 

//#define PIN_SCK    43   // 或者直接用 43（取决于核心定义）
//#define PIN_MOSI   45   // 45
//#define PIN_MISO   47   // 47

// 创建 LoRa 对象（无 TCXO，普通模式）
SX126x lora(PIN_NSS, PIN_RST, PIN_BUSY);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  while (!Serial) delay(10);

  // 调试：测试 SPI 基础通信
  Serial.println("=== SPI Debug ===");
  SPI.begin();
  //SPI.setPins(PIN_MISO,PIN_SCK, PIN_MOSI);
  Serial.println("SPI.begin() done");

  // 测试 SPI 是否能正常工作 - 简单读写测试
  // 设置 NSS 为低电平
  digitalWrite(PIN_NSS, LOW);
  uint8_t testByte = SPI.transfer(0x00);
  digitalWrite(PIN_NSS, HIGH);
  Serial.print("SPI test byte: 0x");
  Serial.println(testByte, HEX);
  Serial.println("=================");

  // 初始化 LoRa 模块
  Serial.println("Initializing LoRa...");
  int16_t ret = lora.begin(RF_FREQUENCY, TX_OUTPUT_POWER);
  if (ret != ERR_NONE) {
    Serial.print("Init failed, error: ");
    Serial.println(ret);
    while(1) delay(1);
  }

  // 配置 LoRa 参数
  lora.LoRaConfig(LORA_SPREADING_FACTOR, 
                  LORA_BANDWIDTH, 
                  LORA_CODINGRATE, 
                  LORA_PREAMBLE_LENGTH, 
                  LORA_PAYLOADLENGTH, 
                  true,   // CRC 开启
                  false); // 不反转中断

  Serial.println("LoRa ready, sending every 1s...");
}

void loop() 
{
static uint32_t counter = 0;
uint8_t txData[255];
sprintf((char *)txData, "[%lu] Hello World %lu", millis(), counter++);
uint8_t len = strlen((char *)txData);

Serial.print("TX: ");
Serial.println((char *)txData);

if (lora.Send(txData, len, SX126x_TXMODE_SYNC)) {
    Serial.println(" OK");
} else {
    Serial.println(" FAIL");
}
delay(500);
}