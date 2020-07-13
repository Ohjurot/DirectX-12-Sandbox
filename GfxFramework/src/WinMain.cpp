#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <WindowFramework/WindowProc.h>
#include <WindowFramework/Window.h>

#define COM_RELEASE(ptrCom) if(ptrCom) {ptrCom->Release(); ptrCom = nullptr;}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// == Create Window ==

	// Windth and Height constants
	const UINT height = 800;
	const UINT width = 800;
	
	// Create window
	WF::Window wnd(L"GfxFramework", 100, 100, width, height);
	wnd.setVisible(TRUE);

	// == Create DirectX Context ==

	// Debug and debug device
#ifdef _DEBUG
	ID3D12Debug* ptrDebug = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&ptrDebug));
	ptrDebug->EnableDebugLayer();
#endif

	// Factory
	IDXGIFactory* ptrFactory = nullptr;
	CreateDXGIFactory(IID_PPV_ARGS(&ptrFactory));

	// Device
	ID3D12Device* ptrDevice = nullptr;
	D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&ptrDevice));

	// Debug device
#ifdef _DEBUG
	ID3D12DebugDevice* ptrDebugDevice = nullptr;
	ptrDevice->QueryInterface(IID_PPV_ARGS(&ptrDebugDevice));
#endif

	// Fence
	ID3D12Fence* ptrFence = nullptr;
	UINT uiFence = 0;
	ptrDevice->CreateFence(
		uiFence,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&ptrFence)
	);

	// Command Que -> Allocator -> List
	ID3D12CommandQueue*			ptrCmdQue = nullptr;
	ID3D12CommandAllocator*		ptrCmdAlloc = nullptr;
	ID3D12GraphicsCommandList*	ptrCmdList = nullptr;

	// Create Cmd Que
	D3D12_COMMAND_QUEUE_DESC queDesc;
	queDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queDesc.NodeMask = NULL;
	ptrDevice->CreateCommandQueue(&queDesc, IID_PPV_ARGS(&ptrCmdQue));

	// Create Cmd alloc
	ptrDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&ptrCmdAlloc));

	// Create Cmd list
	ptrDevice->CreateCommandList(
		NULL,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		ptrCmdAlloc,
		NULL,
		IID_PPV_ARGS(&ptrCmdList)
	);

	// Swap chain
	IDXGISwapChain* ptrSwapChain = nullptr;

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.SampleDesc.Count = 1;		// Not used in D3D12
	swapDesc.SampleDesc.Quality = 0;	// Not used in D3D12
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = wnd.getHandle();
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create swap chain
	ptrFactory->CreateSwapChain(ptrCmdQue, &swapDesc, &ptrSwapChain);

	// Read Descriptor size
	UINT uiDescHeapSizeRTV = ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	UINT uiDescHeapSizeDSV = ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	UINT uiDescHeapSizeCBV_SRV_UAV = ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT uiDescHeapSizeSAMPLER = ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	// RTV Desc Heap
	ID3D12DescriptorHeap* ptrDescHeapRtv = nullptr;

	// Descript RTV heap type
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;

	// Create RTV heap
	ptrDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&ptrDescHeapRtv));

	// Create back buffers
	ID3D12Resource* ptrsBackBuffers[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = ptrDescHeapRtv->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < 2; i++) {
		// Fallback null
		ptrsBackBuffers[i] = nullptr;
		
		// Get and create rtv
		ptrSwapChain->GetBuffer(i, IID_PPV_ARGS(&ptrsBackBuffers[i]));
		ptrDevice->CreateRenderTargetView(ptrsBackBuffers[i], NULL, rtvHeapHandle);
		
		// Move next
		rtvHeapHandle.ptr += uiDescHeapSizeRTV;
	}

	// Create view port
	D3D12_VIEWPORT viewPort;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = (FLOAT)width;
	viewPort.Height = (FLOAT)height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	// Create sicissor rect
	RECT sicRect = {0, 0, width, height};
	
	// Back buffer index
	UINT bufferIndex = 0;

	//  == Application Loop ==
	while (wnd.runMessageLoop()) {
		// Execute commands
		ptrCmdList->Close();
		ID3D12CommandList* arr[] = {
			ptrCmdList
		};
		ptrCmdQue->ExecuteCommandLists(1, arr);
		uiFence++;
		ptrCmdQue->Signal(ptrFence, uiFence);

		// Wait for command execution
		if (ptrFence->GetCompletedValue() < uiFence) {
			HANDLE hEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
			ptrFence->SetEventOnCompletion(uiFence, hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}

		// Reset allocator and list
		ptrCmdAlloc->Reset();
		ptrCmdList->Reset(ptrCmdAlloc, NULL);
		
		// Present buffers (If failed exit application)
		if (FAILED(ptrSwapChain->Present(0, 0))) {
			break;
		}

		// Move buffer Index
		bufferIndex = (bufferIndex + 1) % 2;

		// Set rasterizer objects
		ptrCmdList->RSSetViewports(1, &viewPort);
		ptrCmdList->RSSetScissorRects(1, &sicRect);

		// Set rtv from present state to render target state
		D3D12_RESOURCE_BARRIER baPresentToRtv;
		baPresentToRtv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		baPresentToRtv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		baPresentToRtv.Transition.pResource = ptrsBackBuffers[bufferIndex];
		baPresentToRtv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		baPresentToRtv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		baPresentToRtv.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ptrCmdList->ResourceBarrier(1, &baPresentToRtv);
		
		// Get handle to current rtv desc
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = ptrDescHeapRtv->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += (SIZE_T)uiDescHeapSizeRTV * bufferIndex;

		// Clear rtv with colid color
		static FLOAT m_clearColor[4] = { 0.388f, 0.733f, 0.949f, 1.0f };
		ptrCmdList->ClearRenderTargetView(rtvHandle, m_clearColor, 0, NULL);

		// Set render target to output merger
		ptrCmdList->OMSetRenderTargets(1, &rtvHandle, TRUE, NULL);
		
		// TODO: Render Loop


		// End frame
		D3D12_RESOURCE_BARRIER baRtvToPresent;
		baRtvToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		baRtvToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		baRtvToPresent.Transition.pResource = ptrsBackBuffers[bufferIndex];
		baRtvToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		baRtvToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		baRtvToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		ptrCmdList->ResourceBarrier(1, &baRtvToPresent);
	}



	// == Release DirectX context
	for (UINT i = 0; i < 2; i++) {
		COM_RELEASE(ptrsBackBuffers[i]);
	}
	COM_RELEASE(ptrDescHeapRtv);
	COM_RELEASE(ptrSwapChain);
	COM_RELEASE(ptrCmdList);
	COM_RELEASE(ptrCmdAlloc);
	COM_RELEASE(ptrCmdQue);
	COM_RELEASE(ptrFence);
	COM_RELEASE(ptrDevice);
	COM_RELEASE(ptrFactory);
	
	// Debug output and release
#ifdef _DEBUG
	COM_RELEASE(ptrDebug);
	ptrDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
	COM_RELEASE(ptrDebugDevice);
#endif

	// Return zero
	return 0;
}