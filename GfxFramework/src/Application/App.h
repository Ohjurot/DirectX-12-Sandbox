#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <WindowFramework/Window.h>

namespace Application {
	class GfxApp {
		public:
			virtual HRESULT Init(WF::Window* ptrAppWindow, UINT width, UINT height) = 0;
			virtual HRESULT Loop() = 0;
			virtual HRESULT Shutdown() = 0;
	};
}