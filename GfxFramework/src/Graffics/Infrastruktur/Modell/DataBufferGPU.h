#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <Graffics/Infrastruktur/Modell/DataBufferCPU.h>

#include <Graffics/DirectXContext/Buffers/d3d_VariableUploadBuffer.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace IF3D12 {
	class GPUDataBuffer {
		public:
			GPUDataBuffer();
			GPUDataBuffer(IF3D12::CPUDataBuffer* ptrTargetBuffer);
			~GPUDataBuffer();

			HRESULT update(D3D::Device* ptrDevice);
			VOID preDestructDestroy();
			D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress();

		private:
			UINT m_uiLastSize = 0;

			IF3D12::CPUDataBuffer* m_ptrBuffer;
			ID3D12Resource* m_ptrD3DBuffer;
			D3D::VariableUploadBuffer m_uploadBuffer;
	};
}