#include "Game.h"

HRESULT MY::Game::Init(WF::Window* ptrAppWindow, UINT width, UINT height){
    HRESULT hr;
    
    // == Generic init
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


    // == Init CPU Const Buffer
    m_cpuConstBuffer.matView = DirectX::XMMatrixScaling(((float)height / (float)width), 1.0f, 1.0f);
    m_cpuConstBuffer.matTransform = DirectX::XMMatrixIdentity();
    

    // == Load Shaders
    IF3D12::Shader* ptrShaderRootSig;
    UINT idVs = IF3D12::ShaderRegistry::getRegistry()->getShader(L"SimpleVS.cso");
    UINT idPs = IF3D12::ShaderRegistry::getRegistry()->getShader(L"SimplePS.cso");
    UINT idRs = IF3D12::ShaderRegistry::getRegistry()->getShader(L"RootSignature.cso");
    
    if (idVs < 0 || idPs < 0 || idRs < 0) {
        COM_CREATE_HR_RETURN(hr, ERROR_FILE_NOT_FOUND);
    }

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
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        IID_PPV_ARGS(&m_ptrVertexBuffer)
    ));

    // Copy buffer
    COM_CREATE_HR_RETURN(hr, m_uploadBuffer.setBuffer(m_ptrDevice, m_ptrVertexBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, (void*)c_cpuBuffer, 0, sizeof(Vertex) * 3));

    // Create view
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
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        IID_PPV_ARGS(&m_ptrConstBuffer)
    ));

    // Describe view
    m_constBufferView.BufferLocation = m_ptrConstBuffer->GetGPUVirtualAddress();
    m_constBufferView.SizeInBytes = sizeof(CBuffer);

    // Copy buffer
    COM_CREATE_HR_RETURN(hr, m_uploadBuffer.setBuffer(m_ptrDevice, m_ptrConstBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, &m_cpuConstBuffer, 0, sizeof(CBuffer)));

    // == Create texture
    void* memTex = nullptr;
    SIZE_T sizeBytes = 0;
    UINT texWidth, texHeight;
    DXGI_FORMAT texFormat;
    COM_CREATE_HR_RETURN(hr, Util::TexureLoader::fromFileToMemory(L"./texture.png", m_ptrDevice, &memTex, &sizeBytes, &texWidth, &texHeight, &texFormat));
    assert(memTex);

    // Heap
    D3D12_HEAP_PROPERTIES txHeapProp;
    ZeroMemory(&vbHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    txHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    txHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    txHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    txHeapProp.CreationNodeMask = NULL;
    txHeapProp.VisibleNodeMask = NULL;

    // Describe Resource
    D3D12_RESOURCE_DESC txDesk;
    ZeroMemory(&cbDesk, sizeof(D3D12_RESOURCE_DESC));
    txDesk.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    txDesk.Width = texWidth;
    txDesk.Height = texHeight;
    txDesk.Alignment = 0;
    txDesk.DepthOrArraySize = 1;
    txDesk.MipLevels = 1;
    txDesk.Format = texFormat;
    txDesk.SampleDesc.Count = 1;
    txDesk.SampleDesc.Quality = 0;
    txDesk.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    txDesk.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create texture
    hr = m_ptrDevice->getDevice()->CreateCommittedResource(
        &txHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &txDesk,
        D3D12_RESOURCE_STATE_COMMON,
        NULL,
        IID_PPV_ARGS(&m_ptrTexture)
    );

    // Copy
    D3D::TextureUploader upBuffer;
    if (!FAILED(hr)) {
        hr = upBuffer.setBuffer(m_ptrDevice, m_ptrTexture, D3D12_RESOURCE_STATE_COMMON, memTex, texWidth, texHeight, texFormat, sizeBytes);
    }
    upBuffer.preDestructDestroy();

    // Free temporary memory texture
    if(memTex) {
        free(memTex);
    }

    // Return here to make shure free(...) is called
    if (FAILED(hr)) {
        return hr;
    }

    // == SRV (For Texture)
    // Heap
    D3D12_DESCRIPTOR_HEAP_DESC hpRtHeapDesk;
    hpRtHeapDesk.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    hpRtHeapDesk.NumDescriptors = 1;
    hpRtHeapDesk.NodeMask = NULL;
    hpRtHeapDesk.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    COM_CREATE_HR_RETURN(hr, m_ptrDevice->getDevice()->CreateDescriptorHeap(&hpRtHeapDesk, IID_PPV_ARGS(&m_ptrHeapRootTable)));

    // Descriptor
    D3D12_SHADER_RESOURCE_VIEW_DESC rvDesk;
    rvDesk.Format = texFormat;
    rvDesk.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    rvDesk.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    rvDesk.Texture2D.MipLevels = 1;
    rvDesk.Texture2D.MostDetailedMip = 0;
    rvDesk.Texture2D.PlaneSlice = 0;
    rvDesk.Texture2D.ResourceMinLODClamp = 0.0f;

    m_ptrDevice->getDevice()->CreateShaderResourceView(m_ptrTexture, &rvDesk, m_ptrHeapRootTable->GetCPUDescriptorHandleForHeapStart());

    // == Create PSO
    IF3D12::DrawPipelineDescriptor descriptor;
    ZeroMemory(&descriptor, sizeof(IF3D12::DrawPipelineDescriptor));

    descriptor.cullMode = D3D12_CULL_MODE_NONE;
    descriptor.fillMode = D3D12_FILL_MODE_SOLID;
    descriptor.inputLayoutElementCount = 2;
    descriptor.ptrInputLayout = m_inputDescVertex;
    descriptor.rootSignatureId = idRs;
    descriptor.shaders.idVertex = idVs;
    descriptor.shaders.idPixel = idPs;
    COM_CREATE_HR_RETURN(hr, IF3D12::DrawPipelineDescriptor::makePso(m_ptrDevice->getDevice(), &m_ptrPso, &m_ptrRootSig, -1, &descriptor));

    return S_OK;
}

