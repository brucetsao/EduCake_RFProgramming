#include <SPI.h>
#include <Ethernet.h>		 //　使用網路必需要用的

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};  		//使用這個 MAC ADRESS 當做這台educake 的MAC ADDRESS
IPAddress ip(192, 168, 30, 200);	//預設的網路IP位址，讀者可以自行更改為家中可用的位址	

IPAddress dnServer(168, 95, 1, 1);		//預設的DNS伺服器，本書為Hinet的DNS伺服器，讀者可以自行更改為家中可用的DNS伺服器
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);	//預設的閘道器的位址(就是Rounter或AP的網址)，本書為作者寫書環境的閘道器的位址，讀者可以自行更改為家中可用的閘道器的位址	
// the subnet:
IPAddress subnet(255, 255, 255, 0);		//子網路遮罩，本書為Class C

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);		//宣告伺服器並使用Port 80為通訊埠

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
    Serial.println("Now Program Start") ;
    
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // 啟用 Ethernet 連線，預設會以 DHCP 取得 IP 位址
  if (Ethernet.begin(mac) == 0) {
    Serial.println("I can't get any  IP address from DHCP Server");
    // 無法取得 IP 位址，不做任何事情
    for(;;)
      ;
  }
  // 輸出 IP 位址
  Serial.print("Default IP Address is : ");	//印出伺服器資訊
  Serial.println(ip);		//印出伺服器 IP位址

//   Ethernet.begin(mac, ip, dnServer, gateway, subnet);
   Ethernet.begin(mac);    // 如果要用DHCP 伺服器取得 ip,必需要這樣寫

  server.begin();		//開啟Web伺服器運作
  Serial.print("Server is running at ip: ");		//印出伺服器資訊
  Serial.print(Ethernet.localIP());		//印出伺服器資訊
  Serial.print("from DHCP Server \n");}		//印出伺服器資訊


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();		//如果有人連入WEB伺服器，啟動一個Client連線端
  if (client) {		//如果啟動一個Client連線端成功
    Serial.println("New Client");		//印出"新連線"
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {		//如果連線成功
      if (client.available()) {		//如果連線的Client連線端有送資料過來
        char c = client.read();		//如果連線的Client連線端有送資料過來
        Serial.write(c);		//印出變數c
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {		//如果變數c是換行資料而且目前資料為空
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");		//送出Http 標頭資料(固定要送)
          client.println("Content-Type: text/html");		//送出Http 標頭資料(固定要送)
          client.println("Connection: close");  // the connection will be closed after completion of the response	//送出Http 標頭資料(固定要送)
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec	//送出Http 標頭資料(固定要送)
          client.println();		//送出Http 標頭資料(固定要送)
          client.println("<!DOCTYPE HTML>");		//送出Http 標頭資料(固定要送)
          client.println("<html>");		//送出Http 標頭資料(固定要送)
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {	//將類比腳位0到5得資料讀出來
            int sensorReading = analogRead(analogChannel);
            client.print("Analog Input ");		//送出畫面資料
            client.print(analogChannel);		//送出第幾個類比通訊埠   
            client.print(" is ");		//送出畫面資料
            client.print(sensorReading);		//送出讀出該類比通訊埠 的資料
            client.println("<br />");		//送出網頁換行鍵
          }
          client.println("</html>");		//送出網頁Tag資料(固定要送)
          break;
        }
        if (c == '\n') {		//新的一行
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {		//如果不是換行鍵
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);		//延遲一毫秒
    // close the connection:
    client.stop();		//關閉連線的Client連線端(必需要關閉，不然會掛掉)
    Serial.println("Client Disconnected");		//送出連線斷線
  }
}

