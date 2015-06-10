// WebPot1a - html form for setting an MCP4131 to a value of resistance
// Trick for SPI on the ESP8266 - /CS pin GPIO15 needs 4.7k to Vss
// Tested using an ESP8266-03 with SPI connections:
//  GPIO13 HSPI MOSI --->   MCP4131 3 SDI
//  GPIO14 HSPI CLK  --->   MCP4131 2 SCK    4.7k
//  GPIO15 HSPI CS   --->   MCP4131 1 CS --/\/\/--- Vss     GPIO still needs 4.7k resistor to ground
//
// MCP4131  https://www.sparkfun.com/products/10613
//

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <string.h> 
 
const char* ssid = "chuckshome";
const char* password = "fb890fdf73";

byte address = 0x00;
char buffy[10];

String form = "<form action='pot'>Potientiomenter value: <input type='text' name ='resistor' value='0' size=5'><input type='submit' value='Submit'></form>";

char* itoa(int val, int base){
       static char buf[32] = {0};
       int i = 30;
       for(; val && i ; --i, val /= base)
            buf[i] = "0123456789abcdef"[val % base];
       return &buf[i+1];
}


// HTTP server will listen at port 80
ESP8266WebServer server(80);

void handle_pot() {
float resistor = (float)server.arg("resistor").toInt();
if (resistor < 75) resistor = 75;
if (resistor > 10000) resistor = 10000;

// code to create the MCP3141 setting for the closest value to desired resistor input 
int potval = round((resistor-75)*128/10000);
float finalVal = potval*10000/128+75;
strcpy(buffy,(itoa(finalVal,10)));


SPI.transfer(address);     //  byte address = 0x00;   wiper0
SPI.transfer(potval);

server.send(200, "text/html", "<html><head><META http-equiv='refresh' content='5;URL=/'></head><body>" + String("Pot set for: ") + buffy +String("</body></html>"));

}

void setup(void) {
  Serial.begin(115200);
  Serial.println("");
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  // Set up the endpoints for HTTP server
  //
  // Endpoints can be written as inline functions:
  server.on("/", [](){
   server.send(200, "text/html", form);
  });
  
  // And as regular external functions:
  server.on("/pot", handle_pot);
  
  // Start the server 
  server.begin();
  Serial.println("HTTP server started");
  SPI.begin();
}
 
void loop(void) {
  // check for incomming client connections frequently in the main loop:
  server.handleClient();
}

