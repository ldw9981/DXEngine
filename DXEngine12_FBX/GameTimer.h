#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	void StartTime();
	double GetTime() const;
	double DeltaTime();

	void UpdateTimer();
	void Reset();
	int GetFPS();


private:
	//타이머 관련 변수
	double countsPerSecond = 0.0;
	__int64 countStart = 0;

	int frameCount = 0;
	int fps = 0;
	__int64 prevFrameTime = 0;
};

