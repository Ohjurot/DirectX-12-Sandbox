#include <Windows.h>

#include <Application/App.h>
#include <Application/Flow.h>

#include <Game/Game.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Create stack application
	MY::Game game;
	
	// Describe Application
	Application::Meta meta;
	meta.clientHeight = 800;
	meta.clientWidth = 800;
	meta.name = L"GxfFramework";
	meta.entryPoint = (Application::GfxApp*)&game;

	// Start the flow
	HRESULT result = Application::Flow::Begin(&meta);

	// Error checking
	if (result != S_OK) {
		// Translate HRESULT
		WCHAR hresString[512];
		FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			result, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), 
			hresString,
			255, 
			NULL
		);

		// Message box
		MessageBox(NULL, hresString, L"Something went wrong", MB_OK | MB_ICONERROR);
	}

	// Return zero
	return 0;
}