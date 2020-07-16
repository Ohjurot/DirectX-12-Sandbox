#include "./RootSignature.hlsl"

[RootSignature(MyRS)]
void main(float3 pos : POSITION, out float4 oPos : SV_POSITION) {
	oPos = float4(pos, 1.0f);
}