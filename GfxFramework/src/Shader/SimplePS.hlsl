#include "./RootSignature.hlsl"

struct dIn{
	float4 pos : SV_Position;
	float3 color : COLOR;
};

[RootSignature(MyRS)]
float4 main(dIn data) : SV_TARGET{
	return float4(data.color, 1.0f);
}