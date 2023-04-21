#include "Engine.h"

// 메인 함수.
int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE prevInstance,
	LPSTR lpCmdLine, int nCmdLine)
{
	// 엔진 객체 생성.
	Engine engine(hinstance);

	// 엔진 초기화.
	if (engine.Init() == false) 
		return 0;

	return engine.Run();
}