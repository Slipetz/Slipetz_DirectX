//--------------------------------------------------------------------------------------
// File: WinMain.cpp
//
// Lighting
//
// 
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------

#include "CMesh.h"
#include <fstream>

void Add3DModelToTheWorld(CObject model);
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

	g_hInst = hInstance;

	std::wstring error;

	//--------------------------------------------------------------------------------------
	// Init Direct X and Create Window
	//--------------------------------------------------------------------------------------
	if ( !InitDirectXAndCreateWindow( hInstance, nCmdShow, L"Ryan's Totes Awesome DirectX App", 1024, 768, error ) )
	{
		MessageBox( NULL, error.c_str(), L"InitDirectXAndCreateWindow - Something is Wrong", MB_OK );
		CleanupDevice();
		exit( -1 );
	}

	//--------------------------------------------------------------------------------------
	// Load vertex and pixel shaders
	//--------------------------------------------------------------------------------------
	if ( !LoadAndPrepareShadersDX11( L"OneLightOneTexture.fx", L"VS", L"vs_4_0", 
		L"OneLightOneTexture.fx", L"PS", L"ps_4_0", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Load vertex/pixel shaders - Something is wrong", MB_OK );
		exit( -1 );
	}
	if ( !LoadAndPrepareShadersDX11( L"Sky.fx", L"VS", L"vs_4_0", 
		L"Sky.fx", L"PS", L"ps_4_0", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Load vertex/pixel shaders - Something is wrong", MB_OK );
		exit( -1 );
	}

	if (!LoadAndPrepareShadersDX11(L"CubeMap.fx", L"VS", L"vs_4_0",
		L"CubeMap.fx", L"PS", L"ps_4_0", error))
	{
		MessageBox(NULL, error.c_str(), L"Load vertex/pixel shaders - Something is wrong", MB_OK);
		exit(-1);
	}

	if (!LoadAndPrepareShadersDX11(L"MultipleTextures.fx", L"VS", L"vs_4_0",
		L"MultipleTextures.fx", L"PS", L"ps_4_0", error))
	{
		MessageBox(NULL, error.c_str(), L"Load vertex/pixel shaders - Something is wrong", MB_OK);
		exit(-1);
	}


	//--------------------------------------------------------------------------------------
	// Load textures
	//--------------------------------------------------------------------------------------
	if (!LoadTexture(L"Textures\\CubeMaps\\skyTexture.dds", error))
	{
		MessageBox( NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK );
		exit( -1 );
	}
	if (!LoadTexture(L"Textures\\Metal\\01.jpg", error))
	{
		MessageBox(NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK);
		exit(-1);
	}
	if (!LoadTexture(L"Textures\\Metal\\02.jpg", error))
	{
		MessageBox(NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK);
		exit(-1);
	}
	if (!LoadTexture(L"Textures\\Metal\\03.jpg", error))
	{
		MessageBox(NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK);
		exit(-1);
	}
	if (!LoadTexture(L"Textures\\Metal\\04.jpg", error))
	{
		MessageBox(NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK);
		exit(-1);
	}
	
	//std::wstring fileName = L"Textures\\Bird\\birdInfo.txt";

	//std::wifstream inFile(fileName);
	//if (!inFile)
	//{
	//	MessageBox( NULL, fileName.c_str(), L"Unable to open file - Something is wrong", MB_OK );
	//	exit( -1 );
	//}
	//std::wstring line;
	//while(std::getline(inFile, line))
	//{
	//	//WriteLineToConsole(line);
	//	if (line == L"") continue;

	//	error = L"";
	//	if (!LoadTexture(line,error))
	//	{
	//		MessageBox( NULL, error.c_str(), L"Load texture - Something is wrong", MB_OK );
	//		exit( -1 );
	//	}

	//	g_mBoltTextures.push_back(line);
	//}


	//--------------------------------------------------------------------------------------
	// Load ply files into buffer
	//--------------------------------------------------------------------------------------

	if (!LoadModuleIntoBuffer(L"PlyFiles\\Corellian_Corvette_(Blockade_Runner)_Simplified.ply", error))
	{
		MessageBox( NULL, error.c_str(), L"LoadModuleIntoBuffer - Something is wrong", MB_OK );
		exit( -1 );
	}
	if (!LoadModuleIntoBuffer(L"PlyFiles\\Tie_Interceptor_(Simplified).ply", error))
	{
		MessageBox(NULL, error.c_str(), L"LoadModuleIntoBuffer - Something is wrong", MB_OK);
		exit(-1);
	}

	if (!LoadModuleIntoBuffer(L"PlyFiles\\TIEAdvanced_x1_Prototype_(Vader's_Tie)_(Simplified).ply", error))
	{
		MessageBox(NULL, error.c_str(), L"LoadModuleIntoBuffer - Something is wrong", MB_OK);
		exit(-1);
	}
	if (!LoadModuleIntoBuffer(L"PlyFiles\\TieBomber_(Simplified).ply", error))
	{
		MessageBox(NULL, error.c_str(), L"LoadModuleIntoBuffer - Something is wrong", MB_OK);
		exit(-1);
	}
	

	//Set camera position
	g_Camera.SetPosition(0.0f, 20.0f, -60.0f);

	//Load geometry
	CMesh mesh;
	//Create grid
	mesh.CreateGrid(70.0f,30.0f,25,25);
	//Create box
	float boxSize = 5.0f;
	mesh.CreateBox(boxSize,boxSize,boxSize);

	//--------------------------------------------------------------------------------------
	// Create 3D objects.
	//--------------------------------------------------------------------------------------
	//Specify per object : 
	//plyFileName
	//textureFileName
	//shaderFileName (currently is using the one specified at CObject constructor)
	//material
	//position
	//scale

	/**SKY MAP**/
	CObject sky;
	sky.isSky = true;
	sky.shaderFileName = L"CubeMap.fx";
	sky.texture1FileName = L"Textures\\CubeMaps\\skyTexture.dds";
	sky.plyFileName = mesh.GetBoxName();
	sky.position = XMFLOAT3(0.0f, 7.0f, -5.0f);
	sky.scale = 50.0f;
	sky.rotate = true;
	sky.material = getRandomMaterial();
	Add3DModelToTheWorld(sky);

	//Objects - AKA The Empire's Fighters

	//TOP ROW
	CObject tieBomber;
	tieBomber.plyFileName = L"PlyFiles\\TieBomber_(Simplified).ply";
	tieBomber.shaderFileName = L"MultipleTextures.fx";
	tieBomber.position = XMFLOAT3(0.0f, 48.0f, 20.0f);
	tieBomber.texture1FileName = L"Textures\\Metal\\01.jpg";
	tieBomber.texture2FileName = L"Textures\\Metal\\02.jpg";
	tieBomber.material = getRandomMaterial();
	tieBomber.scale = 0.06f;
	tieBomber.rotation.x = XMConvertToRadians(90.0f);
	//tieBomber.isTransparent = true;
	tieBomber.material.ambient.w = -0.9f;
	Add3DModelToTheWorld(tieBomber);

	//SECOND ROW - Vaders
	CObject vader;
	vader.plyFileName = L"PlyFiles\\TIEAdvanced_x1_Prototype_(Vader's_Tie)_(Simplified).ply";
	vader.shaderFileName = L"MultipleTextures.fx";
	vader.position = XMFLOAT3(-18.0f, 30.0f, 20.0f);
	vader.texture1FileName = L"Textures\\Metal\\03.jpg";
	vader.texture2FileName = L"Textures\\Metal\\01.jpg";
	vader.material = getRandomMaterial();
	vader.scale = 0.06f;
	vader.rotation.x = XMConvertToRadians(90.0f);
	//vader.isTransparent = true;
	vader.material.ambient.w = -0.9f;
	vader.isWireframe = true;
	Add3DModelToTheWorld(vader);

	//RESET WIREFRAME
	vader.isWireframe = false;

	vader.position = XMFLOAT3(18.0f, 30.0f, 20.0f);
	Add3DModelToTheWorld(vader);

	//THIRD ROW - INTERCEPTORS

	CObject interceptor;
	interceptor.plyFileName = L"PlyFiles\\Tie_Interceptor_(Simplified).ply";
	interceptor.shaderFileName = L"MultipleTextures.fx";
	interceptor.position = XMFLOAT3(-28.0f, 15.0f, 20.0f);
	interceptor.texture1FileName = L"Textures\\Metal\\01.jpg";
	interceptor.texture2FileName = L"Textures\\Metal\\04.jpg";
	interceptor.material = getRandomMaterial();
	interceptor.scale = 1.3f;
	interceptor.rotation.x = XMConvertToRadians(90.0f);
	interceptor.isTransparent = true;
	interceptor.material.ambient.w = -0.85f;
	Add3DModelToTheWorld(interceptor);

	//Second Interceptor
	interceptor.position = XMFLOAT3(0.0f, 15.0f, 20.0f);
	interceptor.isWireframe = true;
	interceptor.material.ambient.w = 1.0f;
	Add3DModelToTheWorld(interceptor);

	//RESET WIREFRAME BEFORE 3rd GUY
	interceptor.isWireframe = false;

	//Third Interceptor
	interceptor.position = XMFLOAT3(28.0f, 15.0f, 20.0f);
	interceptor.material.ambient.w = -0.91f;
	Add3DModelToTheWorld(interceptor);

	//Corellian badboys
	CObject corellian;
	corellian.plyFileName = L"PlyFiles\\Corellian_Corvette_(Blockade_Runner)_Simplified.ply";
	corellian.shaderFileName = L"OneLightOneTexture.fx";
	corellian.position = XMFLOAT3(-15.0f, -5.0f, 20.0f);
	corellian.texture1FileName = L"Textures\\Metal\\01.jpg";
	corellian.texture2FileName = L"Textures\\Metal\\01.jpg";
	corellian.material = getRandomMaterial();
	corellian.scale = 1.3f;
	corellian.rotation.x = XMConvertToRadians(270.0f);
	//corellian.isTransparent = true;
	corellian.material.ambient.w = -0.9f;
	Add3DModelToTheWorld(corellian);

	//Second Corellian Beast-Mode!
	corellian.position = XMFLOAT3(15.0f, -5.0f, 20.0f);
	corellian.isWireframe = true;
	Add3DModelToTheWorld(corellian);
	

	



	



	/**********************************************/
	//THE LIGHT BEING USED WITH ONELIGHTONETEXTURE SHADER
	//LIGHTS

	//corellian.position = XMFLOAT3(-15.0f, -5.0f, 20.0f);

	// Update the information in the light....
	g_cbNeverChanges.lights[0].material.diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	g_cbNeverChanges.lights[0].material.ambient = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);

	g_cbNeverChanges.lights[0].material.spec = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	g_cbNeverChanges.lights[0].pos = XMFLOAT4(-16.0f, 0.0f, 4.0f, 1.0f);

	//Direction is used only by directional lights and spotlights
	g_cbNeverChanges.lights[0].dir = XMFLOAT4(0.25f, -1.0f, 0.0f, 1.0f);

	//Set attenuation.
	g_cbNeverChanges.lights[0].attenuation = XMFLOAT4(0.008f, 0.008f, 0.008f, 1.0f);

	//Directional lights do not use range property.
	// x = power, y = range, z = light type, w = not used
	g_cbNeverChanges.lights[0].lightPowerRangeType.x = 5.0f; // Power
	g_cbNeverChanges.lights[0].lightPowerRangeType.y = 10.0f; // Range
	// Type ( 0 = parallel, 1 = point, >1 = spot )
	g_cbNeverChanges.lights[0].lightPowerRangeType.z = 1.0f;  //light type
	g_cbNeverChanges.lights[0].lightPowerRangeType.w = 1.0f; //not used


	/*****************LIGHT 2 -> RED ENGINE******************/
	// Update the information in the light....
	g_cbNeverChanges.lights[1].material.diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	g_cbNeverChanges.lights[1].material.ambient = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);

	g_cbNeverChanges.lights[1].material.spec = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	g_cbNeverChanges.lights[1].pos = XMFLOAT4(17.0f, 0.0f, 4.0f, 1.0f);

	//Direction is used only by directional lights and spotlights
	g_cbNeverChanges.lights[1].dir = XMFLOAT4(0.25f, -1.0f, 0.0f, 1.0f);

	//Set attenuation.
	g_cbNeverChanges.lights[1].attenuation = XMFLOAT4(0.008f, 0.008f, 0.008f, 1.0f);

	//Directional lights do not use range property.
	// x = power, y = range, z = light type, w = not used
	g_cbNeverChanges.lights[1].lightPowerRangeType.x = 5.0f; // Power
	g_cbNeverChanges.lights[1].lightPowerRangeType.y = 10.0f; // Range
	// Type ( 0 = parallel, 1 = point, >1 = spot )
	g_cbNeverChanges.lights[1].lightPowerRangeType.z = 1.0f;  //light type
	g_cbNeverChanges.lights[1].lightPowerRangeType.w = 1.0f; //not used




	// Start the timer...
	::g_TheTimer.Start();

	//--------------------------------------------------------------------------------------
	// Main message loop
	//--------------------------------------------------------------------------------------
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			// Determine how many seconds have gone by
			//	since the last call to this function...
			g_TheTimer.Stop();
			float deltaSeconds = ::g_TheTimer.GetElapsedSeconds();
			g_TheTimer.Reset();
			g_TheTimer.Start();

			SimulationStep(deltaSeconds);

            Render();
        }
    }


    CleanupDevice();

    return ( int )msg.wParam;
}



void Add3DModelToTheWorld(CObject model)
{
	g_vecObjects.push_back(model);
}


