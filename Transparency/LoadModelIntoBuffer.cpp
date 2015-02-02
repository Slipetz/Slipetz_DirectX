#include "global.h"
#include "CPlyFile5nt_DX11.h"

bool LoadModuleIntoBuffer(std::wstring plyFileName, std::wstring &error)
{

	// Load our ply file... in a moment...
	CPlyFile5nt_DX11 myPly;
	if ( !myPly.OpenPLYFile( plyFileName, error ) )	//ply\\bun_zipper_res3.ply
	{
		MessageBox(NULL, error.c_str(), L"Error", MB_OK );
		return false;
	}


	//Check if model has been already loaded
	if (g_mapPlyInfo.find(plyFileName) != g_mapPlyInfo.end())
	{
		//Already exist
		error = L"PLY file already loaded.";
		return false;
	}

	g_mapPlyInfo[plyFileName].numberOfElementsToDraw = myPly.GetNumberOfElements();
	g_mapPlyInfo[plyFileName].numberOfVertices = myPly.GetNumberOfVerticies();
	g_mapPlyInfo[plyFileName].maxExtent = myPly.getMaxExtent();

	myPly.normalizeTheModelBaby();

	//Week 9
	// Calculate texture coordinates...if there weren't any.
	myPly.GenTextureCoordsSpherical( CPlyFile5nt_DX11::POSITIVE_X, 
		CPlyFile5nt_DX11::POSITIVE_Y, 
		true, 1.0f, false );

	int totalNumberOfVertices = myPly.GetNumberOfVerticies() * 2;	// Make it a bit bigger.
	SimpleVertex* tempVertexArray = new SimpleVertex[ totalNumberOfVertices ];
	memset( tempVertexArray, 0, totalNumberOfVertices * sizeof( SimpleVertex ) );

	// Create a temporary "local" index array
	int totalNumberOfElements = myPly.GetNumberOfElements() * 2;
	DWORD* tempIndexArray = new DWORD[ totalNumberOfElements * 3 ];
	memset( tempIndexArray, 0, totalNumberOfElements * 3 * sizeof(DWORD) );

	for ( int index = 0; index != myPly.GetNumberOfVerticies(); index++ )
	{
		tempVertexArray[index].Pos.x = myPly.getVertex_at( index ).xyz.x;
		tempVertexArray[index].Pos.y = myPly.getVertex_at( index ).xyz.y;
		tempVertexArray[index].Pos.z = myPly.getVertex_at( index ).xyz.z;
		tempVertexArray[index].Pos.w = 1.0f;	// W is almost alway 1.0f;

		tempVertexArray[index].Normal.x = myPly.getVertex_at( index ).nx;
		tempVertexArray[index].Normal.y = myPly.getVertex_at( index ).ny;
		tempVertexArray[index].Normal.z = myPly.getVertex_at( index ).nz;
		tempVertexArray[index].Normal.w = 1.0f;	// W is almost alway 1.0f;
		// week 9
		tempVertexArray[index].Tex.x = myPly.getVertex_at(index).tex0u;
		tempVertexArray[index].Tex.y = myPly.getVertex_at(index).tex0v;
		std::wostringstream  wss;
		wss << L"x: " << tempVertexArray[index].Tex.x << std::endl;

		//OutputDebugString(wss.str().c_str());
		//wss << L"";
		//wss << L"y: " << tempVertexArray[index].Tex.y << std::endl;
		//OutputDebugString(wss.str().c_str());
		//

	}

	// Now the maddness starts when we copy the index buffer...
	for ( int triNum = 0; triNum != myPly.GetNumberOfElements(); triNum++ )
	{
		int arrayIndex = triNum * 3;		// Because it's triangles (3 side)

		tempIndexArray[arrayIndex + 0] = myPly.getElement_at( triNum ).vertex_index_1;
		tempIndexArray[arrayIndex + 1] = myPly.getElement_at( triNum ).vertex_index_2;
		tempIndexArray[arrayIndex + 2] = myPly.getElement_at( triNum ).vertex_index_3;
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * totalNumberOfVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = tempVertexArray;

	HRESULT hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &(g_mapPlyInfo[plyFileName].vertexBuffer));
	if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create vertex buffer.";
		return false;
	}

	// Same thing, but with the index buffer...
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * totalNumberOfElements * 3;       
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = tempIndexArray;
	hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &(g_mapPlyInfo[plyFileName].indexBuffer));
	if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create index buffer";
		return false;
	}


	// Delete all of our stuff...
	delete [] tempVertexArray;		// WATCH IT!!
	delete [] tempIndexArray;

	return true;
}