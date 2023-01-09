#pragma once

#include <vector>
#include <map>
#include "Rendering/CubismRenderer.hpp"
#include "CubismFramework.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"
#include "Math/CubismVector2.hpp"
#include "Type/csmMap.hpp"
#include "CubismClippingContext.hpp"
#include "CubismShader_Raylib.hpp"

using Live2D::Cubism::Framework::Rendering::CubismRenderer;
using Live2D::Cubism::Framework::CubismModel;
using Live2D::Cubism::Framework::csmInt32;
using Live2D::Cubism::Framework::csmUint16;
using Live2D::Cubism::Framework::csmFloat32;
using Live2D::Cubism::Framework::csmBool;

class CubismRenderer_Raylib : public CubismRenderer {
public:
	virtual void Initialize(CubismModel* model) override;

    void SetFrameBuffer(int fbo);

    void DrawMeshInternal1(csmInt32 textureNo, csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask, const CubismTextureColor& multiplyColor, const CubismTextureColor& screenColor, bool drawingMask, const CubismClippingContext* cc);

    void InitializeOffscreenFrameBuffer(unsigned int externalColorBuffer = 0);

    void LoadModelTexture(int cubismTextureId, const char* path);

    void DoDrawModel1();

protected:
    virtual ~CubismRenderer_Raylib() override;

    virtual void DoDrawModel() override;

    virtual void DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount
        , csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray
        , csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask) override;

    virtual void SaveProfile() override;

    virtual void RestoreProfile() override;

private:
    void DrawMeshInternal(csmInt32 indexCount, csmInt32 vertexCount, const csmUint16* indexArray,
        const csmFloat32* vertexArray, const csmFloat32* uvArray, csmBool invertedMask,
        bool drawingMask, const CubismClippingContext* cc
    );

    void EnableOffscreenBuffer();

    void DisableOffscreenBuffer();

    bool m_isExternalColorBuffer;
    unsigned int m_oldFbo = 0;
    unsigned int m_fbo = 0;
    unsigned int m_colorBuffer = 0;

    RenderBufferManager m_batch;

    std::pair<int, int> m_clippingMaskBufferSize = { 256, 256 };         // TODO: set it?
    std::vector<CubismClippingContext*> m_clippingContextsForMask;       // all masks, m_clippingContextsForDraw.Distinct().NotNull()
    std::vector<CubismClippingContext*> m_clippingContextsForDraw;       // index is drawable id, may be null (not clipped)
    std::vector<csmInt32> m_sortedDrawables;
    std::map<csmInt32, unsigned int> m_textureIdMapping;
    static std::array<CubismRenderer::CubismTextureColor, 4> s_colorChannels;
};
