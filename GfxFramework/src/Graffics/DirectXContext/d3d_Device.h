#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <d3d12sdklayers.h>

#include <cassert>

#include <Graffics/gfxCommon.h>

namespace D3D {
	class Device {
		public:
			// Construct and destruct
			Device();
			~Device();

			// Init and shutdown
			HRESULT Init(IDXGIAdapter* ptrAdapter = NULL);
			VOID Shutdown();

			// Raw access
			ID3D12Device* getDevice();
			ID3D12CommandQueue* getCommandQue();
			ID3D12GraphicsCommandList* getCommandList();

			// Heap sizes
			UINT getHeapSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

			// Ordered access
			BOOL dispatchCommandList();
			VOID waitForCommandListAndRest(ID3D12PipelineState* ptrPso = NULL);

		private:
			// Device
			ID3D12Device* m_ptrDevice = NULL;

			// Fence
			ID3D12Fence* m_ptrFence = NULL;
			UINT m_uiFenceVal = 0;

			// Command stuff
			ID3D12CommandQueue* m_ptrCmdQue = NULL;
			ID3D12CommandAllocator* m_ptrCmdAlloc = NULL;
			ID3D12GraphicsCommandList* m_ptrCmdList = NULL;

			// Desc Heap size
			UINT m_uiDescHeapSizeRTV = 0;
			UINT m_uiDescHeapSizeDSV = 0;
			UINT m_uiDescHeapSizeCBV_SRV_UAV = 0;
			UINT m_uiDescHeapSizeSAMPLER = 0;

			// Debug
			#ifdef _DEBUG
			ID3D12Debug* m_ptrDebug = NULL;
			ID3D12DebugDevice* m_ptrDebugDevice = NULL;
			#endif
	};
}