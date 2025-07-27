#ifndef ELECTRICSHOCK8_H
#define ELECTRICSHOCK8_H


#include "vdjVideo8.h"
#include <cmath> // for the function floor()
#include <stdio.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <random> // Required for random


#define USE_FFT // uncomment this line to enable the use of FFT (Fast Fourier Transform)


#ifdef USE_FFT
#include <fftw3.h> // we use FFTW library (download it with "vcpkg install fftw3:x64-windows-static")
#pragma comment(lib, "fftw3f.lib")
#include <complex>
#include <vector>
#endif


using namespace std;


//////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////
class CElectricShock8 : public IVdjPluginVideoFx8
{
public:
	CElectricShock8();
	~CElectricShock8();
	HRESULT VDJ_API OnLoad();
	HRESULT VDJ_API OnGetPluginInfo(TVdjPluginInfo8 *info);
	ULONG   VDJ_API Release();
	HRESULT VDJ_API OnParameter(int id);
	HRESULT VDJ_API OnGetParameterString(int id, char* outParam, int outParamSize);
	HRESULT VDJ_API OnDeviceInit();
	HRESULT VDJ_API OnDeviceClose();
	HRESULT VDJ_API OnDraw();
	HRESULT VDJ_API OnStart();
	HRESULT VDJ_API OnStop();
	HRESULT VDJ_API OnAudioSamples(float* buffer, int nb);

private:
	struct D3DXPOSITION
	{
		float x;
		float y;
		float z;
	};
	struct D3DXCOLOR
	{
	public:
		D3DXCOLOR() = default;
		D3DXCOLOR(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

		operator FLOAT* ()
		{
			return &r;
		}

		FLOAT r, g, b, a;
	};
	struct D3DXTEXCOORD
	{
		float tu;
		float tv;
	};

	struct TLVERTEX
	{
		D3DXPOSITION position;
		D3DXCOLOR color;
		D3DXTEXCOORD texture;
	};

	struct InfoTexture2D
	{
		UINT Width;
		UINT Height;
		DXGI_FORMAT Format;
	};

	__declspec(align(16))
		struct PS_CONSTANTBUFFER
	{
		int FX_Select;
		int FX_Activate;
		float FX_Time;
	};

	void OnResizeVideo();
	void OnSlider(int id);
	void DetectBeats();
	HRESULT ReadResource(const WCHAR* resourceType, const WCHAR* resourceName, SIZE_T* size, LPVOID* data);
	#ifdef USE_FFT
		void ComputeFFT(float* buffer, int nb, int fft_size);
	#endif

	HRESULT Initialize_D3D11(ID3D11Device* pDevice);
	void Release_D3D11();
	HRESULT Rendering_D3D11(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView, ID3D11ShaderResourceView* pTextureView, TVertex8* pVertices);
	HRESULT Create_PixelShader_D3D11(ID3D11Device* pDevice);
	HRESULT Create_PixelShaderFromResourceCSOFile_D3D11(ID3D11Device* pDevice, const WCHAR* resourceType, const WCHAR* resourceName);
	HRESULT Create_VertexBufferDynamic_D3D11(ID3D11Device* pDevice);
	HRESULT Update_VertexBufferDynamic_D3D11(ID3D11DeviceContext* ctx);
	HRESULT Update_Vertices_D3D11();
	HRESULT Create_BlendState_D3D11(ID3D11Device* pDevice);
	HRESULT Create_PSConstantBufferDynamic_D3D11(ID3D11Device* pDevice);
	HRESULT Update_PSConstantBufferDynamic_D3D11(ID3D11DeviceContext* ctx);
	HRESULT Update_PSConstantBufferData_D3D11();
	HRESULT GetInfoFromShaderResourceView(ID3D11ShaderResourceView* pShaderResourceView, InfoTexture2D* info);
	HRESULT GetInfoFromRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, InfoTexture2D* info);

	
	ID3D11Device* pD3DDevice;
	ID3D11DeviceContext* pD3DDeviceContext;
	ID3D11RenderTargetView* pD3DRenderTargetView;
	ID3D11Buffer* pNewVertexBuffer;
	ID3D11PixelShader* pPixelShader;
	ID3D11BlendState* pBlendState;
	ID3D11Buffer* pPSConstantBuffer;

	PS_CONSTANTBUFFER m_PSConstantBufferData;
	
	TLVERTEX pNewVertices[6];
	UINT m_VertexCount;
	bool m_DirectX_On;
	int m_WidthOnDeviceInit;
	int m_HeightOnDeviceInit;
	int m_Width;
	int m_Height;
	float m_SliderValue[5];
	float m_alpha;
	float m_VideoScale;
	float m_length;
	double m_SongPosBeatsPrevious;
	int m_FX_Select;
	int m_FX_Select_Random;
	int m_FX_Activate;
	int m_FX_Activate_Previous;
	float m_FX_Time;
	float m_FX_Time_Previous;


	int m_DetectLowFreq;
	float m_minFreq;
	float m_ref_freq;
	const float MIN_FREQ = 20.0f;
	const float MAX_FREQ = 500.0f;

	
	typedef enum _ID_Interface
	{
		ID_INIT,
		ID_SLIDER_1,
		ID_SLIDER_2,
		ID_SLIDER_3,
		ID_SLIDER_4,
		ID_SLIDER_5,
		ID_SWITCH_1,
		ID_SWITCH_2
	} ID_Interface;

	const int ID_SLIDER_MIN = ID_SLIDER_1;
	#ifdef USE_FFT
		const int ID_SLIDER_MAX = ID_SLIDER_5;
	#else
		const int ID_SLIDER_MAX = ID_SLIDER_4;
	#endif

	const UINT MAX_FX = 23;

	const int FX_RANDOM_START = 2;
	const int FX_RANDOM_END = MAX_FX;

	#ifndef SQ
	#define SQ(x) (x * x)
	#endif

	#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(x) { if (x!=nullptr) { x->Release(); x=nullptr; } }
	#endif

};

#endif
