#include "CubismShader_Raylib.hpp"
#include "CubismShaderSource.hpp"
#include "CubismFramework.hpp"
#include "rlgl.h"
#include <cassert>

using Live2D::Cubism::Framework::csmFloat32; // TODO

// TODO
const int MAX_VERTEX_COUNT = 2048;

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
}

RenderBufferManager::RenderBufferManager()
{
	ClearBatch();
}

void RenderBufferManager::ClearBatch()
{
	m_batch.clear();
	for (int i = 0; i < CubismShaderNames_CountFlag; ++i)
	{
		m_poolTail[i] = m_pool[i].begin();
	}
}

void RenderBufferManager::CreateBuffer(int shaderId, const CubismShaderSet*& shaderSet, CubismRenderBufferSet*& bufferSet)
{
	shaderSet = CubismShader_Raylib::GetInstance()->GetShader(shaderId);

	// look for spare buffer item
	if (m_poolTail[shaderId] != m_pool[shaderId].end())
	{
		bufferSet = &*(m_poolTail[shaderId]++);
		goto ADD_TO_BATCH;
	}

	// create one

	m_pool[shaderId].emplace_back();
	m_poolTail[shaderId] = m_pool[shaderId].end();

	auto& buffer = m_pool[shaderId].back();

	buffer.vertexArrayObjectId = rlLoadVertexArray();
	rlEnableVertexArray(buffer.vertexArrayObjectId);

	buffer.vertexBufferPositionId = rlLoadVertexBuffer(NULL, sizeof(csmFloat32) * MAX_VERTEX_COUNT * 2, true);
	rlEnableVertexAttribute(shaderSet->attribPosition);
	rlSetVertexAttribute(shaderSet->attribPosition, 2, RL_FLOAT, false, sizeof(csmFloat32) * 2, NULL);

	buffer.vertexBufferTexcoordId = rlLoadVertexBuffer(NULL, sizeof(csmFloat32) * MAX_VERTEX_COUNT * 2, true);
	rlEnableVertexAttribute(shaderSet->attribTexCoord);
	rlSetVertexAttribute(shaderSet->attribTexCoord, 2, RL_FLOAT, false, sizeof(csmFloat32) * 2, NULL);

	buffer.vertexBufferElementId = rlLoadVertexBufferElement(NULL, sizeof(uint16_t) * MAX_VERTEX_COUNT, true);

	rlDisableVertexArray();

	bufferSet = &buffer;

ADD_TO_BATCH:
	m_batch.push_back(bufferSet);
}

void RenderBufferManager::BeginApplyBatch()
{
	m_applyingBatch = m_batch.begin();
}

void RenderBufferManager::ManuallyApplyBuffer()
{
	ApplyBuffer(**(m_applyingBatch++));
}

void RenderBufferManager::EndApplyBatch()
{
	rlDisableVertexArray();
	rlDisableShader();
	assert(m_applyingBatch == m_batch.end());
}

RenderBufferManager::~RenderBufferManager()
{
	for (auto i : m_batch)
	{
		rlUnloadVertexArray(i->vertexArrayObjectId);
		rlUnloadVertexBuffer(i->vertexBufferPositionId);
		rlUnloadVertexBuffer(i->vertexBufferTexcoordId);
		rlUnloadVertexBuffer(i->vertexBufferElementId);
	}
}

void RenderBufferManager::ApplyBuffer(const CubismRenderBufferSet& b)
{
	rlEnableVertexArray(b.vertexArrayObjectId);
	rlDrawVertexArrayElements(0, b.indexCount, NULL);
}
