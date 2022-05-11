#include <Arduino.h>
//Pins
int Heater_pin = 3; //mosfet control pin


//Variables
float temperature_read = 0.0;
float set_temperature = 100;
float PID_error = 0;
float previous_error = 0;
float elapsedTime, Time, timePrev;
int PID_value = 0;



//PID constants
int kp = 9.1;   int ki = 0.3;   int kd = 1.8;
int PID_p = 0;    int PID_i = 0;    int PID_d = 0;



void setup() {
  pinMode(Heater_pin,OUTPUT);
  analogWriteFreq(980.39);
  Time = millis();
}


void loop() {
 // First we read the real value of temperature
  temperature_read = readThermister();
  //Next we calculate the error between the setpoint and the real value
  PID_error = set_temperature - temperature_read;
  //Calculate the P value
  PID_p = kp * PID_error;
  //Calculate the I value in a range on +-3
  if(-3 < PID_error <3)
  {
    PID_i = PID_i + (ki * PID_error);
  }

  //For derivative we need real time to calculate speed change rate
  timePrev = Time;                            // the previous time is stored before the actual time read
  Time = millis();                            // actual time read
  elapsedTime = (Time - timePrev) / 1000; 
  //Now we can calculate the D calue
  PID_d = kd*((PID_error - previous_error)/elapsedTime);
  //Final total PID value is the sum of P + I + D
  PID_value = PID_p + PID_i + PID_d;

  //We define PWM range between 0 and 255
  if(PID_value < 0)
  {    PID_value = 0;    }
  if(PID_value > 255)  
  {    PID_value = 255;  }
  //Now we can write the PWM signal to the mosfet on digital pin D3
  analogWrite(Heater_pin,255-PID_value);
  previous_error = PID_error;     //Remember to store the previous error for next loop.

  delay(300);
  //lcd.clear();

}



double readThermister() {

}