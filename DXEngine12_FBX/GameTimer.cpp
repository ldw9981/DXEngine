#include "GameTimer.h"
#include <Windows.h>


GameTimer::GameTimer()
{
}


GameTimer::~GameTimer()
{
}

void GameTimer::StartTime()
{
	LARGE_INTEGER frequencyCount;	
	QueryPerformanceFrequency(&frequencyCount);			// 하드웨어 진동수 저장	
	countsPerSecond = (double)frequencyCount.QuadPart;	// 초당 진동수 저장
	// 틱  ( tick = frame )
	QueryPerformanceCounter(&frequencyCount);
	countStart = frequencyCount.QuadPart;
}

double GameTimer::GetTime() const
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	double time = (double)((currentTime.QuadPart - countStart) / countsPerSecond);
	return time;
}

// 틱탄위 현재 시간 구하기
double GameTimer::DeltaTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	__int64 tickCount = currentTime.QuadPart - prevFrameTime;	
	prevFrameTime = currentTime.QuadPart; //이전 프레임시간 저장
	//예외

	if (tickCount < 0)
		tickCount = 0;

	// 프레임시간을 초(단위)시간으로 변환.
	double deltaTime = (double)(tickCount / countsPerSecond);
	if (deltaTime < 0.0)
		deltaTime = 0;

	return deltaTime;
}

void GameTimer::UpdateTimer()
{	
	++frameCount;
	if (GetTime() > 1.0f)
		Reset();
}

void GameTimer::Reset()
{
	fps = frameCount;
	frameCount = 0;
	StartTime();
}

int GameTimer::GetFPS()
{
	return frameCount;
}
