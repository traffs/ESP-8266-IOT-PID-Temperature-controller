#pragma once

#ifndef controller.h 
#define controller.h

#include <Arduino.h>

// extern String State;
// extern String temperatureC;

void controllersetup();
void writeFile();
String readFile();

/* blinks the LED once for an length of <ms> milliseconds. */
String processor(const String& var);

#endif /* controller.h */