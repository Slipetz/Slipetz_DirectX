#include "CPlyInfo.h"

CPlyInfo::CPlyInfo()
{
	numberOfVertices = 0;
	numberOfElementsToDraw =0;
	vertexBuffer = NULL;
	indexBuffer = NULL;
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	plyFileName = L"";
	maxExtent =0;
}

CPlyInfo::~CPlyInfo()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
	}
	if (indexBuffer)
	{
		indexBuffer->Release();
	}
}