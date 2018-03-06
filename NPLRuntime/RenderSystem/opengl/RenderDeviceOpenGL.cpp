#include <stdexcept>
#include "Renderer/VertexDeclarationOpenGL.h"
#include "math/ParaViewport.h"
#include "RenderDeviceOpenGL.h"
//#include "math/ParaColor.h"



using namespace ParaEngine;


namespace ParaEngine
{
	uint32_t ToGLBlendValue(uint32_t value)
	{
		switch (value)
		{
		case RSV_BLEND_ZERO:
			return GL_ZERO;
		case RSV_BLEND_ONE:
			return GL_ONE;
		case RSV_BLEND_SRCCOLOR:
			return GL_SRC_COLOR;
		case RSV_BLEND_INVSRCCOLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case RSV_BLEND_SRCALPHA:
			return GL_SRC_ALPHA;
		case RSV_BLEND_INVSRCALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		case RSV_BLEND_DESTALPHA:
			return GL_DST_ALPHA;
		case RSV_BLEND_INVDESTALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case RSV_BLEND_DESTCOLOR:
			return GL_DST_COLOR;
		case RSV_BLEND_INVDESTCOLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case RSV_BLEND_SRCALPHASAT:
			return GL_SRC_ALPHA_SATURATE;
		//case RSV_BLEND_BOTHSRCALPHA:
		//case RSV_BLEND_BOTHINVSRCALPHA:
		//case RSV_BLEND_BLENDFACTOR:
		//case RSV_BLEND_INVBLENDFACTOR:
		default:
			assert(false);//Unkonw blend value
			break;
		}
		return 0;
	}
}



ParaEngine::RenderDeviceOpenGL::RenderDeviceOpenGL()
	:m_AlphaBlendingChanged(true)
	,m_EnableBlending(false)
	,m_EnableSeparateAlphaBlending(false)
	,m_BlendingChaned(true)
	,m_BlendingSource(RSV_BLEND_SRCALPHA)
	,m_BlendingDest(RSV_BLEND_INVSRCALPHA)
	,m_BlendingAlphaSource(RSV_BLEND_SRCALPHA)
	,m_BlendingAlphaDest(RSV_BLEND_INVSRCALPHA)
	,m_StencilPass(0)
	,m_StencilRefValue(0)
	,m_CurrentIndexBuffer(0)
	,m_CurrentVertexBuffer(0)
	,m_CurrentVertexDeclaration(0)
{
	for (int i = 0;i<8;i++)
	{
		for (int j =0;j<8;j++)
		{
			m_SamplerStates[i][j] = 0;
		}
	}
}

uint32_t ParaEngine::RenderDeviceOpenGL::GetRenderState(const ERenderState& State)
{
	//throw std::logic_error("The method or operation is not implemented.");
	return 0;
}

