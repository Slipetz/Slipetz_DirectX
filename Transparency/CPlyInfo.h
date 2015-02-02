#ifndef _CPlyInfo_HG_
#define _CPlyInfo_HG_


#include <d3dx11.h> 
#include <Windows.h>
#include <xnamath.h>
#include <string>

class CPlyInfo
{
public:
	CPlyInfo();
	virtual ~CPlyInfo();
	int numberOfVertices;
	int numberOfElementsToDraw;
	ID3D11Buffer*			vertexBuffer;			///< Vertex buffer 
	ID3D11Buffer*			indexBuffer;			///< Index buffer
	XMFLOAT3	scale;
	std::wstring plyFileName;
	float maxExtent;
};
#endif