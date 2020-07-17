#include "./RootSignature.hlsl"

struct dIn{
	float4 pos : SV_Position;
    float2 tex : TEXTCORDS;
};

Texture2D tex : register(t0);
sampler smp : register(s0);

[RootSignature(MyRS)]
float4 main(dIn data) : SV_TARGET{
    return tex.Sample(smp, data.tex);
}