HRESULT MY::Game::Loop(){
    // ### Logic: Update CBuffer
    static float angel = 0.0f;
    if (m_ptrWindow->isKeyDown(VK_SPACE)) {
        angel += 0.005f;

        if (angel >= DirectX::XM_PI * 2) angel -= DirectX::XM_PI * 2;

        m_cpuConstBuffer.matTransform = DirectX::XMMatrixRotationZ(angel);
        m_uploadBuffer.setBuffer(m_ptrDevice, m_ptrConstBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, &m_cpuConstBuffer, 0, sizeof(CBuffer));
    }

    // ### GFX: Begin frame
    static FLOAT m_clearColor[4] = { 0.388f, 0.733f, 0.949f, 1.0f };
    m_ptrSwapChain->beginFrame(m_ptrDevice, m_clearColor);
    m_ptrView->bind(m_ptrDevice);

    // Set PSO
    m_ptrDevice->getCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_ptrDevice->getCommandList()->SetGraphicsRootSignature(m_ptrRootSig);
    m_ptrDevice->getCommandList()->SetPipelineState(m_ptrPso);

    // Bind vertex buffer
    m_ptrDevice->getCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

    // Bind constant buffer
    m_ptrDevice->getCommandList()->SetGraphicsRootConstantBufferView(0, m_constBufferView.BufferLocation);

    // Bind texture
    ID3D12DescriptorHeap* arr[] = {
        m_ptrHeapRootTable
    };
    m_ptrDevice->getCommandList()->SetDescriptorHeaps(1, arr);
    m_ptrDevice->getCommandList()->SetGraphicsRootDescriptorTable(1, m_ptrHeapRootTable->GetGPUDescriptorHandleForHeapStart());

    // Draw
    m_ptrDevice->getCommandList()->DrawInstanced(3, 1, 0, 0);

    // ### GFX: End frame
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
    m_uploadBuffer.preDestructDestroy();
    COM_RELEASE(m_ptrHeapRootTable);
    COM_RELEASE(m_ptrTexture);
    COM_RELEASE(m_ptrConstBuffer);
    COM_RELEASE(m_ptrRootSig)
    COM_RELEASE(m_ptrPso);
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
