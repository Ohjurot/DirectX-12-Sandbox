#include "VertexLayout.h"

std::map<UINT, UINT> IF3D12::VertexLayout::c_layoutToBytes = {
	{(UINT)VertexInputType::FLOAT1, 4},
	{(UINT)VertexInputType::FLOAT2, 8},
	{(UINT)VertexInputType::FLOAT3, 12},
	{(UINT)VertexInputType::FLOAT4, 16},
	{(UINT)VertexInputType::UINT1, 4},
	{(UINT)VertexInputType::UINT2, 8},
	{(UINT)VertexInputType::UINT3, 12},
	{(UINT)VertexInputType::UINT4, 16}
};

IF3D12::VertexLayout::VertexLayout(std::initializer_list<VertexInput> inputLayout){
	m_vecInputElements = std::vector<VertexInput>(inputLayout);
}

IF3D12::VertexLayout::~VertexLayout(){
	// Free old memory
	if (m_ptrElements) {
		free(m_ptrElements);
	}
}

void IF3D12::VertexLayout::appendElement(VertexInput inputElement){
	m_vecInputElements.push_back(inputElement);
	m_bIsDirty = TRUE;
}

void IF3D12::VertexLayout::commitChanges(){
	// Check if required
	if (!m_bIsDirty || m_vecInputElements.size() == 0) {
		return;
	}
	
	// Free old memory
	if (m_ptrElements) {
		free(m_ptrElements);
	}

	// Alloc
	m_ptrElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * m_vecInputElements.size());

	// Fill
	UINT offset = 0;
	for (UINT i = 0; i < m_vecInputElements.size(); i++) {
		m_ptrElements[i] = {m_vecInputElements.at(i).name.c_str(), 0, (DXGI_FORMAT)m_vecInputElements.at(i).type, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += c_layoutToBytes[(UINT)m_vecInputElements[i].type];
	}

	m_bIsDirty = FALSE;
}

UINT IF3D12::VertexLayout::getCommitSize(){
	return m_vecInputElements.size();
}

D3D12_INPUT_ELEMENT_DESC* IF3D12::VertexLayout::getPointer(){
	return m_ptrElements;
}
