
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



// telnet defaults to port 23
EthernetServer server(23);		//宣告一個以太網路伺服器物件，並使用Port23 為listening 通訊埠
boolean alreadyConnected = false; // whether or not the client was connected previously 連線旗標

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);	//啟動以太網路
  // start listening for clients
  server.begin();		//啟動	Port23 的伺服器物件
  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Chat server address:");		//印出Chat server address:
  Serial.println(Ethernet.localIP());		//印出網址
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();	//產生一個以太網路用戶端網路物件

  // when the client sends the first byte, say hello:
  if (client) {		//當以太網路用戶端網路物件有人連入
    if (!alreadyConnected) {	//如果是新連線
      // clead out the input buffer:
      client.flush();		//以太網路用戶端網路物件 更新緩衝區資料
      Serial.println("We have a new client");		//印出有新連線
      client.println("Hello, client!");			//送出Hello 歡迎詞
      alreadyConnected = true;		//設定已連線

    }

    if (client.available() > 0) {		//如果以太網路用戶端網路物件有資料送出
      // read the bytes incoming from the client:
      char thisChar = client.read();		//讀取以太網路用戶端網路物件送出的資料到thisChar 變數
      // echo the bytes back to the client:
      server.write(thisChar);		//送出thisChar 變數到server物件
      // echo the bytes to the server as well:
      Serial.write(thisChar);		//印出thisChar 變數
    }
  }
}



