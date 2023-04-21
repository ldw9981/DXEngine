#include "GameTimer.h"



GameTimer::GameTimer()
{
}


GameTimer::~GameTimer()
{
}

void GameTimer::StartTime()
{
	LARGE_INTEGER frequencyCount;

	// 하드웨어 진동수(hz) 저장.
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = (double)frequencyCount.QuadPart;

	// 틱(Tick = Frame).
	QueryPerformanceCounter(&frequencyCount);
	countStart = frequencyCount.QuadPart;
}

double GameTimer::GetTime() const
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	double time
		= (double)((currentTime.QuadPart - countStart)
			/ countsPerSecond);

	return time;
}

double GameTimer::DeltaTime()
{
	// 틱 단위 현재 시간 구하기.
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// 현재 프레임에서 이전 프레임까지 틱 단위로 얼마나 지났는지 계산.
	__int64 tickCount = currentTime.QuadPart - prevFrameTime;

	// 이전 프레임 시간 저장.
	prevFrameTime = currentTime.QuadPart;

	// 예외 처리.
	if (tickCount < 0.0f)
		tickCount = 0.0f;

	// 프레임 시간을 초(단위) 시간으로 변환.
	double deltaTime = (double)(tickCount / countsPerSecond);

	// 예외 처리.
	if (deltaTime < 0.0)
		deltaTime = 0.0;

	return deltaTime;
}

void GameTimer::UpdateTimer()
{
	// 틱 단위로 프레임 수 증가시키기.
	++frameCount;

	// 1초가 지나면 타이머 초기화.
	if (GetTime() > 1.0f)
		Reset();
}

void GameTimer::Reset()
{
	fps = frameCount;
	frameCount = 0;
	StartTime();
	
	// FPS 콘솔에 출력.
	OutputDebugString(L"FPS: ");
	wchar_t buffer[256];
	_itow_s(fps, buffer, 10);
	OutputDebugString(buffer);
	OutputDebugString(L"\n");
}

int GameTimer::GetFPS() const
{
	return fps;
}
