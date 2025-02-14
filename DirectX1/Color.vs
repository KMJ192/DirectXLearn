//GLOBALS
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//TYPEDEFS
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//Vertex Shader
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경
	input.position.w = 1.0f;
	
	//월드, 뷰 및 프로젝션 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//픽셀 쉐이더가 사용할 입력 생상 저장.
	output.color = input.color;

	return output;
}