#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>

#include <Graffics/Infrastruktur/Modell/DataBufferCPU.h>
#include <Graffics/Infrastruktur/Modell/DataBufferGPU.h>

namespace IF3D12 {
	template<class T, UINT count = 1>
	class FDataBuffer {
		public:
			FDataBuffer() :
				m_cpuData(sizeof(T) * count),
				m_gpuData(&m_cpuData)
			{
				ZeroMemory(m_cpuData.getBuffer(), sizeof(T) * count);
			};

			T* getDataPointer(BOOL bMarkDirty = TRUE) {
				// Mark dirty on access
				if (bMarkDirty) {
					markDirty();
				}

				// Return buffer
				return static_cast<T*>(m_cpuData.getBuffer());
			}

			D3D12_GPU_VIRTUAL_ADDRESS getGpuAddress() {
				// Return GPU data
				return m_gpuData.getGPUVirtualAddress();
			}

			HRESULT updateIfDirty(D3D::Device* ptrDevice) {
				// Check target buffer size
				if (!m_cpuData.getSize()) {
					return ERROR_INSUFFICIENT_BUFFER;
				}

				// Check if not dirty
				if (!m_bDirty) {
					return S_OK;
				}

				// Reset dirty
				m_bDirty = FALSE;

				// Update
				return m_gpuData.update(ptrDevice);
			}

			void markDirty() {
				// Mark dirty
				m_bDirty = TRUE;
			}

			void destroy() {
				// Destroy gpu resource and drop cpu buffer
				m_gpuData.preDestructDestroy();
				m_cpuData.drop();
			}

			const UINT elementsCount = count;
			const UINT sizePerElement = sizeof(T);
			const UINT size = sizeof(T) * count;

		private:
			BOOL m_bDirty = FALSE;
			IF3D12::CPUDataBuffer m_cpuData;
			IF3D12::GPUDataBuffer m_gpuData;
	};
}