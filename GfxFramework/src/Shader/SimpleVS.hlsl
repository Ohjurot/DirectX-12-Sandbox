#include "./RootSignature.hlsl"

struct sOut{
	float4 pos : SV_Position;
	float3 color : COLOR;
};

[RootSignature(MyRS)]
void main(float2 pos : POSITION, float3 color : COLOR, out sOut outData)
{
	outData.pos = float4(pos, 0.0f, 1.0f);
	outData.color = color;
}