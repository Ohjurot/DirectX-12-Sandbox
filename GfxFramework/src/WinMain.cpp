#include <Windows.h>

LRESULT CALLBACK OwnWindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	switch (uMsg) {
		case WM_KEYDOWN:
			{
				MessageBox(hwnd, L"KEYDOWN", L"At unkonow window", MB_OK);
			}
			break;

		case WM_NCCREATE:
			{
				// Extract create struct
				CREATESTRUCT* ptrCrStruct = (CREATESTRUCT*)lParam;
				// Set window long ptr to create pointer
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ptrCrStruct->lpCreateParams);

				// Increment refcount
				UINT* ptrRunning = (UINT*)ptrCrStruct->lpCreateParams;
				*ptrRunning = *ptrRunning + 1;
			}
			break;
	
		case WM_DESTROY:
			{
				// Decrement ref count
				UINT* ptrRunning = (UINT*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
				*ptrRunning = *ptrRunning - 1;
			}
			break;
	}


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Describe window class
	WNDCLASSEX cex;
	ZeroMemory(&cex, sizeof(WNDCLASSEX));
	cex.cbSize = sizeof(WNDCLASSEX);

	cex.style = CS_OWNDC;
	cex.lpfnWndProc = &OwnWindowProc;
	cex.cbClsExtra = 0;
	cex.cbWndExtra = 0;
	cex.hInstance = hInstance;
	cex.hIcon = NULL;
	cex.hIconSm = NULL;
	cex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	cex.hbrBackground = NULL;
	cex.lpszMenuName = NULL;
	cex.lpszClassName = L"MyWndClass";

	// Register window class
	RegisterClassEx(&cex);

	UINT run = 0;

	// Create Window
	HWND wnd = CreateWindowEx(
		NULL,
		L"MyWndClass",
		L"GfxFramework",
		WS_BORDER | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		100,
		100,
		800,
		800,
		NULL,
		NULL,
		hInstance,
		&run
	);

	HWND wnd2 = CreateWindowEx(
		NULL,
		L"MyWndClass",
		L"GfxFramework",
		WS_BORDER | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		900,
		10,
		800,
		800,
		NULL,
		NULL,
		hInstance,
		&run
	);

	// Create MSG
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	
	// Message loop
	while (run) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Game loop
	}

	// Destroy window
	DestroyWindow(wnd);
	DestroyWindow(wnd2);

	// Unregister window class
	UnregisterClass(L"MyWndClass", hInstance);

	return 0;
}