#include "Window.h"

WF::Window::Window(LPCWSTR name, UINT x, UINT y, UINT sx, UINT sy){
	// Create rect
	RECT wr;
	wr.top = 0;
	wr.bottom = sy;
	wr.left = 0;
	wr.right = sx;

	// Adjust rect
	AdjustWindowRect(&wr, WS_BORDER | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

	// Create Window
	m_hwnd = CreateWindowEx(
		NULL,
		WF::WindowProc::getClassName(),
		name,
		WS_BORDER | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x,
		y,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this
	);
}

WF::Window::~Window(){
	// Check if handle is valid and destroy
	if (m_hwnd) {
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}
}

BOOL WF::Window::runMessageLoop(){
	// Stop and return false if window is not valid
	if (!m_hwnd) {
		return FALSE;
	}

	// Do message loop
	while (PeekMessage(&m_msg, m_hwnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}

	// Return true
	return TRUE;
}

LRESULT WF::Window::processMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg) {
		// Window destroyd
		case WM_DESTROY:
			{
				// Destroy window and retur own handled
				DestroyWindow(m_hwnd);
				m_hwnd = NULL;
				return NULL;
			}
			break;

		// Window key down
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			{
				// Set bit 
				m_keyState.set((UCHAR)wParam);
				return NULL;
			}
			break;

		// Window key up
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				// Clear bit
				m_keyState.reset((UCHAR)wParam);
				return NULL;
			}
			break;

		// Window in focus
		case WM_SETFOCUS:
			{
				// Window in focus
				m_bInFocus = TRUE;
			}
			break;

		// Window out of focus
		case WM_KILLFOCUS:
			{
				// No Foucs
				m_bInFocus = FALSE;
				// Clear all bits
				m_keyState.reset();
			}
			break;


		// Mouse Keys
		case WM_LBUTTONDOWN:
			{
				m_mouseState.lKeyDown = TRUE;
			}
			break;
		case WM_MBUTTONDOWN:
			{
				m_mouseState.mKeyDown = TRUE;
			}
			break;
		case WM_RBUTTONDOWN:
			{
				m_mouseState.rKeyDown = TRUE;
			}
			break;
		case WM_LBUTTONUP:
			{
				m_mouseState.lKeyDown = FALSE;
			}
			break;
		case WM_MBUTTONUP:
			{
				m_mouseState.mKeyDown = FALSE;
			}
			break;
		case WM_RBUTTONUP:
			{
				m_mouseState.rKeyDown = FALSE;
			}
			break;

		// Mouse move
		case WM_MOUSEMOVE:
			{
				m_mouseState.posX = GET_X_LPARAM(lParam);
				m_mouseState.posY = GET_Y_LPARAM(lParam);
			}
			break;

		// ...


	}

	// Fallback default window proc
	return DefWindowProc(wnd, msg, wParam, lParam);
}

BOOL WF::Window::isKeyDown(UCHAR keyCode){
	return m_keyState.test(keyCode);
}

BOOL WF::Window::isInFocus(){
	return m_bInFocus;
}

WF::MouseState WF::Window::getMouseState(){
	return m_mouseState;
}

HWND WF::Window::getHandle(){
	return m_hwnd;
}

BOOL WF::Window::setVisible(BOOL visible){
	// Return false if handle is not valid
	if (m_hwnd == NULL) {
		return FALSE;
	}

	// Show / Hide
	if (visible) {
		return ShowWindow(m_hwnd, SW_SHOW);
	} else {
		return ShowWindow(m_hwnd, SW_HIDE);
	}
}
