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


#define AVG_OR_NAH 1
#define MOVING_WINDOW 1
#define REP_THRESHHOLD 0.35

int isRep;
int repCount = 0;

// the loop routine runs over and over again forever:
void loop() {


  if (AVG_OR_NAH == 0) {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    float voltage = sensorValue * (5.0 / 1023.0);

    // print out the value you read:
    Serial.println(voltage);
  }


  else {
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
    if (avg < REP_THRESHHOLD) {
      if (isRep == 1) {
        isRep = 0;
      }
      avg = -1;
    }
    else {
      if (isRep == 0) {
        isRep = 1;
        repCount++;
        Serial.println("repCount----------------------------------------");
        Serial.println(repCount);
      }
    }


    //quantize the voltage
    //voltage = (int)(voltage/0.4);

    // print out the value you read:
    Serial.println(avg);
  }

}
