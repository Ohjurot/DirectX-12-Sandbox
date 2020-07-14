#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <cassert>

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace D3D {
	class View {
		public:
			View(UINT width, UINT height);

			VOID bind(D3D::Device* ptrD3DDevice);

		private:
			D3D12_VIEWPORT m_viewPort;
			RECT m_sicRect;
	};
}