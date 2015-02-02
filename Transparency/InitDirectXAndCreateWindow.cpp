#include "global.h"

bool InitDirectXAndCreateWindow( HINSTANCE hInstance, int nCmdShow, 
	std::wstring title, 
	UINT width, UINT height, 
	std::wstring &error )
{
	error = L"OK";
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = (HICON) LoadImage(hInstance,L"directx.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE|LR_SHARED);
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"DirectXProject";
	wcex.hIconSm = NULL;
	if( !RegisterClassEx( &wcex ) )
	{
		error = L"ERROR: Could not register window class";
		return false;
	}

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	g_hWnd = CreateWindow( L"DirectXProject", title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL );
	if( !g_hWnd )
	{
		error = L"Could not create window.";
		g_hWnd = 0;	// The handle to the window is invalid, so clear it.
		return false;
	}

	ShowWindow( g_hWnd, nCmdShow );

	// Now we init DirectX...
	HRESULT hr = S_OK;

	// We are getting the size of the window we just created.
	GetClientRect( g_hWnd, &rc );
	g_windowWidth = rc.right - rc.left;
	g_windowHeight = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
	//#ifdef _DEBUG
	//    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//#endif
	//
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_windowWidth;
	sd.BufferDesc.Height = g_windowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
		if( SUCCEEDED( hr ) )
			break;
	}
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create the DirectX device or SwapChain.";
		return false;
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not get at the back buffer.";
		return false;
	}

	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not set the render target to the back buffer.";
		return false;
	}


	// NOW, the depth-stencil buffer...
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof( descDepth ) );
	descDepth.Width = g_windowWidth;
	descDepth.Height = g_windowHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D( &descDepth, 
		NULL, 
		&g_pDepthStencilBuffer );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create the depth-stencil buffer.";
		return false;
	}
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof( descDSV ) );
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencilBuffer, 
		&descDSV, 
		&g_pDepthStencilView );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create view of depth-stencil buffer.";
		return false;
	}	

	g_pImmediateContext->OMSetRenderTargets( 1, 
		&g_pRenderTargetView, 
		g_pDepthStencilView );

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>( g_windowWidth );
	vp.Height = static_cast<FLOAT>( g_windowHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports( 1, &vp );


	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBNeverChanges);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_pCBNeverChanges constant buffer.";
		return false;
	}

	bd.ByteWidth = sizeof(CBChangeOnResize);
	hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangeOnResize );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_pCBChangeOnResize constant buffer.";
		return false;
	}

	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangesEveryFrame );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_pCBChangesEveryFrame constant buffer.";
		return false;
	}


	// Texture stuff...
	// *NEW* (sort of) This is where you specify what the sampler state in the shader is. 

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to set the samLinear texture sampling state.";
		return false;
	}

	sampDesc.Filter = ::D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 4;
	hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerAnisotropic );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to set the samAnisotropic texture sampling state.";
		return false;
	}



	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	hr=g_pd3dDevice->CreateRasterizerState(&wireframeDesc, &g_pWireframeRS);
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_pWireframeRS render state.";
		return false;
	}



	//SkyMap
	D3D11_RASTERIZER_DESC skyMapDesc;
	ZeroMemory(&skyMapDesc, sizeof(D3D11_RASTERIZER_DESC));
	skyMapDesc.FillMode = D3D11_FILL_SOLID;
	skyMapDesc.CullMode = D3D11_CULL_NONE;
	//skyMapDesc.FrontCounterClockwise = false;
	//skyMapDesc.DepthClipEnable = true;

	hr=g_pd3dDevice->CreateRasterizerState(&skyMapDesc, &g_RSCullNone);
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_RSCullNone rasterizer.";
		return false;
	}
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr=g_pd3dDevice->CreateDepthStencilState(&dssDesc, &g_DSLessEqual);
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create g_DSLessEqual stencil state.";
		return false;
	}

	//Transparency
	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;//D3D11_BLEND_OP_MIN;//D3D11_BLEND_OP_SUBTRACT;//D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = g_pd3dDevice->CreateBlendState(&transparentDesc, &g_BSTransparency);
	if (FAILED(hr))
	{
		error = L"ERROR: Can't create g_BSTransparency blender state.";
		return false;
	}



	return true;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if( g_pImmediateContext ) g_pImmediateContext->ClearState();

	if( g_pVertexBuffer ) g_pVertexBuffer->Release();
	if( g_pVertexLayout ) g_pVertexLayout->Release();

	if( g_pRenderTargetView ) g_pRenderTargetView->Release();
	if( g_pSwapChain ) g_pSwapChain->Release();
	if( g_pImmediateContext ) g_pImmediateContext->Release();
	if( g_pd3dDevice ) g_pd3dDevice->Release();
	//
	if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
	if(g_pCBChangeOnResize) g_pCBChangeOnResize->Release();
	if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
	if(g_pWireframeRS) g_pWireframeRS->Release();

	//Release vertex shaders
	std::map<std::wstring,ID3D11VertexShader*>::iterator itvs = g_mVertexShader.begin();
	for(itvs; itvs != g_mVertexShader.end();itvs++)
	{
		if (itvs->second)
		{
			itvs->second->Release();
		}
	}
	//Release pixel shaders
	std::map<std::wstring,ID3D11PixelShader*>::iterator itpx = g_mPixelShader.begin();
	for (itpx; itpx != g_mPixelShader.end();itpx++)
	{
		if (itpx->second)
		{
			itpx->second->Release();
		}
	}

	//Release textures
	std::map<std::wstring, ID3D11ShaderResourceView*>::iterator itTextures = g_mTextures.begin();
	for (itTextures; itTextures != g_mTextures.end(); itTextures++)
	{
		if (itTextures->second)
		{
			itTextures->second->Release();
		}
	}

	//Release filters
	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pSamplerAnisotropic) g_pSamplerAnisotropic->Release();


	if(g_DSLessEqual) g_DSLessEqual->Release();
	if(g_RSCullNone) g_RSCullNone->Release();

	if (g_BSTransparency) g_BSTransparency->Release();
}