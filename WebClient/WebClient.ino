
#include <SPI.h>
#include <Ethernet.h>		//　使用網路必需要用的


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};		//使用這個 MAC ADRESS 當做這台educake 的MAC ADDRESS
IPAddress ip(192, 168, 30, 200);		//預設的網路IP位址，讀者可以自行更改為家中可用的位址	
IPAddress dnServer(168, 95, 1, 1);		//預設的DNS伺服器，本書為Hinet的DNS伺服器，讀者可以自行更改為家中可用的DNS伺服器
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);		//預設的閘道器的位址(就是Rounter或AP的網址)，本書為作者寫書環境的閘道器的位址，讀者可以自行更改為家中可用的閘道器的位址	
// the subnet:
IPAddress subnet(255, 255, 255, 0);		//子網路遮罩，本書為Class C

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)		//www.google.com 的網址
char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {		//要求DHCP 伺服器發放ip位址等資訊
    Serial.println("Failed to configure Ethernet using DHCP");		//要求DHCP 伺服器發放ip位址失敗，發出警告訊息
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
      Ethernet.begin(mac, ip, dnServer, gateway, subnet);		//使用系統預設的ip位址

  }
  // give the Ethernet shield a second to initialize:
  delay(1000);			//延持1000毫秒
  Serial.println("connecting...");		//印出連線中字句....


  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {		//如果Client物件連到 server(74,125,232,128) www.google.com的網址 成功的話
    Serial.println("connected");		//印出連線成功字句....
    // Make a HTTP request:		
	//以下是固定http 通訊 protocol
    client.println("GET /search?q=arduino HTTP/1.1");		
    client.println("Host: www.google.com");		//以下是告知連到www.google.com
    client.println("Connection: close");		//連完後關閉
    client.println();
  }
  else {
    // if you didn't get a connection to the server:		
    Serial.println("connection failed");		// 連接www.google.com 失敗，告訴使用者連接失敗
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {		//如果Client物件有東西回傳
    char c = client.read();		//讀出Client物件，並存到變數c
    Serial.print(c);			//印出變數c
  }	

  // if the server's disconnected, stop the client:
  if (!client.connected()) {		//如果Client已經關閉連線
    Serial.println();
    Serial.println("disconnecting.");			//印出中斷連線成功字句....
    client.stop();		//連現中止

    // do nothing forevermore:
    while (true);			//永久迴圈中斷程式
  }
}

