#include "global.h"

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	// Clear the back buffer 
	//float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	float ClearColor[4] = { 0.69f, 0.77f, 0.87f, 1.0f};
	g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

	// Clear the depth buffer to 1.0 (max depth)
	::g_pImmediateContext->ClearDepthStencilView( ::g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// Initialize the world matrices
	XMMATRIX localMatrixWorld = XMMatrixIdentity();
	XMMATRIX localMatrixView = XMMatrixIdentity();
	XMMATRIX localMatrixProjection = XMMatrixIdentity();


	//XMMatrixPerspectiveFovLH
	g_Camera.SetLens( 0.25f * XM_PI, static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight), 1.0f, 1000.0f);

	g_Camera.UpdateViewMatrix();

	XMFLOAT3 camEye = g_Camera.GetPosition();
	g_cbNeverChanges.eye = XMFLOAT4(camEye.x, camEye.y, camEye.z, 1.0f);

	XMFLOAT3 camTarget = g_Camera.GetLook();
	g_cbNeverChanges.target = XMFLOAT4(camTarget.x, camTarget.y, camTarget.z, 1.0f);

	localMatrixView = g_Camera.View(); //XMMatrixLookAtLH( Eye, At, Up );

	g_cbNeverChanges.mView = XMMatrixTranspose( localMatrixView );

	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &g_cbNeverChanges, 0, 0 );



	localMatrixProjection = g_Camera.Proj();

	g_cbChangeOnResize.mProjection = XMMatrixTranspose( localMatrixProjection );
	g_pImmediateContext->UpdateSubresource( g_pCBChangeOnResize, 0, NULL, &g_cbChangeOnResize, 0, 0 );

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

//Set blend state.
g_pImmediateContext->OMSetBlendState(g_BSTransparency, blendFactor, 0xffffffff);

//Restore blend state.
g_pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);


	//Draw all objects
	std::vector<CObject>::iterator itObject = g_vecObjects.begin();

	for(itObject; itObject != g_vecObjects.end(); itObject++)
	{


		// Initialize the world matrix
		localMatrixWorld = XMMatrixIdentity();

		// Take the data from the Object iterator and load 
		//	a bunch of matrices with those values and set the 
		//	world transform...
		XMMATRIX matTranslate;							//D3DXMATRIX
		XMMATRIX matRotateX, matRotateY, matRotateZ;		//D3DXMATRIX
		XMMATRIX matFinal;								//D3DXMATRIX

		matTranslate = XMMatrixIdentity();
		matRotateX = XMMatrixIdentity();
		matRotateY = XMMatrixIdentity();
		matRotateZ = XMMatrixIdentity();

		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		

		if (itObject->isTransparent) {
			//Set blend state.
			g_pImmediateContext->OMSetBlendState(g_BSTransparency, blendFactor, 0xffffffff);
		}
		
		if (itObject->isWireframe) {
			g_pImmediateContext->RSSetState(g_pWireframeRS);
		}
		

		if (itObject->isSky)
		{
			//For sky map, use camera position. 
			itObject->position = g_Camera.GetPosition();


			g_pImmediateContext->OMSetDepthStencilState(g_DSLessEqual, 0);
			g_pImmediateContext->RSSetState(g_RSCullNone);
			//Set cube map
			g_pImmediateContext->PSSetShaderResources( 2, 1, &g_mTextures[itObject->texture1FileName] ); 
		}


		// Translation matrix...
		matTranslate = ::XMMatrixTranslation(itObject->position.x, itObject->position.y, itObject->position.z);
		//Three rotational matrices...
		matRotateX = XMMatrixRotationX(itObject->rotation.x);
		matRotateY = XMMatrixRotationY(itObject->rotation.y);
		matRotateZ = XMMatrixRotationZ(itObject->rotation.z);

		matFinal = XMMatrixIdentity();

		XMMATRIX matScale; 
		matScale = XMMatrixIdentity();
		matScale = XMMatrixScaling(itObject->scale, itObject->scale, itObject->scale);
		matFinal = XMMatrixMultiply(matFinal, matScale);

		matFinal = ::XMMatrixMultiply( matFinal, matRotateX );
		matFinal = ::XMMatrixMultiply( matFinal, matRotateY );
		matFinal = ::XMMatrixMultiply( matFinal, matRotateZ );

		matFinal = ::XMMatrixMultiply( matFinal, matTranslate );

		localMatrixWorld = XMMatrixMultiply( localMatrixWorld, matFinal );
		g_cbChangesEveryFrame.mWorld = XMMatrixTranspose( localMatrixWorld );

		//Update material
		//ambient
		g_cbChangesEveryFrame.colour.ambient = itObject->material.ambient;


		//diffuse
		g_cbChangesEveryFrame.colour.diffuse = itObject->material.diffuse;


		//specular
		g_cbChangesEveryFrame.colour.spec = itObject->material.spec;


		//Update texOffsetAndZoom
		g_cbChangesEveryFrame.texOffsetAndZoom[0].Offset = itObject->textOffsetAndZoom[0].Offset;
		g_cbChangesEveryFrame.texOffsetAndZoom[0].Zoom = itObject->textOffsetAndZoom[0].Zoom;
		g_cbChangesEveryFrame.texOffsetAndZoom[1].Offset = itObject->textOffsetAndZoom[1].Offset;
		g_cbChangesEveryFrame.texOffsetAndZoom[1].Zoom = itObject->textOffsetAndZoom[1].Zoom;


		g_pImmediateContext->UpdateSubresource( g_pCBChangesEveryFrame, 0, NULL, &g_cbChangesEveryFrame, 0, 0 );

		UINT stride = sizeof( SimpleVertex );
		UINT offset = 0;

		//Set vertex buffer
		g_pImmediateContext->IASetVertexBuffers(0,1,&(g_mapPlyInfo[itObject->plyFileName].vertexBuffer), &stride, &offset);
		//Set index buffer
		g_pImmediateContext->IASetIndexBuffer(g_mapPlyInfo[itObject->plyFileName].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set primitive topology
		g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		//If no vertex shader continue.
		if(!g_mVertexShader[itObject->shaderFileName])continue;
		// Set vertex shader
		g_pImmediateContext->VSSetShader( g_mVertexShader[itObject->shaderFileName], NULL, 0 );
		g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
		g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
		g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );

		//If no pixel shader continue.
		if(!g_mPixelShader[itObject->shaderFileName])continue;
		// Set pixel shader
		g_pImmediateContext->PSSetShader( g_mPixelShader[itObject->shaderFileName], NULL, 0 );
		g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
		g_pImmediateContext->PSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );

		//Set texture 1
		g_pImmediateContext->PSSetShaderResources( 0, 1, &g_mTextures[itObject->texture1FileName] ); 
		//Set texture 2
		g_pImmediateContext->PSSetShaderResources(1,1, &g_mTextures[itObject->texture2FileName]);

		g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear ); 
		g_pImmediateContext->PSSetSamplers( 1, 1, &g_pSamplerAnisotropic ); 


		//Draw object
		g_pImmediateContext->DrawIndexed(g_mapPlyInfo[itObject->plyFileName].numberOfElementsToDraw * 3, 0, 0);

		// Restore default render state.
		g_pImmediateContext->RSSetState(0);

		// Clear depth stencil state.
		g_pImmediateContext->OMSetDepthStencilState(NULL, 0);

		//Restore blend state.
		g_pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);


	}

	// Present the information rendered to the back buffer to the front buffer (the screen)
	g_pSwapChain->Present( 0, 0 );

	return;
}