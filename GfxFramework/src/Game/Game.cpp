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

    return S_OK;
}

HRESULT MY::Game::Loop(){
    // Begin frame
    static FLOAT m_clearColor[4] = { 0.388f, 0.733f, 0.949f, 1.0f };
    m_ptrSwapChain->beginFrame(m_ptrDevice, m_clearColor);

    // TODO: Insert rendering code

    // End frame
    m_ptrSwapChain->endFrame(m_ptrDevice);
    m_ptrDevice->dispatchCommandList();
    m_ptrDevice->waitForCommandListAndRest();
    if (FAILED(m_ptrSwapChain->present())) {
        return m_ptrDevice->getDevice()->GetDeviceRemovedReason();
    }
    return S_OK;
}

HRESULT MY::Game::Shutdown(){
    // Shutdown members
    m_ptrSwapChain->Shutdown();
    m_ptrDevice->Shutdown();

    return S_OK;
}
