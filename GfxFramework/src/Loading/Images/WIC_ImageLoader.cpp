#include "WIC_ImageLoader.h"

std::vector<Util::TexureLoader::WicICV> Util::TexureLoader::s_FormateConversion = {
    // =======================================================================================================================
    // Source: https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to
    // THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    // ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    // THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    // PARTICULAR PURPOSE.
    //
    // Copyright (c) Microsoft Corporation. All rights reserved.
    //
    // https://go.microsoft.com/fwlink/?LinkId=248926
    // https://go.microsoft.com/fwlink/?LinkId=248929

    // Note target GUID in this conversion table must be one of those directly supported formats (above).
    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

#ifdef DXGI_1_2_FORMATS

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

#else

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGRA5551,         GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGR565,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

#endif // DXGI_1_2_FORMATS

    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

    { GUID_WICPixelFormat96bppRGBFixedPoint,    GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif

    // We don't support n-channel formats

    // =======================================================================================================================
};

std::vector<Util::TexureLoader::WicDx> Util::TexureLoader::s_FormateConversionDx = {
    // =======================================================================================================================
    // Source: https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to
    // THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    // ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    // THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    // PARTICULAR PURPOSE.
    //
    // Copyright (c) Microsoft Corporation. All rights reserved.
    //
    // https://go.microsoft.com/fwlink/?LinkId=248926
    // https://go.microsoft.com/fwlink/?LinkId=248929
    
    { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

    { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
    { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

    { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

    { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },
    { GUID_WICPixelFormat32bppRGBE,             DXGI_FORMAT_R9G9B9E5_SHAREDEXP },

#ifdef DXGI_1_2_FORMATS

    { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
    { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

#endif // DXGI_1_2_FORMATS

    { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
    { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
    { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
    { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

    { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    { GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
#endif

    // =======================================================================================================================
};

HRESULT Util::TexureLoader::fromFileToMemory(LPCWSTR fileName, D3D::Device* ptrDevice, void** ppMemory, SIZE_T* pSize, UINT* pWidth, UINT* pHeight, DXGI_FORMAT* ptrResultFormat) {
    HRESULT hr;

    // Create wic factory
    IWICImagingFactory* ptrFactory = NULL;
    if (FAILED(hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptrFactory)))) {
        return hr;
    }

    // Create decoder from file
    IWICBitmapDecoder* ptrDecoder = NULL;
    if (FAILED(hr = ptrFactory->CreateDecoderFromFilename(fileName, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &ptrDecoder))){
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Get frame zero
    IWICBitmapFrameDecode* ptrFrameDecoder = NULL;
    if (FAILED(hr = ptrDecoder->GetFrame(0, &ptrFrameDecoder))) {
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Read the pixel format
    WICPixelFormatGUID frameNativeFormat;
    WICPixelFormatGUID targetFormat;
    if (FAILED(hr = ptrFrameDecoder->GetPixelFormat(&frameNativeFormat))) {
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Check if format has direct conversabiliy
    DXGI_FORMAT targetFormatDx;
    if (!selectConverterDx(frameNativeFormat, &targetFormatDx)) {
        // Try to find WIC to WIC converter
        if (!selectConverterWic(frameNativeFormat, &targetFormat)) {
            return ERROR_NOT_SUPPORTED;
        }

        if (!selectConverterDx(targetFormat, &targetFormatDx)) {
            return E_FAIL;
        }
    }
    else {
        memcpy(&targetFormat, &frameNativeFormat, sizeof(GUID));
    }

    // Get format info
    IWICComponentInfo* ptrCmpInfo = NULL;
    if (FAILED(hr = ptrFactory->CreateComponentInfo(targetFormat, &ptrCmpInfo))) {
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Get type
    WICComponentType ptrCmpType;
    if (FAILED(hr = ptrCmpInfo->GetComponentType(&ptrCmpType))) {
        COM_RELEASE(ptrCmpInfo);
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Check type
    if (ptrCmpType != WICPixelFormat) {
        COM_RELEASE(ptrCmpInfo);
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return ERROR_NOT_SUPPORTED;
    }

    // Get info from type
    IWICPixelFormatInfo* ptrFormatInfo = NULL;
    if (FAILED(hr = ptrCmpInfo->QueryInterface(IID_PPV_ARGS(&ptrFormatInfo)))) {
        COM_RELEASE(ptrCmpInfo);
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Get BBP
    UINT uiBitsPerPixel;
    if (FAILED(hr = ptrFormatInfo->GetBitsPerPixel(&uiBitsPerPixel))) {
        COM_RELEASE(ptrFormatInfo);
        COM_RELEASE(ptrCmpInfo);
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Release
    COM_RELEASE(ptrFormatInfo);
    COM_RELEASE(ptrCmpInfo);

    // Check device support
    D3D12_FEATURE_DATA_FORMAT_SUPPORT fSupport;
    ZeroMemory(&fSupport, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT));
    fSupport.Format = targetFormatDx;

    if (FAILED(hr = ptrDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &fSupport, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)))) {
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return E_FAIL;
    }

    if (!((fSupport.Support1) & (1ULL << D3D12_FORMAT_SUPPORT1_TEXTURE2D))) {
        // Fallback
        memcpy(&frameNativeFormat, &GUID_WICPixelFormat32bppRGBA, sizeof(GUID));
        targetFormatDx = DXGI_FORMAT_R8G8B8A8_UNORM;
        uiBitsPerPixel = 32;
    }

    // Image size
    UINT width, height;
    if (FAILED(hr = ptrFrameDecoder->GetSize(&width, &height))) {
        COM_RELEASE(ptrFrameDecoder);
        COM_RELEASE(ptrDecoder);
        COM_RELEASE(ptrFactory);
        return hr;
    }

    // Tempory memory allocation
    UINT rowPitch = (width * uiBitsPerPixel + 7) / 8;
    SIZE_T imageSize = (SIZE_T)rowPitch * (SIZE_T)height;
    void* workMemory = malloc(imageSize);

    // Check if direct copy is possible
    if (memcmp(&frameNativeFormat, &targetFormat, sizeof(GUID)) == 0) {
        if (FAILED(hr = ptrFrameDecoder->CopyPixels(NULL, rowPitch, (UINT)imageSize, (BYTE*)workMemory))) {
            COM_RELEASE(ptrFrameDecoder);
            COM_RELEASE(ptrDecoder);
            COM_RELEASE(ptrFactory);
            return hr;
        }
    }
    else {
        // Format conversion
        IWICFormatConverter* ptrFormatConverter = NULL;
        if (FAILED(hr = ptrFactory->CreateFormatConverter(&ptrFormatConverter))) {
            COM_RELEASE(ptrFrameDecoder);
            COM_RELEASE(ptrDecoder);
            COM_RELEASE(ptrFactory);
            return hr;
        }

        if (FAILED(hr = ptrFormatConverter->Initialize(ptrFrameDecoder, targetFormat, WICBitmapDitherTypeErrorDiffusion, NULL, 0, WICBitmapPaletteTypeCustom))){
            COM_RELEASE(ptrFormatConverter);
            COM_RELEASE(ptrFrameDecoder);
            COM_RELEASE(ptrDecoder);
            COM_RELEASE(ptrFactory);
            return hr;
        }
        
        if (FAILED(hr = ptrFormatConverter->CopyPixels(NULL, rowPitch, (UINT)imageSize, (BYTE*)workMemory))) {
            COM_RELEASE(ptrFormatConverter);
            COM_RELEASE(ptrFrameDecoder);
            COM_RELEASE(ptrDecoder);
            COM_RELEASE(ptrFactory);
            return hr;
        }

        COM_RELEASE(ptrFormatConverter);
    }

    // Copy to output
    *ppMemory = workMemory;
    *ptrResultFormat = targetFormatDx;
    *pSize = imageSize;
    *pWidth = width;
    *pHeight = height;

    // Release Objects
    COM_RELEASE(ptrFrameDecoder);
    COM_RELEASE(ptrDecoder);
    COM_RELEASE(ptrFactory);

    return S_OK;
}

BOOL Util::TexureLoader::selectConverterDx(GUID ptrSrcGuid, DXGI_FORMAT* ptrDxFormat){
    // Check if converter is in store
    for (auto it = s_FormateConversionDx.begin(); it != s_FormateConversionDx.end(); it++) {
        // Check match
        if (memcmp(&it->guid, &ptrSrcGuid, sizeof(GUID)) == 0) {
            *ptrDxFormat = it->dxFormat;
            return TRUE;
        }
    }

    // Return fallback false
    return FALSE;
}

BOOL Util::TexureLoader::selectConverterWic(GUID srcGuid, GUID* ptrSourceGuid){
    // Check if converter is in store
    for (auto it = s_FormateConversion.begin(); it != s_FormateConversion.end(); it++) {
        // Check match
        if (memcmp(&it->guid, &srcGuid, sizeof(GUID)) == 0) {
            memcpy(ptrSourceGuid, &it->formatGuid, sizeof(GUID));
            return TRUE;
        }
    }

    // Return fallback false
    return FALSE;
}
