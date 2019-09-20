// Timer.h
#pragma once


class MyTimer {
public:
	// delay in milliseconds
	MyTimer() {}
	MyTimer(unsigned long delay);
	~MyTimer();

	void start(unsigned long delay);
	bool expired();

private:
	unsigned long delay;
	unsigned long lastTime;
};
