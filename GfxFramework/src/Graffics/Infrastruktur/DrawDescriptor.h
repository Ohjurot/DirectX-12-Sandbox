#pragma once

#include <Windows.h>
#include <d3d12.h>

#include <Graffics/Infrastruktur/ShaderRegistry.h>

namespace IF3D12 {
	typedef INT IIDDescriptor;

	struct DrawPipelineDescriptor {
		struct _ShaderIds {
			INT idVertex;
			INT idPixel;
		};

		INT rootSignatureId;
		_ShaderIds shaders;
		D3D12_CULL_MODE cullMode;
		D3D12_FILL_MODE fillMode;
		
		UINT inputLayoutElementCount;
		D3D12_INPUT_ELEMENT_DESC* ptrInputLayout;

		bool operator==(const DrawPipelineDescriptor& b);
		static HRESULT makePso(ID3D12Device* ptrDevice, ID3D12PipelineState** ppPso, ID3D12RootSignature** ppRootSignature, INT idCurrentRoot, DrawPipelineDescriptor* ptrDescriptor);
	};
}
