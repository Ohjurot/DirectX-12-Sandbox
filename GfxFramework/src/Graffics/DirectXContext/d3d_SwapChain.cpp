#include "d3d_SwapChain.h"

D3D::SwapChain::SwapChain(UINT width, UINT height, HWND windowHandle){
	// Copy values
	m_uiWidth = width;
	m_uiHeight = height;
	m_targetWindowHandle = windowHandle;
}

D3D::SwapChain::~SwapChain(){
	assert("Object destroyed bevor Shutdown() was called" && (m_ptrSwapChain == NULL));
}

HRESULT D3D::SwapChain::Init(D3D::Device* ptrD3DDevice){
	// Result HRESULT
	HRESULT hr;

	// Create Facotry
	COM_SAFE_CREATE_HR_RETURN(hr, CreateDXGIFactory(IID_PPV_ARGS(&m_TptrFactory)));

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferDesc.Width = m_uiWidth;
	swapDesc.BufferDesc.Height = m_uiHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.SampleDesc.Count = 1;		// Not used in D3D12
	swapDesc.SampleDesc.Quality = 0;	// Not used in D3D12
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = m_targetWindowHandle;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create swap chain
	COM_SAFE_CREATE_HR_RETURN(hr, m_TptrFactory->CreateSwapChain(ptrD3DDevice->getCommandQue(), &swapDesc, &m_ptrSwapChain));

	// Descript RTV heap type for swap chain buffers
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;

	// Create RTV heap for buffers
	COM_SAFE_CREATE_HR_RETURN(hr, ptrD3DDevice->getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_ptrRtvDescHeap)));

	// Create back buffers
	D3D12_CPU_DESCRIPTOR_HANDLE heapHandle = m_ptrRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < 2; i++) {
		// Get and create rtv
		COM_SAFE_CREATE_HR_RETURN(hr, m_ptrSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ptrsBackBuffers[i])));
		ptrD3DDevice->getDevice()->CreateRenderTargetView(m_ptrsBackBuffers[i], NULL, heapHandle);

		// Move next
		heapHandle.ptr += ptrD3DDevice->getHeapSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Release facoty
	COM_RELEASE(m_TptrFactory);
	
	// Return OK
	return S_OK;
}

VOID D3D::SwapChain::Shutdown(){
	// Release back buffers
	COM_RELEASE(m_ptrsBackBuffers[0]);
	COM_RELEASE(m_ptrsBackBuffers[1]);

	// Release heap and swap chain
	COM_RELEASE(m_ptrRtvDescHeap);
	COM_RELEASE(m_ptrSwapChain);

	// Release if shutdown call while init
	COM_RELEASE(m_TptrFactory);
}

VOID D3D::SwapChain::beginFrame(D3D::Device* ptrD3DDevice, FLOAT clearColor[4]){
	// Set rtv from present state to render target state
	D3D12_RESOURCE_BARRIER baPresentToRtv;
	baPresentToRtv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	baPresentToRtv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	baPresentToRtv.Transition.pResource = m_ptrsBackBuffers[m_uiBackBufferIndex];
	baPresentToRtv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	baPresentToRtv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	baPresentToRtv.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ptrD3DDevice->getCommandList()->ResourceBarrier(1, &baPresentToRtv);

	// Get handle to current rtv desc
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_ptrRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += (SIZE_T)ptrD3DDevice->getHeapSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * m_uiBackBufferIndex;

	// Clear rtv with colid color
	ptrD3DDevice->getCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);

	// Set as render target
	ptrD3DDevice->getCommandList()->OMSetRenderTargets(1, &rtvHandle, TRUE, NULL);
}

VOID D3D::SwapChain::endFrame(D3D::Device* ptrD3DDevice){
	// Set rtv from render target state to present state
	D3D12_RESOURCE_BARRIER baRtvToPresent;
	baRtvToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	baRtvToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	baRtvToPresent.Transition.pResource = m_ptrsBackBuffers[m_uiBackBufferIndex];
	baRtvToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	baRtvToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	baRtvToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	ptrD3DDevice->getCommandList()->ResourceBarrier(1, &baRtvToPresent);
}

HRESULT D3D::SwapChain::present(){
	// Swap buffer index
	m_uiBackBufferIndex = (m_uiBackBufferIndex + 1) % 2;
	// Present and return result
	return m_ptrSwapChain->Present(0, 0);
}
