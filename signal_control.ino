#include <bluefruit.h>
#define BUF_LENGTH          20         //一筆資料多大
#define DEVICE_NAME       "Test1"
#define TICK_INTERVAL_us    2778       //360Hz
#define test_s_uuid 0xFFFF             //16bits 一個16進位數字代表4bits(2^4)
#define test_uuid 0xA234

/* ECG Service: 00001523-1212-EFDE-1523-785FEABCD123
 * ECG RAW : 00001524-1212-EFDE-1523-785FEABCD123
 */
/*const uint8_t HS_UUID_SERVICE[] =
{
    0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
    0xDE, 0xEF, 0x12, 0x12, 0x23, 0x15, 0x00, 0x00
};
const uint8_t HS_UUID_CHR_RAW[] =
{
    0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 
    0x75, 0x4C, 0x3E, 0x50,    2,    0, 0x3D, 0x71
};*/

static uint8_t adc_value[BUF_LENGTH] = {0,}; //存取資料的array，uint8_t代表0~255
static int counter = 0;
static bool connected2 = false;
static int Red = 13;
static int IR = 12;

BLEDis  bledis;    // DIS (Device Information Service) helper class instance
BLEService        hs(test_s_uuid);
BLECharacteristic hsraw(test_uuid);

extern "C"
{
  void SysTick_Handler(void)
  {
     if (connected2){
      adc_value[counter] = analogRead(A2);
      counter++;
      if (counter >= BUF_LENGTH) {
        hsraw.notify(adc_value, BUF_LENGTH);
        counter = 0;
      }
    }
    /*int raw_data = analogRead(A2);
    Serial.write(raw_data);
    delay(100);*/
  }
} // extern C

void setup()
{
  pinMode(Red, OUTPUT);      //設定Red的這個腳位為一輸出訊號
  pinMode(IR, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);      //開啟序列傳輸並設定baud rate=11520bps
  Serial.println("Bluefruit52 nRF Blinky Example");
  Serial.println("------------------------------\n");
  analogReadResolution(8);

  Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);  
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.setTxPower(4);
  
//設定SPI
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Periph.setConnInterval(6, 12);  //設定連接的時間間隔6us~12us

  Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer("Yutech, Taiwan");
  bledis.setModel("TriAnswer");
  bledis.begin();

//針對servive及characteristic進行設定
  hs.begin();
  hsraw.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  hsraw.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hsraw.setMaxLen(BUF_LENGTH);
  hsraw.setFixedLen(BUF_LENGTH);
  hsraw.begin();
  Serial.println("Setting up the advertising");
  startAdv();
  SysTick_Config( (F_CPU/1000000)*TICK_INTERVAL_us );
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(hs);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop()
{
  static int raw_data = analogRead(A2);
  digitalWrite(Red, HIGH);  //Red亮
  delay(1);
  Serial.write(raw_data);
  delay(1000);//等1秒
  
  digitalWrite(Red, LOW);
  delay(1);
  Serial.write(raw_data);
  delay(1000);
  
  digitalWrite(IR, HIGH);  //IR亮
  delay(1);
  Serial.write(raw_data);
  delay(1000);
  
  digitalWrite(IR, LOW);
  delay(1);
  Serial.write(raw_data);
  delay(1000);
}
/*void loop2()
{
  delay(2000);
  digitalWrite(ledPin2, HIGH);  //LED亮
  delay(1000);
  digitalWrite(ledPin2, LOW);
  delay(3000);*/


void connect_callback(uint16_t conn_handle)
{
  BLEConnection* conn = Bluefruit.Connection(conn_handle);
  Serial.println("Connected");
  delay(1000);
  connected2 = true;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  connected2 = false;
}
