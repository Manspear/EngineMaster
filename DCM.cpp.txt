#include "DCM.h"



DCM::DCM()
{

}

DCM::~DCM()
{
}

void DCM::Initialize(ID3D11Device * gDevice, ID3D11DepthStencilView * depthStencilView, ID3D11RenderTargetView * gBackbufferRTV, D3D11_VIEWPORT &vp,
	ID3D11GeometryShader* gGeometryShader, ID3D11PixelShader* gPixelShader, ID3D11VertexShader* gVertexShader, ID3D11InputLayout* gVertexLayout,
	ID3D11Buffer* gConstantBuffer, ID3D11VertexShader* gVertexShaderBS, ID3D11InputLayout* gVertexLayoutBS)
{
	this->gDevice = gDevice;
	this->depthStencilView = depthStencilView;
	this->gBackbufferRTV = gBackbufferRTV;
	this->vp = vp;

	this->gGeometryShader = gGeometryShader;
	this->gPixelShader = gPixelShader;
	this->gVertexShader = gVertexShader;
	this->gVertexLayout = gVertexLayout;
	this->gConstantBuffer = gConstantBuffer;
	this->gVertexShaderBS = gVertexShaderBS;
	this->gVertexLayoutBS = gVertexLayoutBS;
}

void DCM::Dynamic_Cube_Map(ID3D11Device *gDevice)
{
	static const int CubeMapSize = 256;

	// Chapter I || Texture describtion
	//
	// Keywords
	// Resource = ("2D texture", "CreateTexture2D")
	// View = ("Depth Stencil View", "TEXTURECUBE"). Det föregående ska användas på de här sätten. Sättet VertexShadern, GeometryShadern eller PixelShadern ska tolka informationen.
	//
	// Cubemap is a special texture array with 6 elements (one for each face). Think of Autodesk Maya standard cube UV.
	// We bind this as a render target to draw to the cube faces,
	// and also as a shader resource, so we can use it in a pixel shader.
	//
	//This TEXTURE2D description with the D3D11_RESOURCE_MISC_TEXTURECUBE miscFlag included tells DirX to "tolka" the texture as a cube map
	//The D3D11_RESOURCE_MISC_GENERATE_MIPS right beside allows us to generateMips() later on in the engine.cpp
	//A mip map is that pyramid with textures, from high res to low res and the top. For distances
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	ID3D11Texture2D* cubeTex = 0;
	gDevice->CreateTexture2D(&texDesc, 0, &cubeTex);

	//
	// Chapter II || Render Target Views & Shader Resource View
	//
	// Create a Render Target View to each cube map face (each element in the texture array).
	//
	// *Reminder*. A render target is a buffer where the video card draws pixels for a scene.
	// The default render target is called the back buffer, this is the part of 
	// video memory that contains the next frame to be drawn.
	//
	// We need to add a render target for each face of the cube, making the face act like a screen.
	// This is done later on with this line inside of a for loop 
	// gDeviceContext->OMSetRenderTargets(1, &DCM_RenderTargetView[i], mDynamicCubeMapDSV);
	//
	// *Reminder* Bind 1 or more render targets atomically and the depth-stencil buffer to the output-merger stage.
	// The maximum number of active render targets a device can have active at any given time is set by a 
	// #define in D3D11.h called D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT
	//

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;
	for (int i = 0; i < 6; ++i)// Create a render target view to each face
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;// If we would put a 3 here, only the 3rd cubeface would be rendered. 
		gDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &DCM_RenderTargetView[i]);
	}

	//
	// And now we create a Shader Resource View.
	//
	// *Reminder* A Shader Resource View is designed to give the VertexShader, GeometryShader and PixelShader the resources of 
	// the shader (Texture2D, R8G8B8A8, MipLevels etc..)  and tells them how these recources should be used (TEXTURECUBE)
	//
	// You can create shader-resource views of video resources such as moving textures OR Cube Map Renderings
	// See reflective areas as video recordings live streaming
	// 

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;
	gDevice->CreateShaderResourceView(cubeTex, &srvDesc, &DCM_ShaderResourceView);
	cubeTex -> Release();// chansar eftersom ReleaseCOM(cubeTex); funkade inte. såg denna lösning på nätet
	
	//
	// Chapter III || Depth buffer & Viewport
	//
	// Generally the cube map faces will have a different resolution than the main back buffer. Your screen might have 4k resolution 
	// but that doesnt mean that you want your sword in the game to have 4k resolution as well.
	//
	// The cube map needs a Depth Stencil View for the same reason as the main camera needs it.
	//
	// *Reminder* A Depth Stencil View is used to sort out when two or more pixels take up the same location in the view frustum. Each pixels have a "z" value (xyz)
	// angled towards where you're rendering from, light or camera. Thats called depth value and we use that to determine which pixel to keep
	// 

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	gDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex);

	// and now for the Depth stencil view

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	gDevice->CreateDepthStencilView(depthTex, &dsvDesc, &DCM_DepthStencilView);

	depthTex->Release();

	//
	// and finally the faces viewports 
	//		

	DCM_CubeMapViewport.TopLeftX = 0.0f;
	DCM_CubeMapViewport.TopLeftY = 0.0f;
	DCM_CubeMapViewport.Width = (float)CubeMapSize;
	DCM_CubeMapViewport.Height = (float)CubeMapSize;
	DCM_CubeMapViewport.MinDepth = 0.0f;
	DCM_CubeMapViewport.MaxDepth = 1.0f;
}

