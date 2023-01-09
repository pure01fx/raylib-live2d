#include <cmath>
#include "CubismRenderer_Raylib.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "Model/CubismModel.hpp"
#include "CubismShader_Raylib.hpp"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::Rendering;
using Live2D::Cubism::Framework::CubismModel;

std::array<CubismRenderer::CubismTextureColor, 4> CubismRenderer_Raylib::s_colorChannels = {
	CubismRenderer::CubismTextureColor(1, 0, 0, 0),
	CubismRenderer::CubismTextureColor(0, 1, 0, 0),
	CubismRenderer::CubismTextureColor(0, 0, 1, 0),
	CubismRenderer::CubismTextureColor(0, 0, 0, 1)
};

void CubismRenderer::StaticRelease()
{
	// CubismRenderer_Raylib::DoStaticRelease(); // TODO: may be useful?
}

CubismRenderer* CubismRenderer::Create()
{
	return CSM_NEW CubismRenderer_Raylib();
}


void CubismRenderer_Raylib::Initialize(CubismModel* model)
{
	if (model->IsUsingMasking()) // else: m_clippingContextsForDraw.size() = 0
	{
		auto drawableCount = model->GetDrawableCount();
		auto drawableMasks = model->GetDrawableMasks();
		auto drawableMaskCounts = model->GetDrawableMaskCounts();

		m_clippingContextsForDraw.reserve(drawableCount);

		for (csmInt32 i = 0; i < drawableCount; i++)
		{
			if (drawableMaskCounts[i] <= 0)
			{
				m_clippingContextsForDraw.emplace_back(nullptr);
				continue;
			}

			CubismClippingContext* cc;
			for (const auto clippingContext : m_clippingContextsForMask)
			{
				if (clippingContext->IsSameClipping(drawableMasks[i], drawableMaskCounts[i]))
				{
					cc = clippingContext;
					goto NEXT;
				}
			}

			cc = CSM_NEW CubismClippingContext(drawableMasks[i], drawableMaskCounts[i]);
			m_clippingContextsForMask.emplace_back(cc);

		NEXT:
			cc->drawables.emplace_back(i);
			m_clippingContextsForDraw.emplace_back(cc);
		}

		if (m_fbo == 0)
		{
			InitializeOffscreenFrameBuffer();
		}
	}
	m_sortedDrawables.assign(model->GetDrawableCount(), 0);

	CubismRenderer::Initialize(model);
}

void CubismRenderer_Raylib::SetFrameBuffer(int fbo)
{
	m_oldFbo = fbo;
}

CubismRenderer_Raylib::~CubismRenderer_Raylib()
{
	for (auto clippingContext : m_clippingContextsForMask)
	{
		delete clippingContext;
	}

	if (m_colorBuffer && !m_isExternalColorBuffer)
	{
		rlUnloadTexture(m_colorBuffer);
		m_colorBuffer = 0;
	}

	if (m_fbo)
	{
		rlUnloadTexture(m_fbo);
		m_fbo = 0;
	}

	for (auto p : m_textureIdMapping)
	{
		rlUnloadTexture(p.second);
	}
}

void expandRect(Rectangle& rect, float w, float h)
{
	rect.x -= w, rect.width += w * 2;
	rect.y -= h, rect.height -= h * 2;
}

void CubismRenderer_Raylib::EnableOffscreenBuffer()
{
	rlViewport(0, 0, m_clippingMaskBufferSize.first, m_clippingMaskBufferSize.second);
	rlEnableFramebuffer(m_fbo);
	rlClearColor(255, 255, 255, 255);
	rlClearScreenBuffers();
}

void CubismRenderer_Raylib::DisableOffscreenBuffer()
{
	rlEnableFramebuffer(m_oldFbo);
	rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
}

