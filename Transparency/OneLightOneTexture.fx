
#include "LightHelper.fx"

#define MAXTEXT 2
#define MAXLIGHTS 10

struct TextDesc
{
	float2 Offset;
	float2 Zoom;
};



//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
	float4 eye;
	float4 target;
	LightDesc lights[10];
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;				// Changes for every single object..
	MaterialInfo colour;
	TextDesc texOffsetAndZoom[MAXTEXT];
};

Texture2D texture00 : register( t0 );
Texture2D texture01 : register( t1 );

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


	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 1.0f);



	// Sum the light contribution from each light source.
	float4 A, D, S;

	for (int lightIndex = 0; lightIndex < MAXLIGHTS; lightIndex++)
	{

		if (lights[lightIndex].lightPowerRangeType.z == 0.0f)		// Parallel
		{
			ComputeDirectionalLight(colour, lights[lightIndex], input.VertexNormalWorld.xyz, eye.xyz, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}
		else if (lights[lightIndex].lightPowerRangeType.z == 1.0f)	// Point 
		{
			ComputePointLight(colour, lights[lightIndex], input.VertexPosWorld.xyz, input.VertexNormalWorld.xyz, eye.xyz, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}
		else // Then it must be a spot light
		{
			ComputeSpotLight(colour, lights[lightIndex], input.VertexPosWorld.xyz, input.VertexNormalWorld.xyz, eye.xyz, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}

	}

	float4 litColor = ambient + diffuse + spec;

		// Common to take alpha from diffuse material.
		// litColor.a = colour.diffuse.a;

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
   tex1Offset *= texOffsetAndZoom[1].Zoom;
   

   float4 texColour0 = texture00.Sample( samAnisotropic, tex1Offset ) + litColor;

   texColour0 = normalize(texColour0);

 
   //use alpha from material
   texColour0.w = litColor.w;

   return texColour0;
	//return normalize(texColour0);

}


