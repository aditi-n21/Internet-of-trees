/* ADXL345
 * i2c bus SDA = GPIO0; SCL = GPIO2
 *
 * Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 * USER: admin PASSWOR: admin
 */  

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
//ADXL345
#include <Wire.h>

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *password = "12345678";
ESP8266WebServer server(80);



#define DEBUG true
#define Serial if(DEBUG)Serial

#define DEVICE (0x53) // Device address as specified in data sheet
float last_value = 0;
float prelast_value = 0;

int show_count = 0; 
int trigger_count = 0;
float trigger_value = -5; //DEFAULT VALUE ???
float current_value = 0;


#define ADXL345_MG2G_MULTIPLIER (0.004)
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
//char DATAY0 = 0x34;    //Y-Axis Data 0
//char DATAY1 = 0x35;    //Y-Axis Data 1
//char DATAZ0 = 0x36;    //Z-Axis Data 0
//char DATAZ1 = 0x37;    //Z-Axis Data 1

float max_x=0;
float min_x=0;
float cal_x=0;
float x = 0;

//Check if header is present and correct
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;  
}

//root page can be accessed only if authentification is ok
void handleRoot(){
  Serial.println("Enter handleRoot");
  String header;

  if (!is_authentified())
  {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  
  if (server.hasArg("RESET"))
  {
      Serial.println("Reset max and min values");
      min_x=0;
      max_x=0;
      String content = "<html><body><h2>ADXL Demo</h2>";
      content += " <a href=\"/login\">reset</a><BR><BR>";
      server.send(200, "text/html", content);
  }
  
  
  
  String content = "<html><body><h2>ADXL demo</h2>";
  //<H2>hello, you successfully connected to esp8266!</H2><br>";

  if (server.hasHeader("User-Agent"))
  {
    //content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
     content += "x :" + String((int)current_value ) + " Corr(x):" + String((float)current_value - cal_x ) + " min_x:" + String((float)min_x) + " max_x:" + String((float)max_x) + "<br><br>";
     content += "Trigger value:" + String((float)trigger_value) + " trigger count:" + String(trigger_count) + "<br><br>";
  }
  content += " <a href=\"/login?DISCONNECT=YES\">disconnect</a><BR><BR>";
  content += " <a href=\"/?RESET=YES\">Reset min and max values</a><br><br>";
  content += " <a href=\"/settings\">Settings</a>";
  //last line 
  content += " </body></html>";
  server.send(200, "text/html", content);



}

void handleSettings()
{
  //reading
  if (server.hasArg("TRIGGER"))
  {
    trigger_value = server.arg("TRIGGER").toInt();
    return;
  }

  //setting
  String msg = " <br><br> <a href=\"/ \">Home</a>";
  String content = "<html><body><form action='/settings' method='POST'><br>";
  content += "Value:<input type='number' name='TRIGGER' min='-500' max='500' placeholder='trigger value'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  server.send(200, "text/html", content);
}

//login page, also called for disconnect
void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }

  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}
//no need authentification

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


float readAccel() 
{
  Serial.print("readAccel");
  uint8_t howManyBytesToRead = 6; //6 for all axes
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
  short x =0;
   x = (((short)_buff[1]) << 8) | _buff[0];
  //short y = (((short)_buff[3]) << 8) | _buff[2];
  //short z = (((short)_buff[5]) << 8) | _buff[4];
  Serial.println(x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD);
  return x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  //x = x + cal_x;

  
  //Serial.print("x: "); 
  //Serial.print( x*2./512 );
  //Serial.print(" y: ");
  //Serial.print( y*2./512 );
  //Serial.print(" z: ");
  //Serial.print( z*2./512 );
  //Serial.print("X: "); Serial.print( x);

  //Serial.println( sqrtf(x*x+y*y+z*z)*2./512 );

//getX() = read16(ADXL345_REG_DATAX0);
//x = getX() * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  
}

void writeTo(byte address, byte val) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[]) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while(Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

void setup() {
  delay(1000);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/settings", handleSettings);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works without need of authentification");
  });
  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  
  server.begin();
  Serial.println("HTTP server started");


//ADXL345
  // i2c bus SDA = GPIO0; SCL = GPIO2
  Wire.begin(0,2);      
  
  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);
  
  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);

  int i =0;
  for(i=0; i<11; i++)
  {
    //uint8_t howManyBytesToRead = 6;
    //readFrom( DATAX0, howManyBytesToRead, _buff);
    float calib_x ;//= (((short)_buff[1]) << 8) | _buff[0];
    calib_x = readAccel();
    //if(i==0)
    // cal_x = x;
    if(i>0)
     cal_x = cal_x + calib_x;
    Serial.println(calib_x);
    delay(100);
  }

  cal_x = cal_x/10;
  Serial.print("cal_x: ");Serial.println(cal_x); 
  
}

void loop() {
  server.handleClient();
  current_value = readAccel();  // read ONLY x, for the y and x modify the readAccel function
  
  if((current_value - cal_x) > max_x)
    max_x = current_value - cal_x;
  if((current_value - cal_x) < min_x)
    min_x = current_value - cal_x;

  Serial.print("x: ");Serial.print(current_value);  Serial.print(" x(corrected): ");Serial.print(current_value - cal_x);    
  Serial.print(" Min:" );Serial.print(min_x); Serial.print(" Max:" ); Serial.println(max_x);    
  Serial.print("Trigger value:"); Serial.print(trigger_value); Serial.print(" Count:"); Serial.println(trigger_count);
  delay(100);   // only read every 100ms

}
