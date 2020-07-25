#include "Texture.h"

D3D::TextureUploader IF3D12::Texture::s_textureUploader = D3D::TextureUploader();
UINT IF3D12::Texture::s_uiUploadRefCount = 0;

IF3D12::Texture::Texture(LPCWSTR fileName) :
    m_strFile(fileName)
{
    s_uiUploadRefCount++;
}

IF3D12::Texture::~Texture(){
    // Decrement reference counter
    s_uiUploadRefCount--;

    // Check if no reference exists
    if (s_uiUploadRefCount == 0) {
        // Destroy uploader
        s_textureUploader.preDestructDestroy();
    }

    // Destroy own
    COM_RELEASE(m_ptrTexture);
}

HRESULT IF3D12::Texture::load(D3D::Device* ptrDevice){
    HRESULT hr;
    
    // == Create texture
    void* memTex = nullptr;
    SIZE_T sizeBytes = 0;
    UINT texWidth, texHeight;
    COM_CREATE_HR_RETURN(hr, Util::TexureLoader::fromFileToMemory(m_strFile, ptrDevice, &memTex, &sizeBytes, &texWidth, &texHeight, &m_format));
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
    txDesk.Format = m_format;
    txDesk.SampleDesc.Count = 1;
    txDesk.SampleDesc.Quality = 0;
    txDesk.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    txDesk.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create texture
    hr = ptrDevice->getDevice()->CreateCommittedResource(
        &txHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &txDesk,
        D3D12_RESOURCE_STATE_COMMON,
        NULL,
        IID_PPV_ARGS(&m_ptrTexture)
    );

    // Copy
    if (!FAILED(hr)) {
        hr = s_textureUploader.setBuffer(ptrDevice, m_ptrTexture, D3D12_RESOURCE_STATE_COMMON, memTex, texWidth, texHeight, m_format, sizeBytes);
    }

    // Free temporary memory texture
    if (memTex) {
        free(memTex);
    }

    // Return here to make shure free(...) is called
    if (FAILED(hr)) {
        return hr;
    }

    // OK
    return S_OK;
}

ID3D12Resource* IF3D12::Texture::getResourcePointer(){
    return m_ptrTexture;
}

DXGI_FORMAT IF3D12::Texture::getFormat(){
    return m_format;
}

D3D12_SHADER_RESOURCE_VIEW_DESC IF3D12::Texture::createViewDesc(){
    D3D12_SHADER_RESOURCE_VIEW_DESC rvDesk;
    rvDesk.Format = m_format;
    rvDesk.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    rvDesk.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    rvDesk.Texture2D.MipLevels = 1;
    rvDesk.Texture2D.MostDetailedMip = 0;
    rvDesk.Texture2D.PlaneSlice = 0;
    rvDesk.Texture2D.ResourceMinLODClamp = 0.0f;

    return rvDesk;
}
