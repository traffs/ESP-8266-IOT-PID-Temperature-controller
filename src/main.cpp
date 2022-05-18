#include <Arduino.h>
#include "controller.h"
#include <math.h>

int Heater_pin = 13;

unsigned int Rs = 100000;
double Vcc = 3.3;
float B_param = 3950; //

float T0 = 298.15; // 25C in kelvin
float R1 = 100000; // resistance at T0

// Variables
float temperature_read = 0.0;
float set_temperature = 45;
float PID_error = 0;
float previous_error = 0;
float elapsedTime, Time, timePrev;
int PID_value = 0;

// PID constants
int kp = 50;  //9.1 15
int ki = 0.4; //0.4  0.9
int kd = 1.8; //1.8  2.1
// int kp = 30.93;
// int ki = 2.13;
// int kd = 299.02;
int PID_p = 0;
int PID_i = 0;
int PID_d = 0;

int AnalogRead()
{
  int val = 0;
  for (int i = 0; i < 20; i++)
  {
    val += analogRead(A0);
    delay(1);
  }

  val = val / 20;
  //  val = analogRead(A0);
  return val;
}

double Thermister(int val)
{
  double V_NTC = (double)val * Vcc / 1024;
  //  Serial.print("voltage ");
  //  Serial.println(V_NTC);
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  //  Serial.print("resistance ");
  //  Serial.println(R_NTC);
  //  R_NTC = log(R_NTC);
  //  double Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  double Temp = (T0 * B_param) / (T0 * log(R_NTC / Rs) + B_param); // Temperature in Kelvin
  Temp = Temp - 273.15;
  return Temp;
}

String State = String(set_temperature, 1);
String temperatureC = String(temperature_read, 2);

void setup()
{
  Serial.begin(115200);
  pinMode(Heater_pin, OUTPUT);
  // analogWriteFreq(400.39);
  controllersetup();
  Time = millis();
}

void loop()
{
  // First we read the real value of temperature
  temperature_read = Thermister(AnalogRead());

  temperatureC = String(temperature_read, 2);

  Serial.print("temperature_read ");
  Serial.println(temperature_read);
  Serial.println(set_temperature);
  // Next we calculate the error between the setpoint and the real value
  PID_error = set_temperature - temperature_read;
  // Serial.print("error ");
  // Serial.println(PID_error);
  // Calculate the P value
  PID_p = kp * PID_error;
  // Calculate the I value in a range on +-3
  if (PID_error > -3 && PID_error < 3)
  {
    PID_i = PID_i + (ki * PID_error);
  }

  // For derivative we need real time to calculate speed change rate
  timePrev = Time; // the previous time is stored before the actual time read
  Time = millis(); // actual time read
  elapsedTime = (Time - timePrev) / 1000;

  Serial.print("elapsed time ");
  Serial.println(elapsedTime);
  // Now we can calculate the D calue
    Serial.print("delta error ");
  Serial.println(PID_error - previous_error);
  PID_d = kd * ((PID_error - previous_error) / elapsedTime);
  // Final total PID value is the sum of P + I + D
  PID_value = PID_p + PID_i + PID_d;

  // We define PWM range between 0 and 1023
  if (PID_value < 0)
  {
    PID_value = 0;
  }
  if (PID_value > 700) //use 700 max
  {
    PID_value = 700;
  }

      Serial.print("Pid ");
  Serial.println(PID_value);
  // int final_value = 1023-PID_value;
  // Serial.println(final_value);
  // Now we can write the PWM signal to the mosfet on digital pin D3
  analogWrite(Heater_pin, PID_value);
  previous_error = PID_error; // Remember to store the previous error for next loop.

  delay(300);
  // lcd.clear();
}