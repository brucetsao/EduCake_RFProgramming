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

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetClient client;		//宣告伺服器並使用Port 80為通訊埠
IPAddress server(192, 168, 30, 100); 		//宣告WEB伺服器網址(連線用)
String  server2 = String("192.168.30.100"); 		//宣告WEB伺服器網址( 連線字串用)
  String connectstr ;			//宣告傳參數字串變數
  String connectstr2 ;			//宣告WEB伺服器傳參數字串變數	
  
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
 connectstr = "" ;		//清空傳參數字串變數
 connectstr2 = "" ;		//清空WEB伺服器傳參數字串變數	
 
   connectstr = "humidity=" + String((int)DHT.humidity)+"&"+"temperature=" + String((int)DHT.temperature);
		//組出PHP 傳參數字串變數內容,並把溫濕度資料含入
     Serial.print((int)DHT.humidity) ;		//印出濕度資料
     Serial.print("/") ;
     Serial.print((int)DHT.temperature) ;	//印出溫度資料	
     Serial.print("/") ;
     Serial.println(connectstr) ;
     
  	if (client.connect(server,80)) { // REPLACE WITH YOUR SERVER ADDRESS  連線到WEB伺服器，如果成功連接上
        connectstr2 = "POST /http://"+server2+"/iot/dhtdata/dhtadd2.php?"+connectstr ;		//組出PHP GET傳參數字串變數內容,並把溫濕度資料含入
                Serial.println(millis() ); 		//印出(connectstr2)變數內容
                Serial.println(connectstr2); 		//印出(connectstr2)變數內容
	       client.print(connectstr2); 		//透過網路，送出(connectstr2)	變數內容	
                Serial.print("\n\n\n");			//印出三個換行鍵
   client.println(" HTTP/1.1");
   Serial.println(" HTTP/1.1");
   client.print("Host: http://");
   client.println(server2);
   
   Serial.println(server2);
   client.println("User-Agent: Arduino");
   Serial.println("User-Agent: Arduino");
   client.println("Accept: text/html");
   Serial.println("Accept: text/html"); 
   client.println(); 
     delay(1000);
	}
        else
        {
                 Serial.println("Connecting server ERROR");		//印出連線失敗
        }
    

	if (client.connected()) { 		//如果連線中
                   Serial.println("Close Connection");		//印出關閉連線
		client.stop();	// DISCONNECT FROM THE SERVER		//關閉連線
	}

	delay(3000); //延遲3000毫秒


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
