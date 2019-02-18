#pragma once
#include <chrono>

class Timer
{
private:
	bool isRunning = false;
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
	
public:
	Timer();
	~Timer();
	double GetMillisecondsElapse();
	void Restart();
	bool Stop();
	bool Start();
};

