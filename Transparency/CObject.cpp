#include "global.h"


CObject::CObject()
{
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = 1.0f;
	plyFileName = L"";
	material = getRandomMaterial();
	rotate = false;
	shaderFileName = L"OneLightOneTexture.fx";
	texture1FileName = L"";
	texture2FileName = L"";
	boltTextIndex = 0;
	timeCounter = 0.0f;

	//Use as addition in shader. 
	textOffsetAndZoom[0].Offset.x = 0.0f;
	textOffsetAndZoom[0].Offset.y = 0.0f;

	//Used as multiplication in shader.
	textOffsetAndZoom[0].Zoom.x = 1.0f;
	textOffsetAndZoom[0].Zoom.y = 1.0f;

	textOffsetAndZoom[1].Zoom = XMFLOAT2(1.0f, 1.0f);
	textOffsetAndZoom[1].Offset = XMFLOAT2(0.0f, 0.0f);

	isSky = false;
	isTransparent = false;
	isWireframe = false;

}

void CObject::UpdateBoltTexture(float deltaSeconds, float timeInterval)
{
	timeCounter += deltaSeconds;
	


	if (timeCounter >= timeInterval)
	{
		timeCounter = 0.0f;
		boltTextIndex++;
	}
	if (boltTextIndex >= g_mBoltTextures.size())
	{
		boltTextIndex = 0;
	}

	texture1FileName = g_mBoltTextures[boltTextIndex];

}

CObject::~CObject(){}