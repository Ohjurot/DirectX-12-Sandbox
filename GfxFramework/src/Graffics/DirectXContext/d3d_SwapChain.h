#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <cassert>

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace D3D {
	class SwapChain {
		public:
			// Construct / Destrucy
			SwapChain(UINT width, UINT height, HWND windowHandle);
			~SwapChain();

			// Init and shutdown
			HRESULT Init(D3D::Device* ptrD3DDevice);
			VOID Shutdown();

			// Function
			VOID beginFrame(D3D::Device* ptrD3DDevice, FLOAT clearColor[4]);
			VOID endFrame(D3D::Device* ptrD3DDevice);
			HRESULT present();

		private:
			UINT m_uiWidth, m_uiHeight;
			HWND m_targetWindowHandle;

			IDXGIFactory* m_TptrFactory = NULL;

			IDXGISwapChain* m_ptrSwapChain = NULL;
			ID3D12DescriptorHeap* m_ptrRtvDescHeap = NULL;
			ID3D12Resource* m_ptrsBackBuffers[2] = {NULL, NULL};

			UINT m_uiBackBufferIndex = 0;
	};
}