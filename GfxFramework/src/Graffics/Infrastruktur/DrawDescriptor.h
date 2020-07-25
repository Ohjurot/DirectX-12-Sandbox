#pragma once

#include <Windows.h>
#include <d3d12.h>

#include <Graffics/Infrastruktur/ShaderRegistry.h>
#include <Graffics/Infrastruktur/Modell/VertexLayout.h>

namespace IF3D12 {
	typedef INT IIDDescriptor;

	struct DrawPipelineDescriptor {
		INT rootSignatureId;
		struct {
			INT idVertex;
			INT idPixel;
		} shaders;
		
		D3D12_CULL_MODE cullMode;
		D3D12_FILL_MODE fillMode;
		
		IF3D12::VertexLayout* ptrLayout;

		bool operator==(const DrawPipelineDescriptor& b);
		static HRESULT makePso(ID3D12Device* ptrDevice, ID3D12PipelineState** ppPso, ID3D12RootSignature** ppRootSignature, INT idCurrentRoot, DrawPipelineDescriptor* ptrDescriptor);
	};
}
