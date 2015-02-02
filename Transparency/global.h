#ifndef _global_HG_
#define _global_HG_


//Includes
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "CPlyInfo.h"
#include "CHRTimer.h"
#include "CConsole.h"
#include "CCamera.h"

enum ConfigSettings
{
	CAMERA = 0,
	LIGHT_POSITION = 1,
	LIGHT_DIRECTION = 2,
	LIGHT_ATTENUATION = 3,
	LIGHT_MATERIAL_AMBIENT = 4,
	LIGHT_MATERIAL_DIFFUSE = 5,
	LIGHT_MATERIAL_SPECULAR = 6,
	LIGHT_RANGE = 7,
	LIGHT_POWER = 8,
	NONE = 9

};



enum ConfigSettingsDirection
{
	INCREASE = 0,
	DECREASE = 1
};



//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

#define  MAXTEX 2
struct TextDesc
{
	XMFLOAT2 Offset;
	XMFLOAT2 Zoom;
};

struct MaterialInfo
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;		// D3DXVECTOR4 -- float4 in shader
	XMFLOAT4 spec;
};

struct LightDesc
{
	XMFLOAT4 pos;		// D3DXVECTOR4
	XMFLOAT4 dir;		// D3DXVECTOR4
	MaterialInfo material;
	XMFLOAT4 attenuation;	// D3DXVECTOR4
	// x = power, y = range, z = light type, w = not used
	XMFLOAT4 lightPowerRangeType;	// D3DXVECTOR4
};

struct SimpleVertex
{
	XMFLOAT4 Pos;
	XMFLOAT4 Normal;
	XMFLOAT2 Tex; //Week 9
};

#define MAXLIGHTS 10


struct CBNeverChanges
{
	XMMATRIX mView;			//matrix View;
	XMFLOAT4 eye;			//float4 eye;
	XMFLOAT4 target;		//float4 target;
	LightDesc lights[MAXLIGHTS];
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
};


struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
	// ADD the colour of the bunny
	MaterialInfo colour;  

	TextDesc texOffsetAndZoom[MAXTEX];
};

//--------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------
class CObject
{
public:
	CObject();
	virtual ~CObject();
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	float scale;
	std::wstring plyFileName;
	MaterialInfo material;
	bool rotate;
	std::wstring shaderFileName;
	std::wstring texture1FileName;
	std::wstring texture2FileName;
	TextDesc textOffsetAndZoom[MAXTEX];
	bool isSky;
	bool isTransparent;
	bool isWireframe;

	void UpdateBoltTexture(float deltaSeconds, float timeInterval);
private:
	int boltTextIndex;
	float timeCounter;
};


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
extern HINSTANCE               g_hInst;
extern HWND                    g_hWnd;
extern D3D_DRIVER_TYPE         g_driverType;
extern D3D_FEATURE_LEVEL       g_featureLevel;
extern ID3D11Device*           g_pd3dDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRenderTargetView;
extern ID3D11InputLayout*      g_pVertexLayout;
extern ID3D11Buffer*           g_pVertexBuffer;
extern ID3D11DepthStencilView*		g_pDepthStencilView;
extern ID3D11RasterizerState* g_pWireframeRS;
extern ID3D11Texture2D*			g_pDepthStencilBuffer;

//Constant buffers
extern ID3D11Buffer*			g_pCBNeverChanges;
extern ID3D11Buffer*			g_pCBChangeOnResize;
extern ID3D11Buffer*			g_pCBChangesEveryFrame;

//Constant buffers
extern CBNeverChanges		g_cbNeverChanges;
extern CBChangeOnResize		g_cbChangeOnResize;
extern CBChangesEveryFrame	g_cbChangesEveryFrame;

extern UINT g_windowWidth;
extern UINT g_windowHeight;

extern std::map<std::wstring, CPlyInfo> g_mapPlyInfo;
extern std::vector<CObject> g_vecObjects;
extern CHRTimer g_TheTimer;

extern ConfigSettings g_csCurrentSelection;
extern ConfigSettingsDirection g_csCurrentDirection;
extern std::map<std::wstring,ID3D11VertexShader*> g_mVertexShader;
extern std::map<std::wstring, ID3D11PixelShader*> g_mPixelShader;

extern std::map<std::wstring, ID3D11ShaderResourceView*> g_mTextures; //Week 9
extern ID3D11SamplerState*		g_pSamplerLinear;// Week 9
extern ID3D11SamplerState*		g_pSamplerAnisotropic;	// Week 9


extern CCamera g_Camera;

extern std::vector<std::wstring> g_mBoltTextures;

extern ID3D11DepthStencilState* g_DSLessEqual;
extern ID3D11RasterizerState*	g_RSCullNone;
extern ID3D11BlendState*		g_BSTransparency; 

//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------
bool InitDirectXAndCreateWindow( HINSTANCE hInstance, int nCmdShow, 
	std::wstring title, 
	UINT width, UINT height, 
	std::wstring &error );

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

void Render();

void SimulationStep(float deltaSeconds); 

void CleanupDevice();

bool LoadModuleIntoBuffer(std::wstring plyFileName, std::wstring &error);


bool LoadAndPrepareShadersDX11( std::wstring vertexShaderFileName, std::wstring vertexShaderEntryPoint, std::wstring vertexShaderModel,
	std::wstring pixelShaderFileName, std::wstring pixelShaderEntryPoint, std::wstring pixelShaderModel,
	std::wstring &error);

template <class T>
T getRand(T lowRange, T highRange)
{
	if ( lowRange > highRange )
	{
		T temp = lowRange;
		lowRange = highRange;
		highRange = temp;
	}
	T delta = highRange - lowRange;
	delta = ( static_cast<T>( rand() ) / static_cast<T>(RAND_MAX) ) * delta;
	return delta + lowRange;
}

template <class T>
void writeLineToConsole(std::wstring text, T value)
{
	//T tmpText = text;
	std::wstringstream wss;
	wss << text << value;
	CConsole::getConsole()->WriteLineToConsole(wss.str());
	return;
}

MaterialInfo getRandomMaterial(void);

void WriteLineToConsole(std::wstring text);
void WriteLineToConsole(std::wstring text, float value);
void WriteLineToConsole(std::wstring text, int value);

bool LoadTexture(std::wstring textureFile, std::wstring &error); //Week 9

#endif