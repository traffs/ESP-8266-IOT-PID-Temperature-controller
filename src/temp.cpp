#include <Arduino.h>
#include <math.h>

#include <math.h>

unsigned int Rs = 223529.4117647059;
double Vcc = 3.3;
float B_param = 3950; //

float T0 = 298.15; //25C in kelvin
float R1 = 100000; //resistance at T1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(Thermister(AnalogRead()));
  delay(1000);

}

int AnalogRead() {
  int val = 0;
  for(int i = 0; i < 20; i++) {
    val += analogRead(A0);
    delay(1);
  }

  val = val / 20;
  val = analogRead(A0);
  return val;
}

double Thermister(int val) {
  
  double V_NTC = (double)val * Vcc/ 1024;
  Serial.print("voltage ");
  Serial.println(V_NTC);
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  Serial.print("resistance ");
  Serial.println(R_NTC);
//  R_NTC = log(R_NTC);
//  double Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  double Temp = (T0*B_param)/(T0*log(R_NTC/Rs)+B_param); //Temperature in Kelvin
  Temp = Temp - 273.15;         
  return Temp;

}