void CubismRenderer_Raylib::DoDrawModel()
{
	m_batch.ClearBatch();
	if (GetModel()->IsUsingMasking())
	{
		// setup clipping context
		int activeClipCount = 0;
		for (auto clippingContext : m_clippingContextsForMask)
		{
			clippingContext->UpdateBounds(GetModel());
			activeClipCount += clippingContext->active == 1;
		}

		if (activeClipCount == 0)
		{
			goto NO_ACTIVE_MASK;
		}

		if (IsUsingHighPrecisionMask())
		{
			for (auto cc : m_clippingContextsForMask)
			{
				cc->layoutChannel = 0;
				cc->layoutRegion.x = 0.0f;
				cc->layoutRegion.y = 0.0f;
				cc->layoutRegion.width = 1.0f;
				cc->layoutRegion.height = 1.0f;
			}
		}
		else
		{
			const int channelCount = 4;
			auto cc_it = m_clippingContextsForMask.begin();
			const auto cc_end = m_clippingContextsForMask.end();
			int channelMod = activeClipCount % channelCount;
			for (int channelId = 0; channelId < channelCount; ++channelId)
			{
				int channelContains = activeClipCount / channelCount;
				if (channelMod)
				{
					channelContains += 1;
					channelMod -= 1;
				}
				int colCount = std::max(1, (int)std::round(std::sqrt(channelContains)));
				int rowCount = channelContains / colCount + (channelContains % colCount != 0);
				float colSize = 1.0f / colCount;
				float rowSize = 1.0f / rowCount;
				for (int currentPos = 0; currentPos < channelContains; ++currentPos)
				{
					for (; cc_it != cc_end && !(*cc_it)->active; ++cc_it);
					if (cc_it != cc_end)
					{
						auto cc = *cc_it;
						cc_it++;
						cc->layoutChannel = channelId;
						cc->layoutRegion.x = colSize * (currentPos % colCount);
						cc->layoutRegion.y = rowSize * (currentPos / colCount);
						cc->layoutRegion.width = colSize;
						cc->layoutRegion.height = rowSize;
					}
				}
			}
		}

		const float margin = 0.005; // impl = official's, but margin is smaller.

		for (auto cc : m_clippingContextsForMask)
		{
			Rectangle boundOnModel;
			float scaleX, scaleY;
			if (IsUsingHighPrecisionMask())
			{
				// TODO:
				const float ppu = GetModel()->GetPixelsPerUnit();
				const float maskPixelWidth = m_clippingMaskBufferSize.first;
				const float maskPixelHeight = m_clippingMaskBufferSize.second;
				const float physicalMaskWidth = cc->layoutRegion.width * maskPixelWidth;
				const float physicalMaskHeight = cc->layoutRegion.height * maskPixelHeight;

				boundOnModel = cc->clipBound;

				if (boundOnModel.width * ppu > physicalMaskWidth)
				{
					expandRect(boundOnModel, margin * boundOnModel.width, 0);
					scaleX = cc->layoutRegion.width / boundOnModel.width;
				}
				else
				{
					scaleX = ppu / physicalMaskWidth;
				}

				if (boundOnModel.height * ppu > physicalMaskHeight)
				{
					expandRect(boundOnModel, 0, margin * boundOnModel.height);
					scaleY = cc->layoutRegion.height / boundOnModel.height;
				}
				else
				{
					scaleY = ppu / physicalMaskHeight;
				}
			}
			else
			{
				boundOnModel = cc->clipBound;
				expandRect(boundOnModel, margin * boundOnModel.width, margin * boundOnModel.height);
				scaleX = cc->layoutRegion.width / boundOnModel.width;
				scaleY = cc->layoutRegion.height / boundOnModel.height;
			}

			cc->drawMatrix = MatrixMultiply(MatrixMultiply(
				MatrixTranslate(-boundOnModel.x, -boundOnModel.y, 0),
				MatrixScale(scaleX, scaleY, 1)),
				MatrixTranslate(cc->layoutRegion.x, cc->layoutRegion.y, 0));

			cc->maskMatrix = MatrixMultiply(MatrixMultiply(
				cc->drawMatrix,
				MatrixScale(2, 2, 1)),
				MatrixTranslate(-1, -1, 0));


			if (!IsUsingHighPrecisionMask())
			{
				for (auto clip : cc->clippings)
				{
					// 頂点情報が更新されておらず、信頼性がない場合は描画をパスする
					// TODO: really?
					if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clip))
					{
						continue;
					}

					IsCulling(GetModel()->GetDrawableCulling(clip) != 0);

					DrawMeshInternal(
						GetModel()->GetDrawableVertexIndexCount(clip),
						GetModel()->GetDrawableVertexCount(clip),
						GetModel()->GetDrawableVertexIndices(clip),
						GetModel()->GetDrawableVertices(clip),
						reinterpret_cast<const csmFloat32*>(GetModel()->GetDrawableVertexUvs(clip)),
						false,
						true,
						cc);
				}
			}
		}

	NO_ACTIVE_MASK:
		(void)0;
	}

	const auto drawableCount = GetModel()->GetDrawableCount();
	const auto drawableRenderOrder = GetModel()->GetDrawableRenderOrders();

	for (int drawable = 0; drawable < m_sortedDrawables.size(); ++drawable)
	{
		m_sortedDrawables[drawableRenderOrder[drawable]] = drawable;
	}

	for (const auto drawableIndex : m_sortedDrawables)
	{
		if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
		{
			continue;
		}

		CubismClippingContext* cc = m_clippingContextsForDraw.size() > 0 ? m_clippingContextsForDraw[drawableIndex] : nullptr;
		if (cc && cc->active && IsUsingHighPrecisionMask())
		{
			// TODO: not active?
			for (const auto clipDrawIndex : cc->clippings)
			{
				if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
				{
					continue;
				}

				IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0); // TODO: changed

				DrawMeshInternal(
					GetModel()->GetDrawableVertexIndexCount(clipDrawIndex),
					GetModel()->GetDrawableVertexCount(clipDrawIndex),
					GetModel()->GetDrawableVertexIndices(clipDrawIndex),
					GetModel()->GetDrawableVertices(clipDrawIndex),
					reinterpret_cast<const csmFloat32*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex)),
					false,
					true,
					// drawing mask
					cc);
			}
		}

		IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0); // TODO: changed

		DrawMeshInternal(
			GetModel()->GetDrawableVertexIndexCount(drawableIndex),
			GetModel()->GetDrawableVertexCount(drawableIndex),
			GetModel()->GetDrawableVertexIndices(drawableIndex),
			GetModel()->GetDrawableVertices(drawableIndex),
			reinterpret_cast<const csmFloat32*>(GetModel()->GetDrawableVertexUvs(drawableIndex)),
			GetModel()->GetDrawableInvertedMask(drawableIndex),
			false,
			// not drawing mask
			cc && cc->active ? cc : nullptr);
	}

	rlDisableVertexArray();
	rlDisableShader();
}

