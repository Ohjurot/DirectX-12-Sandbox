#pragma once

#include <Windows.h>
#include <WindowFramework/IMessageReciver.h>

namespace WF {
	class WindowProc {
		public:
			static LPCWSTR getClassName();

			static LRESULT CALLBACK MessageProcSetup(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK MessageProcRun(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

		private:
			WindowProc();
			~WindowProc();

			HINSTANCE m_hInstance;

			static WindowProc s_instance;
			static LPCWSTR c_className;
	};
}