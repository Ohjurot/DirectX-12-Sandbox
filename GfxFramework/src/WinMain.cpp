#include <Windows.h>

#include <WindowFramework/WindowProc.h>
#include <WindowFramework/Window.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Create window
	WF::Window wnd(L"GfxFramework", 100, 100, 800, 800);
	wnd.setVisible(TRUE);

	// Execute window loop
	while (wnd.runMessageLoop()) {
		// Game loop
		WF::MouseState ms = wnd.getMouseState();

		if (ms.posX >= 400 && ms.posY >= 400 && ms.lKeyDown) {
			MessageBox(NULL, L"Test", L"Test", MB_OK);
		}
	}

	return 0;
}