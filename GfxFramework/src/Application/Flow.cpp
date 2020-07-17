#include "Flow.h"

HRESULT Application::Flow::Begin(Meta* ptrAppMeta){
    HRESULT hr;

    // Coinit
    if (FAILED(hr = CoInitialize(NULL))) {
        return hr;
    }
    
    // Create Window
    WF::Window wnd(ptrAppMeta->name, 100, 100, ptrAppMeta->clientWidth, ptrAppMeta->clientHeight);

    // Init app
    if (FAILED(hr = ptrAppMeta->entryPoint->Init(&wnd, ptrAppMeta->clientWidth, ptrAppMeta->clientHeight))) {
        return hr;
    }

    // Show Window
    wnd.setVisible(TRUE);

    // Application Loop
    while (wnd.runMessageLoop()) {
        // Run application
        if (FAILED(hr = ptrAppMeta->entryPoint->Loop())) {
            return hr;
        }
    }

    // Shutdown
    CoUninitialize();
    return ptrAppMeta->entryPoint->Shutdown();
}
