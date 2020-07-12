#include "Window.h"

WF::Window::Window(LPCWSTR name, UINT x, UINT y, UINT sx, UINT sy){
	// Create Window
	m_hwnd = CreateWindowEx(
		NULL,
		WF::WindowProc::getClassName(),
		name,
		WS_BORDER | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x,
		y,
		sx,
		sy,
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
		case WM_DESTROY:
			{
				// Destroy window and retur own handled
				DestroyWindow(m_hwnd);
				m_hwnd = NULL;
				return NULL;
			}
			break;

		// ...
	}

	// Fallback default window proc
	return DefWindowProc(wnd, msg, wParam, lParam);
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