void CubismRenderer_Raylib::DoDrawModel1()
{
	m_batch.BeginApplyBatch();

	rlDisableScissorTest();
	rlDisableDepthTest();
	rlEnableColorBlend();

	if (GetModel()->IsUsingMasking())
	{
		// setup clipping context
		int activeClipCount = 0;
		for (auto clippingContext : m_clippingContextsForMask)
		{
			if (clippingContext->active)
			{
				activeClipCount = 1;
				break;
			}
		}

		if (activeClipCount == 0)
		{
			goto NO_ACTIVE_MASK;
		}

		if (!IsUsingHighPrecisionMask())
		{
			EnableOffscreenBuffer();
		}

		if (!IsUsingHighPrecisionMask())
		{
			for (auto cc : m_clippingContextsForMask)
			{
				for (auto clip : cc->clippings)
				{
					// 頂点情報が更新されておらず、信頼性がない場合は描画をパスする
					// TODO: really?
					if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clip))
					{
						continue;
					}

					IsCulling(GetModel()->GetDrawableCulling(clip) != 0);

					DrawMeshInternal1(
						GetModel()->GetDrawableTextureIndex(clip),
						GetModel()->GetDrawableOpacity(clip),
						CubismRenderer::CubismBlendMode_Normal,
						false,
						GetModel()->GetMultiplyColor(clip),
						GetModel()->GetScreenColor(clip),
						true,
						cc);
				}
			}
		}

		if (!IsUsingHighPrecisionMask())
		{
			DisableOffscreenBuffer();
		}

	NO_ACTIVE_MASK:
		(void)0;
	}

	for (const auto drawableIndex : m_sortedDrawables)
	{
		if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
		{
			continue;
		}

		CubismClippingContext* cc = m_clippingContextsForDraw.size() > 0 ? m_clippingContextsForDraw[drawableIndex] : nullptr;

		if (cc && cc->active && IsUsingHighPrecisionMask())
		{
			EnableOffscreenBuffer();

			// TODO: not active?
			for (const auto clipDrawIndex : cc->clippings)
			{
				if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
				{
					continue;
				}

				IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0); // TODO: changed

				DrawMeshInternal1(
					GetModel()->GetDrawableTextureIndex(clipDrawIndex),
					GetModel()->GetDrawableOpacity(clipDrawIndex),
					CubismRenderer::CubismBlendMode_Normal,
					false,
					// not inverted
					GetModel()->GetMultiplyColor(clipDrawIndex),
					GetModel()->GetScreenColor(clipDrawIndex),
					true,
					// drawing mask
					cc);
			}

			DisableOffscreenBuffer();
		}

		IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0); // TODO: changed

		DrawMeshInternal1(
			GetModel()->GetDrawableTextureIndex(drawableIndex),
			GetModel()->GetDrawableOpacity(drawableIndex),
			GetModel()->GetDrawableBlendMode(drawableIndex),
			GetModel()->GetDrawableInvertedMask(drawableIndex),
			GetModel()->GetMultiplyColor(drawableIndex),
			GetModel()->GetScreenColor(drawableIndex),
			false,
			// not drawing mask
			cc && cc->active ? cc : nullptr);
	}

	m_batch.EndApplyBatch();
}

