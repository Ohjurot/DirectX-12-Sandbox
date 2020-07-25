#pragma once

#include <d3d12.h>
#include <dxgi.h>

#include <Graffics/DirectXContext/Texture/d3d_TextureUploader.h>
#include <Loading/Images/WIC_ImageLoader.h>

namespace IF3D12 {
	class Texture {
		public:
			Texture(LPCWSTR fileName);
			~Texture();

			HRESULT load(D3D::Device* ptrDevice);
			ID3D12Resource* getResourcePointer();
			DXGI_FORMAT getFormat();
			D3D12_SHADER_RESOURCE_VIEW_DESC createViewDesc();

		private:
			LPCWSTR m_strFile;
			ID3D12Resource* m_ptrTexture = NULL;
			BOOL m_bRefCommited = FALSE;
			DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;

			static D3D::TextureUploader s_textureUploader;
			static UINT s_uiUploadRefCount;
	};
}