#pragma once

#include <Windows.h>
#include <windowsx.h>

#include <bitset>

#include <WindowFramework/IMessageReciver.h>
#include <WindowFramework/WindowProc.h>

namespace WF {
	struct MouseState {
		BOOL lKeyDown, mKeyDown, rKeyDown;
		UINT posX, posY;
	};

	class Window : public WF::IMessageReciver {
		public:
			Window(LPCWSTR name, UINT x, UINT y, UINT sx, UINT sy);
			~Window();

			// Message looping
			BOOL runMessageLoop();
			LRESULT processMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// State function
			BOOL isKeyDown(UCHAR keyCode);
			BOOL isInFocus();
			WF::MouseState getMouseState();

			// Window function
			HWND getHandle();
			BOOL setVisible(BOOL visible);

		private:
			HWND m_hwnd;
			MSG m_msg = {};

			BOOL m_bInFocus = FALSE;
			std::bitset<256> m_keyState;
			WF::MouseState m_mouseState = {};
	};
}