#include "d3dBlogLoader.h"

HRESULT Util::BlobLoader::loadBlob(LPCWSTR filePath, ID3DBlob** ppBlob){
    // Open existing file for reading
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    // Check if file is valid if nor return last error
    if (hFile == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    // Get file size and create blob
    DWORD fileSize = GetFileSize(hFile, NULL);
    HRESULT hr;
    if (FAILED(hr = D3DCreateBlob(fileSize, ppBlob))) {
        // On error return error code
        return hr;
    }

    // Read to blob
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if(!ReadFile(hFile, (*ppBlob)->GetBufferPointer(), fileSize, NULL, NULL)){
        // Return last error
        return GetLastError();
    }

    // Return OK
    return S_OK;
}

HRESULT Util::BlobLoader::loadBlob(LPWSTR fileName, ID3DBlob** ppBlob){
    return loadBlob(static_cast<LPCWSTR>(fileName), ppBlob);
}
