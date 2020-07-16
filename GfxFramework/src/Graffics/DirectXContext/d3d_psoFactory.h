#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>s

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>


namespace D3D {
	class PSOFactory {
		public:
			static HRESULT createPso(D3D::Device* ptrDevice, ID3D12PipelineState** ppPso, ID3DBlob* ptrVertexShader, ID3DBlob* ptrPixelShader, 
				ID3D12RootSignature* ptrRootSignature, UINT numInputElements, D3D12_INPUT_ELEMENT_DESC* ptrInputElements, 
				D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE, D3D12_FILL_MODE fillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID);
	};
}