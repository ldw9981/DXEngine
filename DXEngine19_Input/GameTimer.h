#pragma once

#include <Windows.h>

class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	// 게임 타이머 관련 메소드.
	void StartTime();
	double GetTime() const;
	double DeltaTime();

	void UpdateTimer();
	void Reset();
	int GetFPS() const;

private:

	// 타이머 관련 변수.
	double countsPerSecond = 0.0;
	__int64 countStart = 0;

	int frameCount = 0;
	int fps = 0;

	__int64 prevFrameTime = 0;
};