#include "global.h"

std::string UNICODEtoASCII( std::wstring UNICODEstring )
{
	std::stringstream ssReturnASCII;
	for ( std::wstring::iterator itChar = UNICODEstring.begin(); itChar != UNICODEstring.end(); itChar++ )
	{
		char theChar = static_cast<char>( *itChar );
		ssReturnASCII << theChar;
	}
	return ssReturnASCII.str();
}

std::wstring ASCIItoUNICODE( std::string ASCIIstring )
{
	std::wstringstream ssReturnUNICODE;
	for ( std::string::iterator itChar = ASCIIstring.begin(); itChar != ASCIIstring.end(); itChar++ )
	{
		wchar_t theChar = static_cast<wchar_t>( *itChar );
		ssReturnUNICODE << theChar;
	}
	return ssReturnUNICODE.str();
}

// This is based on the helper file from the DirectX 11 SDK
// Uses all wstring (instead of a mix of unicode and ASCII)
// Returns the compiled shader in ppBlobOut.
// false if error with error text in 'error' wstring.
bool CompileShaderFromFile( std::wstring shaderFileName, 
	std::wstring entryPoint, 
	std::wstring shaderModel, 
	ID3DBlob** ppBlobOut, 
	bool bDebug, std::wstring &error )
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	if ( bDebug )	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// 
	ID3DBlob* pErrorBlob = NULL;
	std::string ASCIIentryPoint = UNICODEtoASCII( entryPoint);
	std::string ASCIIshaderModel = UNICODEtoASCII( shaderModel);
	hr = D3DX11CompileFromFile( shaderFileName.c_str(), NULL, NULL, 
		ASCIIentryPoint.c_str(), 
		ASCIIshaderModel.c_str(), 
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
		{
			std::string ASCIIerror( (char*)pErrorBlob->GetBufferPointer() );
			error = ASCIItoUNICODE ( ASCIIerror );
		}
		if( pErrorBlob ) pErrorBlob->Release();

		return false;
	}
	// Release the blob if was OK
	if( pErrorBlob ) pErrorBlob->Release();

	return true;
}

bool LoadAndPrepareShadersDX11( std::wstring vertexShaderFileName, std::wstring vertexShaderEntryPoint, std::wstring vertexShaderModel,
	std::wstring pixelShaderFileName, std::wstring pixelShaderEntryPoint, std::wstring pixelShaderModel,
	std::wstring &error)
{
	HRESULT hr = S_OK;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	if ( !CompileShaderFromFile( vertexShaderFileName, vertexShaderEntryPoint, vertexShaderModel, &pVSBlob, true, error ) )
	{
		std::wstringstream ssError;
		ssError << L"VS ERROR: Could not compile the " << vertexShaderFileName 
			<< L" (" << vertexShaderEntryPoint << L")." << std::endl
			<< error 
			<< L"Sorry it didn't work out.";
		error = ssError.str();

		MessageBox( NULL, error.c_str(), L"Load vertex/pixel shaders - Something is wrong", MB_OK );

		return false;
	}


	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &(g_mVertexShader[vertexShaderFileName]) );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
		//return hr;
		error = L"ERROR: Could not create Vertex Shader: " + vertexShaderFileName;
		return false;
	}

	if (!g_pVertexLayout)
	{

		// Define the input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 } // Week_9

		};
		UINT numElements = ARRAYSIZE( layout );

		// Create the input layout
		hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(), &g_pVertexLayout );
		pVSBlob->Release();
		if( FAILED( hr ) )
		{	

			error = L"ERROR: Could not create Input Layout.";
			return false;
		}

		// Set the input layout
		g_pImmediateContext->IASetInputLayout( g_pVertexLayout );


	}
	
	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	if ( !CompileShaderFromFile( pixelShaderFileName, pixelShaderEntryPoint, pixelShaderModel, &pPSBlob, true, error ) )
	{
		std::wstringstream ssError;
		ssError << L"VS ERROR: Could not compile the " << pixelShaderFileName 
			<< L" (" << pixelShaderEntryPoint << L")." << std::endl
			<< error 
			<< L"Sorry it didn't work out.";		
		error = ssError.str();
		return false;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &(g_mPixelShader[pixelShaderFileName]) );
	pPSBlob->Release();
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create Pixel Shader:" + pixelShaderFileName;
		return false;
	}

	return true;
}