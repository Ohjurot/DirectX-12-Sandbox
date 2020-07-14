#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <Application/App.h>
#include <Graffics/DirectXContext/d3d_Device.h>
#include <Graffics/DirectXContext/d3d_SwapChain.h>
#include <Graffics/DirectXContext/d3d_View.h>

namespace MY {
	class Game : public Application::GfxApp {
		public:
			HRESULT Init(WF::Window* ptrAppWindow, UINT width, UINT height);
			HRESULT Loop();
			HRESULT Shutdown();

		private:
			WF::Window* m_ptrWindow;

			D3D::Device* m_ptrDevice;
			D3D::SwapChain* m_ptrSwapChain;
			D3D::View* m_ptrView;
	};
}