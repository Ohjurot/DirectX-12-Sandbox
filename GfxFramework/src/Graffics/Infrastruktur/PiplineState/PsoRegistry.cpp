#include "PsoRegistry.h"

IF3D12::PsoRegistry IF3D12::PsoRegistry::s_psoRegistry = IF3D12::PsoRegistry();

bool IF3D12::PsoRegistry::PsoDeskPso::operator==(const IF3D12::DrawPipelineDescriptor& desk){
    return (desk == descriptor);
}

IF3D12::PsoRegistry* IF3D12::PsoRegistry::getRegistry(){
    return &s_psoRegistry;
}

UINT IF3D12::PsoRegistry::createPso(IF3D12::DrawPipelineDescriptor* desc){
    // Try to find
    for (UINT id = 0; id < m_vecPso.size(); id++) {
        if (m_vecPso.at(id) == *desc) {
            return id;
        }
    }

    // Create new
    PsoRegistry::PsoDeskPso pdp;
    pdp.descriptor = *desc;
    pdp.ptrPipelineState = NULL;

    // Push back
    m_vecPso.push_back(pdp);

    return m_vecPso.size() - 1;
}

ID3D12PipelineState* IF3D12::PsoRegistry::getPso(UINT id){
    // Check bounds
    if (id >= m_vecPso.size()) {
        return NULL;
    }

    // Return 
    return m_vecPso.at(id).ptrPipelineState;
}

ID3D12RootSignature* IF3D12::PsoRegistry::getRoot(UINT id){
    // Try to find
    auto it = m_mapRootSig.find(id);

    // Return null on fail
    if (it == m_mapRootSig.end()) {
        return NULL;
    }

    // Return pointer
    it->second;
}

HRESULT IF3D12::PsoRegistry::createPsoObjects(D3D::Device* ptrDevice){
    HRESULT hr;

    // Iterate
    for (auto it = m_vecPso.begin(); it != m_vecPso.end(); it++) {
        // Check for null pointer
        if (it->ptrPipelineState == NULL) {
            // Try to get PSO
            auto ppPso = m_mapRootSig.find(it->descriptor.rootSignatureId);
            if (ppPso == m_mapRootSig.end()) {
                m_mapRootSig.emplace(std::pair<UINT, ID3D12RootSignature*>(it->descriptor.rootSignatureId, NULL));
                ppPso = m_mapRootSig.find(it->descriptor.rootSignatureId);
            }

            // Create
            if (FAILED(hr = IF3D12::DrawPipelineDescriptor::makePso(ptrDevice->getDevice(), &it->ptrPipelineState, &ppPso->second, it->descriptor.rootSignatureId, &it->descriptor))) {
                return hr;
            }
        }
    }

    // Ok
    return S_OK;
}

IF3D12::PsoRegistry::~PsoRegistry(){
    // Release all pso states
    for (auto it = m_vecPso.begin(); it != m_vecPso.end(); it++) {
        COM_RELEASE(it->ptrPipelineState);
    }

    // Release all map root sigs
    for (auto it = m_mapRootSig.begin(); it != m_mapRootSig.end(); it++) {
        COM_RELEASE(it->second);
    }
}
