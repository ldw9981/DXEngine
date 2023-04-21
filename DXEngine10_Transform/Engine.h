#pragma once
#include "DXApp.h"

class Engine : public DXApp
{
public:
	Engine(HINSTANCE hinstance);
	~Engine();

	bool Init() override;
	void Update() override;
	void Render() override;

	void RotateObject();			// 물체 회전 함수.
};