void DCM::BuildCubeFaceCamera(float x, float y, float z, float w)
{
	// Generate the cube map about the given position
	XMFLOAT4 center(x, y, z, w);
	XMFLOAT4 worldUp(0.0f, 1.0f, 0.0f, 1.0f);

	// Look along each coordinate axis.
	XMFLOAT4 targets[6] =
	{
	XMFLOAT4(x + 1.0f, y, z, w), // +X
	XMFLOAT4(x - 1.0f, y, z, w), // -X

	XMFLOAT4(x, y + 1.0f, z, w), // +Y
	XMFLOAT4(x, y - 1.0f, z, w), // -Y

	XMFLOAT4(x, y, z + 1.0f, w), // +Z
	XMFLOAT4(x, y, z - 1.0f, w) // -Z
	};

	XMFLOAT4 ups[6] =
	{
		XMFLOAT4(0.0f, 1.0f, 0.0f, w), // +X
		XMFLOAT4(0.0f, 1.0f, 0.0f, w), // -X

		XMFLOAT4(0.0f, 0.0f, -1.0f, w), // +Y
		XMFLOAT4(0.0f, 0.0f, +1.0f, w), // -Y

		XMFLOAT4(0.0f, 1.0f, 0.0f, w), // +Z
		XMFLOAT4(0.0f, 1.0f, 0.0f, w) // -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		DCM_CubeMapCamera[i].LookAt(center, targets[i], ups[i]);
	}
}//end, moved drawscene to engine

void DCM::DCM_Render_Main(GModel* listOfModels, GModelList* modelListObject)
{

	// Generate the cube map by rendering to each cube map face.
	gDeviceContext->RSSetViewports(1, &DCM_CubeMapViewport);

	//vvvvvvvvvv	< Dynamic cube map >	vvvvvvvvvvvvv

	for (int i = 0; i < 6; i++)
	{
		gDeviceContext->ClearRenderTargetView(DCM_RenderTargetView[i], reinterpret_cast<const float*>(&Colors::Silver));//fortsätt läsa sid 486
		gDeviceContext->ClearDepthStencilView(DCM_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* DCM_RenderTargetView = &DCM_RenderTargetView[i]; // för den ville ha //&DCM_RenderTargetView[i]
																					   //Bind cube map face as render target
		gDeviceContext->OMSetRenderTargets(1, &DCM_RenderTargetView, DCM_DepthStencilView);

		//Draw the scene with exception of the center sphere, to this cube map face
		Render_Enviroment(listOfModels, modelListObject);//		med DCM vertex // pixelshader?  Hur når jag render utan Engine engine object
	}

	// Restore old viewport and render targets.
	gDeviceContext->RSSetViewports(1, &vp);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);


	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	// Have hardware generate lower mipmap levels of cube map.
	gDeviceContext->GenerateMips(DCM_ShaderResourceView);

	// Now draw the scene as normal, but WITH the CENTER SPHERE. // som jag i for(6) loopen genererade? HUR?
	//Render(); //vanliga
}

void DCM::Render_Enviroment(GModel* listOfModels, GModelList* modelListObject)
{
	float clearColor[] = { 1, 0.537, 0.812, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);

	UINT32 vertexSize;
	UINT32 offset = 0; //This <----, when handling multiple buffers on the same object, is equal to the length of the current buffer element in bytes. Otherwise 0.

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);

	listOfModels = modelListObject->getModelList();

	for (int bufferCounter = 0; bufferCounter < modelListObject->numberOfModels; bufferCounter++)
	{
		if (listOfModels[bufferCounter].hasBlendShape())
		{	// gör en check om dcm här också, blend shapes ska ju också kunna ha
			gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
			gDeviceContext->VSSetConstantBuffers(0, 1, &listOfModels[bufferCounter].bsWBuffer);
			gDeviceContext->VSSetShader(gVertexShaderBS, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayoutBS);
			vertexSize = sizeof(float) * 16;
		}
		else {
			gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
			gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayout);
			vertexSize = sizeof(float) * 8;
		}

		gDeviceContext->GSSetConstantBuffers(1, 1, &listOfModels[bufferCounter].modelConstantBuffer);

		//each model only one vertex buffer. Exceptions: Objects with separate parts, think stone golem with floating head, need one vertex buffer per separate geometry.
		gDeviceContext->PSSetShaderResources(0, listOfModels[bufferCounter].getNumberOfTextures(), listOfModels[bufferCounter].modelTextureView);

		gDeviceContext->IASetVertexBuffers(0, 1, &listOfModels[bufferCounter].modelVertexBuffer, &vertexSize, &offset);
		gDeviceContext->IASetIndexBuffer(listOfModels[bufferCounter].modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		gDeviceContext->DrawIndexed(listOfModels[bufferCounter].modelVertices.size(), 0, 0);
	}
}

ID3D11RenderTargetView * DCM::getDCM_RenderTargetView(int i)
{
	return this->DCM_RenderTargetView[i];
}

ID3D11DepthStencilView * DCM::getDCM_DepthStencilView()
{
	return this->DCM_DepthStencilView;
}

GCamera DCM::getDCM_CubeMapCamera(int i)
{
	return this->DCM_CubeMapCamera[i];
}

D3D11_VIEWPORT DCM::getDCM_CubeMapViewport()
{
	return this->DCM_CubeMapViewport;
}

ID3D11ShaderResourceView * DCM::getShaderResourceView()
{
	return this->DCM_ShaderResourceView;
}
