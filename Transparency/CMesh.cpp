#include "CMesh.h"

CMesh::CMesh()
{
	_gridName = L"grid";
	_boxName = L"box";
	_sphereName = L"sphere";
}
std::wstring CMesh::GetGridName()
{
	return _gridName;
}

bool CMesh::CreateGrid(float width, float depth, UINT m, UINT n)
{

	UINT vertexCount = m*n;
	UINT faceCount   = (m-1)*(n-1)*2;

	std::wstring plyFileName = GetGridName();

	g_mapPlyInfo[plyFileName].maxExtent = width;
	//
	// Create the vertices.
	//

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	float dx = width / (n-1);
	float dz = depth / (m-1);

	float du = 1.0f / (n-1);
	float dv = 1.0f / (m-1);

	int totalNumberOfVertices = m * n * 2;	// Make it a bit bigger.
	SimpleVertex* tempVertexArray = new SimpleVertex[ totalNumberOfVertices ];
	memset( tempVertexArray, 0, totalNumberOfVertices * sizeof( SimpleVertex ) );

	int countVertices = 0;

	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			tempVertexArray[i*n+j].Pos.x = x;
			tempVertexArray[i*n+j].Pos.y = 0.0f;
			tempVertexArray[i*n+j].Pos.z = z;
			tempVertexArray[i*n+j].Pos.w = 1.0f; 

			tempVertexArray[i*n+j].Normal.x = 0.0f;
			tempVertexArray[i*n+j].Normal.y = 1.0f;
			tempVertexArray[i*n+j].Normal.z = 0.0f; 
			tempVertexArray[i*n+j].Normal.w = 1.0f; 

			//meshData.Vertices[i*n+j].Position = XMFLOAT3(x, 0.0f, z);
			//meshData.Vertices[i*n+j].Normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//meshData.Vertices[i*n+j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			//meshData.Vertices[i*n+j].TexC.x = j*du;
			//meshData.Vertices[i*n+j].TexC.y = i*dv;
			tempVertexArray[i*n+j].Tex.x = j*du;
			tempVertexArray[i*n+j].Tex.y = i*dv;

			countVertices++;
		}
	}

	g_mapPlyInfo[plyFileName].numberOfVertices = countVertices;

	// Create a temporary "local" index array
	int totalNumberOfElements = m * n * 2;
	DWORD* tempIndexArray = new DWORD[ totalNumberOfElements * 3 ];
	memset( tempIndexArray, 0, totalNumberOfElements * 3 * sizeof(DWORD) );

	int countIndexes =0;
	// Iterate over each quad and compute indices.
	UINT k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(UINT j = 0; j < n-1; ++j)
		{
			tempIndexArray[k]   = i*n+j;
			tempIndexArray[k+1] = i*n+j+1;
			tempIndexArray[k+2] = (i+1)*n+j;

			tempIndexArray[k+3] = (i+1)*n+j;
			tempIndexArray[k+4] = i*n+j+1;
			tempIndexArray[k+5] = (i+1)*n+j+1;

			k += 6; // next quad

			countIndexes++;
		}
	}

	g_mapPlyInfo[plyFileName].numberOfElementsToDraw = countIndexes * 3;

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
		MessageBox( NULL, L"ERROR: Unable to create vertex buffer.", L"CreateGrid - Something is wrong", MB_OK );
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
		MessageBox( NULL, L"ERROR: Unable to create index buffer", L"CreateGrid - Something is wrong", MB_OK );
		return false;
	}


	// Delete all of our stuff...
	delete [] tempVertexArray;		// WATCH IT!!
	delete [] tempIndexArray;

	return true;
}

