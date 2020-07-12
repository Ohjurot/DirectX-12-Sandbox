#include <Windows.h>

#include <WindowFramework/WindowProc.h>
#include <WindowFramework/Window.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Create window
	WF::Window wnd(L"GfxFramework", 10, 10, 800, 800);
	
	// Execute window loop
	while (wnd.runMessageLoop()) {
		// Game loop

	}

	return 0;
}