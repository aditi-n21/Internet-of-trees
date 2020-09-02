/* Interface Accelerometer with NodeMCU
 * By TheCircuit
 */

const int xPin = A0;    //x-axis of the Accelerometer 

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

}

void loop() {
  
  int x = analogRead(xPin);
  delay(100);

  Serial.print("X-axis : ");
  Serial.println(x);

}
