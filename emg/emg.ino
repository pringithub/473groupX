/*
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
  This example code is in the public domain.
*/
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

#define AVG_OR_NAH          1
#define MOVING_WINDOW       1
#define REP_THRESHHOLD_HIGH 2.5
#define REP_THRESHHOLD_LOW  1.1

int inRep=0;
int repCount = 0;
float lastAverage=-1;

unsigned long pulseStart=0, pulseEnd=0, pulseWidth=0;
unsigned long deadStart=0, deadEnd=0, deadWidth=0;

// the loop routine runs over and over again forever:
void loop() {

    float timeslice[MOVING_WINDOW];
    //get "moving" average
    // ************
    // |wndw |wndw |
    for (int i = 0; i < MOVING_WINDOW; ++i) {
      // read the input on analog pin 0:
      int sensorValue = analogRead(A0);
      // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
      float voltage = sensorValue * (5.0 / 1023.0);
      timeslice[i] = voltage;
    }
    //get the average
    float avg = 0;
    for (int i = 0; i < MOVING_WINDOW; ++i) {
      avg += timeslice[i];
    }
    avg /= MOVING_WINDOW;
   
    //edge detection for reps
    if (double(avg) >= REP_THRESHHOLD_HIGH) {
      //start of pulse
      if (lastAverage >= REP_THRESHHOLD_LOW) {
        pulseStart = millis();
        Serial.print("pulseStart: ");Serial.print(pulseStart);
        deadWidth = (repCount > 0) ? pulseStart - pulseEnd : 0;    
        //Serial.println(20);             
      }      
    }
    //still avove threshholding levels
    //if inRep, will keep going
    //if !inRep, won't start 
    else if(avg >= REP_THRESHHOLD_LOW) {}
    else {//avg < REP_THRESHHOLD_LOW
     
      //end of pulse
      if(lastAverage > -1) {        
        pulseEnd = millis();
        Serial.print("     pulseEnd: ");Serial.print(pulseEnd);
        pulseWidth = pulseEnd - pulseStart;
        Serial.print("      pulseWidth: ");Serial.println(pulseWidth);
        //get rid of questionable reps
        if(pulseWidth > 500) {
          repCount++;
//          Serial.println(10);
        }
      }
      
      avg = -1;      
    }
    lastAverage = avg;
    //quantize the voltage
    //voltage = (int)(voltage/0.4);
    // print out the value you read:
//    Serial.println(avg);
  
}
