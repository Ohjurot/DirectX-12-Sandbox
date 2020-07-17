#include "./RootSignature.hlsl"

struct cBuf
{
	float4x4 matView;
	float4x4 matTranform;
};

struct sOut
{
	float4 pos : SV_Position;
    float2 tex : TEXTCORDS;
};

ConstantBuffer<cBuf> genericBuffer : register(b0);

[RootSignature(MyRS)]
void main(float2 pos : POSITION, float2 tex : TEXTCORDS, out sOut outData)
{
	outData.pos = mul(mul(float4(pos, 0.0f, 1.0f), genericBuffer.matTranform), genericBuffer.matView);
	outData.tex = tex;
}