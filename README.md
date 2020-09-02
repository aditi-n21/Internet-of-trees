# Internet-of-trees
“Fire is a good servant, but a bad master”. The problem with forest fires is that the forests are usually remote, abandoned areas with trees, dry and parching wood, 
leaves, and so forth that act as a fuel source. The fire destroys most of the flora and fauna of the region, hence severely affecting the ecological balance. 
While India loses Rs 1,176 crore a year to forest fires, a mere Rs 45-50 crore is allocated per annum under the Forest Fire prevention and Management Fund, which remains unspent.

In this work, we relied on Wireless Sensor Network (WSN). All nodes planted on trees with known location use temperature and smoke sensors as they are programmed on a certain
 threshold temperature, above it the node will send an alarm message to the source station. The base station sends the data to cloud-based software for processing by monitoring 
temperature changes over multiple sensors, map its shape and predict its path—functions they expect to be hugely helpful for firefighting.

#DETAILED METHODOLOGY

CASE 1: During Normal Conditions
At a particular time interval if the temperature ‘T’ which is output of temperature sensor is below the threshold value will indicate that there is no forest fire.
These temperature values along with the value of humidity and smoke will be transmitted in real time to base station so as monitor the forest environment condition continuously. 
Any increase in the temperature than normal threshold value will give us indication of chances of fire.

CASE 2: During Forest Fire
Now if the temperature becomes greater than the normal or threshold temperature at the particular time an alert is send to base station through call and SMS.
Also the sensor values are send continuously to the base station. The SMS will indicate the real time values of sensor data.

The overall system “Forest fire Detection and prediction over IoT” consists of several sensor Modules

•	LM35 – Analog temperature sensor gives Temperature value

•	DHT 11 – Digital Humidity sensor gives moisture value

•	ADXL335 – accelerometer gives vibration of motion values.

This whole sensor values uploaded to the open source cloud Thing speak. In that cloud we can visualize the values in form of plots – live updated values
Using that cloud API, we can predict the scenario if it is exceeding the preset values it automatically sent mail to users.
