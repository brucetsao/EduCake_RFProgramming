#include <SPI.h>
#include <Ethernet.h>      	 //　使用網路必需要用的

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


// Enter the IP address of the server you're connecting to:
IPAddress server(140, 112, 172, 11);		//台大PTT網址

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use  port 10002):
EthernetClient client;		//宣告一個TCP/IP的Client物件

void setup() {
  // start the Ethernet connection:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);		// 啟用 Ethernet 連線

  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // give the Ethernet shield a second to initialize:
  delay(1000);		//延持1000毫秒
  Serial.println("connecting...");		//印出連線中字句....

  // if you get a connection, report back via serial:
  if (client.connect(server, 23)) {		//如果Client物件連到 server(140, 112, 172, 11);台大PTT網址 成功的話
    Serial.println("connected");		//印出連線成功字句....
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");		//印出連線失敗字句....
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {		//如果Client物件有東西回傳
    char c = client.read();		//讀出Client物件，並存到變數c
    Serial.print(c);		//印出變數c	//
  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {		//如果我們從監控畫面輸入文字，
    char inChar = Serial.read();		//讀出監控畫面輸入文字，並存到變數c
    if (client.connected()) {		//如果連線中
      client.print(inChar);		//將輸入文字傳到Client物件
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {		//如果Client物件已斷線
    Serial.println();
    Serial.println("disconnecting.");		////印出已斷線
    client.stop();			//Client物件關閉
    // do nothing:
    while (true);		//永久迴圈，不再做事
  }
}




