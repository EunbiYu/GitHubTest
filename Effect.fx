// 카메라 변환 행렬과 투영 변환 행렬을 위한 쉐이더 변수를 선언한다.
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
}

// 월드 변환 행렬을 위한 쉐이더 변수를 선언한다.(슬롯 1을 사용)
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
}

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};
// 정점-쉐이더의 출력을 위한 구조체이다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
}