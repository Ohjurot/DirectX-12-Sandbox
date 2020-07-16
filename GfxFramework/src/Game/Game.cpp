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

    // Init Const Buffer
    m_cpuConstBuffer.matView = DirectX::XMMatrixScaling(((float)height / (float)width), 1.0f, 1.0f);
    m_cpuConstBuffer.matTransform = DirectX::XMMatrixIdentity();
    
    // == Load Vertex shader
    // From file
    HANDLE hFVertex = CreateFile(L"./SimpleVS.cso", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFVertex == INVALID_HANDLE_VALUE) {
        return ERROR_FILE_NOT_FOUND;
    }
    DWORD hFLen = GetFileSize(hFVertex, NULL);
    SetFilePointer(hFVertex, NULL, NULL, FILE_BEGIN);
    COM_CREATE_HR_RETURN(hr, D3DCreateBlob(hFLen, &m_ptrBlbVertex));
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
    COM_CREATE_HR_RETURN(hr, D3DCreateBlob(hFLenP, &m_ptrBlbPixel));
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
    COM_CREATE_HR_RETURN(hr, D3DCreateBlob(hFLenR, &sig));
    ReadFile(hFRoot, sig->GetBufferPointer(), hFLenR, NULL, NULL);
    CloseHandle(hFRoot);

    // Create
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateRootSignature(NULL, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&m_ptrRootSig)));
    COM_RELEASE(sig);

    // == Create Vertex buffer
    // Describe heap
    D3D12_HEAP_PROPERTIES vbHeapProp;
    ZeroMemory(&vbHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    vbHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    vbHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    vbHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    vbHeapProp.CreationNodeMask = NULL;
    vbHeapProp.VisibleNodeMask = NULL;

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
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateCommittedResource(
        &vbHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vbDesk,
        D3D12_RESOURCE_STATE_COPY_DEST,
        NULL,
        IID_PPV_ARGS(&m_ptrVertexBuffer)
    ));

    // == Create upload heap ==
    // Describe heap
    D3D12_HEAP_PROPERTIES vbUploadHeapProp;
    ZeroMemory(&vbUploadHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    vbUploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
    vbUploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    vbUploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    vbUploadHeapProp.CreationNodeMask = NULL;
    vbUploadHeapProp.VisibleNodeMask = NULL;

    // Create Upload buffer
    ID3D12Resource* ptrUploadBuffer = NULL;
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateCommittedResource(
        &vbUploadHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vbDesk,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        IID_PPV_ARGS(&ptrUploadBuffer)
    ));

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

    // == Create Constant buffer
    // Describe heap
    D3D12_HEAP_PROPERTIES cbHeapProp;
    ZeroMemory(&vbHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    cbHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    cbHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    cbHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    cbHeapProp.CreationNodeMask = NULL;
    cbHeapProp.VisibleNodeMask = NULL;

    // Describe Resource
    D3D12_RESOURCE_DESC cbDesk;
    ZeroMemory(&cbDesk, sizeof(D3D12_RESOURCE_DESC));
    cbDesk.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    cbDesk.Width = sizeof(CBuffer);
    cbDesk.Height = 1;
    cbDesk.DepthOrArraySize = 1;
    cbDesk.MipLevels = 1;
    cbDesk.Format = DXGI_FORMAT_UNKNOWN;
    cbDesk.SampleDesc.Count = 1;
    cbDesk.SampleDesc.Quality = 0;
    cbDesk.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    cbDesk.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create const buffer
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateCommittedResource(
        &cbHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &cbDesk,
        D3D12_RESOURCE_STATE_COPY_DEST,
        NULL,
        IID_PPV_ARGS(&m_ptrConstBuffer)
    ));

    // Describe view
    m_constBufferView.BufferLocation = m_ptrConstBuffer->GetGPUVirtualAddress();
    m_constBufferView.SizeInBytes = sizeof(CBuffer);

    // == Create upload heap ==
    // Create Upload buffer
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateCommittedResource(
        &vbUploadHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &cbDesk,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        IID_PPV_ARGS(&ptrUploadBuffer)
    ));

    // == Upload Const Data
    // Map buffer to system memory
    ptrUploadBuffer->Map(0, NULL, &mem);

    // Copy to maped region
    memcpy(mem, &m_cpuConstBuffer, sizeof(CBuffer));

    // Unmap
    ptrUploadBuffer->Unmap(0, NULL);
    mem = nullptr;

    // Copy buffer
    m_ptrDevice->getCommandList()->CopyBufferRegion(m_ptrConstBuffer, 0, ptrUploadBuffer, 0, sizeof(CBuffer));

    // Set resource to read state
    D3D12_RESOURCE_BARRIER baCbToRead;
    baCbToRead.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    baCbToRead.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    baCbToRead.Transition.pResource = m_ptrConstBuffer;
    baCbToRead.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    baCbToRead.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    baCbToRead.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    m_ptrDevice->getCommandList()->ResourceBarrier(1, &baCbToRead);

    // Execute
    m_ptrDevice->dispatchCommandList();
    m_ptrDevice->waitForCommandListAndReset();

    // Free
    COM_RELEASE(ptrUploadBuffer);

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
    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateGraphicsPipelineState(&psoDesk, IID_PPV_ARGS(&m_ptrPso)));

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

    // Bind constant buffer
    m_ptrDevice->getCommandList()->SetGraphicsRootConstantBufferView(0, m_constBufferView.BufferLocation);

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

    // Update CBuffer
    static float angel = 0.0f;
    if (m_ptrWindow->isKeyDown(VK_SPACE)) {
        angel += 0.01f;

        if (angel > DirectX::XM_PI * 2) angel -= DirectX::XM_PI * 2;

        m_cpuConstBuffer.matTransform = DirectX::XMMatrixRotationZ(angel);

        // Update buffer on gpu
        // Set resource to read state
        D3D12_RESOURCE_BARRIER cbReadToCopy;
        cbReadToCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        cbReadToCopy.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        cbReadToCopy.Transition.pResource = m_ptrConstBuffer;
        cbReadToCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cbReadToCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
        cbReadToCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        m_ptrDevice->getCommandList()->ResourceBarrier(1, &cbReadToCopy);

        // Create Upload heap
        // Describe heap
        D3D12_HEAP_PROPERTIES uplHeapProp;
        ZeroMemory(&uplHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
        uplHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
        uplHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uplHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uplHeapProp.CreationNodeMask = NULL;
        uplHeapProp.VisibleNodeMask = NULL;

        // Describe Resource
        D3D12_RESOURCE_DESC uplHeapDesc;
        ZeroMemory(&uplHeapDesc, sizeof(D3D12_RESOURCE_DESC));
        uplHeapDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uplHeapDesc.Width = sizeof(CBuffer);
        uplHeapDesc.Height = 1;
        uplHeapDesc.DepthOrArraySize = 1;
        uplHeapDesc.MipLevels = 1;
        uplHeapDesc.Format = DXGI_FORMAT_UNKNOWN;
        uplHeapDesc.SampleDesc.Count = 1;
        uplHeapDesc.SampleDesc.Quality = 0;
        uplHeapDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        uplHeapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        // Create Vertex buffer
        ID3D12Resource* ptrUpload = NULL;
        m_ptrDevice->getDevice()->CreateCommittedResource(
            &uplHeapProp,
            D3D12_HEAP_FLAG_NONE,
            &uplHeapDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            NULL,
            IID_PPV_ARGS(&ptrUpload)
        );

        // Copy to buffer
        void* mem;
        ptrUpload->Map(0, NULL, &mem);
        memcpy(mem, &m_cpuConstBuffer, sizeof(CBuffer));
        ptrUpload->Unmap(0, NULL);

        // Copy
        m_ptrDevice->getCommandList()->CopyBufferRegion(m_ptrConstBuffer, 0, ptrUpload, 0, sizeof(CBuffer));

        m_ptrDevice->dispatchCommandList();
        m_ptrDevice->waitForCommandListAndReset();

        COM_RELEASE(ptrUpload);

        // Set resource to read state
        D3D12_RESOURCE_BARRIER cbReadToRead;
        cbReadToRead.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        cbReadToRead.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        cbReadToRead.Transition.pResource = m_ptrConstBuffer;
        cbReadToRead.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cbReadToRead.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        cbReadToRead.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        m_ptrDevice->getCommandList()->ResourceBarrier(1, &cbReadToRead);
    }

   

    return S_OK;
}

HRESULT MY::Game::Shutdown(){
    // === VIDEO 4 ===
    COM_RELEASE(m_ptrConstBuffer);
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
