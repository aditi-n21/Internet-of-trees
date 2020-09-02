#include <ESP8266WiFi.h>
#include <DHT.h>  // Including library for dht
const char* ssid     = "vivo 1606"; // Your ssid
const char* password = "123456789"; // Your Password
float temp_celsius = 0;
float temp_fahrenheit = 0;
WiFiServer server(80);
//merged
String apiKey = "97YGGMCOQBQ7PPP0";     //  Enter your Write API key from ThingSpeak
const char* server = "api.thingspeak.com";
const int gasPin = A0;

#define DHTPIN 14         //pin where the dht11 is connected
 
DHT dht(DHTPIN, DHT11);

WiFiClient client;
void setup() {
  
Serial.begin(115200);
pinMode(A0, INPUT);   
delay(10);
 dht.begin();
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi is connected");
server.begin();
Serial.println("Server started");
Serial.println(WiFi.localIP());
}
void loop() {
  
      float s=analogRead(gasPin);  
      float h = dht.readHumidity();
      float t = dht.readTemperature();


temp_celsius = (analogRead(A0) * 330.0) / 1023.0;   // To convert analog values to Celsius We have 3.3 V on our board and we know that output voltage of LM35 varies by 10 mV to every degree Celsius rise/fall. So , (A0*3300/10)/1023 = celsius
temp_fahrenheit = temp_celsius * 1.8 + 32.0;
Serial.print("  Temperature = ");
Serial.print(temp_celsius);
Serial.print(" Celsius, ");
Serial.print(temp_fahrenheit);
Serial.println(" Fahrenheit");

//merged
Serial.println(s);
delay(1000);
 if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                             Serial.print("SMOKE SENSOR: ");
                             Serial.print(s);
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}
WiFiClient client = server.available();
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 10");  // update the page after 10 sec
client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Digital Thermometer</strong></span></p>");
client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (*C)= ");
client.println(temp_celsius);
client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (F) = ");
client.println(temp_fahrenheit);
client.print("</p>");
client.println("</html>");
delay(5000);
}
