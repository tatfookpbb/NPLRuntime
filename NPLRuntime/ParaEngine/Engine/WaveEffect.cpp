//-----------------------------------------------------------------------------
// Class: WaveEffect
// Authors:	Li,Xizhi
// Emails:	LiXizhi@yeah.net
// Date:	2005.1.16
//-----------------------------------------------------------------------------
#include "ParaEngine.h"
#include "WaveEffect.h"
#include "Globals.h"
#include "EffectManager.h"

#include "ParaWorldAsset.h"
#if USE_DIRECTX_RENDERER
#include "RenderDeviceD3D9.h"
#endif

namespace ParaEngine
{
	WaveEffect::WaveEffect()
		:m_pBackbufferCopy(NULL)
	{
		m_waveParam[0] = 0;
		m_waveParam[1] = 0.5f;
		m_waveParam[2] = 0.05f;
		m_waveParam[3] = 0.05f;
	}

	WaveEffect::~WaveEffect()
	{
		InvalidateDeviceObjects();
	}

	HRESULT WaveEffect::InvalidateDeviceObjects()
	{
		SAFE_RELEASE(m_pBackbufferCopy);
		return S_OK;
	}

	HRESULT WaveEffect::RestoreDeviceObjects()
	{
		auto* pDevice = CGlobals::GetRenderDevice();

		EPixelFormat surfaceFmt = EPixelFormat::A8R8G8B8;
		auto viewport = CGlobals::GetRenderDevice()->GetViewport();

		m_pBackbufferCopy = CGlobals::GetRenderDevice()->CreateTexture(viewport.Width, viewport.Height, surfaceFmt, ETextureUsage::RenderTarget);


		if(!m_pBackbufferCopy)
			return E_FAIL;


		if(m_pNoiseMap == NULL)
			m_pNoiseMap = CGlobals::GetAssetManager()->LoadTexture("","Texture/Aries/ShaderResource/waveMap.dds",TextureEntity::StaticTexture);

		m_quadVertices[0].p  = Vector3(-1,-1,0);
		m_quadVertices[0].uv = Vector2(0,1);
		m_quadVertices[1].p  = Vector3(1,-1,0);
		m_quadVertices[1].uv = Vector2(1,1);
		m_quadVertices[2].p  = Vector3(-1,1,0);
		m_quadVertices[2].uv = Vector2(0,0);
		m_quadVertices[3].p  = Vector3(1,1,0);
		m_quadVertices[3].uv = Vector2(1,0);

		float xHalfPixel = 0.5f/viewport.Width;
		float yHalfPixel = 0.5f/viewport.Height;

		for (int i=0;i<4;++i)
		{
			m_quadVertices[i].uv.x += xHalfPixel;
			m_quadVertices[i].uv.y += yHalfPixel;
		}
		return S_OK;
	}

	HRESULT WaveEffect::Render()
	{
		auto pDevice = CGlobals::GetRenderDevice();
		
		EffectManager* pEffectManager = CGlobals::GetEffectManager();
		pEffectManager->BeginEffect(TECH_SCREEN_WAVE);
		CEffectFile* pEffectFile = pEffectManager->GetCurrentEffectFile();

		if(pEffectFile !=0 && pEffectFile->begin(true))
		{
			if(pEffectFile->BeginPass(0))
			{
				auto pBackBuffer = CGlobals::GetRenderDevice()->GetRenderTarget(0);
				
				if(!pBackBuffer->StretchRect(m_pBackbufferCopy, NULL, NULL, ETextureFilter::Linear))
				{
					OUTPUT_LOG("wave effect StretchRect() failed!\r\n");
					return E_FAIL;
				}
				
				CGlobals::GetRenderDevice()->SetTexture(0,m_pBackbufferCopy);
				CGlobals::GetRenderDevice()->SetTexture(1,m_pNoiseMap->GetTexture());
				

				pDevice->SetRenderState(ERenderState::CULLMODE,RSV_CULL_NONE);
				pDevice->SetRenderState(ERenderState::ZENABLE,false);

				m_waveParam[0] = fmodf((float)CGlobals::GetGameTime() * m_waveParam[3],900000.0f);
				GETD3D(CGlobals::GetRenderDevice())->SetVertexShaderConstantF(0,m_waveParam,1);

				CGlobals::GetRenderDevice()->DrawPrimitiveUP(EPrimitiveType::TRIANGLESTRIP,2,m_quadVertices,sizeof(mesh_vertex_plain));
			
				pDevice->SetRenderState(ERenderState::CULLMODE,RSV_CULL_CCW);
				pDevice->SetRenderState(ERenderState::ZENABLE,true);
				//clean up
				pBackBuffer->Release();
				GETD3D(CGlobals::GetRenderDevice())->SetTexture(1,NULL);
				pEffectFile->EndPass();
				return S_OK;
			}
		}
		return S_FALSE;
	}
}