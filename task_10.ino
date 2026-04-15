#include <Arduino_APDS9960.h>
#include <PDM.h>
#include <Arduino_BMI270_BMM150.h>

short sampleBuffer [256];
volatile int samplesRead = 0;

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read (sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1500);
  PDM.onReceive(onPDMdata);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
    
  }

  if (!PDM.begin (1, 16000)) {
    Serial.println("Failed to start PDM microphone");
    while (1);
  }

  if(!IMU.begin()) {
    Serial.println("Failed to initialiaze IMU");
    while (1);
  }

  Serial.println("Workspace situation test begin");

}

void loop() {
  // put your main code here, to run repeatedly:
  int proximity;
  if (APDS.proximityAvailable()) {
    proximity = APDS.readProximity();
  }

  

  int r, g, b, c;
  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, c);
  }
  
   
  int level = 0; 
  if(samplesRead) {
    long sum = 0;
    for (int i = 0; i < samplesRead; i++) {
      sum+= abs(sampleBuffer[i]);
    }
    level = sum / samplesRead;
    Serial.println(level);
    samplesRead = 0;
  }

  float x, y, z;

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope (x, y, z);
    Serial.print (x, 3);
    Serial.print(",");
    Serial.print (y, 3);
    Serial.print(",");
    Serial.println(z, 3);
  }

  int sound = 0;
  int dark = 0;
  int motion = 0;
  int near = 0;

  // motion check
  float motionval = abs(x) + abs(y) + abs(z);
  if (motionval > 50) {
    motion = 1;
  }
  // color check
  if (c < 800) {
    dark = 1;
  }
  // sound check
  if (level > 60) {
    sound = 1;
  }
  // prox check
  if (proximity < 120) {
    near = 1;
  }

  String finalLabel = "";
  

  if (sound == 0 && dark == 0 && motion == 0 && near == 0) {
    finalLabel = ("QUIET_BRIGHT_STEADY_FAR");

  } else if (sound == 1 && dark == 0 && motion == 0 && near == 0) {
    finalLabel =("NOISY_BRIGHT_STEADY_FAR");
  } else if (sound == 0 && dark == 1 && motion == 0 && near == 1) {
    finalLabel =("QUIET_DARK_STEADY_NEAR");
  } else if (sound == 1 && dark == 0 && motion == 1 && near == 1) {
    finalLabel =("NOISY_BRIGHT_MOVING_NEAR");
  }
  

  Serial.print("raw,mic=");
  Serial.print(level);
  Serial.print(",clear=");
  Serial.print(c);
  Serial.print(",motion=");
  Serial.print(motionval);
  Serial.print(",prox=");
  Serial.println(proximity);

  Serial.print("flags,sound=");
  Serial.print(sound);
  Serial.print(",dark=");
  Serial.print(dark);
  Serial.print(",moving=");
  Serial.print(motion);
  Serial.print(",near=");
  Serial.println(near);

  Serial.print("state, ");
  Serial.println(finalLabel);
  delay (100);
}
