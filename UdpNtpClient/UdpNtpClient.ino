
#include <SPI.h>
#include <Ethernet.h>		//　使用網路必需要用的
#include <EthernetUdp.h>	//　使用網路UDP通訊協定必需要用的

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


unsigned int localPort = 8888;       //要校時必須設定一個local port來給UDP使用
		// local port to listen for UDP packets	

char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server	校時伺服器

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message	封包大小 48位元組

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets	收入封包資料變數

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;		//宣告一個UDP網路物件

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);		//宣告監控畫面的通訊速率
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {		//要求DHCP 伺服器發放ip位址等資訊
    Serial.println("Failed to configure Ethernet using DHCP");		//要求DHCP 伺服器發放ip位址失敗，發出警告訊息
    // no point in carrying on, so do nothing forevermore:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);		//使用系統預設的ip位址

  }
  Udp.begin(localPort);		//啟動UDP 
}

void loop()
{
  sendNTPpacket(timeServer); // send an NTP packet to a time server   傳送封包到time.nist.gov NTP server 校時伺服器

  // wait to see if a reply is available
  delay(1000);		//延持1000毫秒
  if ( Udp.parsePacket() ) {		//如果收到 time.nist.gov NTP server 校時伺服器 傳送封包
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer		//讀取封包

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);		//將高位元組轉到highWord 變數
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);		//將低位元組轉到lowWord變數
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;			//將高低位元組合成一個變數
    Serial.print("Seconds since Jan 1 1900 = " );			//印出時間
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");		//印出Unix time = 
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;		//設定unix系統初始時間
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;		//計算取得時間差
    // print Unix time:
    Serial.println(epoch);		//印出時間差


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day) 印出共有多少天
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'  印出多少小時
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute) 印出多少分鐘
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second	印出多少秒
  }
  // wait ten seconds before asking for the time again
  delay(10000);			//延持10000毫秒
}

//送出NTP封包函數
// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(char* address)	//送出NTP封包函數
{
		//送出NTP封包函數
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);	//設定packetBuffer
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123		利用Port 123來傳送到address變數中的ip位址
  Udp.write(packetBuffer, NTP_PACKET_SIZE);		//傳送packetBuffer變數資料
  Udp.endPacket();		//傳送結束
}


