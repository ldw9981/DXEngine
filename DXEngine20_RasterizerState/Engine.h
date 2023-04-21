#pragma once
#include "DXApp.h"

class Engine : public DXApp
{
public:
	Engine(HINSTANCE hinstance);
	~Engine();

	bool Init() override;
	void ProcessInput(double deltaTime) override;
	void Update(double deltaTime) override;
	void Render() override;
	void DrawMesh();
private:

	DIMOUSESTATE currentMouseState;
};