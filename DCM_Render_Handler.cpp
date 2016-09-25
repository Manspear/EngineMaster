#include "DCM_Render_Handler.h"



DCM_Render_Handler::DCM_Render_Handler()
{
}

DCM_Render_Handler::~DCM_Render_Handler()
{
}

void DCM_Render_Handler::Initialize(ID3D11Device * gDevice, ID3D11DepthStencilView * depthStencilView, ID3D11RenderTargetView * gBackbufferRTV, D3D11_VIEWPORT &vp,
	ID3D11GeometryShader* gGeometryShader, ID3D11PixelShader* gPixelShader, ID3D11VertexShader* gVertexShader, ID3D11InputLayout* gVertexLayout,
	ID3D11Buffer* gConstantBuffer, ID3D11VertexShader* gVertexShaderBS, ID3D11InputLayout* gVertexLayoutBS, ID3D11DeviceContext* gDeviceContext)
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
	this->gDeviceContext = gDeviceContext;
}


void DCM_Render_Handler::DCM_Render(GModel* model, DCM *dcm)
{
	// Generate the cube map by rendering to each cube map face.
	gDeviceContext->RSSetViewports(1, &model->dcm.getDCM_CubeMapViewport());// listOfModels->dcm.getDCM_CubeMapViewport
	for (int i = 0; i < 6; i++)
	{
		// Clear cube map face Render Target View and Depth Buffer.
		gDeviceContext->ClearRenderTargetView(dcm->getDCM_RenderTargetView(i), reinterpret_cast<const float*>(&Colors::Silver));
		gDeviceContext->ClearDepthStencilView(dcm->getDCM_DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		ID3D11RenderTargetView* DCM_RenderTargetView2 = dcm->getDCM_RenderTargetView(i);
		gDeviceContext->OMSetRenderTargets(1, &DCM_RenderTargetView2, dcm->getDCM_DepthStencilView());

		//Draw the scene with exception of the center sphere, to this cube map face
		Render_Enviroment(model);//		med DCM vertex // pixelshader?
	}

	// Restore old viewport and render targets.
	gDeviceContext->RSSetViewports(1, &vp);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);

	// Have hardware generate lower mipmap levels of cube map.
	gDeviceContext->GenerateMips(dcm->getShaderResourceView());

	// Now draw the scene as normal
	//Render(); //vanliga
}

void DCM_Render_Handler::Render_Enviroment(GModel* model)
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

	listOfModels = modelList.getModelList();

	for (int i = 0; i < modelList.numberOfModels; i++)
	{
		if (listOfModels[i].hasBlendShape())
		{
			gDeviceContext->VSSetShader(gVertexShaderBS, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayoutBS);
			gDeviceContext->VSSetConstantBuffers(0, 1, &listOfModels[i].bsWBuffer);
			vertexSize = sizeof(float) * 16;
		}
		else if (!listOfModels[i].hasDCM())
		{
			gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayout);
			vertexSize = sizeof(float) * 8;
		}

		gDeviceContext->GSSetConstantBuffers(1, 1, &model[i].modelConstantBuffer);

		//each model only one vertex buffer. Exceptions: Objects with separate parts, think stone golem with floating head, need one vertex buffer per separate geometry.
		gDeviceContext->PSSetShaderResources(0, model[i].getNumberOfTextures(), model[i].modelTextureView);

		gDeviceContext->IASetVertexBuffers(0, 1, &model[i].modelVertexBuffer, &vertexSize, &offset);
		gDeviceContext->IASetIndexBuffer(model[i].modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		gDeviceContext->DrawIndexed(model[i].modelVertices.size(), 0, 0);
	}
}