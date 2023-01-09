#include "CubismShader_Raylib.hpp"
#include "CubismShaderSource.hpp"
#include "CubismFramework.hpp"
#include "rlgl.h"

using Live2D::Cubism::Framework::csmFloat32; // TODO

// TODO
const int MAX_VERTEX_COUNT = 8192;
// memory size of GPU memory:
// 7 (shader count) * 32 (RENDERER_BUFFER_OBJECT_QUEUE_COUNT) * 8192 (MAX_VERTEX_COUNT) * (sizeof(csmFloat32) * 4 + sizeof(uint16_t))
// ~ 32MB GPU memory
// TODO: can be optimized

CubismShader_Raylib* CubismShader_Raylib::m_instance;

CubismShader_Raylib* CubismShader_Raylib::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new CubismShader_Raylib();
        m_instance->LoadShaders();
    }
    return m_instance;
}

const CubismShaderSet* CubismShader_Raylib::GetShader(int id)
{
    return &m_shaders[id];
}

const CubismRenderBufferSet* CubismShader_Raylib::GetBuffer(const CubismShaderSet* shaderSet)
{
    const_cast<CubismShaderSet*>(shaderSet)->currentBuffer = (shaderSet->currentBuffer + 1) % RAYLIB_LIVE2D_RENDERER_BUFFER_OBJECT_QUEUE_COUNT;
    return &shaderSet->buffers[shaderSet->currentBuffer];
}

