#include "WindowProc.h"

LPCWSTR WF::WindowProc::c_className = L"GfxWindowCls";
WF::WindowProc WF::WindowProc::s_instance = WF::WindowProc();

LPCWSTR WF::WindowProc::getClassName(){
	return c_className;
}

LRESULT WF::WindowProc::MessageProcSetup(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
	// Wait for create message
	if (msg == WM_NCCREATE) {
		// Cast lParam to createstruct
		CREATESTRUCT* ptrCrs = (CREATESTRUCT*)lParam;

		// Extract pointer to IMessageReciver
		WF::IMessageReciver* ptrReciver = (WF::IMessageReciver*)ptrCrs->lpCreateParams;
		SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)ptrReciver);
		SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)&WF::WindowProc::MessageProcRun);

		// Call run proc and return
		return WF::WindowProc::MessageProcRun(wnd, msg, wParam, lParam);
	}

	// Normaly return defproc
	return DefWindowProc(wnd, msg, wParam, lParam);
}

LRESULT WF::WindowProc::MessageProcRun(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
	// Extract IMessageReciver
	WF::IMessageReciver* ptrReciver = (WF::IMessageReciver*)GetWindowLongPtr(wnd, GWLP_USERDATA);
	// Call reciver
	ptrReciver->processMessage(wnd, msg, wParam, lParam);
}

WF::WindowProc::WindowProc() {
	// Get hinstance
	m_hInstance = GetModuleHandle(NULL);

	// Describe window class
	WNDCLASSEX cex;
	ZeroMemory(&cex, sizeof(WNDCLASSEX));
	cex.cbSize = sizeof(WNDCLASSEX);

	cex.style = CS_OWNDC;
	cex.lpfnWndProc = &WF::WindowProc::MessageProcSetup;
	cex.cbClsExtra = 0;
	cex.cbWndExtra = 0;
	cex.hInstance = m_hInstance;
	cex.hIcon = NULL;
	cex.hIconSm = NULL;
	cex.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	cex.hbrBackground = NULL;
	cex.lpszMenuName = NULL;
	cex.lpszClassName = c_className;

	// Register window class
	RegisterClassEx(&cex);
}

WF::WindowProc::~WindowProc(){
	// Unregister class
	MessageBox(NULL, L"Tada", L"Le", MB_OK);
	UnregisterClass(c_className, m_hInstance);
}
