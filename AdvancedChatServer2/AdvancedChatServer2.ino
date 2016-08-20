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
boolean alreadyConnected ; // whether or not the client was connected previously 連線旗標
boolean ConnectedFlag[10] ; // whether or not the client was connected previously 連線旗標
int connectNo = 0 ;			//連線號碼
  EthernetClient client ;		//產生一個傾聽的以太網路用戶端網路物件
  EthernetClient Connectclient[10];		//產生十個以太網路用戶端網路物件


void setup() {

  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);	//啟動乙太網路連線
  // start listening for clients
  server.begin();		//啟動	Port23 乙太網路連線
  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
    initConnectingFlag() ;  //　初始化連線旗標
    
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Chat server address:");		//印出Chat server address:
  Serial.println(Ethernet.localIP());		//印出網址
  DisplayConnectingStatus() ;				//顯示連線狀況
}

void loop() {
  // wait for a new client:
    client = server.available();		//產生傾聽的以太網路用戶端網路物件
    
  // when the client sends the first byte, say hello:
  if (client) {		//當以太網路用戶端網路物件有人連入
    connectNo = 0 ;
    while (connectNo <10)		//迴圈十組連線
    {
        if (!ConnectedFlag[connectNo]) {	//如果是新連線
          // clead out the input buffer:
              Connectclient[connectNo] =client ;
            Connectclient[connectNo].flush();		//以太網路用戶端網路物件 更新緩衝區資料
          Serial.println("We have a new client");		//印出有新連線
          client.println("Hello, client!");		//送出Hello 歡迎詞
          ConnectedFlag[connectNo] = true;		//設定已連線
          break ;
        }
       connectNo ++ ;
    }
  }
      connectNo = 0 ;
        while (connectNo <10)
    {
        if (!Connectclient[connectNo].connected()) 		//檢查是否斷線
               ConnectedFlag[connectNo] = false;		//設定連線旗標參數=false(未連線)

        if (Connectclient[connectNo].available() > 0) 
        {	//如果以太網路用戶端網路物件有資料送出
          // clead out the input buffer:
          char thisChar = Connectclient[connectNo].read();		//讀取以太網路用戶端網路物件送出的資料到thisChar 變數
          // echo the bytes back to the client:
          Serial.print("Connect ");		//印出連線中
          Serial.print(connectNo);		//印出第幾號連線
          Serial.print(":");		//印出":"
          server.write(thisChar);		//送出thisChar 變數到server物件
          // echo the bytes to the server as well:
          Serial.write(thisChar);		//印出thisChar 變數
        }
       connectNo ++ ;
    }

}

void initConnectingFlag()
{		//初始化連線旗標參數
  for(connectNo=0 ; connectNo < 10 ; connectNo++)
  {
    ConnectedFlag[connectNo] = false ;		//設定連線旗標參數=false(未連線)
 
  }
}

void DisplayConnectingStatus()
{	//顯示連線狀況
  for(connectNo=0 ; connectNo < 10 ; connectNo++)
  {
    if (ConnectedFlag[connectNo])		//如果連線中
    {		
        Serial.print("Connection ") ;		//該區顯示連線中的資訊
        Serial.print(connectNo) ;
        Serial.print(": Connected \n") ;
    }
    else		//如果未連線中
    {
        Serial.print("Connection ") ;		//該區顯示等待連線中的資訊
        Serial.print(connectNo) ;
        Serial.print(": Waiting Connecting \n") ;
    }
       
  // Connectclient[connectNo]  = server.available();		//產生一個以太網路用戶端網路物件
  }
}

