#include "DrawDescriptor.h"

bool IF3D12::DrawPipelineDescriptor::operator==(const DrawPipelineDescriptor& b){
    return(memcmp(&b, this, sizeof(DrawPipelineDescriptor)) == 0);
}

HRESULT IF3D12::DrawPipelineDescriptor::makePso(ID3D12Device* ptrDevice, ID3D12PipelineState** ppPso, ID3D12RootSignature** ppRootSignature, INT idCurrentRoot, DrawPipelineDescriptor* ptrDescriptor){
    // Load shaders
    IF3D12::Shader* ptrRootSignature, *ptrVertexShader, *ptrPixelShader;
    if (!IF3D12::ShaderRegistry::getRegistry()->getShader(ptrDescriptor->rootSignatureId, &ptrRootSignature) ||
        !IF3D12::ShaderRegistry::getRegistry()->getShader(ptrDescriptor->shaders.idVertex, &ptrVertexShader) ||
        !IF3D12::ShaderRegistry::getRegistry()->getShader(ptrDescriptor->shaders.idPixel, &ptrPixelShader)
    ) {
        return ERROR_NOT_FOUND;
    }

    // Create root signature
    if (ptrDescriptor->rootSignatureId != idCurrentRoot) {
        // Release old
        if (*ppRootSignature) {
            (*ppRootSignature)->Release();
            *ppRootSignature = NULL;
        }

        // Create new
        HRESULT hrTemp;
        if (FAILED(hrTemp = ptrDevice->CreateRootSignature(NULL, ptrRootSignature->ptrData, ptrRootSignature->size, IID_PPV_ARGS(ppRootSignature)))) {
            return hrTemp;
        }
    }

    // Get Size
    if (ptrDescriptor->ptrLayout->getCommitSize() == 0) {
        return E_NOT_SUFFICIENT_BUFFER;
    }

    // Commit
    ptrDescriptor->ptrLayout->commitChanges();

    // Describe rasterizer
    D3D12_RASTERIZER_DESC rasterizerDesk;
    ZeroMemory(&rasterizerDesk, sizeof(D3D12_RASTERIZER_DESC));

    rasterizerDesk.FillMode = ptrDescriptor->fillMode;
    rasterizerDesk.CullMode = ptrDescriptor->cullMode;
    rasterizerDesk.DepthClipEnable = FALSE;
    rasterizerDesk.FrontCounterClockwise = FALSE;
    rasterizerDesk.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesk.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesk.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesk.MultisampleEnable = FALSE;
    rasterizerDesk.AntialiasedLineEnable = FALSE;
    rasterizerDesk.ForcedSampleCount = 0;
    rasterizerDesk.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // Describe PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesk;
    ZeroMemory(&psoDesk, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    psoDesk.pRootSignature = *ppRootSignature;
    psoDesk.InputLayout.NumElements = ptrDescriptor->ptrLayout->getCommitSize();
    psoDesk.InputLayout.pInputElementDescs = ptrDescriptor->ptrLayout->getPointer();
    psoDesk.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesk.VS.BytecodeLength = ptrVertexShader->size;
    psoDesk.VS.pShaderBytecode = ptrVertexShader->ptrData;
    psoDesk.PS.BytecodeLength = ptrPixelShader->size;
    psoDesk.PS.pShaderBytecode = ptrPixelShader->ptrData;
    psoDesk.RasterizerState = rasterizerDesk;
    psoDesk.BlendState.AlphaToCoverageEnable = FALSE;
    psoDesk.BlendState.IndependentBlendEnable = FALSE;
    psoDesk.BlendState.RenderTarget[0] = {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    psoDesk.DepthStencilState.DepthEnable = FALSE;
    psoDesk.SampleMask = UINT_MAX;
    psoDesk.NumRenderTargets = 1;
    psoDesk.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesk.SampleDesc.Count = 1;
    psoDesk.SampleDesc.Quality = 0;

    // Create State
    return ptrDevice->CreateGraphicsPipelineState(&psoDesk ,IID_PPV_ARGS(ppPso));
}
