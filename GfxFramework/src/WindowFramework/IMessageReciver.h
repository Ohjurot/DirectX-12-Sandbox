#pragma once

#include <Windows.h>

namespace WF{
	class IMessageReciver {
		public:
			virtual LRESULT processMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	};
}