// 
// 
// 

#include <Arduino.h>
#include "MyTimer.h"

MyTimer::MyTimer(unsigned long delay) {
	start(delay);
}

MyTimer::~MyTimer() {}

void MyTimer::start(unsigned long delay) {
	this->delay = delay;
	lastTime = millis();
}

bool MyTimer::expired() {
	return millis() > (lastTime + delay);
}
