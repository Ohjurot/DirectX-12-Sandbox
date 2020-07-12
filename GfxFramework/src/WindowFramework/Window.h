#pragma once

#include <WindowFramework/IMessageReciver.h>
#include <WindowFramework/WindowProc.h>

namespace WF {
	class Window : public WF::IMessageReciver {
		public:
			Window(LPCWSTR name, UINT x, UINT y, UINT sx, UINT sy);
			~Window();

			// Message looping
			BOOL runMessageLoop();
			LRESULT processMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// Window function
			BOOL setVisible(BOOL visible);

		private:
			HWND m_hwnd;
			MSG m_msg = {};
	};
}