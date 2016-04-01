#include "DeferredShader.h"

DeferredShader::DeferredShader()
{
	gVertexShader = 0;
	gVixelShader = 0;
	gVertexlayout = 0;
	sampleStateWrap = 0;
	matrixBuffer = 0;
}

DeferredShader::DeferredShader(const DeferredShader &)
{

}

DeferredShader::~DeferredShader()
{
}

void DeferredShader::initialize(ID3D11Device *, ID3D11DeviceContext *, HWND)
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);


	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();
	//create vertex shader
	ID3DBlob* pVSbs = nullptr;
	D3DCompileFromFile(
		L"VertexShaderBS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVSbs,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);


	gDevice->CreateVertexShader(pVSbs->GetBufferPointer(), pVSbs->GetBufferSize(), nullptr, &gVertexShaderBS);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDescBS[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HRESULT hr = gDevice->CreateInputLayout(inputDescBS, ARRAYSIZE(inputDescBS), pVSbs->GetBufferPointer(), pVSbs->GetBufferSize(), &gVertexLayoutBS);
	// we do not need anymore this COM object, so we release it.

	pVSbs->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	//Creating a Sampler for the Pixel Shader
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.MipLODBias = 0;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; //Maybe this does something bad? Hmm... Keep an eye out.
	sampDesc.BorderColor[0] = 1.f;
	sampDesc.BorderColor[1] = 1.f;
	sampDesc.BorderColor[2] = 0.f;
	sampDesc.BorderColor[3] = 1.f; //Not sure why this RGB value's w value must be 1... Alpha maybe?
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 12;

	hr = gDevice->CreateSamplerState(&sampDesc, &gPSTextureSampler);

	//Creating Geometry Shader...
	ID3DBlob* pGS = nullptr; //This may be used for error handling!
	D3DCompileFromFile(
		L"GeometryShader.hlsl", //The L here specifies unicode vs. ansii... Dunno exactly.
		nullptr,
		nullptr,
		"GS_main",
		"gs_4_0",
		0,
		0,
		&pGS,
		nullptr
		);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();
}

void DeferredShader::shutdown()
{

}

bool DeferredShader::Render(int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView *)
{
	return false;
}

void DeferredShader::initializeShader(WCHAR*, WCHAR*)
{

}

void DeferredShader::shutdownShader()
{

}

void DeferredShader::outputShaderErrorMessage(ID3DBlob*, WCHAR)
{

}

void DeferredShader::outputShaderErrorMessage(ID3DBlob *, WCHAR)
{

}

void DeferredShader::setShaderParameters(XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView *)
{

}

void DeferredShader::RenderShader(int)
{

}