void CubismShader_Raylib::LoadShaders()
{
    // setup mask
    m_shaders[0].shaderProgram = rlLoadShaderCode(VertShaderSrcSetupMask, FragShaderSrcSetupMask);
    m_shaders[0].attribPosition = rlGetLocationAttrib(m_shaders[0].shaderProgram, "a_position");
    m_shaders[0].attribTexCoord = rlGetLocationAttrib(m_shaders[0].shaderProgram, "a_texCoord");
    m_shaders[0].samplerTexture0 = rlGetLocationUniform(m_shaders[0].shaderProgram, "s_texture0");
    m_shaders[0].uniformClipMatrix = rlGetLocationUniform(m_shaders[0].shaderProgram, "u_clipMatrix");
    m_shaders[0].unifromChannelFlag = rlGetLocationUniform(m_shaders[0].shaderProgram, "u_channelFlag");
    m_shaders[0].uniformBaseColor = rlGetLocationUniform(m_shaders[0].shaderProgram, "u_baseColor");
    m_shaders[0].uniformMultiplyColor = rlGetLocationUniform(m_shaders[0].shaderProgram, "u_multiplyColor");
    m_shaders[0].uniformScreenColor = rlGetLocationUniform(m_shaders[0].shaderProgram, "u_screenColor");

    // normal
    m_shaders[1].shaderProgram = rlLoadShaderCode(VertShaderSrc, FragShaderSrc);
    m_shaders[1].attribPosition = rlGetLocationAttrib(m_shaders[1].shaderProgram, "a_position");
    m_shaders[1].attribTexCoord = rlGetLocationAttrib(m_shaders[1].shaderProgram, "a_texCoord");
    m_shaders[1].samplerTexture0 = rlGetLocationUniform(m_shaders[1].shaderProgram, "s_texture0");
    m_shaders[1].uniformMatrix = rlGetLocationUniform(m_shaders[1].shaderProgram, "u_matrix");
    m_shaders[1].uniformBaseColor = rlGetLocationUniform(m_shaders[1].shaderProgram, "u_baseColor");
    m_shaders[1].uniformMultiplyColor = rlGetLocationUniform(m_shaders[1].shaderProgram, "u_multiplyColor");
    m_shaders[1].uniformScreenColor = rlGetLocationUniform(m_shaders[1].shaderProgram, "u_screenColor");

    // normal (masked)
    m_shaders[2].shaderProgram = rlLoadShaderCode(VertShaderSrcMasked, FragShaderSrcMask);
    m_shaders[2].attribPosition = rlGetLocationAttrib(m_shaders[2].shaderProgram, "a_position");
    m_shaders[2].attribTexCoord = rlGetLocationAttrib(m_shaders[2].shaderProgram, "a_texCoord");
    m_shaders[2].samplerTexture0 = rlGetLocationUniform(m_shaders[2].shaderProgram, "s_texture0");
    m_shaders[2].samplerTexture1 = rlGetLocationUniform(m_shaders[2].shaderProgram, "s_texture1");
    m_shaders[2].uniformMatrix = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_matrix");
    m_shaders[2].uniformClipMatrix = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_clipMatrix");
    m_shaders[2].unifromChannelFlag = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_channelFlag");
    m_shaders[2].uniformBaseColor = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_baseColor");
    m_shaders[2].uniformMultiplyColor = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_multiplyColor");
    m_shaders[2].uniformScreenColor = rlGetLocationUniform(m_shaders[2].shaderProgram, "u_screenColor");

    // normal (masked, inverted)
    m_shaders[3].shaderProgram = rlLoadShaderCode(VertShaderSrcMasked, FragShaderSrcMaskInverted);
    m_shaders[3].attribPosition = rlGetLocationAttrib(m_shaders[3].shaderProgram, "a_position");
    m_shaders[3].attribTexCoord = rlGetLocationAttrib(m_shaders[3].shaderProgram, "a_texCoord");
    m_shaders[3].samplerTexture0 = rlGetLocationUniform(m_shaders[3].shaderProgram, "s_texture0");
    m_shaders[3].samplerTexture1 = rlGetLocationUniform(m_shaders[3].shaderProgram, "s_texture1");
    m_shaders[3].uniformMatrix = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_matrix");
    m_shaders[3].uniformClipMatrix = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_clipMatrix");
    m_shaders[3].unifromChannelFlag = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_channelFlag");
    m_shaders[3].uniformBaseColor = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_baseColor");
    m_shaders[3].uniformMultiplyColor = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_multiplyColor");
    m_shaders[3].uniformScreenColor = rlGetLocationUniform(m_shaders[3].shaderProgram, "u_screenColor");

    // pma
    m_shaders[4].shaderProgram = rlLoadShaderCode(VertShaderSrc, FragShaderSrcPremultipliedAlpha);
    m_shaders[4].attribPosition = rlGetLocationAttrib(m_shaders[4].shaderProgram, "a_position");
    m_shaders[4].attribTexCoord = rlGetLocationAttrib(m_shaders[4].shaderProgram, "a_texCoord");
    m_shaders[4].samplerTexture0 = rlGetLocationUniform(m_shaders[4].shaderProgram, "s_texture0");
    m_shaders[4].uniformMatrix = rlGetLocationUniform(m_shaders[4].shaderProgram, "u_matrix");
    m_shaders[4].uniformBaseColor = rlGetLocationUniform(m_shaders[4].shaderProgram, "u_baseColor");
    m_shaders[4].uniformMultiplyColor = rlGetLocationUniform(m_shaders[4].shaderProgram, "u_multiplyColor");
    m_shaders[4].uniformScreenColor = rlGetLocationUniform(m_shaders[4].shaderProgram, "u_screenColor");

    // pma (masked)
    m_shaders[5].shaderProgram = rlLoadShaderCode(VertShaderSrcMasked, FragShaderSrcMaskPremultipliedAlpha);
    m_shaders[5].attribPosition = rlGetLocationAttrib(m_shaders[5].shaderProgram, "a_position");
    m_shaders[5].attribTexCoord = rlGetLocationAttrib(m_shaders[5].shaderProgram, "a_texCoord");
    m_shaders[5].samplerTexture0 = rlGetLocationUniform(m_shaders[5].shaderProgram, "s_texture0");
    m_shaders[5].samplerTexture1 = rlGetLocationUniform(m_shaders[5].shaderProgram, "s_texture1");
    m_shaders[5].uniformMatrix = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_matrix");
    m_shaders[5].uniformClipMatrix = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_clipMatrix");
    m_shaders[5].unifromChannelFlag = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_channelFlag");
    m_shaders[5].uniformBaseColor = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_baseColor");
    m_shaders[5].uniformMultiplyColor = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_multiplyColor");
    m_shaders[5].uniformScreenColor = rlGetLocationUniform(m_shaders[5].shaderProgram, "u_screenColor");

    // pma (masked, inverted)
    m_shaders[6].shaderProgram = rlLoadShaderCode(VertShaderSrcMasked, FragShaderSrcMaskInvertedPremultipliedAlpha);
    m_shaders[6].attribPosition = rlGetLocationAttrib(m_shaders[6].shaderProgram, "a_position");
    m_shaders[6].attribTexCoord = rlGetLocationAttrib(m_shaders[6].shaderProgram, "a_texCoord");
    m_shaders[6].samplerTexture0 = rlGetLocationUniform(m_shaders[6].shaderProgram, "s_texture0");
    m_shaders[6].samplerTexture1 = rlGetLocationUniform(m_shaders[6].shaderProgram, "s_texture1");
    m_shaders[6].uniformMatrix = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_matrix");
    m_shaders[6].uniformClipMatrix = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_clipMatrix");
    m_shaders[6].unifromChannelFlag = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_channelFlag");
    m_shaders[6].uniformBaseColor = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_baseColor");
    m_shaders[6].uniformMultiplyColor = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_multiplyColor");
    m_shaders[6].uniformScreenColor = rlGetLocationUniform(m_shaders[6].shaderProgram, "u_screenColor");

    for (auto& shaderSet : m_shaders)
    {
        shaderSet.currentBuffer = 0;
        for (int i = 0; i < RAYLIB_LIVE2D_RENDERER_BUFFER_OBJECT_QUEUE_COUNT; ++i)
        {
            auto& buffer = shaderSet.buffers[i];
            // VAO
            buffer.vertexArrayObjectId = rlLoadVertexArray();
            rlEnableVertexArray(buffer.vertexArrayObjectId);

            // VBO (attribPosition)
            buffer.vertexBufferPositionId = rlLoadVertexBuffer(NULL, sizeof(csmFloat32) * MAX_VERTEX_COUNT * 2, true);
            rlEnableVertexAttribute(shaderSet.attribPosition);
            rlSetVertexAttribute(shaderSet.attribPosition, 2, RL_FLOAT, false, sizeof(csmFloat32) * 2, NULL);

            // VBO (attribTexCoord)
            buffer.vertexBufferTexcoordId = rlLoadVertexBuffer(NULL, sizeof(csmFloat32) * MAX_VERTEX_COUNT * 2, true);
            rlEnableVertexAttribute(shaderSet.attribTexCoord);
            rlSetVertexAttribute(shaderSet.attribTexCoord, 2, RL_FLOAT, false, sizeof(csmFloat32) * 2, NULL);

            // EBO
            buffer.vertexBufferElementId = rlLoadVertexBufferElement(NULL, sizeof(uint16_t) * MAX_VERTEX_COUNT, true);

            rlDisableVertexArray();
            rlDisableVertexBuffer();
            rlDisableVertexBufferElement();
        }
    }
}
