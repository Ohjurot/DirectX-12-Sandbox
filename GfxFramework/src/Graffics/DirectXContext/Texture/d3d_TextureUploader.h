#pragma once

#include <d3d12.h>

#include <Graffics/gfxCommon.h>
#include <Graffics/DirectXContext/d3d_Device.h>

namespace D3D {
	class TextureUploader {
		public:
			~TextureUploader() {
				COM_RELEASE(m_ptrUploadBuffer);
			}

			HRESULT setBuffer(D3D::Device* ptrDevice, ID3D12Resource* ptrTargetTexture, D3D12_RESOURCE_STATES currentState, void* srcData, UINT width, UINT height, DXGI_FORMAT format, UINT copySize) {
				// Calculate pitch
				D3D12_SUBRESOURCE_FOOTPRINT pitchedDescSrc;
				pitchedDescSrc.Format = format;
				pitchedDescSrc.Width = width;
				pitchedDescSrc.Height = height;
				pitchedDescSrc.Depth = 1;
				pitchedDescSrc.RowPitch = 256;

				while (pitchedDescSrc.RowPitch < (copySize / height)) {
					pitchedDescSrc.RowPitch = pitchedDescSrc.RowPitch * 2;
				}

				// Check buffers
				void* copySrc = srcData;
				SIZE_T copySizem = copySize;
				if (copySize != pitchedDescSrc.RowPitch * height) {
					copySizem = pitchedDescSrc.RowPitch * (SIZE_T)height;
					copySrc = malloc(copySizem);

					// Copy rows
					for (UINT y = 0; y < height; y++) {
						memcpy((void*)((SIZE_T)copySrc + ((SIZE_T)pitchedDescSrc.RowPitch * y)), (void*)((SIZE_T)srcData + (((SIZE_T)copySize / (SIZE_T)height) * y)), (SIZE_T)copySize / height);
					}
				}
				
				// Check if buffer creation is needed
				if (!m_ptrUploadBuffer || copySizem > m_bufferSize || format != m_lastFormat) {
					COM_RELEASE(m_ptrUploadBuffer);
					m_bufferSize = copySizem;
					m_lastFormat = format;

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
					uplHeapDesc.Width = copySizem;
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
						// Free memory if self allocated
						if (copySrc != srcData) {
							free(copySrc);
						}

						return hr;
					}
				}

				// Map local buffer
				void* mem = nullptr;
				m_ptrUploadBuffer->Map(0, NULL, &mem);
				if (!mem) {
					// Free memory if self allocated
					if (copySrc != srcData) {
						free(copySrc);
					}
					return E_FAIL;
				}

				// Copy data and unmap
				memcpy(mem, copySrc, copySizem);
				m_ptrUploadBuffer->Unmap(0, NULL);

				// Set target
				D3D12_RESOURCE_BARRIER toCopySource;
				toCopySource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				toCopySource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				toCopySource.Transition.pResource = ptrTargetTexture;
				toCopySource.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				toCopySource.Transition.StateBefore = currentState;
				toCopySource.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
				ptrDevice->getCommandList()->ResourceBarrier(1, &toCopySource);


				D3D12_TEXTURE_COPY_LOCATION copyLocationTo;
				copyLocationTo.pResource = ptrTargetTexture;
				copyLocationTo.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				copyLocationTo.SubresourceIndex = 0;

				D3D12_TEXTURE_COPY_LOCATION copyLocationFrom;
				copyLocationFrom.pResource = m_ptrUploadBuffer;
				copyLocationFrom.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
				copyLocationFrom.PlacedFootprint.Offset = 0;
				copyLocationFrom.PlacedFootprint.Footprint = pitchedDescSrc;

				// Copy
				ptrDevice->getCommandList()->CopyTextureRegion(&copyLocationTo, 0, 0, 0, &copyLocationFrom, NULL);

				// Set Read
				D3D12_RESOURCE_BARRIER toOldState;
				toOldState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				toOldState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				toOldState.Transition.pResource = ptrTargetTexture;
				toOldState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				toOldState.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				toOldState.Transition.StateAfter = currentState;
				ptrDevice->getCommandList()->ResourceBarrier(1, &toOldState);

				// Execute
				ptrDevice->dispatchCommandList();
				ptrDevice->waitForCommandListAndReset();

				// Free memory if self allocated
				if (copySrc != srcData) {
					free(copySrc);
				}

				// OK
				return S_OK;
			}

			VOID preDestructDestroy() {
				COM_RELEASE(m_ptrUploadBuffer);
			}

		private:
			ID3D12Resource* m_ptrUploadBuffer = NULL;
			DXGI_FORMAT m_lastFormat = DXGI_FORMAT_UNKNOWN;
			UINT m_bufferSize = 0;
	};
}