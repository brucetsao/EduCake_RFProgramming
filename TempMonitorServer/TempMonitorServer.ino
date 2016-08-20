#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "dht.h"    　
//dht DHT;
 
#define DHT11_PIN 7
dht DHT;
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};
IPAddress ip(192, 168, 30, 200);
IPAddress dnServer(168, 95, 1, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  int chk = DHT.read11(DHT11_PIN);
   if ( chkDHT(chk) == 0)    //　簡　查
       {
           Serial.println("ERROR on init DHT Sensor") ;
          while (true) ;
       }
       
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  
}

void loop() {

   EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          /*
          
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          */
            client.print("Humidity: ");
            client.print(DHT.humidity, 1);
            client.println("<br>");
            client.print("Temperature: ");
            client.print(DHT.temperature, 1);
            client.println("<br>");
            
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  
 
  delay(2000);
}

unsigned int chkDHT( int chk )
{
   switch (chk)
  {
    case DHTLIB_OK: 
                Serial.println("DHT init is OK,\t");
                return 1;
    case DHTLIB_ERROR_CHECKSUM:
                Serial.println("DHT Checksum error,\t");
                return 0;
    case DHTLIB_ERROR_TIMEOUT:
                Serial.println("DHT Time out error,\t");
               return 0;
    default:
                Serial.println("DHT Unknown error,\t");
               return 0;
  }

}
