#include "global.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11DepthStencilView*		g_pDepthStencilView = NULL;
ID3D11RasterizerState* g_pWireframeRS = NULL;
ID3D11Texture2D*			g_pDepthStencilBuffer = NULL;

//Constant buffers
ID3D11Buffer*			g_pCBNeverChanges = NULL;
ID3D11Buffer*			g_pCBChangeOnResize = NULL;
ID3D11Buffer*			g_pCBChangesEveryFrame = NULL;

CBNeverChanges		g_cbNeverChanges;
CBChangeOnResize	g_cbChangeOnResize;
CBChangesEveryFrame	g_cbChangesEveryFrame;

UINT g_windowWidth = 0;
UINT g_windowHeight = 0;

std::map<std::wstring, CPlyInfo> g_mapPlyInfo;
std::vector<CObject> g_vecObjects;
CHRTimer g_TheTimer;
ConfigSettings g_csCurrentSelection = NONE;
ConfigSettingsDirection g_csCurrentDirection = INCREASE;
std::map<std::wstring,ID3D11VertexShader*> g_mVertexShader;
std::map<std::wstring, ID3D11PixelShader*> g_mPixelShader;

std::map<std::wstring, ID3D11ShaderResourceView*> g_mTextures; //Week 9
ID3D11SamplerState*		g_pSamplerLinear = NULL; //Week 9
ID3D11SamplerState*		g_pSamplerAnisotropic = NULL; // Week 9

std::vector<std::wstring> g_vecBoltAnimations;

CCamera g_Camera;

std::vector<std::wstring> g_mBoltTextures;

ID3D11DepthStencilState*	g_DSLessEqual = NULL;
ID3D11RasterizerState*		g_RSCullNone = NULL;
ID3D11BlendState*			g_BSTransparency = NULL;

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		if( pErrorBlob ) pErrorBlob->Release();
		return hr;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------
MaterialInfo getRandomMaterial(void)
{
	MaterialInfo randMaterial;
	// Place 1.0f in all the 4th locations (so math will work or ambient will be 'off')
	randMaterial.ambient.w = 1.0f;
	randMaterial.diffuse.w = 1.0f;
	randMaterial.spec.w = 1.0f;

	float ambMin = 0.1f; 
	float ambMax = 1.0f;
	randMaterial.ambient.x = getRand<float>(ambMin, ambMax);
	randMaterial.ambient.y = getRand<float>(ambMin, ambMax);
	randMaterial.ambient.z = getRand<float>(ambMin, ambMax);

	float diffMin = 0.1f;
	float diffMax = 1.0f;
	randMaterial.diffuse.x = getRand<float>(diffMin, diffMax);
	randMaterial.diffuse.y = getRand<float>(diffMin, diffMax);
	randMaterial.diffuse.z = getRand<float>(diffMin, diffMax);

	
	float specMin = 0.1f;
	float specMax = 1.0f;
	randMaterial.spec.x = getRand<float>(specMin, specMax);
	randMaterial.spec.y = getRand<float>(specMin, specMax);
	randMaterial.spec.z = getRand<float>(specMin, specMax);

	return randMaterial;
}

void WriteLineToConsole(std::wstring text)
{
	CConsole::getConsole()->WriteLineToConsole(text);
}

void WriteLineToConsole(std::wstring text, float value)
{
	std::wostringstream wss; 
	wss << text << value;
	CConsole::getConsole()->WriteLineToConsole(wss.str());
}

void WriteLineToConsole(std::wstring text, int value)
{
	std::wostringstream wss; 
	wss << text << value;
	CConsole::getConsole()->WriteLineToConsole(wss.str());
}

//This function can load any of the following image formats: 
// BMP, JPG, PNG, DDS, TIFF, GIF, and WMP.
bool LoadTexture(std::wstring textureFile, std::wstring &error)
{
	HRESULT hr = S_OK;

	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice,
		textureFile.c_str(),
		NULL,
		NULL,
		&g_mTextures[textureFile],
		NULL);

	if (FAILED(hr))
	{
		error = L"Unable to load texture: " + textureFile;
		return false;
	}


	return true;

}