const int GL_ZERO = 0;
const int GL_ONE = 1;
const int GL_ONE_MINUS_SRC_COLOR = 0x301;
const int GL_ONE_MINUS_SRC_ALPHA = 0x303;
const int GL_DST_COLOR = 0x306;
const int GL_FUNC_ADD = 0x8006;

void CubismRenderer_Raylib::DrawMeshInternal(csmInt32 indexCount, csmInt32 vertexCount, const csmUint16* indexArray, const csmFloat32* vertexArray, const csmFloat32* uvArray, csmBool invertedMask, bool drawingMask, const CubismClippingContext* cc)
{
	const CubismShaderSet* shaderSet;
	CubismRenderBufferSet* bufferSet;

	if (drawingMask)
	{
		m_batch.CreateBuffer(CubismShaderNames_SetupMask, shaderSet, bufferSet);
	}
	else
	{
		m_batch.CreateBuffer(1 + ((cc != nullptr) ? (invertedMask ? 2 : 1) : 0) + (IsPremultipliedAlpha() ? 3 : 0), shaderSet, bufferSet);
	}

	rlEnableVertexArray(bufferSet->vertexArrayObjectId);
	rlUpdateVertexBuffer(bufferSet->vertexBufferPositionId, vertexArray, sizeof(csmFloat32) * vertexCount * 2, 0);
	rlUpdateVertexBuffer(bufferSet->vertexBufferTexcoordId, uvArray, sizeof(csmFloat32) * vertexCount * 2, 0);
	rlUpdateVertexBufferElements(bufferSet->vertexBufferElementId, indexArray, sizeof(csmUint16) * indexCount, 0);

	bufferSet->indexCount = indexCount;
}


