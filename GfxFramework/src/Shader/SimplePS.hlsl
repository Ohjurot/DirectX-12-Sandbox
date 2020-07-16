#include "./RootSignature.hlsl"

[RootSignature(MyRS)]
float4 main(float4 pos : SV_POSITION) : SV_TARGET{
	return float4(1.0, 1.0f, 1.0f, 1.0f);
}