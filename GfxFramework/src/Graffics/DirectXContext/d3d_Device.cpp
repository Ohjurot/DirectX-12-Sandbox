#include "d3d_Device.h"

D3D::Device::Device(){
	
}

D3D::Device::~Device(){
	assert("Object destroyed bevor Shutdown() was called" && (m_ptrDevice == NULL));
}

HRESULT D3D::Device::Init(IDXGIAdapter* ptrAdapter){
	// Store HRESULT
	HRESULT hr;
	
	// Create Debug
	#ifdef _DEBUG
		COM_SAFE_CREATE_HR_RETURN(hr, D3D12GetDebugInterface(IID_PPV_ARGS(&m_ptrDebug)));
		m_ptrDebug->EnableDebugLayer();
	#endif

	// Create Device
	COM_SAFE_CREATE_HR_RETURN(hr, D3D12CreateDevice(ptrAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_ptrDevice)));

	// Query debug device
	#ifdef _DEBUG
		m_ptrDevice->QueryInterface(IID_PPV_ARGS(&m_ptrDebugDevice));
	#endif

	// Create Fence
	COM_SAFE_CREATE_HR_RETURN(hr, m_ptrDevice->CreateFence(
		m_uiFenceVal,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_ptrFence)
	));

	// Command Que
	D3D12_COMMAND_QUEUE_DESC queDesc;
	queDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queDesc.NodeMask = NULL;
	COM_SAFE_CREATE_HR_RETURN(hr, m_ptrDevice->CreateCommandQueue(&queDesc, IID_PPV_ARGS(&m_ptrCmdQue)));

	
	for (UINT i = 0; i < 2; i++) {
		// Command alloc
		COM_SAFE_CREATE_HR_RETURN(hr, m_ptrDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ptrCmdAlloc[i])));

		// Command list
		COM_SAFE_CREATE_HR_RETURN(hr, m_ptrDevice->CreateCommandList(
			NULL,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_ptrCmdAlloc[i],
			NULL,
			IID_PPV_ARGS(&m_ptrCmdList[i])
		));
	}

	// Read heap sizes
	m_uiDescHeapSizeRTV = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_uiDescHeapSizeDSV = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_uiDescHeapSizeCBV_SRV_UAV = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_uiDescHeapSizeSAMPLER = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	// Return final S_OK
	return S_OK;
}

VOID D3D::Device::Shutdown(){
	// Command
	for (UINT i = 0; i < 2; i++) {
		COM_RELEASE(m_ptrCmdList[i]);
		COM_RELEASE(m_ptrCmdAlloc[i]);
	}
	COM_RELEASE(m_ptrCmdQue);
	COM_RELEASE(m_ptrFence);

	// Device
	COM_RELEASE(m_ptrDevice);
		
	// Debug
	#ifdef _DEBUG
		COM_RELEASE(m_ptrDebug);
		m_ptrDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
		COM_RELEASE(m_ptrDebugDevice);
	#endif
}

ID3D12Device* D3D::Device::getDevice(){
	return m_ptrDevice;
}

ID3D12CommandQueue* D3D::Device::getCommandQue(){
	return m_ptrCmdQue;
}

ID3D12GraphicsCommandList* D3D::Device::getCommandList(){
	return m_ptrCmdList[m_uiCurrentList];
}

UINT D3D::Device::getHeapSize(D3D12_DESCRIPTOR_HEAP_TYPE type){
	switch (type) {
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return m_uiDescHeapSizeCBV_SRV_UAV;

		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return m_uiDescHeapSizeDSV;

		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return m_uiDescHeapSizeRTV;

		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			return m_uiDescHeapSizeSAMPLER;

		default:
			return 0;
	}
}

BOOL D3D::Device::dispatchCommandList(){
	// Check if command list is bussy and return false if it is
	if (m_uiFenceVal > 1 + m_ptrFence->GetCompletedValue()) {
		return FALSE;
	}

	// Close command list
	m_ptrCmdList[m_uiCurrentList]->Close();

	// Que into array
	ID3D12CommandList* arr[] = {
		m_ptrCmdList[m_uiCurrentList]
	};

	// Execute command list
	m_ptrCmdQue->ExecuteCommandLists(1, arr);

	// Increment and signal fence
	m_uiFenceVal++;
	m_ptrCmdQue->Signal(m_ptrFence, m_uiFenceVal);

	// Swap lists
	m_uiCurrentList = (m_uiCurrentList + 1) % 2;

	return TRUE;
}

VOID D3D::Device::waitForCommandListAndReset(ID3D12PipelineState* ptrPso){
	// Check if windows event hanlde is required (que still running)
	if (m_ptrFence->GetCompletedValue() < m_uiFenceVal - 1) {
		// Create event handle
		HANDLE hEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
		
		// Assign handle to fence
		m_ptrFence->SetEventOnCompletion(m_uiFenceVal, hEvent);
		
		// Wait for event completion
		WaitForSingleObject(hEvent, INFINITE);

		// Close handle to event
		CloseHandle(hEvent);
	}

	// Reset allocator and list
	m_ptrCmdAlloc[m_uiCurrentList]->Reset();
	m_ptrCmdList[m_uiCurrentList]->Reset(m_ptrCmdAlloc[m_uiCurrentList], ptrPso);
}
