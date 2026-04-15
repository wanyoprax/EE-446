#include <Arduino_APDS9960.h>
#include <Arduino_HS300x.h>
#include <Arduino_BMI270_BMM150.h>
unsigned long lastEventTime = 0;
const unsigned long cooldown = 1500;


float prev_mag = 0;
float prevtemp = 0;
float prevhum = 0;
int prevr = 0;
int prevg = 0;
int prevb = 0;
int prevc = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1500);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU.");
    while (1);
  }
  Serial.println("Magnetometer test started");
  Serial.println("mx, my, mz");

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
    
  }

  if (!HS300x.begin()) {
    Serial.println("Failed to initialize humidity and temperature");
    while (1);
  }

}

void loop() {
  float x, y, z;

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField (x, y, z);
  }
  float mag = abs (x) + abs (y) + abs (z);
  

  int r, g, b, c;
  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, c);
  }
  

  float temperature = HS300x.readTemperature();
  float humidity = HS300x.readHumidity();
  

  int humid_jump = 0;
  int temp_rise = 0;
  int mag_shift = 0;
  int light_or_color_change = 0;
  
  float tempthresh = 1.0;
  float humthresh = 5.0;
  int lightthresh = 50;
  float magthresh = 100;
  String finalLabel = "";


  if (abs(humidity - prevhum) > humthresh) {
    humid_jump = 1;
  }

  if ((temperature - prevtemp) > tempthresh) {
    temp_rise = 1;
  }
  
  if (abs(mag - prev_mag) > magthresh) {
    mag_shift = 1;
  }

  if (abs(c - prevc) > lightthresh) {
    light_or_color_change = 1;
  }

  prevhum = humidity;
  prevtemp = temperature;
  prev_mag = mag;
  prevr = r;
  prevg = g;
  prevb = b;
  prevc = c; 

  bool cooldownActive = (millis() - lastEventTime) < cooldown;

  if (humid_jump == 0 && temp_rise == 0 && mag_shift == 0 && light_or_color_change == 0) {
    finalLabel = "BASELINE_NORMAL";
  } 
  else if (!cooldownActive) {
    if (humid_jump == 1 && temp_rise == 1 ) {
      finalLabel = "BREATH_OR_WARM_AIR_EVENT";
      lastEventTime = millis();
    } else if (mag_shift == 1) {
      finalLabel = "MAGNETIC_DISTURBANCE_EVENT";
      lastEventTime = millis();
    } else if (light_or_color_change == 1) {
      finalLabel = "LIGHT_OR_COLOR_CHANGE_EVENT";
      lastEventTime = millis();
    }
  } else {
    finalLabel = "BASELINE_NORMAL";
  }


  Serial.print("raw,rh=");
  Serial.print(humidity);
  Serial.print(",temp=");
  Serial.print(temperature);
  Serial.print(",mag=");
  Serial.print(mag);
  Serial.print(",r=");
  Serial.print(r);
  Serial.print(",g=");
  Serial.print(g);
  Serial.print(",b=");
  Serial.print(b);
  Serial.print(",clear=");
  Serial.println(c);

  Serial.print("flags,humid_jump=");
  Serial.print(humid_jump);
  Serial.print(",temp_rise=");
  Serial.print(temp_rise);
  Serial.print(",mag_shift=");
  Serial.print(mag_shift);
  Serial.print(",light_or_color_change=");
  Serial.println(light_or_color_change);

  Serial.print("event,");
  Serial.println(finalLabel);







delay (1200);

}
