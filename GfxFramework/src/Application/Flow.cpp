#include "Flow.h"

HRESULT Application::Flow::Begin(Meta* ptrAppMeta){
    // Create Window
    WF::Window wnd(ptrAppMeta->name, 100, 100, ptrAppMeta->clientWidth, ptrAppMeta->clientHeight);

    // Init app
    HRESULT hr;
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
    return ptrAppMeta->entryPoint->Shutdown();
}
