#pragma once

#include <Windows.h>
#include <wincodec.h>
#include <d3d12.h>
#include <dxgi.h>

#include <vector>

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace Util {
	class TexureLoader {
		public:
			struct WicICV {
				WICPixelFormatGUID guid;
				WICPixelFormatGUID formatGuid;
			};

			struct WicDx {
				WICPixelFormatGUID guid;
				DXGI_FORMAT dxFormat;
			};

		public:
			static HRESULT fromFileToMemory(LPCWSTR fileName, D3D::Device* ptrDevice, void** ppMemory, SIZE_T* pSize, UINT* pWidth, UINT* pHeight, DXGI_FORMAT* ptrResultFormat);
			static HRESULT fromMemoryToMemory(void* ptrDataIn, SIZE_T blobSize, D3D::Device* ptrDevice, void** ppMemory, SIZE_T* pSize, UINT* pWidth, UINT* pHeight, DXGI_FORMAT* ptrResultFormat);

		private:
			static BOOL selectConverterDx(GUID ptrSrcGuid, DXGI_FORMAT* ptrDxFormat);
			static BOOL selectConverterWic(GUID srcGuid, GUID* ptrSourceGuid);

		private:
			static std::vector<WicICV> s_FormateConversion;
			static std::vector<WicDx> s_FormateConversionDx;
	};
}