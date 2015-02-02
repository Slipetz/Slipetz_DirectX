
#define MAXTEXTII 2

struct TextDesc
{
	float2 Offset;
	float2 Zoom;
};

struct MaterialInfo
{
    float4 diffuse;
	float4 ambient;
    float4 spec;
};

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
	float4 eye;
	float4 target;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;				// Changes for every single object..
	MaterialInfo colour;
	TextDesc texOffsetAndZoom[MAXTEXTII];
};

Texture2D texture00 : register( t0 );
Texture2D texture01 : register( t1 );
TextureCube textureCube : register( t2 );

SamplerState samLinear : register( s0 );
SamplerState samAnisotropic : register( s1 );

struct VS_INPUT
{
	float4 VertexPos : POSITION;
	float4 VertexNorm : NORMAL;
	float2 Tex : TEXCOORD; //Week 9
};

struct PS_INPUT		// DX11
{
    float4 VertexPosMVP : SV_POSITION;
	float4 VertexPosWorld : POSITION;
	float4 VertexNormalWorld : NORMAL;
	float2 Tex : TEXCOORD; //Week 9
	float3 CubeTex : TEXCOORD1;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	// Combine the matrices first...
	matrix matFinalMVP = mul( World, View );
	matFinalMVP = mul( matFinalMVP, Projection );

	// ADDED
	float4 pos = input.VertexPos;

	//pos.x += Offset.x;
	//pos.y += Offset.y;
	//pos.z += Offset.z;

	output.CubeTex = pos.xyz;

	output.VertexPosMVP = pos;				//input.VertexPos;
	// To place the vertex in the correct location on screen:
	output.VertexPosMVP = mul( pos, matFinalMVP ); //input.VertexPos

	// Passed to the pixel shader for correct lighting:
	output.VertexPosWorld = mul( pos, World ); //input.VertexPos
	output.VertexNormalWorld = mul( input.VertexNorm, World );
	//Added
	output.VertexNormalWorld = normalize(output.VertexNormalWorld);


	// Pass the texture coordinates to the pixel shader
	// (remember, if we don't pass them, the pixel shader is unaware of them)
	output.Tex = input.Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{

   float4 litColor = float4(0.0f, 0.0f, 0.0f,1.0f);
   litColor += colour.diffuse;//;
   //return litColor;

   //input.Tex.x u every simulationStep increase its value by x.xxf.
   //input.Tex.y v
   //texOffsetAndZoom[0]
   //texOffsetAndZoom[1]

   float2 tex1Offset = texOffsetAndZoom[0].Offset;
   tex1Offset += input.Tex;
   tex1Offset *= texOffsetAndZoom[0].Zoom;

   float2 tex2Offset = texOffsetAndZoom[1].Offset;
   tex2Offset += input.Tex;
   tex2Offset *= texOffsetAndZoom[1].Zoom;
   

   float4 texColour0 = textureCube.Sample( samLinear, input.CubeTex ) ;

   return texColour0;

   //return normalize(texColour0);





}