bool CMesh::CreateBox(float width, float height, float depth)
{

	std::wstring plyFileName = GetBoxName();
	g_mapPlyInfo[plyFileName].maxExtent = width;
	g_mapPlyInfo[plyFileName].numberOfVertices = 24;

	//
	// Create the vertices.
	//

	int totalNumberOfVertices = 24 * 2;	// Make it a bit bigger.
	SimpleVertex* tempVertexArray = new SimpleVertex[ totalNumberOfVertices ];
	memset( tempVertexArray, 0, totalNumberOfVertices * sizeof( SimpleVertex ) );


	//Vertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

	// Fill in the front face vertex data.
	//v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[0].Pos = XMFLOAT4(-w2, -h2, -d2, 1.0f);
	tempVertexArray[0].Normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	tempVertexArray[0].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	tempVertexArray[1].Pos = XMFLOAT4(-w2, +h2, -d2, 1.0f);
	tempVertexArray[1].Normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	tempVertexArray[1].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	tempVertexArray[2].Pos = XMFLOAT4(+w2, +h2, -d2, 1.0f);
	tempVertexArray[2].Normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	tempVertexArray[2].Tex = XMFLOAT2(1.0f, 0.0f);

	//v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[3].Pos = XMFLOAT4(+w2, -h2, -d2, 1.0f);
	tempVertexArray[3].Normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	tempVertexArray[3].Tex = XMFLOAT2(1.0f, 1.0f);


	// Fill in the back face vertex data.
	//v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[4].Pos = XMFLOAT4(-w2, -h2, +d2, 1.0f);
	tempVertexArray[4].Normal = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[4].Tex = XMFLOAT2(1.0f, 1.0f);

	//v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[5].Pos = XMFLOAT4(+w2, -h2, +d2, 1.0f);
	tempVertexArray[5].Normal = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[5].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	tempVertexArray[6].Pos = XMFLOAT4(+w2, +h2, +d2, 1.0f);
	tempVertexArray[6].Normal = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[6].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	tempVertexArray[7].Pos = XMFLOAT4(-w2, +h2, +d2, 1.0f);
	tempVertexArray[7].Normal = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[7].Tex = XMFLOAT2(1.0f, 0.0f);


	// Fill in the top face vertex data.
	//v[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[8].Pos = XMFLOAT4(-w2, +h2, -d2, 1.0f);
	tempVertexArray[8].Normal = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	tempVertexArray[8].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	tempVertexArray[9].Pos = XMFLOAT4(-w2, +h2, +d2, 1.0f);
	tempVertexArray[9].Normal = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	tempVertexArray[9].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	tempVertexArray[10].Pos = XMFLOAT4(+w2, +h2, +d2, 1.0f);
	tempVertexArray[10].Normal = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	tempVertexArray[10].Tex = XMFLOAT2(1.0f, 0.0f);

	//v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[11].Pos = XMFLOAT4(+w2, +h2, -d2, 1.0f);
	tempVertexArray[11].Normal = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	tempVertexArray[11].Tex = XMFLOAT2(1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	//v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	tempVertexArray[12].Pos = XMFLOAT4(-w2, -h2, -d2, 1.0f);
	tempVertexArray[12].Normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	tempVertexArray[12].Tex = XMFLOAT2(1.0f, 1.0f);

	//v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[13].Pos = XMFLOAT4(+w2, -h2, -d2, 1.0f);
	tempVertexArray[13].Normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	tempVertexArray[13].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	tempVertexArray[14].Pos = XMFLOAT4(+w2, -h2, +d2, 1.0f);
	tempVertexArray[14].Normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	tempVertexArray[14].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	tempVertexArray[15].Pos = XMFLOAT4(-w2, -h2, +d2, 1.0f);
	tempVertexArray[15].Normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	tempVertexArray[15].Tex = XMFLOAT2(1.0f, 0.0f);

	// Fill in the left face vertex data.
	//v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	tempVertexArray[16].Pos = XMFLOAT4(-w2, -h2, +d2, 1.0f);
	tempVertexArray[16].Normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[16].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	tempVertexArray[17].Pos = XMFLOAT4(-w2, +h2, +d2, 1.0f);
	tempVertexArray[17].Normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[17].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	tempVertexArray[18].Pos = XMFLOAT4(-w2, +h2, -d2, 1.0f);
	tempVertexArray[18].Normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[18].Tex = XMFLOAT2(1.0f, 0.0f);

	//v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	tempVertexArray[19].Pos = XMFLOAT4(-w2, -h2, -d2, 1.0f);
	tempVertexArray[19].Normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[19].Tex = XMFLOAT2(1.0f, 1.0f);

	// Fill in the right face vertex data.
	//v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	tempVertexArray[20].Pos = XMFLOAT4(+w2, -h2, -d2, 1.0f);
	tempVertexArray[20].Normal = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[20].Tex = XMFLOAT2(0.0f, 1.0f);

	//v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	tempVertexArray[21].Pos = XMFLOAT4(+w2, +h2, -d2, 1.0f);
	tempVertexArray[21].Normal = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[21].Tex = XMFLOAT2(0.0f, 0.0f);

	//v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	tempVertexArray[22].Pos = XMFLOAT4(+w2, +h2, +d2, 1.0f);
	tempVertexArray[22].Normal = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[22].Tex = XMFLOAT2(1.0f, 0.0f);

	//v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	tempVertexArray[23].Pos = XMFLOAT4(+w2, -h2, +d2, 1.0f);
	tempVertexArray[23].Normal = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	tempVertexArray[23].Tex = XMFLOAT2(1.0f, 1.0f);

	// Create a temporary "local" index array
	int totalNumberOfElements = 36 * 2;
	DWORD* tempIndexArray = new DWORD[ totalNumberOfElements * 3 ];
	memset( tempIndexArray, 0, totalNumberOfElements * 3 * sizeof(DWORD) );


	//UINT i[36];

	// Fill in the front face index data
	tempIndexArray[0] = 0; tempIndexArray[1] = 1; tempIndexArray[2] = 2;
	tempIndexArray[3] = 0; tempIndexArray[4] = 2; tempIndexArray[5] = 3;

	// FtempIndexArrayll tempIndexArrayn the back face tempIndexArrayndex data
	tempIndexArray[6] = 4; tempIndexArray[7]  = 5; tempIndexArray[8]  = 6;
	tempIndexArray[9] = 4; tempIndexArray[10] = 6; tempIndexArray[11] = 7;

	// FtempIndexArrayll tempIndexArrayn the top face tempIndexArrayndex data
	tempIndexArray[12] = 8; tempIndexArray[13] =  9; tempIndexArray[14] = 10;
	tempIndexArray[15] = 8; tempIndexArray[16] = 10; tempIndexArray[17] = 11;

	// FtempIndexArrayll tempIndexArrayn the bottom face tempIndexArrayndex data
	tempIndexArray[18] = 12; tempIndexArray[19] = 13; tempIndexArray[20] = 14;
	tempIndexArray[21] = 12; tempIndexArray[22] = 14; tempIndexArray[23] = 15;

	// FtempIndexArrayll tempIndexArrayn the left face tempIndexArrayndex data
	tempIndexArray[24] = 16; tempIndexArray[25] = 17; tempIndexArray[26] = 18;
	tempIndexArray[27] = 16; tempIndexArray[28] = 18; tempIndexArray[29] = 19;

	// FtempIndexArrayll tempIndexArrayn the rtempIndexArrayght face tempIndexArrayndex data
	tempIndexArray[30] = 20; tempIndexArray[31] = 21; tempIndexArray[32] = 22;
	tempIndexArray[33] = 20; tempIndexArray[34] = 22; tempIndexArray[35] = 23;

	g_mapPlyInfo[plyFileName].numberOfElementsToDraw = 36;


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
		MessageBox( NULL, L"ERROR: Unable to create vertex buffer.", L"CreateBox - Something is wrong", MB_OK );
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
		MessageBox( NULL, L"ERROR: Unable to create index buffer", L"CreateBox - Something is wrong", MB_OK );
		return false;
	}


	// Delete all of our stuff...
	delete [] tempVertexArray;		// WATCH IT!!
	delete [] tempIndexArray;

	return true;

	return true;
}

std::wstring CMesh::GetBoxName()
{
	return _boxName;
}

bool CMesh::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
	std::wstring plyFileName = GetSphereName();
	g_mapPlyInfo[plyFileName].maxExtent = radius;
	//g_mapPlyInfo[plyFileName].numberOfVertices = 24;

	return true;
}

std::wstring CMesh::GetSphereName()
{
	return _sphereName;
}

CMesh::~CMesh(){}