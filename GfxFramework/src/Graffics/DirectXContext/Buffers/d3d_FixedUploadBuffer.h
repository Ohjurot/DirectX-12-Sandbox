#pragma once

#include <d3d12.h>

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace D3D {
	template<UINT bufferSize>
	class FixedUploadBuffer {
		public:
			~FixedUploadBuffer() {
				COM_RELEASE(m_ptrUploadBuffer);
			}

			HRESULT setBuffer(D3D::Device* ptrDevice, ID3D12Resource* ptrTargetBuffer, D3D12_RESOURCE_STATES currentState, void* srcData, UINT offsetInTarget, UINT copySize) {
				// Check if buffer creation is needed
				if (!m_ptrUploadBuffer) {
					// Describe Upload heap
					D3D12_HEAP_PROPERTIES uplHeapProp;
					ZeroMemory(&uplHeapProp, sizeof(D3D12_HEAP_PROPERTIES));
					uplHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
					uplHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					uplHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					uplHeapProp.CreationNodeMask = NULL;
					uplHeapProp.VisibleNodeMask = NULL;

					// Describe upload buffer
					D3D12_RESOURCE_DESC uplHeapDesc;
					ZeroMemory(&uplHeapDesc, sizeof(D3D12_RESOURCE_DESC));
					uplHeapDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					uplHeapDesc.Width = bufferSize;
					uplHeapDesc.Height = 1;
					uplHeapDesc.DepthOrArraySize = 1;
					uplHeapDesc.MipLevels = 1;
					uplHeapDesc.Format = DXGI_FORMAT_UNKNOWN;
					uplHeapDesc.SampleDesc.Count = 1;
					uplHeapDesc.SampleDesc.Quality = 0;
					uplHeapDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					uplHeapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

					// Create Vertex buffer
					HRESULT hr = ptrDevice->getDevice()->CreateCommittedResource(
						&uplHeapProp,
						D3D12_HEAP_FLAG_NONE,
						&uplHeapDesc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						NULL,
						IID_PPV_ARGS(&m_ptrUploadBuffer)
					);

					// Check return value
					if (FAILED(hr)) {
						return hr;
					}
				}

				// Check size
				if (copySize > bufferSize) {
					return ERROR_INVALID_USER_BUFFER;
				}

				// Map local buffer
				void* mem = nullptr;
				m_ptrUploadBuffer->Map(0, NULL, &mem);
				if (!mem) {
					return E_FAIL;
				}

				// Copy data and unmap
				memcpy(mem, srcData, copySize);
				m_ptrUploadBuffer->Unmap(0, NULL);

				// Set target
				D3D12_RESOURCE_BARRIER toCopySource;
				toCopySource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				toCopySource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				toCopySource.Transition.pResource = ptrTargetBuffer;
				toCopySource.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				toCopySource.Transition.StateBefore = currentState;
				toCopySource.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
				ptrDevice->getCommandList()->ResourceBarrier(1, &toCopySource);

				// Copy
				ptrDevice->getCommandList()->CopyBufferRegion(ptrTargetBuffer, offsetInTarget, m_ptrUploadBuffer, 0, copySize);

				// Set Read
				D3D12_RESOURCE_BARRIER toOldState;
				toOldState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				toOldState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				toOldState.Transition.pResource = ptrTargetBuffer;
				toOldState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				toOldState.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				toOldState.Transition.StateAfter = currentState;
				ptrDevice->getCommandList()->ResourceBarrier(1, &toOldState);
				
				// Execute
				ptrDevice->dispatchCommandList();
				ptrDevice->waitForCommandListAndReset();
			}

			VOID preDestructDestroy() {
				COM_RELEASE(m_ptrUploadBuffer);
			}

		private:
			ID3D12Resource* m_ptrUploadBuffer;
	};
}