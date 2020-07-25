#include "Modell.h"

void IF3D12::IModell::bind(D3D::Device* ptrDevice){
	// Internal placeholder
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	

	// Query
	this->getVertexData(&vbView, NULL);
	this->getIndexData(&ibView, NULL);

	// Bind
	ptrDevice->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
	ptrDevice->getCommandList()->IASetIndexBuffer(&ibView);
}

void IF3D12::IModell::draw(D3D::Device* ptrDevice, UINT numInstance, IInstance* ptrInstances, IF3D12::GPUDataBuffer* ptrGpuBuffers){
	// Update all buffers
	UINT i;
	for (i = 0; i < numInstance; i++) {
		// Validate size
		if (ptrGpuBuffers[i].getCpuBuffer()->getSize() == ptrInstances[i].getSize()) {
			// Copy
			memcpy(ptrGpuBuffers[i].getCpuBuffer()->getBuffer(), ptrInstances[i].getMemoryPointer(), ptrGpuBuffers[i].getCpuBuffer()->getSize());
			// Update
			ptrGpuBuffers[i].update(ptrDevice);
		}
	}

	// Query
	UINT numV, numI;
	this->getVertexData(NULL, &numV);
	this->getIndexData(NULL, &numI);

	// Draw
	ptrDevice->getCommandList()->DrawIndexedInstanced(numI, i, 0, 0, 0);
	ptrDevice->dispatchCommandList();
	ptrDevice->waitForCommandListAndReset();
}
