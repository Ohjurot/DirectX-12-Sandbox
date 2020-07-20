#include "d3d_psoFactory.h"

HRESULT D3D::PSOFactory::createPso(D3D::Device* ptrDevice, ID3D12PipelineState** ppPso, INT idVertexShader, INT idPixelShader,
    ID3D12RootSignature* ptrRootSignature, UINT numInputElements, D3D12_INPUT_ELEMENT_DESC* ptrInputElements, D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode){
    
    // Load shaders
    IF3D12::Shader* ptrVertexShader = nullptr;
    IF3D12::ShaderRegistry::getRegistry()->getShader(idVertexShader, &ptrVertexShader);
    if (!ptrVertexShader) {
        return E_FAIL;
    }

    IF3D12::Shader* ptrPixelShader = nullptr;
    IF3D12::ShaderRegistry::getRegistry()->getShader(idPixelShader, &ptrPixelShader);
    if (!ptrPixelShader) {
        return E_FAIL;
    }


    // Describe rasterizer
    D3D12_RASTERIZER_DESC rasterizerDesk;
    ZeroMemory(&rasterizerDesk, sizeof(D3D12_RASTERIZER_DESC));

    rasterizerDesk.FillMode = fillMode;
    rasterizerDesk.CullMode = cullMode;
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

    psoDesk.pRootSignature = ptrRootSignature;
    psoDesk.InputLayout.NumElements = numInputElements;
    psoDesk.InputLayout.pInputElementDescs = ptrInputElements;
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

    // Create
    return ptrDevice->getDevice()->CreateGraphicsPipelineState(&psoDesk, IID_PPV_ARGS(ppPso));
}
