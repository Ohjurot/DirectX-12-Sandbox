#pragma once

#include <Windows.h>
#include <d3dcompiler.h>

namespace Util {
	class BlobLoader {
		public:
			static HRESULT loadBlob(LPCWSTR filePath, ID3DBlob** ppBlob);
			static HRESULT loadBlob(LPWSTR fileName, ID3DBlob** ppBlob);
	};
}