bool ParaEngine::RenderDeviceOpenGL::SetRenderState(const ERenderState State, const uint32_t Value)
{
	switch (State)
	{
	case ERenderState::CULLMODE:
		{
			//http://www.cnblogs.com/onlyDIABLO/p/3853125.html
			if (Value == RSV_CULL_CW) {
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);//glCullFace(GL_FRONT);
			}
			else if (Value == RSV_CULL_CCW) {
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CW);//glCullFace(GL_BACK);
			}
			else if (Value == RSV_CULL_NONE) {
				glDisable(GL_CULL_FACE);
			}
		}
		break;
	case ERenderState::ZENABLE:
	{
		// z test enable
		if (Value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		break;
	}
	case ERenderState::ZFUNC:
	{
		// z test enable
		if (Value == RSV_CMP_LESSEQUAL)
			glDepthFunc(GL_LEQUAL);
		else if (Value == RSV_CMP_GREATER)
			glDepthFunc(GL_GREATER);
		else if (Value == RSV_CMP_GREATEREQUAL)
			glDepthFunc(GL_GEQUAL);
		else if (Value == RSV_CMP_LESS)
			glDepthFunc(GL_LESS);
		else if (Value == RSV_CMP_NEVER)
			glDepthFunc(GL_NEVER);
		else if (Value == RSV_CMP_NOTEQUAL)
			glDepthFunc(GL_NOTEQUAL);
		else if (Value == RSV_CMP_ALWAYS)
			glDepthFunc(GL_ALWAYS);
		else
			glDepthFunc(GL_LEQUAL);
		break;
	}
	case ERenderState::ZWRITEENABLE:
	{
		// z write enable
		glDepthMask(Value ? GL_TRUE : GL_FALSE);
		break;
	}
	case ERenderState::SEPARATEALPHABLENDENABLE:
	{
		bool bEnabled = Value ? true : false;
		if (bEnabled == m_EnableSeparateAlphaBlending)break;
		m_EnableSeparateAlphaBlending = bEnabled;
		if (bEnabled)
		{
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}

		break;
	}
	case ERenderState::ALPHABLENDENABLE:
	{
		bool bEnabled = Value ? true : false;
		if (bEnabled == m_EnableBlending) break;
		m_EnableBlending = bEnabled;
		if (bEnabled)
		{
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
		
		break;
	}
	case ERenderState::SRCBLENDALPHA:
	{
		if (Value == m_BlendingAlphaSource)break;
		m_BlendingAlphaSource = Value;
		m_AlphaBlendingChanged = true;
	}
	break;
	case ERenderState::DESTBLENDALPHA:
	{
		if (Value == m_BlendingAlphaDest)break;
		m_BlendingAlphaDest = Value;
		m_AlphaBlendingChanged = true;
	}
	break;
	case ERenderState::SRCBLEND:
	{
		if (Value == m_BlendingSource)break;
		m_BlendingSource = Value;
		m_BlendingChaned = true;
	}
	break;
	case ERenderState::DESTBLEND:
	{
		if (Value == m_BlendingDest)break;
		m_BlendingDest = Value;
		m_BlendingChaned = true;
	}
	break;
	case ERenderState::STENCILENABLE:
	{
		if (Value)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
		break;
	}
	case ERenderState::STENCILFUNC:
	{
		if (Value == RSV_CMP_EQUAL)
		{
			// draw only where stencil's value is Value
			glStencilFunc(GL_EQUAL, m_StencilRefValue, 0xFF);
		}
		else if (Value == RSV_CMP_ALWAYS)
		{
			glStencilFunc(GL_ALWAYS, m_StencilRefValue, 0xFF);
		}
		else if (Value == RSV_CMP_NEVER)
		{
			glStencilFunc(GL_NEVER, m_StencilRefValue, 0xFF);
		}
		break;
	}
	case ERenderState::STENCILWRITEMASK:
	{
		glStencilMask(Value);
		break;
	}
	case ERenderState::STENCILPASS:
	{
		if (Value == RSV_STENCILOP_REPLACE)
		{
			m_StencilPass = GL_REPLACE;
			glStencilOp(m_StencilPass, GL_KEEP, GL_KEEP);
		}
		break;
	}
	case ERenderState::STENCILREF:
	{
		m_StencilRefValue = Value;
		break;
	}
	case ERenderState::SCISSORTESTENABLE:
	{
		if (Value)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
		break;
	}
	default:
		//assert(false,"Unknow render state.");
		break;
	}

	return S_OK;
}