void CubismRenderer_Raylib::DrawMeshInternal1(csmInt32 textureNo, csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask, const CubismTextureColor& multiplyColor, const CubismTextureColor& screenColor, bool drawingMask, const CubismClippingContext* cc)
{
	const auto textureId = m_textureIdMapping[textureNo];
	CSM_ASSERT(textureId);

	IsCulling() ? rlEnableBackfaceCulling() : rlDisableBackfaceCulling(); // TODO: restore it. We assume that we use CCW only so we don't change front face here.
	CubismTextureColor modelColorRGBA = GetModelColor();
	if (!drawingMask)
	{
		modelColorRGBA.A *= opacity;
		if (IsPremultipliedAlpha())
		{
			modelColorRGBA.R *= modelColorRGBA.A;
			modelColorRGBA.G *= modelColorRGBA.A;
			modelColorRGBA.B *= modelColorRGBA.A;
		}
	}

	const CubismShaderSet* shaderSet;

	if (drawingMask)
	{
		shaderSet = CubismShader_Raylib::GetInstance()->GetShader(CubismShaderNames_SetupMask);

		rlSetBlendFactorsSeparate(GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_FUNC_ADD);
		rlSetBlendMode(RL_BLEND_CUSTOM_SEPARATE);
		rlEnableShader(shaderSet->shaderProgram);

		rlActiveTextureSlot(0);
		rlEnableTexture(textureId);
		rlSetUniform(shaderSet->samplerTexture0, &GL_ZERO, RL_SHADER_UNIFORM_INT, 1); // TODO: ?

		const CubismTextureColor& colorChannel = s_colorChannels[cc->layoutChannel];
		rlSetUniform(shaderSet->unifromChannelFlag, &colorChannel.R, RL_SHADER_UNIFORM_VEC4, 1);
		rlSetUniformMatrix(shaderSet->uniformClipMatrix, cc->maskMatrix);

		const Rectangle& rect = cc->layoutRegion;
		float uniformBaseColor[4] = { rect.x * 2.0f - 1.0f, rect.y * 2.0f - 1.0f, (rect.x + rect.width) * 2.0f - 1.0f, (rect.y + rect.height) * 2.0f - 1.0f };
		rlSetUniform(shaderSet->uniformBaseColor, uniformBaseColor, RL_SHADER_UNIFORM_VEC4, 1);
		rlSetUniform(shaderSet->uniformMultiplyColor, &multiplyColor.R, RL_SHADER_UNIFORM_VEC4, 1);
		rlSetUniform(shaderSet->uniformScreenColor, &screenColor.R, RL_SHADER_UNIFORM_VEC4, 1);
	}
	else
	{
		shaderSet = CubismShader_Raylib::GetInstance()->GetShader(1 + ((cc != nullptr) ? (invertedMask ? 2 : 1) : 0) + (IsPremultipliedAlpha() ? 3 : 0));

		switch (colorBlendMode)
		{
		default:
			CSM_ASSERT(0);
		case CubismRenderer::CubismBlendMode_Normal:
			rlSetBlendFactorsSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_FUNC_ADD);
			break;

		case CubismRenderer::CubismBlendMode_Additive:
			rlSetBlendFactorsSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE, GL_FUNC_ADD, GL_FUNC_ADD);
			break;

		case CubismRenderer::CubismBlendMode_Multiplicative:
			rlSetBlendFactorsSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE, GL_FUNC_ADD, GL_FUNC_ADD);
			break;
		}
		rlSetBlendMode(RL_BLEND_CUSTOM_SEPARATE);

		rlEnableShader(shaderSet->shaderProgram);

		if (cc)
		{
			rlActiveTextureSlot(1);
			rlEnableTexture(m_colorBuffer);
			rlSetUniform(shaderSet->samplerTexture1, &GL_ONE, RL_SHADER_UNIFORM_INT, 1); // TODO:
			rlSetUniformMatrix(shaderSet->uniformClipMatrix, cc->drawMatrix);
			const CubismRenderer::CubismTextureColor& colorChannel = s_colorChannels[cc->layoutChannel];
			rlSetUniform(shaderSet->unifromChannelFlag, &colorChannel.R, RL_SHADER_UNIFORM_VEC4, 1);
		}

		rlActiveTextureSlot(0);
		rlEnableTexture(textureId);
		rlSetUniform(shaderSet->samplerTexture0, &GL_ZERO, RL_SHADER_UNIFORM_INT, 1); // TODO:

		rlSetUniformMatrix(shaderSet->uniformMatrix, MatrixTranspose(*reinterpret_cast<Matrix*>(GetMvpMatrix().GetArray()))); // TODO:
		rlSetUniform(shaderSet->uniformBaseColor, &modelColorRGBA.R, RL_SHADER_UNIFORM_VEC4, 1);
		rlSetUniform(shaderSet->uniformMultiplyColor, &multiplyColor.R, RL_SHADER_UNIFORM_VEC4, 1);
		rlSetUniform(shaderSet->uniformScreenColor, &screenColor.R, RL_SHADER_UNIFORM_VEC4, 1);
	}

	m_batch.ManuallyApplyBuffer();
}

void CubismRenderer_Raylib::InitializeOffscreenFrameBuffer(unsigned int externalColorBuffer)
{
	if (m_colorBuffer && !m_isExternalColorBuffer)
	{
		rlUnloadTexture(m_colorBuffer);
		m_colorBuffer = 0;
	}

	if (m_fbo)
	{
		rlUnloadTexture(m_fbo);
		m_fbo = 0;
	}

	if (externalColorBuffer)
	{
		m_isExternalColorBuffer = true;
		m_colorBuffer = externalColorBuffer;
	}
	else
	{
		m_isExternalColorBuffer = false;
		m_colorBuffer = rlLoadTexture(NULL, (int)m_clippingMaskBufferSize.first, (int)m_clippingMaskBufferSize.second, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1); // already unloaded
	}

	m_fbo = rlLoadFramebuffer(114514, 1919810);
	rlFramebufferAttach(m_fbo, m_colorBuffer, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlDisableFramebuffer();
}

void CubismRenderer_Raylib::LoadModelTexture(int cubismTextureId, const char* path)
{
	auto texture = LoadTexture(path);
	GenTextureMipmaps(&texture);
	rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_MIP_LINEAR);
	rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
	m_textureIdMapping[cubismTextureId] = texture.id;
}

void CubismRenderer_Raylib::DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount, csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray, csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask)
{
	CSM_ASSERT(0);
}

void CubismRenderer_Raylib::SaveProfile()
{
	CSM_ASSERT(0);
}

void CubismRenderer_Raylib::RestoreProfile()
{
	CSM_ASSERT(0);
}
