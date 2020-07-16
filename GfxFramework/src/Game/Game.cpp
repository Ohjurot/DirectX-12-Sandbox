#include "Game.h"

HRESULT MY::Game::Init(WF::Window* ptrAppWindow, UINT width, UINT height){
    HRESULT hr;
    
    // Store window pointer
    m_ptrWindow = ptrAppWindow;

    // Init D3D
    m_ptrDevice = new D3D::Device();
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->Init(NULL));

    // Init swap chain
    m_ptrSwapChain = new D3D::SwapChain(width, height, ptrAppWindow->getHandle());
    COM_CREATE_HR_RETURN(hr, m_ptrSwapChain->Init(m_ptrDevice));

    // Init view
    m_ptrView = new D3D::View(width, height);

    
    // === VIDEO 4 ===

    // == Load Vertex shader
    // From file
    HANDLE hFVertex = CreateFile(L"./SimpleVS.cso", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFVertex == INVALID_HANDLE_VALUE) {
        return ERROR_FILE_NOT_FOUND;
    }
    DWORD hFLen = GetFileSize(hFVertex, NULL);
    SetFilePointer(hFVertex, NULL, NULL, FILE_BEGIN);
    D3DCreateBlob(hFLen, &m_ptrBlbVertex);
    ReadFile(hFVertex, m_ptrBlbVertex->GetBufferPointer(), hFLen, NULL, NULL);
    CloseHandle(hFVertex);

    // == Load Pixel shader
    // From File
    HANDLE hFPixel = CreateFile(L"./SimplePS.cso", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFVertex == INVALID_HANDLE_VALUE) {
        return ERROR_FILE_NOT_FOUND;
    }
    DWORD hFLenP = GetFileSize(hFPixel, NULL);
    SetFilePointer(hFPixel, NULL, NULL, FILE_BEGIN);
    D3DCreateBlob(hFLenP, &m_ptrBlbPixel);
    ReadFile(hFPixel, m_ptrBlbPixel->GetBufferPointer(), hFLenP, NULL, NULL);
    CloseHandle(hFPixel);

    // == Load Root signature
    // From File
    ID3D10Blob* sig = NULL;
    HANDLE hFRoot = CreateFile(L"./RootSignature.cso", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFVertex == INVALID_HANDLE_VALUE) {
        return ERROR_FILE_NOT_FOUND;
    }
    DWORD hFLenR = GetFileSize(hFRoot, NULL);
    SetFilePointer(hFRoot, NULL, NULL, FILE_BEGIN);
    D3DCreateBlob(hFLenR, &sig);
    ReadFile(hFRoot, sig->GetBufferPointer(), hFLenR, NULL, NULL);
    CloseHandle(hFRoot);

    // Create
    hr = m_ptrDevice->getDevice()->CreateRootSignature(NULL, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&m_ptrRootSig));
    COM_RELEASE(sig);

    // == Create Vertex buffer
    // Describe heap
    D3D12_HEAP_PROPERTIES vbHeapProp;
    ZeroMemory(&vbHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    vbHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    vbHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    vbHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    // Describe Resource
    D3D12_RESOURCE_DESC vbDesk;
    ZeroMemory(&vbDesk, sizeof(D3D12_RESOURCE_DESC));
    vbDesk.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vbDesk.Width = (sizeof(Vertex) * 3);
    vbDesk.Height = 1;
    vbDesk.DepthOrArraySize = 1;
    vbDesk.MipLevels = 1;
    vbDesk.Format = DXGI_FORMAT_UNKNOWN;
    vbDesk.SampleDesc.Count = 1;
    vbDesk.SampleDesc.Quality = 0;
    vbDesk.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    vbDesk.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create Vertex buffer
    m_ptrDevice->getDevice()->CreateCommittedResource(
        &vbHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vbDesk,
        D3D12_RESOURCE_STATE_COPY_DEST,
        NULL,
        IID_PPV_ARGS(&m_ptrVertexBuffer)
    );

    // == Create upload heap ==
    // Describe heap
    D3D12_HEAP_PROPERTIES vbUploadHeapProp;
    ZeroMemory(&vbUploadHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    vbUploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
    vbUploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    vbUploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    // Create Upload buffer
    ID3D12Resource* ptrUploadBuffer = NULL;
    m_ptrDevice->getDevice()->CreateCommittedResource(
        &vbUploadHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vbDesk,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        IID_PPV_ARGS(&ptrUploadBuffer)
    );

    // == Upload Vertext Data
    // Map buffer to system memory
    void* mem = nullptr;
    ptrUploadBuffer->Map(0, NULL, &mem);

    // Copy to maped region
    memcpy(mem, c_cpuBuffer, sizeof(Vertex) * 3);

    // Unmap
    ptrUploadBuffer->Unmap(0, NULL);
    mem = nullptr;

    // Copy buffer
    m_ptrDevice->getCommandList()->CopyBufferRegion(m_ptrVertexBuffer, 0, ptrUploadBuffer, 0, sizeof(Vertex) * 3);
    
    // Set resource to read state
    D3D12_RESOURCE_BARRIER baToRead;
    baToRead.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    baToRead.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    baToRead.Transition.pResource = m_ptrVertexBuffer;
    baToRead.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    baToRead.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    baToRead.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    m_ptrDevice->getCommandList()->ResourceBarrier(1, &baToRead);
    
    // Execute
    m_ptrDevice->dispatchCommandList();
    m_ptrDevice->waitForCommandListAndReset();

    // Free upload buffer
    COM_RELEASE(ptrUploadBuffer);

    // == Create Vertex buffer view
    ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    m_vertexBufferView.BufferLocation = m_ptrVertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = sizeof(Vertex) * 3;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);

    // == Create PSO
    
    // Describe rasterizer
    D3D12_RASTERIZER_DESC rasterizerDesk;
    ZeroMemory(&rasterizerDesk, sizeof(D3D12_RASTERIZER_DESC));

    rasterizerDesk.FillMode = D3D12_FILL_MODE_SOLID; 
    rasterizerDesk.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesk.DepthClipEnable = FALSE;
    rasterizerDesk.FrontCounterClockwise = FALSE;
    rasterizerDesk.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesk.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesk.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesk.MultisampleEnable = FALSE;
    rasterizerDesk.AntialiasedLineEnable = FALSE;
    rasterizerDesk.ForcedSampleCount = 0;
    rasterizerDesk.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // Describe PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesk;
    ZeroMemory(&psoDesk, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    psoDesk.pRootSignature = m_ptrRootSig;
    psoDesk.InputLayout.NumElements = 2;
    psoDesk.InputLayout.pInputElementDescs = m_inputDescVertex;
    psoDesk.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesk.VS.BytecodeLength = m_ptrBlbVertex->GetBufferSize(); 
    psoDesk.VS.pShaderBytecode = m_ptrBlbVertex->GetBufferPointer();
    psoDesk.PS.BytecodeLength = m_ptrBlbPixel->GetBufferSize();
    psoDesk.PS.pShaderBytecode = m_ptrBlbPixel->GetBufferPointer();
    psoDesk.RasterizerState = rasterizerDesk;
    psoDesk.BlendState.AlphaToCoverageEnable = FALSE;
    psoDesk.BlendState.IndependentBlendEnable = FALSE;
    psoDesk.BlendState.RenderTarget[0] = {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    psoDesk.DepthStencilState.DepthEnable = FALSE;
    psoDesk.SampleMask = UINT_MAX;
    psoDesk.NumRenderTargets = 1;
    psoDesk.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesk.SampleDesc.Count = 1;
    psoDesk.SampleDesc.Quality = 0;

    // Create
    m_ptrDevice->getDevice()->CreateGraphicsPipelineState(&psoDesk, IID_PPV_ARGS(&m_ptrPso));

    // ===========================

    return S_OK;
}

HRESULT MY::Game::Loop(){
    // Begin frame
    static FLOAT m_clearColor[4] = { 0.388f, 0.733f, 0.949f, 1.0f };
    m_ptrSwapChain->beginFrame(m_ptrDevice, m_clearColor);
    m_ptrView->bind(m_ptrDevice);

    // === VIDEO 4 ===

    // Set PSO
    m_ptrDevice->getCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_ptrDevice->getCommandList()->SetGraphicsRootSignature(m_ptrRootSig);
    m_ptrDevice->getCommandList()->SetPipelineState(m_ptrPso);

    // Bind vertex buffer
    m_ptrDevice->getCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

    // Draw
    m_ptrDevice->getCommandList()->DrawInstanced(3, 1, 0, 0);

    // ====================

    // End frame
    m_ptrSwapChain->endFrame(m_ptrDevice);
    m_ptrDevice->dispatchCommandList();
    m_ptrDevice->waitForCommandListAndReset();
    if (FAILED(m_ptrSwapChain->present())) {
        return m_ptrDevice->getDevice()->GetDeviceRemovedReason();
    }
    return S_OK;
}

HRESULT MY::Game::Shutdown(){
    // === VIDEO 4 ===
    COM_RELEASE(m_ptrRootSig)
    COM_RELEASE(m_ptrPso);
    COM_RELEASE(m_ptrBlbPixel);
    COM_RELEASE(m_ptrBlbVertex);
    COM_RELEASE(m_ptrVertexBuffer);
    // ================

    // Shutdown members
    m_ptrSwapChain->Shutdown();
    m_ptrDevice->Shutdown();

    // Delete ptrs
    delete m_ptrView;
    delete m_ptrSwapChain;
    delete m_ptrDevice;

    return S_OK;
}