bool ParaEngine::RenderDeviceOpenGL::SetClipPlane(uint32_t Index, const float* pPlane)
{
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::ReadPixels(int nLeft, int nTop, int nWidth, int nHeight, void* pDataOut, uint32_t nDataFormat /*= 0*/, uint32_t nDataType /*= 0*/)
{
	return true;
}

int ParaEngine::RenderDeviceOpenGL::GetMaxSimultaneousTextures()
{
	return 4;
}




bool ParaEngine::RenderDeviceOpenGL::SetTexture(uint32_t stage, DeviceTexturePtr_type texture)
{
	glActiveTexture(GL_TEXTURE0 + stage);
	glBindTexture(GL_TEXTURE_2D, texture);
	//auto error = glGetError();
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::DrawPrimitive(EPrimitiveType PrimitiveType, uint32_t StartVertex, uint32_t PrimitiveCount)
{
	ApplyBlendingModeChange();
	if (PrimitiveType == EPrimitiveType::TRIANGLELIST)
		glDrawArrays(GL_TRIANGLES, StartVertex, PrimitiveCount * 3);
	else if (PrimitiveType == EPrimitiveType::TRIANGLESTRIP)
		glDrawArrays(GL_TRIANGLE_STRIP, StartVertex, PrimitiveCount + 2);
	else if (PrimitiveType == EPrimitiveType::TRIANGLEFAN)
		glDrawArrays(GL_TRIANGLE_FAN, StartVertex, PrimitiveCount + 2);
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::DrawPrimitiveUP(EPrimitiveType PrimitiveType, uint32_t PrimitiveCount, const void* pVertexStreamZeroData, uint32_t VertexStreamZeroStride)
{
	ApplyBlendingModeChange();
	if (m_CurrentIndexBuffer)
	{
		SetIndices(0);
	}
	if (m_CurrentVertexBuffer)
	{
		SetStreamSource(0, 0, 0, 0);
	}
	if (m_CurrentVertexDeclaration)
	{
		m_CurrentVertexDeclaration->ApplyAttribute(pVertexStreamZeroData);
	}

	if (PrimitiveType == EPrimitiveType::TRIANGLELIST)
		glDrawArrays(GL_TRIANGLES, 0, PrimitiveCount * 3);
	else if (PrimitiveType == EPrimitiveType::TRIANGLESTRIP)
		glDrawArrays(GL_TRIANGLE_STRIP, 0, PrimitiveCount + 2);
	else if (PrimitiveType == EPrimitiveType::TRIANGLEFAN)
		glDrawArrays(GL_TRIANGLE_FAN, 0, PrimitiveCount + 2);

	return true;

}

bool ParaEngine::RenderDeviceOpenGL::SetSamplerState(uint32_t stage, ESamplerStateType type, uint32_t value)
{
	switch (type)
	{
	case ParaEngine::ESamplerStateType::ADDRESSU:
	case ParaEngine::ESamplerStateType::ADDRESSV:
	case ParaEngine::ESamplerStateType::MAGFILTER:
	case ParaEngine::ESamplerStateType::MINFILTER:
		m_SamplerStates[stage][(int)type] = value;
		break;
	default:
		break;
	}
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetVertexDeclaration(CVertexDeclaration* pVertexDeclaration)
{
	if (pVertexDeclaration)
	{
		glBindVertexArray(0);
		pVertexDeclaration->EnableAttribute();
		pVertexDeclaration->ApplyAttribute();
		m_CurrentVertexDeclaration = pVertexDeclaration;
	}

	return true;
}

bool ParaEngine::RenderDeviceOpenGL::CreateVertexDeclaration(VertexElement* pVertexElements, CVertexDeclaration** ppDecl)
{
	CVertexDeclaration* pDecl = new CVertexDeclaration(pVertexElements);
	*ppDecl = pDecl;
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetIndices(IndexBufferDevicePtr_type pIndexData)
{
	if (pIndexData != m_CurrentIndexBuffer)
	{
		m_CurrentIndexBuffer = pIndexData;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexData);
	}

	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetStreamSource(uint32_t StreamNumber, VertexBufferDevicePtr_type pStreamData, uint32_t OffsetInBytes, uint32_t Stride)
{
	if (pStreamData != m_CurrentVertexBuffer)
	{
		m_CurrentVertexBuffer = pStreamData;
		glBindBuffer(GL_ARRAY_BUFFER, pStreamData);
		if (pStreamData && m_CurrentVertexDeclaration)
		{
			m_CurrentVertexDeclaration->EnableAttribute();
			m_CurrentVertexDeclaration->ApplyAttribute();
		}

	}

	return true;
	
}

void ParaEngine::RenderDeviceOpenGL::BeginRenderTarget(uint32_t width, uint32_t height)
{
	m_RenderTargetWidth = width;
	m_RenderTargetHeight = height;
	m_isBeginRenderTarget = true;

}

void ParaEngine::RenderDeviceOpenGL::EndRenderTarget()
{
	m_RenderTargetWidth = 0;
	m_RenderTargetHeight = 0;
	m_isBeginRenderTarget = false;
}

bool ParaEngine::RenderDeviceOpenGL::BeginScene()
{
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::EndScene()
{
	return true;

}

int ParaEngine::RenderDeviceOpenGL::GetStencilBits()
{
	static bool once = true;
	static GLint g_sStencilBits = -1;
	if (once)
	{
		glGetIntegerv(GL_STENCIL_BITS, &g_sStencilBits);
		if (g_sStencilBits <= 0)
		{
			
		}
		once = false;
	}
	return g_sStencilBits;
}

bool ParaEngine::RenderDeviceOpenGL::DrawIndexedPrimitive(EPrimitiveType Type, int BaseVertexIndex, uint32_t MinIndex, uint32_t NumVertices, uint32_t indexStart, uint32_t PrimitiveCount)
{
	ApplyBlendingModeChange();

	if (Type == EPrimitiveType::TRIANGLELIST)
		glDrawElements(GL_TRIANGLES, PrimitiveCount * 3, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16)*indexStart));
	else if (Type == EPrimitiveType::TRIANGLESTRIP)
		glDrawElements(GL_TRIANGLE_STRIP, PrimitiveCount + 2, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16)*indexStart));
	else if (Type == EPrimitiveType::TRIANGLEFAN)
		glDrawElements(GL_TRIANGLE_FAN, PrimitiveCount + 2, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16)*indexStart));
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::DrawIndexedPrimitiveUP(EPrimitiveType PrimitiveType, uint32_t MinVertexIndex, uint32_t NumVertices, uint32_t PrimitiveCount, const void * pIndexData, PixelFormat IndexDataFormat, const void* pVertexStreamZeroData, uint32_t VertexStreamZeroStride)
{
	ApplyBlendingModeChange();
	if (m_CurrentVertexDeclaration)
	{
		m_CurrentVertexDeclaration->ApplyAttribute(pVertexStreamZeroData);
	}

	if (PrimitiveType == EPrimitiveType::TRIANGLELIST)
		glDrawElements(GL_TRIANGLES, PrimitiveCount * 3, GL_UNSIGNED_SHORT, (GLvoid*)(pIndexData));
	else if (PrimitiveType == EPrimitiveType::TRIANGLESTRIP)
		glDrawElements(GL_TRIANGLE_STRIP, PrimitiveCount + 2, GL_UNSIGNED_SHORT, (GLvoid*)(pIndexData));
	else if (PrimitiveType == EPrimitiveType::TRIANGLEFAN)
		glDrawElements(GL_TRIANGLE_FAN, PrimitiveCount + 2, GL_UNSIGNED_SHORT, (GLvoid*)(pIndexData));
	else if (PrimitiveType == EPrimitiveType::LINELIST)
		glDrawElements(GL_LINES, PrimitiveCount * 2, GL_UNSIGNED_SHORT, (GLvoid*)(pIndexData));
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetTransform(ETransformsStateType State, DeviceMatrix_ptr pMatrix)
{
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetFVF(uint32_t FVF)
{
	return true;
}

void ParaEngine::RenderDeviceOpenGL::SetCursorPosition(int X, int Y, uint32_t Flags)
{
	
}

bool ParaEngine::RenderDeviceOpenGL::GetSamplerState(uint32_t stage, ESamplerStateType type, uint32_t* value)
{
	if (value)
		*value = m_SamplerStates[stage][(int)type];
	return true;
}

Rect ParaEngine::RenderDeviceOpenGL::GetViewport()
{
	return m_CurrentViewPort;
}

bool ParaEngine::RenderDeviceOpenGL::SetViewport(const Rect& viewport)
{
	m_CurrentViewPort = viewport;
	glViewport((GLint)(viewport.x), (GLint)(viewport.y), (GLsizei)(viewport.z), (GLsizei)(viewport.w));
	//auto error = glGetError();
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::Clear(bool color, bool depth, float stencil)
{
	uint32_t fields = 0;
	if (depth)
	{
		fields |= GL_DEPTH_BUFFER_BIT;
	}
	if (color)
	{
		fields |= GL_COLOR_BUFFER_BIT;
	}
	if (stencil)
	{
		fields |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(fields);
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetScissorRect(RECT* pRect)
{
	//throw std::logic_error("The method or operation is not implemented.");
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::GetScissorRect(RECT* pRect)
{
	//throw std::logic_error("The method or operation is not implemented.");
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetClearColor(const Color4f& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetClearDepth(const float depth)
{
	glClearDepth(depth);
	return true;
}

bool ParaEngine::RenderDeviceOpenGL::SetClearStencil(const int stencil)
{
	glClearStencil(stencil);
	return true;
}

void ParaEngine::RenderDeviceOpenGL::ApplyBlendingModeChange()
{
	
	if (m_AlphaBlendingChanged)
	{
		uint32_t asrc = ToGLBlendValue(m_BlendingAlphaSource);
		uint32_t adst = ToGLBlendValue(m_BlendingAlphaDest);

		uint32_t src = ToGLBlendValue(m_BlendingSource);
		uint32_t dst = ToGLBlendValue(m_BlendingDest);
		glBlendFuncSeparate(src, dst, asrc, adst);
		m_AlphaBlendingChanged = false;
	}else if (m_BlendingChaned)
	{
		uint32_t src = ToGLBlendValue(m_BlendingSource);
		uint32_t dst = ToGLBlendValue(m_BlendingDest);
		glBlendFunc(src, dst);
		m_BlendingChaned = false;
	}

}
