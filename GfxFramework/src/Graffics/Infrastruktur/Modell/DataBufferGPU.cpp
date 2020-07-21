#include "DataBufferGPU.h"

IF3D12::GPUDataBuffer::GPUDataBuffer(){
    m_ptrBuffer = NULL;
    m_ptrD3DBuffer = NULL;
}

IF3D12::GPUDataBuffer::GPUDataBuffer(IF3D12::CPUDataBuffer* ptrTargetBuffer){
	m_ptrBuffer = ptrTargetBuffer;
	m_ptrD3DBuffer = NULL;
	m_uploadBuffer = D3D::VariableUploadBuffer();

	m_uiLastSize = 0;
}

IF3D12::GPUDataBuffer::~GPUDataBuffer(){
    preDestructDestroy();
}

HRESULT IF3D12::GPUDataBuffer::update(D3D::Device* ptrDevice){
    HRESULT hr;

    // Check buffer
    if (!m_ptrBuffer) {
        return ERROR_INSUFFICIENT_BUFFER;
    }
    
    // Check if buffer need receration
	if (!m_ptrD3DBuffer || m_uiLastSize != m_ptrBuffer->getSize()) {
		COM_RELEASE(m_ptrD3DBuffer);

        // Describe heap
        D3D12_HEAP_PROPERTIES bufferHeapProp;
        ZeroMemory(&bufferHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
        bufferHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
        bufferHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        bufferHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        bufferHeapProp.CreationNodeMask = NULL;
        bufferHeapProp.VisibleNodeMask = NULL;

        // Describe Resource
        D3D12_RESOURCE_DESC bufferDesk;
        ZeroMemory(&bufferDesk, sizeof(D3D12_RESOURCE_DESC));
        bufferDesk.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesk.Width = m_ptrBuffer->getSize();
        bufferDesk.Height = 1;
        bufferDesk.DepthOrArraySize = 1;
        bufferDesk.MipLevels = 1;
        bufferDesk.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesk.SampleDesc.Count = 1;
        bufferDesk.SampleDesc.Quality = 0;
        bufferDesk.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesk.Flags = D3D12_RESOURCE_FLAG_NONE;

        // Create Vertex buffer
        COM_CREATE_HR_RETURN(hr, ptrDevice->getDevice()->CreateCommittedResource(
            &bufferHeapProp,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesk,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            NULL,
            IID_PPV_ARGS(&m_ptrD3DBuffer)
        ));

        m_uiLastSize = m_ptrBuffer->getSize();
	}

    // Upload Data
    return m_uploadBuffer.setBuffer(ptrDevice, m_ptrD3DBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, m_ptrBuffer->getBuffer(), 0, m_ptrBuffer->getSize());
}

VOID IF3D12::GPUDataBuffer::preDestructDestroy(){
    COM_RELEASE(m_ptrD3DBuffer);
}

D3D12_GPU_VIRTUAL_ADDRESS IF3D12::GPUDataBuffer::getGPUVirtualAddress(){
    if (m_ptrD3DBuffer) {
        return m_ptrD3DBuffer->GetGPUVirtualAddress();
    }

    return NULL;
}
