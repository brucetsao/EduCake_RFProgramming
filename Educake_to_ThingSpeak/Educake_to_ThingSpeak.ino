#include <SPI.h>
#include <Ethernet.h>	//　使用網路必需要用的
#include <Wire.h>		//　使用網路必需要用的
#include "dht.h"    　		//使用DHT11溫濕度感測器所需要的函式庫
#include <WString.h>
 
#define DHT11_PIN 7		//宣告DHT11溫濕度感測器通訊腳位
dht DHT;			//宣告DHT11溫濕度感測器物件
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};		//使用這個 MAC ADRESS 當做這台educake 的MAC ADDRESS
IPAddress ip(192, 168, 30, 200);		//預設的網路IP位址，讀者可以自行更改為家中可用的位址	
IPAddress dnServer(168, 95, 1, 1);		//預設的DNS伺服器，本書為Hinet的DNS伺服器，讀者可以自行更改為家中可用的DNS伺服器
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);		//預設的閘道器的位址(就是Rounter或AP的網址)，本書為作者寫書環境的閘道器的位址，讀者可以自行更改為家中可用的閘道器的位址			
// the subnet:
IPAddress subnet(255, 255, 255, 0);		//子網路遮罩，本書為Class C

// ThingSpeak Settings
byte server[]  = { 184, 106, 153, 149 }; //  ThingSpeak API 位址
String writeAPIKey = "4I2190QE5X7KM714 ";    // DHT Sensor ThingSpeak Channel的 Write API Key 
const int updateInterval = 60000;        // 每多少毫秒更新一次資料



// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetClient client;		//宣告伺服器並使用Port 80為通訊埠
// Variable Setup
long lastConnectionTime = 0; 		//上次更新時間
boolean lastConnected = false;		//最後一次是否連線成功之旗標變數
int resetCounter = 0;			//重置計數器變數


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);			//宣告監控畫面的通訊速率
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  int chk = DHT.read11(DHT11_PIN);		//讀取DHT11溫濕度感測器 狀態資料，並檢查是否可以使用
   if ( chkDHT(chk) == 0)    //　使用chkDHT函式檢查是否DHT11溫濕度感測器 狀態資料是否正常，不正常傳回0，正常傳回1
       {
           Serial.println("ERROR on init DHT Sensor") ;		//告知使用者DHT11溫濕度感測器不正常
          while (true) ;		//程式終止
       }
       delay(3000);			//延遲3000毫秒
  // start the Ethernet connection and the server:
   // start the Ethernet connection:
      if (Ethernet.begin(mac) == 0) {		//要求DHCP 伺服器發放ip位址等資訊
        Serial.println("Failed to configure Ethernet using DHCP");		//要求DHCP 伺服器發放ip位址失敗，發出警告訊息
        // no point in carrying on, so do nothing forevermore:
        // try to congifure using IP address instead of DHCP:
          Ethernet.begin(mac, ip, dnServer, gateway, subnet);		//使用系統預設的ip位址
    
      }
  // give the Ethernet shield a second to initialize:
  delay(1000);			//延遲1000毫秒
  Serial.println("connecting...");		//印出連線中字句....
  Serial.println(Ethernet.localIP());		//印出裝置的ip位址.
  
}

void loop() {
  char ww ;
     String data1 = String((int)DHT.humidity);		//取得濕度資料，並轉成字串型態
     String data2 = String((int)DHT.temperature);	//取得溫度資料，並轉成字串型態
     String senddata = "field1="+data1+"&"+"field2="+data2 ;	//將溫濕度資料轉成ThingSpeak 寫入字串格式
 	
   // Print Update Response to Serial Monitor
  if (client.available())		//如果連線有資料送來
  {
    char c = client.read();		//讀出資料，並存當c之字元變數
    Serial.print(c);			//印出c之字元變數內容
  }
  
  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)		//如果未連線且上次有連線
  {
    Serial.println();
    Serial.println("...disconnected.");		//印出斷線
    Serial.println();
    
    client.stop();				//網路斷線
  }
  
  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateInterval))		//如果未連線且送資料的時間到了
  {
    updateThingSpeak(senddata);		//呼叫寫入ThingSpeak寫入資料函式
  }
  
  lastConnected = client.connected();		//將連線狀態存入lastConnected 旗標變數之中

}

unsigned int chkDHT( int chk )		//檢核DHT11溫濕度感測器 狀態資料是否正確
{
   switch (chk)		//檢查chk變數;多重檢查
  {
    case DHTLIB_OK: 		//如果是ok狀態
                Serial.println("DHT init is OK,\t");		//印出ok狀態
                return 1;
    case DHTLIB_ERROR_CHECKSUM:		//如果是數值加總不正確狀態
                Serial.println("DHT Checksum error,\t");		//印出數值加總不正確狀態
                return 0;
    case DHTLIB_ERROR_TIMEOUT:		//如果是在固定時間讀不到正確資料
                Serial.println("DHT Time out error,\t");		//印出在固定時間讀不到正確資料
               return 0;
    default:			//不知名錯誤
                Serial.println("DHT Unknown error,\t");		//不知名錯誤
               return 0;
  }

}

void updateThingSpeak(String tsData)
{
  if (client.connect(server, 80))		//用port 80之http: 通訊server變數內容的伺服器
  { 		//如果連線成功

    Serial.println("Connected to ThingSpeak...");		//印出連到ThingSpeak物聯網網站
    Serial.println();
        
    client.print("POST /update HTTP/1.1\n");		//ThingSpeak物聯網網站必要的tag資料
    client.print("Host: api.thingspeak.com\n");		//ThingSpeak物聯網網站必要的tag資料
    client.print("Connection: close\n");		//ThingSpeak物聯網網站必要的tag資料
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");		//將寫入API Key 傳入ThingSpeak物聯網網站必要的tag資料
    client.print("Content-Type: application/x-www-form-urlencoded\n");		//ThingSpeak物聯網網站必要的tag資料
    client.print("Content-Length: ");		//ThingSpeak物聯網網站必要的tag資料
    client.print(tsData.length());		//傳入資料長度
    client.print("\n\n");			//送入兩個回歸字元

    client.print(tsData);			//將溫濕度資料傳送到ThingSpeak
    
    lastConnectionTime = millis();			//將目前時間記錄到最後更新時間變數	lastConnectionTime 
    
    resetCounter = 0;			//將變數resetCounter 歸零
    
  }
  else
  {
    Serial.println("Connection Failed.");   	//印出連線失敗
    Serial.println();			//送入回歸字元

    
    resetCounter++;			//將變數resetCounter +1
    
    if (resetCounter >=5 ) //如果連入計數器大於五
		{
			resetEthernetShield();		//重置網路卡
		}		

    lastConnectionTime = millis(); 		//將目前時間記錄到最後更新時間變數
  }
}

void resetEthernetShield()
{
  Serial.println("Resetting Ethernet Shield.");   	//印出重置網路卡
  Serial.println();
  
  client.stop();		//網路斷線
  delay(1000);			//延遲1000毫秒	
  
  Ethernet.begin(mac, ip, gateway, subnet);		//使用系統預設的ip位址
  delay(1000);			//延遲1000毫秒	
}

