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

    // == Input layout
    m_inputLayout.appendElement({ IF3D12::VertexInputType::FLOAT2, "POSITION" });
    m_inputLayout.appendElement({ IF3D12::VertexInputType::FLOAT2, "TEXTCORDS" });

    // == Create Vertex buffer
    m_bufferVertexCpu = IF3D12::CPUDataBuffer((void*)c_cpuBuffer, sizeof(Vertex) * 3);
    m_bufferVertex = IF3D12::GPUDataBuffer(&m_bufferVertexCpu);
    m_bufferVertex.update(m_ptrDevice);

    // Create view
    ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    m_vertexBufferView.BufferLocation = m_bufferVertex.getGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = sizeof(Vertex) * 3;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);


    // == Create Constant buffer
    m_bufferConstCpu = IF3D12::CPUDataBuffer(&m_cpuConstBuffer, sizeof(CBuffer));
    m_bufferConst = IF3D12::GPUDataBuffer(&m_bufferConstCpu);
    m_bufferConst.update(m_ptrDevice);

    // Describe view
    m_constBufferView.BufferLocation = m_bufferConst.getGPUVirtualAddress();
    m_constBufferView.SizeInBytes = sizeof(CBuffer);

    // == Create texture
    void* memTex = nullptr;
    SIZE_T sizeBytes = 0;
    UINT texWidth, texHeight;
    DXGI_FORMAT texFormat;
    COM_CREATE_HR_RETURN(hr, Util::TexureLoader::fromFileToMemory(L"./texture.png", m_ptrDevice, &memTex, &sizeBytes, &texWidth, &texHeight, &texFormat));
    assert(memTex);

    // Heap
    D3D12_HEAP_PROPERTIES txHeapProp;
    ZeroMemory(&txHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
    txHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    txHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    txHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    txHeapProp.CreationNodeMask = NULL;
    txHeapProp.VisibleNodeMask = NULL;

    // Describe Resource
    D3D12_RESOURCE_DESC txDesk;
    ZeroMemory(&txDesk, sizeof(D3D12_RESOURCE_DESC));
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
    descriptor.ptrLayout = &m_inputLayout;
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
        m_bufferConst.update(m_ptrDevice);
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
    m_bufferConst.preDestructDestroy();
    COM_RELEASE(m_ptrRootSig)
    COM_RELEASE(m_ptrPso);
    m_bufferVertex.preDestructDestroy();
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
