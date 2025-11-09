#include "ElectrickShock8.h"


//------------------------------------------------------------------------------------------
CElectrickShock8::CElectrickShock8()
{
	pD3DDevice = nullptr; 
	pD3DDeviceContext = nullptr;
	pNewVertexBuffer = nullptr;
	pPixelShader = nullptr;
	pD3DRenderTargetView = nullptr;
	pBlendState = nullptr;
	pPSConstantBuffer = nullptr;
	ZeroMemory(pNewVertices, 6 * sizeof(TVertex8));
	ZeroMemory(m_SliderValue, 5 * sizeof(float));
	m_PSConstantBufferData = {};
	m_DirectX_On = false;
	m_WidthOnDeviceInit = 0;
	m_HeightOnDeviceInit = 0;
	m_Width = 0;
	m_Height = 0;
	m_VertexCount = 0;
	m_alpha = 1.0f;
	m_length = 0.0f;
	m_SongPosBeatsPrevious = 0;
	m_ref_freq = 0;
	m_DetectLowFreq = 0;
	m_minFreq = 0;
	m_VideoScale = 1.0f;
	m_FX_Select = 0;
	m_FX_Activate = 0;
	m_FX_Activate_Previous = 0;
	m_FX_Time = 0.0f;
	m_FX_Time_Previous = 0;
	m_FX_Select_Random = 0;
}
//------------------------------------------------------------------------------------------
CElectrickShock8::~CElectrickShock8()
{

}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnLoad()
{
	HRESULT hr = S_FALSE;

	hr = DeclareParameterSlider(&m_SliderValue[0], ID_SLIDER_1, "VideoAlpha", "A", 0.0f);
	hr = DeclareParameterSlider(&m_SliderValue[1], ID_SLIDER_2, "VideoScale", "VS", 0.3f);
	hr = DeclareParameterSlider(&m_SliderValue[2], ID_SLIDER_3, "Length", "L", 0.25f);
	hr = DeclareParameterSlider(&m_SliderValue[3], ID_SLIDER_4, "FX Select", "FX", 0.0f);
	hr = DeclareParameterSwitch(&m_FX_Select_Random, ID_SWITCH_1, "FX Select Random", "FXR", 0.0f);

	#ifdef USE_FFT
	hr = DeclareParameterSlider(&m_SliderValue[4], ID_SLIDER_5, "MinFreq", "FQ", 0.3f);
	hr = DeclareParameterSwitch(&m_DetectLowFreq, ID_SWITCH_2, "Detect LowFreq", "LF", 0.0f);
	#endif

	OnParameter(ID_INIT);
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnGetPluginInfo(TVdjPluginInfo8 *info)
{
	info->Author = "djcel";
	info->PluginName = "ElectrickShock8";
	info->Description = "It acts like a negative effect at each beat.";
	info->Flags = 0x00; // VDJFLAG_VIDEO_OUTPUTRESOLUTION | VDJFLAG_VIDEO_OUTPUTASPECTRATIO;
	info->Version = "2.0.1 (64-bit)";

	return S_OK;
}
//------------------------------------------------------------------------------------------
ULONG VDJ_API CElectrickShock8::Release()
{
	delete this;
	return 0;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnParameter(int id)
{
	if (id == ID_INIT)
	{
		for (int i = ID_SLIDER_MIN; i <= ID_SLIDER_MAX; i++) OnSlider(i);
	}

	OnSlider(id);

	return S_OK;
}
//------------------------------------------------------------------------------------------
void CElectrickShock8::OnSlider(int id)
{
	switch (id)
	{
		case ID_SLIDER_1:
			m_alpha = 1.0f - m_SliderValue[0];
			break;

		case ID_SLIDER_2:
			m_VideoScale = m_SliderValue[1] * 0.5f;
			break;

		case ID_SLIDER_3:
			m_length = m_SliderValue[2];
			break;

		case ID_SLIDER_4:
			m_FX_Select = int(m_SliderValue[3] * float(MAX_FX));
			break;

		case ID_SLIDER_5:
			m_minFreq = MIN_FREQ + m_SliderValue[4] * (MAX_FREQ - MIN_FREQ);
			break;
	}
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnGetParameterString(int id, char* outParam, int outParamSize)
{
	switch (id)
	{
		case ID_SLIDER_1:
			sprintf_s(outParam, outParamSize, "%.0f%%", (1.0f - m_alpha) * 100);
			break;

		case ID_SLIDER_2:
			sprintf_s(outParam, outParamSize, "%.2f", m_VideoScale);
			break;

		case ID_SLIDER_3:
			sprintf_s(outParam, outParamSize, "%.2f beat", m_length);
			break;

		case ID_SLIDER_4:
			switch (m_FX_Select)
			{
				case 0:
					sprintf_s(outParam, outParamSize, "None");
					break;

				case 1:
					sprintf_s(outParam, outParamSize, "Negative");
					break;

				case 2:
					sprintf_s(outParam, outParamSize, "2 bands V");
					break;

				case 3:
					sprintf_s(outParam, outParamSize, "4 bands V");
					break;

				case 4:
					sprintf_s(outParam, outParamSize, "8 bands V");
					break;

				case 5:
					sprintf_s(outParam, outParamSize, "16 bands V");
					break;

				case 6:
					sprintf_s(outParam, outParamSize, "32 bands V");
					break;

				case 7:
					sprintf_s(outParam, outParamSize, "32 bands H");
					break;

				case 8:
					sprintf_s(outParam, outParamSize, "16 bands H");
					break;

				case 9:
					sprintf_s(outParam, outParamSize, "8 bands H");
					break;

				case 10:
					sprintf_s(outParam, outParamSize, "4 bands H");
					break;

				case 11:
					sprintf_s(outParam, outParamSize, "2 bands H");
					break;

				case 12:
					sprintf_s(outParam, outParamSize, "2 bands H+V");
					break;

				case 13:
					sprintf_s(outParam, outParamSize, "4 bands H+V");
					break;

				case 14:
					sprintf_s(outParam, outParamSize, "8 bands H+V");
					break;

				case 15:
					sprintf_s(outParam, outParamSize, "16 bands H+V");
					break;

				case 16:
					sprintf_s(outParam, outParamSize, "32 bands H+V");
					break;

				case 17:
					sprintf_s(outParam, outParamSize, "32 bands H+V Inv");
					break;

				case 18:
					sprintf_s(outParam, outParamSize, "16 bands H+V Inv");
					break;

				case 19:
					sprintf_s(outParam, outParamSize, "8 bands H+V Inv");
					break;

				case 20:
					sprintf_s(outParam, outParamSize, "4 bands H+V Inv");
					break;

				case 21:
					sprintf_s(outParam, outParamSize, "2 bands H+V Inv");
					break;

				case 22:
					sprintf_s(outParam, outParamSize, "Triangle");
					break;

				case 23:
					sprintf_s(outParam, outParamSize, "Circle");
					break;

				case 24:
					sprintf_s(outParam, outParamSize, "NegativeAdv1");
					break;

				case 25:
					sprintf_s(outParam, outParamSize, "NegativeAdv2");
					break;

				case 26:
					sprintf_s(outParam, outParamSize, "NegativeAdv3");
					break;

				case 27:
					sprintf_s(outParam, outParamSize, "NegativeAdv4");
					break;

				case 28:
					sprintf_s(outParam, outParamSize, "NegativeAdv5");
					break;
			}
			break;

		case ID_SLIDER_5:
			sprintf_s(outParam, outParamSize, "%.0f Hz", m_minFreq);
			break;
	}

	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnDeviceInit()
{
	HRESULT hr = S_FALSE;

	m_DirectX_On = true;
	m_WidthOnDeviceInit = width;
	m_HeightOnDeviceInit = height;
	m_Width = width;
	m_Height = height;

	hr = GetDevice(VdjVideoEngineDirectX11, (void**)  &pD3DDevice);
	if(hr!=S_OK || pD3DDevice==NULL) return E_FAIL;

	hr = Initialize_D3D11(pD3DDevice);

	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnDeviceClose()
{
	Release_D3D11();
	SAFE_RELEASE(pD3DRenderTargetView);
	SAFE_RELEASE(pD3DDeviceContext);
	m_DirectX_On = false;
	
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnStart() 
{
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnStop() 
{
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnDraw()
{
	HRESULT hr = S_FALSE;
	ID3D11ShaderResourceView *pTexture = nullptr;
	TVertex8* vertices = nullptr;

	if (width != m_Width || height != m_Height)
	{
		OnResizeVideo();
	}

	if (!pD3DDevice) return S_FALSE;

	pD3DDevice->GetImmediateContext(&pD3DDeviceContext);
	if (!pD3DDeviceContext) return S_FALSE;

	pD3DDeviceContext->OMGetRenderTargets(1, &pD3DRenderTargetView, nullptr);
	if (!pD3DRenderTargetView) return S_FALSE;

	// We get current texture and vertices
	hr = GetTexture(VdjVideoEngineDirectX11, (void**)&pTexture, &vertices);
	if (hr != S_OK) return S_FALSE;

	hr = Rendering_D3D11(pD3DDevice, pD3DDeviceContext, pD3DRenderTargetView, pTexture, vertices);
	if (hr != S_OK) return S_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT VDJ_API CElectrickShock8::OnAudioSamples(float* buffer, int nb)
{ 
	#ifdef USE_FFT
		int FFT_SIZE = 512; // Size of the FFT (must be a power of 2)
		ComputeFFT(buffer, nb, FFT_SIZE);
		return S_OK; 
	#else
		return E_NOTIMPL;
	#endif
}
//-----------------------------------------------------------------------
void CElectrickShock8::OnResizeVideo()
{
	m_Width = width;
	m_Height = height;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Initialize_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;

	hr = Create_VertexBufferDynamic_D3D11(pDevice);
	if (hr != S_OK) return S_FALSE;

	hr = Create_PixelShader_D3D11(pDevice);
	if (hr != S_OK) return S_FALSE;

	hr = Create_BlendState_D3D11(pDevice);
	if (hr != S_OK) return S_FALSE;

	hr = Create_PSConstantBufferDynamic_D3D11(pDevice);
	if (hr != S_OK) return S_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------
void CElectrickShock8::Release_D3D11()
{
	SAFE_RELEASE(pNewVertexBuffer);
	SAFE_RELEASE(pPixelShader);
	SAFE_RELEASE(pBlendState);
	SAFE_RELEASE(pPSConstantBuffer);
}
// -----------------------------------------------------------------------
HRESULT CElectrickShock8::Rendering_D3D11(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView, ID3D11ShaderResourceView* pTextureView, TVertex8* pVertices)
{
	HRESULT hr = S_FALSE;

#ifdef _DEBUG
	InfoTexture2D InfoRTV = {};
	InfoTexture2D InfoSRV = {};
	hr = GetInfoFromRenderTargetView(pRenderTargetView, &InfoRTV);
	hr = GetInfoFromShaderResourceView(pTextureView, &InfoSRV);
#endif

	DetectBeats();

	hr = DrawDeck();
	if (hr != S_OK) return S_FALSE;

	D3DXCOLOR Color = {};
	if (m_FX_Activate) Color = { 1.0f, 1.0f, 1.0f, m_alpha };
	else Color = { 0.0f, 0.0f, 0.0f, m_alpha };

	if (pRenderTargetView)
	{
		FLOAT backgroundColor[4] = { Color.r, Color.g ,Color.b ,Color.a };
		pDeviceContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	}

	hr = Update_VertexBufferDynamic_D3D11(pDeviceContext);
	if (hr != S_OK) return S_FALSE;

	hr = Update_PSConstantBufferDynamic_D3D11(pDeviceContext);
	if (hr != S_OK) return S_FALSE;

	
	if (pPixelShader)
	{
		pDeviceContext->PSSetShader(pPixelShader, nullptr, 0);
	}

	if (pPSConstantBuffer)
	{
		pDeviceContext->PSSetConstantBuffers(0, 1, &pPSConstantBuffer);
	}
	
	if (pTextureView)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &pTextureView);
	}

	if (pBlendState)
	{
		//pDeviceContext->OMSetBlendState(pBlendState, nullptr, 0xFFFFFFFF);
	}
	
	if (pNewVertexBuffer)
	{
		UINT m_VertexStride = sizeof(TLVERTEX);
		UINT m_VertexOffset = 0;
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, &pNewVertexBuffer, &m_VertexStride, &m_VertexOffset);
	}
	
	pDeviceContext->Draw(m_VertexCount, 0);
	
	return S_OK;
}
// ---------------------------------------------------------------------- -
HRESULT CElectrickShock8::Create_VertexBufferDynamic_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;

	if (!pDevice) return S_FALSE;

	// Set the number of vertices in the vertex array.
	m_VertexCount = 6; // = ARRAYSIZE(pNewVertices);
	
	// Fill in a buffer description.
	D3D11_BUFFER_DESC VertexBufferDesc;
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // CPU_Access=Write_Only & GPU_Access=Read_Only
	VertexBufferDesc.ByteWidth = sizeof(TLVERTEX) * m_VertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //D3D11_BIND_INDEX_BUFFER
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allow CPU to write in buffer
	VertexBufferDesc.MiscFlags = 0;

	hr = pDevice->CreateBuffer(&VertexBufferDesc, NULL, &pNewVertexBuffer);
	if (hr != S_OK || !pNewVertexBuffer) return S_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Update_VertexBufferDynamic_D3D11(ID3D11DeviceContext* ctx)
{
	HRESULT hr = S_FALSE;

	if (!ctx) return S_FALSE;
	if (!pNewVertexBuffer) return S_FALSE;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	hr = ctx->Map(pNewVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	if (hr != S_OK) return S_FALSE;

	//TLVERTEX* pNewVertices = (TLVERTEX*) MappedSubResource.pData;

	hr = Update_Vertices_D3D11();

	memcpy(MappedSubResource.pData, pNewVertices, m_VertexCount * sizeof(TLVERTEX));

	ctx->Unmap(pNewVertexBuffer, NULL);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Update_Vertices_D3D11()
{
	float frameWidth = (float) m_Width;
	float frameHeight = (float) m_Height;

	float deltaWidth = frameWidth * m_VideoScale;
	float deltaHeight = frameHeight * m_VideoScale;

	D3DXPOSITION P1 = { 0.0f + deltaWidth, 0.0f + deltaHeight, 0.0f }, // Top Left
		P2 = { 0.0f + deltaWidth, frameHeight - deltaHeight, 0.0f }, // Bottom Left
		P3 = { frameWidth - deltaWidth, 0.0f + deltaHeight, 0.0f }, // Top Right
		P4 = { frameWidth - deltaWidth, frameHeight - deltaHeight, 0.0f }; // Bottom Right
	D3DXCOLOR color_vertex = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_alpha); // White color with alpha layer
	D3DXTEXCOORD T1 = { 0.0f , 0.0f }, T2 = { 0.0f , 1.0f }, T3 = { 1.0f , 0.0f }, T4 = { 1.0f , 1.0f };

	// Triangle n°1 (Bottom Right)
	pNewVertices[0] = { P3 , color_vertex , T3 };
	pNewVertices[1] = { P4 , color_vertex , T4 };
	pNewVertices[2] = { P2 , color_vertex , T2 };

	// Triangle n°2 (Top Left)
	pNewVertices[3] = { P2 , color_vertex , T2 };
	pNewVertices[4] = { P1 , color_vertex , T1 };
	pNewVertices[5] = { P3 , color_vertex , T3 };


	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Create_PixelShader_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;
	const WCHAR* pShaderHLSLFilepath = L"PixelShader.hlsl";
	const WCHAR* pShaderCSOFilepath = L"PixelShader.cso";
	const WCHAR* resourceType = RT_RCDATA;
	const WCHAR* resourceName = L"PIXELSHADER_CSO";

	SAFE_RELEASE(pPixelShader);

	hr = Create_PixelShaderFromResourceCSOFile_D3D11(pDevice, resourceType, resourceName);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Create_PixelShaderFromResourceCSOFile_D3D11(ID3D11Device* pDevice, const WCHAR* resourceType, const WCHAR* resourceName)
{
	HRESULT hr = S_FALSE;
	
	void* pShaderBytecode = nullptr;
	SIZE_T BytecodeLength = 0;

	hr = ReadResource(resourceType, resourceName, &BytecodeLength, &pShaderBytecode);
	if (hr != S_OK) return S_FALSE;
	
	hr = pDevice->CreatePixelShader(pShaderBytecode, BytecodeLength, nullptr, &pPixelShader);

	return hr;
}


//-----------------------------------------------------------------------
HRESULT CElectrickShock8::ReadResource(const WCHAR* resourceType, const WCHAR* resourceName, SIZE_T* size, LPVOID* data)
{
	HRESULT hr = S_FALSE;

	HRSRC rc = FindResource(hInstance, resourceName, resourceType);
	if (!rc) return S_FALSE;

	HGLOBAL rcData = LoadResource(hInstance, rc);
	if (!rcData) return S_FALSE;

	*size = (SIZE_T)SizeofResource(hInstance, rc);
	if (*size == 0) return S_FALSE;

	*data = LockResource(rcData);
	if (*data == nullptr) return S_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Create_BlendState_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc;
	ZeroMemory(&RenderTargetBlendDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	RenderTargetBlendDesc.BlendEnable = TRUE;
	RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR; // The data source is color data (RGB) from a pixel shader. No pre-blend operation.
	RenderTargetBlendDesc.DestBlend = D3D11_BLEND_DEST_COLOR; // The data source is color data from a rendertarget. No pre-blend operation.
	RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA; // The data source is alpha data from a pixel shader. No pre-blend operation.
	RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA; // The data source is alpha data from a rendertarget. No pre-blend operation. 
	RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA; // D3D11_COLOR_WRITE_ENABLE_ALL


	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.IndependentBlendEnable = FALSE;
	BlendStateDesc.RenderTarget[0] = RenderTargetBlendDesc;

	hr = pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Create_PSConstantBufferDynamic_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;

	if (!pDevice) return E_FAIL;

	UINT SIZEOF_PS_CONSTANTBUFFER = sizeof(PS_CONSTANTBUFFER);
	UINT CB_BYTEWIDTH = SIZEOF_PS_CONSTANTBUFFER + 0xf & 0xfffffff0;

	D3D11_BUFFER_DESC ConstantBufferDesc = {};
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;  // CPU_Access=Write_Only & GPU_Access=Read_Only
	ConstantBufferDesc.ByteWidth = CB_BYTEWIDTH;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // Allow CPU to write in buffer
	ConstantBufferDesc.MiscFlags = 0;

	// Create the constant buffer to send to the cbuffer in hlsl file
	hr = pDevice->CreateBuffer(&ConstantBufferDesc, nullptr, &pPSConstantBuffer);
	if (hr != S_OK || !pPSConstantBuffer) return S_FALSE;

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Update_PSConstantBufferDynamic_D3D11(ID3D11DeviceContext* ctx)
{
	HRESULT hr = S_FALSE;

	if (!ctx) return S_FALSE;
	if (!pPSConstantBuffer) return S_FALSE;

	hr = Update_PSConstantBufferData_D3D11();

	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = ctx->Map(pPSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	if (hr != S_OK) return S_FALSE;

	memcpy(MappedSubResource.pData, &m_PSConstantBufferData, sizeof(PS_CONSTANTBUFFER));

	ctx->Unmap(pPSConstantBuffer, 0);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::Update_PSConstantBufferData_D3D11()
{
	m_PSConstantBufferData.FX_Select = m_FX_Select;
	m_PSConstantBufferData.FX_Activate = m_FX_Activate;
	m_PSConstantBufferData.FX_Time = m_FX_Time;

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::GetInfoFromShaderResourceView(ID3D11ShaderResourceView* pShaderResourceView, InfoTexture2D* info)
{
	HRESULT hr = S_FALSE;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	pShaderResourceView->GetDesc(&viewDesc);

	DXGI_FORMAT ViewFormat = viewDesc.Format;
	D3D11_SRV_DIMENSION ViewDimension = viewDesc.ViewDimension;

	ID3D11Resource* pResource = nullptr;
	pShaderResourceView->GetResource(&pResource);
	if (!pResource) return S_FALSE;

	if (ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
	{
		ID3D11Texture2D* pTexture = nullptr;
		hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
		if (hr != S_OK || !pTexture) return S_FALSE;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		pTexture->GetDesc(&textureDesc);

		info->Format = textureDesc.Format;
		info->Width = textureDesc.Width;
		info->Height = textureDesc.Height;

		SAFE_RELEASE(pTexture);
	}

	SAFE_RELEASE(pResource);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CElectrickShock8::GetInfoFromRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, InfoTexture2D* info)
{
	HRESULT hr = S_FALSE;

	D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	pRenderTargetView->GetDesc(&viewDesc);

	DXGI_FORMAT ViewFormat = viewDesc.Format;
	D3D11_RTV_DIMENSION ViewDimension = viewDesc.ViewDimension;

	ID3D11Resource* pResource = nullptr;
	pRenderTargetView->GetResource(&pResource);
	if (!pResource) return S_FALSE;

	if (ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2D)
	{
		ID3D11Texture2D* pTexture = nullptr;
		hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
		if (hr != S_OK || !pTexture) return S_FALSE;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		pTexture->GetDesc(&textureDesc);

		info->Format = textureDesc.Format;
		info->Width = textureDesc.Width;
		info->Height = textureDesc.Height;

		SAFE_RELEASE(pTexture);
	}

	SAFE_RELEASE(pResource);

	return S_OK;
}
//-----------------------------------------------------------------------
void CElectrickShock8::DetectBeats()
{
	double fbp = SongPosBeats - floor(SongPosBeats);
	if (fbp >= 0 && fbp < m_length)
	{
		if (m_DetectLowFreq)
		{
			if (m_ref_freq <= m_minFreq)
			{
				m_FX_Activate = 1;
			}
			else
			{
				m_FX_Activate = 0;
			}
		}
		else
		{
			m_FX_Activate = 1;
		}
	}
	else
	{
		m_FX_Activate = 0;
	}

	if ((SongPosBeats != m_SongPosBeatsPrevious) && (m_FX_Activate == 1) && (m_FX_Activate_Previous == 0))
	{
		m_SongPosBeatsPrevious = SongPosBeats;
		
		float FX_Time_New = (m_FX_Time == 0.0f) ? 1.0f : 0.0f;
		if (FX_Time_New != m_FX_Time_Previous)
		{
			m_FX_Time = !m_FX_Time;

			if (m_FX_Select_Random)
			{
				/*
				if ((m_FX_Select + 1) > FX_RANDOM_END) m_FX_Select = FX_RANDOM_START;
				else m_FX_Select = m_FX_Select + 1;
				*/
				
				// Create a random number generator
				random_device rd;  // Seed
				mt19937 gen(rd()); // Mersenne Twister engine
				uniform_int_distribution<> dist(FX_RANDOM_START, FX_RANDOM_END);

				m_FX_Select = dist(gen); // Set m_FX_Select to a random value
			}

			m_FX_Time_Previous = m_FX_Time;
		}
	}

	m_FX_Activate_Previous = m_FX_Activate;

#ifdef _DEBUG
	char debug_fx[50];
	sprintf_s(debug_fx, 50 * sizeof(char), "SongPosBeats = %.3f", float(SongPosBeats));
	OutputDebugStringA(debug_fx);
	OutputDebugStringA("\n");
	sprintf_s(debug_fx, 50 * sizeof(char), "m_FX_Select = %d", m_FX_Select);
	OutputDebugStringA(debug_fx);
	OutputDebugStringA("\n");
	sprintf_s(debug_fx, 50 * sizeof(char), "m_FX_Activate = %d", m_FX_Activate);
	OutputDebugStringA(debug_fx);
	OutputDebugStringA("\n");
	sprintf_s(debug_fx, 50 * sizeof(char), "m_FX_Time = %.2f", m_FX_Time);
	OutputDebugStringA(debug_fx);
	OutputDebugStringA("\n\n");
#endif
}
//-----------------------------------------------------------------------
#ifdef USE_FFT
void CElectrickShock8::ComputeFFT(float* buffer, int nb, int fft_size)
{
	int mono_input_size = min(nb, fft_size);
	vector<float> mono_input(fft_size, 0.0f);
	float inLeft = 0.0f;
	float inRight = 0.0f;
	float magnitude = 0.0f;
	float max_amplitude = 0.0f;
	int ref_j = 0;
	float real = 0.0f; 
	float imag = 0.0f;

	// We convert the signal to mono first
	for (int i = 0; i < mono_input_size; ++i)
	{
		inLeft = buffer[2 * i];     // Left Channel
		inRight = buffer[2 * i + 1]; // Right Channel
		mono_input[i] = 0.5f * (inLeft + inRight); // Average to mono
	}

	// We compute the FFT
	int half = fft_size / 2;
	size_t n = half + 1;
	float* in = fftwf_alloc_real(fft_size);
	fftwf_complex* out = fftwf_alloc_complex(n);

	copy(mono_input.begin(), mono_input.end(), in);

	fftwf_plan plan = fftwf_plan_dft_r2c_1d(fft_size, in, out, FFTW_ESTIMATE);
	fftwf_execute(plan);

	for (int i = 0; i <= half; ++i)
	{
		real = out[i][0];
		imag = out[i][1];
		magnitude = sqrtf(SQ(real)+ SQ(imag));

		if (magnitude > max_amplitude)
		{
			max_amplitude = magnitude;
			ref_j = i;
		}
	}

	if (max_amplitude == 0)
	{
		m_ref_freq = 0;
	}
	else
	{
		m_ref_freq = (float(ref_j) * float(SampleRate)) / float(fft_size);
	}

	// We clean
	fftwf_destroy_plan(plan);
	fftwf_free(in);
	fftwf_free(out);
}
#endif