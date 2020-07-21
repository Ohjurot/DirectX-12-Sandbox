#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <string>
#include <vector>
#include <map>
#include <initializer_list>

namespace IF3D12 {
	enum class VertexInputType : UINT {
		FLOAT1 = DXGI_FORMAT_R32_FLOAT,
		FLOAT2 = DXGI_FORMAT_R32G32_FLOAT,
		FLOAT3 = DXGI_FORMAT_R32G32B32_FLOAT,
		FLOAT4 = DXGI_FORMAT_R32G32B32A32_FLOAT,

		UINT1 = DXGI_FORMAT_R32_UINT,
		UINT2 = DXGI_FORMAT_R32G32_UINT,
		UINT3 = DXGI_FORMAT_R32G32B32_UINT,
		UINT4 = DXGI_FORMAT_R32G32B32A32_UINT,
	};

	struct VertexInput {
		VertexInputType type;
		std::string name;
	};

	class VertexLayout {
		public:
			VertexLayout() {};
			VertexLayout(std::initializer_list<VertexInput> inputLayout);
			~VertexLayout();
			
			void appendElement(VertexInput inputElement);

			void commitChanges();
			UINT getCommitSize();
			D3D12_INPUT_ELEMENT_DESC* getPointer();
		private:
			D3D12_INPUT_ELEMENT_DESC* m_ptrElements = NULL;
			std::vector<VertexInput> m_vecInputElements;
			BOOL m_bIsDirty = TRUE;

			static std::map<UINT, UINT> c_layoutToBytes;
	};
}