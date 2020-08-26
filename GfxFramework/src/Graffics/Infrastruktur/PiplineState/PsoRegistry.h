#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <Graffics/gfxCommon.h>
#include <Graffics/Infrastruktur/DrawDescriptor.h>
#include <Graffics/DirectXContext/d3d_Device.h>

#include <vector>
#include <map>

namespace IF3D12 {	
	class PsoRegistry {
		private:
			struct PsoDeskPso {
				IF3D12::DrawPipelineDescriptor descriptor;
				ID3D12PipelineState* ptrPipelineState;

				bool operator==(const IF3D12::DrawPipelineDescriptor& desk);
			};
	
		public:
			static PsoRegistry* getRegistry();

			UINT createPso(IF3D12::DrawPipelineDescriptor* desc);
			ID3D12PipelineState* getPso(UINT id);
			ID3D12RootSignature* getRoot(UINT id);
			HRESULT createPsoObjects(D3D::Device* ptrDevice);

		private:
			PsoRegistry() {};
			~PsoRegistry();

			std::vector<PsoRegistry::PsoDeskPso> m_vecPso;
			std::map<UINT, ID3D12RootSignature*> m_mapRootSig;

			static PsoRegistry s_psoRegistry;
	};
}